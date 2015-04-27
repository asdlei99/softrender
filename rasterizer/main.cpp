#include "rasterizer.h"
#include "utilities/camera_controller.h"
#include "utilities/object_utilities.h"
#include "base/ui.h"
using namespace rasterizer;

Canvas* canvas;
Application* app;

void MainLoop();
void TestTextureLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("rasterizer", 800, 600);
	canvas = app->GetCanvas();
    
    UI::SetFont("resources/DejaVuSans.ttf");
    UI::SetIconImage("resources/blender_icons16.png");
    
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}

std::vector<MeshPtr> mesh;
Transform trans;
void MainLoop()
{
    if (mesh.size() == 0)
    {
		Rasterizer::Initialize();
        Rasterizer::canvas = canvas;
		Rasterizer::camera = CameraPtr(new Camera());
		CameraController::InitCamera(Rasterizer::camera);

//		mesh.push_back(CreatePlane());
//		position = Vector3(0, 0, -20);
//		rotation = Vector3(0, 0, 0);
//		scale = Vector3(10, 10, 10);
//		MaterialPtr material(new Material());
//		material->diffuseTexture = Texture::LoadTexture("resources/cube/default.png");
//		material->normalTexture = Texture::LoadTexture("resources/sponza/textures/lion_ddn.tga");
//		mesh[0]->materials.push_back(material);
//        Rasterizer::light = LightPtr(new Light());
//        Rasterizer::light->type = Light::LightType_Point;
//        Rasterizer::light->position = Vector3(0.f, 0.f, -15.f);
//        Rasterizer::light->range = 10.f;
//        Rasterizer::light->atten0 = 2.f;
//        Rasterizer::light->atten1 = 2.f;
//        Rasterizer::light->atten2 = 1.f;
//        Rasterizer::light->Initilize();

        LoadSponzaMesh(mesh, trans);
		Rasterizer::light = LightPtr(new Light());
		Rasterizer::light->type = Light::LightType_Directional;
		Rasterizer::light->position = Vector3(0, 300, 0);
		Rasterizer::light->direction = Vector3(-1.f, -1.f, -1.f).Normalize();
		Rasterizer::light->range = 1000.f;
		Rasterizer::light->Initilize();
        
		//Mesh::LoadMesh(mesh, "resources/head/head.OBJ");
		//position = Vector3(0, 10, -50);
		//rotation = Vector3(10, 10, 0);
		//scale = Vector3(250, 250, 250);
        
  //      CameraController::moveScale = 0.5f;
  //      Mesh::LoadMesh(mesh, "resources/cornell-box/CornellBox-Sphere.obj");
  //      position = Vector3(0, -1, -2);
  //      rotation = Vector3(0, 0, 0);
  //      scale = Vector3(1, 1, 1);
  //      Rasterizer::light = LightPtr(new Light());
  //      Rasterizer::light->type = Light::LightType_Point;
		//Rasterizer::light->color = Color::white;
  //      //Rasterizer::light->position = Vector3(5.f, 0.f, -2.f);
		//Rasterizer::light->position = Vector3(0.f, 5.f, -2.f);
  //      Rasterizer::light->direction = Vector3(0.f, -1.f, 0.f);
  //      Rasterizer::light->range = 10.f;
  //      Rasterizer::light->atten0 = 2.f;
  //      Rasterizer::light->atten1 = 2.f;
  //      Rasterizer::light->atten2 = 1.f;
  //      Rasterizer::light->Initilize();
        

		for (auto& m : mesh)
		{
			if (m->normals.size() <= 0)
				m->RecalculateNormals();
		}
    }

	//Rasterizer::light->position -= Vector3(0.01f, 0, 0);
	//Rasterizer::light->position -= Vector3(0, 0.01f, 0);
	CameraController::UpdateCamera();

	canvas->Clear();

	Matrix4x4 transM = trans.GetMatrix();

	for (auto& m : mesh)
	{
		if (m->materials.size() > 0) Rasterizer::material = m->materials[0];
		Rasterizer::DrawMesh(*m, transM);
	}

    canvas->Present();

    UI::Begin();

	char fs[512];
	sprintf(fs, "%.3f", app->GetDeltaTime());

	uiBeginLayout();

	int root = UI::Root();
	int label = UI::Label(root, -1, fs, Color::red, UI_HFILL | UI_TOP);

	int panel = UI::Panel(root, UI_TOP | UI_RIGHT, 200, 0);
	uiSetBox(panel, UI_COLUMN);

	int box = UI::Box(panel, UI_ROW, UI_HFILL | UI_TOP);
	static int menu_status = -1;
	int menu1 = UI::Radio(box, BND_ICON_MESH_MONKEY, NULL, &menu_status);
	int menu2 = UI::Radio(box, BND_ICON_CAMERA_DATA, NULL, &menu_status);
	int menu3 = UI::Radio(box, BND_ICON_TEXTURE, NULL, &menu_status);
	int menu4 = UI::Radio(box, BND_ICON_MATERIAL, NULL, &menu_status);
	int menu5 = UI::Radio(box, BND_ICON_LAMP, NULL, &menu_status);
	int menu6 = UI::Radio(box, BND_ICON_TEXTURE_SHADED, NULL, &menu_status);

	if (menu_status == menu1)
	{
		static int isMeshDrawPoint = (int)Rasterizer::isDrawPoint;
		static int isMeshDrawWireframe = (int)Rasterizer::isDrawWireFrame;
		static int isMeshDrawTextured = (int)Rasterizer::isDrawTextured;

		int box1 = UI::Box(panel, UI_COLUMN, UI_HFILL | UI_TOP);
		uiSetMargins(box1, 10, 10, 10, 10);
		UI::Check(box1, "Point", &isMeshDrawPoint);
		UI::Check(box1, "WireFrame", &isMeshDrawWireframe);
		UI::Check(box1, "Textured", &isMeshDrawTextured);

		Rasterizer::isDrawPoint = (isMeshDrawPoint != 0);
		Rasterizer::isDrawWireFrame = (isMeshDrawWireframe != 0);
		Rasterizer::isDrawTextured = (isMeshDrawTextured != 0);
	}

	uiEndLayout();

	UI::DrawUI(root, BND_CORNER_TOP);
	uiProcess((int)(glfwGetTime()*1000.0));
    
    UI::End();
}

void TestTextureLoop()
{
	static TexturePtr tex = nullptr;
	if (tex == nullptr)
	{
		tex = Texture::LoadTexture("resources/crytek-sponza/textures/lion_bump.png");
		tex->ConvertBumpToNormal();
//		if (tex != nullptr)
//		{
//			tex->filterMode = Texture::FilterMode_Point;
//			tex->GenerateMipmaps();
//		}
	}

	canvas->Clear();

	TestTexture(canvas, Vector4(0, 0, 512, 512), *tex, 0);
	//TestTexture(canvas, Vector4(256, 256, 512, 512), *tex, 1);
	canvas->Present();
}
