#include "object_utilities.h"
using namespace rasterizer;

MeshPtr CreatePlane(const MaterialPtr& mat)
{
    //MeshPtr mesh = std::make_shared<Mesh>();
	MeshPtr mesh = MeshPtr(new Mesh());
	mesh->vertices.push_back(Vector3(-1, -1, 0));
    mesh->vertices.push_back(Vector3(1, -1, 0));
    mesh->vertices.push_back(Vector3(1, 1, 0));
    mesh->vertices.push_back(Vector3(-1, 1, 0));
    
    mesh->normals.push_back(Vector3(0, 0, 1));
    mesh->normals.push_back(Vector3(0, 0, 1));
    mesh->normals.push_back(Vector3(0, 0, 1));
    mesh->normals.push_back(Vector3(0, 0, 1));
    
    mesh->texcoords.push_back(Vector2(0, 0));
    mesh->texcoords.push_back(Vector2(1, 0));
    mesh->texcoords.push_back(Vector2(1, 1));
    mesh->texcoords.push_back(Vector2(0, 1));

	mesh->materials.push_back(mat);
	mesh->materials.push_back(mat);
	mesh->materials.push_back(mat);
	mesh->materials.push_back(mat);

    mesh->indices.push_back(0);
    mesh->indices.push_back(1);
    mesh->indices.push_back(2);
    mesh->indices.push_back(2);
    mesh->indices.push_back(3);
    mesh->indices.push_back(0);

    mesh->CalculateTangents();
    
    return mesh;
}

void LoadSponzaMesh(std::vector<MeshPtr>& mesh, Transform& trans)
{
    Mesh::LoadMesh(mesh, "resources/crytek-sponza/sponza.obj");
    trans.position = Vector3(0, 0, 0);
    trans.rotation = Vector3(0, 0, 0);
    trans.scale = Vector3(1, 1, 1);

	for (auto& m : mesh)
	{
		if (m->normals.size() <= 0)
			m->RecalculateNormals();
	}
}
