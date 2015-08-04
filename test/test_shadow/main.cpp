#include "rasterizer.h"
#include "transform_controller.hpp"
#include "object_utilities.h"
using namespace rasterizer;

Application* app;

void MainLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("shadow", 800, 600);
	Rasterizer::Initialize(800, 600);
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}

struct Vertex
{
	Vector3 position;
	Vector3 normal;
};

struct SMVaryingData
{
	Vector4 position;
	static std::vector<VaryingDataElement> GetDecl()
	{
		static std::vector<VaryingDataElement> decl = {
			{ 0, VaryingDataDeclUsage_SVPOSITION, VaryingDataDeclFormat_Vector4 }
		};

		return decl;
	}
};

struct VaryingData
{
	Vector4 position;
	Vector3 normal;
	Vector3 worldPos;

	static std::vector<VaryingDataElement> GetDecl()
	{
		static std::vector<VaryingDataElement> decl = {
			{ 0, VaryingDataDeclUsage_SVPOSITION, VaryingDataDeclFormat_Vector4 },
			{ 16, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 28, VaryingDataDeclUsage_POSITION, VaryingDataDeclFormat_Vector3 }
		};

		return decl;
	}
};

struct ShadowMapPrePass : Shader<Vertex, SMVaryingData>
{
	SMVaryingData vert(const Vertex& input) override
	{
		SMVaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		return output;
	}
};

struct ObjShader : Shader<Vertex, VaryingData>
{
	Color ambientColor = Color(0.f, 0.1f, 0.1f, 0.1f);
	Texture2DPtr shadowMap = nullptr;
	Matrix4x4 lightVPM;

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.normal = _Object2World.MultiplyVector(input.normal);
		output.worldPos = _Object2World.MultiplyPoint3x4(input.position);
		return output;
	}

	Color frag(const VaryingData& input) override
	{
		Color color = Color::white;

		Vector4 proj = lightVPM.MultiplyPoint(input.worldPos);
		Vector2 smuv = Vector2((proj.x / proj.w + 1.f) / 2.f, (proj.y / proj.w + 1.f) / 2.f);
		float shadow = Tex2DProjInterpolated(shadowMap, smuv, proj.z / proj.w, 0.005f);

		Vector3 lightDir;
		float lightAtten;
		InitLightArgs(input.worldPos, lightDir, lightAtten);

		color.rgb *= Mathf::Max(0.f, lightDir.Dot(input.normal)) * lightAtten * (1.f - shadow);
		return color + ambientColor;
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static Transform objectTrans;
	static Transform planeTrans;
	static TransformController objectCtrl;
	static MeshWrapper<Vertex> objectMesh;
	static MeshWrapper<Vertex> planeMesh;
	static std::shared_ptr<ShadowMapPrePass> smPrePass = std::make_shared<ShadowMapPrePass>();
	static std::shared_ptr<ObjShader> objectShader = std::make_shared<ObjShader>();
	static LightPtr light;
	static CameraPtr camera;
	static RenderTexturePtr shadowMap = std::make_shared<RenderTexture>(1024.f, 1024.f);

	if (!isInitilized)
    {
		isInitilized = true;

		camera = CameraPtr(new Camera());
		camera->SetPerspective(60.f, 1.33333f, 0.3f, 20.f);
		camera->transform.position = Vector3(0.f, 0.f, -2.f);
		Rasterizer::camera = camera;

		light = LightPtr(new Light());
		light->type = Light::LightType_Directional;
		light->transform.position = Vector3(0, 0, 0);
		light->transform.rotation = Quaternion(Vector3(45.f, -45.f, 0.f));
		light->Initilize();
		Rasterizer::light = light;

		planeTrans.position = Vector3(0.f, -1.f, 0.f);
		planeTrans.rotation = Quaternion(Vector3(90.f, 0.f, 0.f));
		planeTrans.scale = Vector3(10.f, 10.f, 1.f);
		MeshPtr mesh = CreatePlane();
		planeMesh.vertices.clear();
		planeMesh.indices.clear();
		int vertexCount = mesh->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
			planeMesh.vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->normals[i] });
		for (auto idx : mesh->indices) planeMesh.indices.emplace_back((uint16_t)idx);

		objectTrans.position = Vector3(0.f, 0.f, 2.f);
		std::vector<MeshPtr> meshes;
		Mesh::LoadMesh(meshes, "resources/knot.obj");
		//Mesh::LoadMesh(meshes, "resources/cube/cube.obj");
		mesh = meshes[0];
		objectMesh.vertices.clear();
		objectMesh.indices.clear();
		vertexCount = mesh->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
			objectMesh.vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->normals[i] });
		for (auto idx : mesh->indices) objectMesh.indices.emplace_back((uint16_t)idx);
    }

	objectCtrl.MouseRotate(objectTrans);

	//Render ShadowMap
	Rasterizer::SetRenderTarget(shadowMap);
	Rasterizer::Clear(true, false, Color::black);

	Matrix4x4 cameraVM = camera->viewMatrix();
	Matrix4x4 cameraPM = camera->projectionMatrix();

	Matrix4x4 cameraVPIM = (cameraPM * cameraVM).Inverse();
	CameraPtr lightCamera = light->BuildShadowMapVirtualCamera(cameraVPIM, Vector3::zero, Vector3::zero);
	Matrix4x4 lightVM = lightCamera->viewMatrix();
	Matrix4x4 lightPM = lightCamera->projectionMatrix();

	Rasterizer::camera = lightCamera;
	Rasterizer::SetShader(smPrePass);
	Rasterizer::renderState.cull = RenderState::CullType_Off;
	//Rasterizer::renderState.renderType = RenderState::RenderType_ShadowPrePass;
	Rasterizer::modelMatrix = objectTrans.localToWorldMatrix();
	Rasterizer::renderData.AssignVertexBuffer(objectMesh.vertices);
	Rasterizer::renderData.AssignIndexBuffer(objectMesh.indices);
	Rasterizer::Submit();

	//Render Scene
	Rasterizer::SetRenderTarget(nullptr);
	Rasterizer::Clear(true, true, Color(1.f, 0.19f, 0.3f, 0.47f));

	Rasterizer::camera = camera;
	BitmapPtr bitmap = shadowMap->GetDepthBuffer();
	objectShader->shadowMap = Texture2D::CreateWithBitmap(bitmap);
	objectShader->lightVPM = lightPM * lightVM;
	Rasterizer::SetShader(objectShader);
	Rasterizer::renderState.cull = RenderState::CullType_Back;
	//Rasterizer::renderState.renderType = RenderState::RenderType_Stardand;

	Rasterizer::modelMatrix = planeTrans.localToWorldMatrix();
	Rasterizer::renderData.AssignVertexBuffer(planeMesh.vertices);
	Rasterizer::renderData.AssignIndexBuffer(planeMesh.indices);
	Rasterizer::Submit();

	Rasterizer::modelMatrix = objectTrans.localToWorldMatrix();
	Rasterizer::renderData.AssignVertexBuffer(objectMesh.vertices);
	Rasterizer::renderData.AssignIndexBuffer(objectMesh.indices);
	Rasterizer::Submit();

    Rasterizer::Present();
}
