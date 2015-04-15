#include "rasterizer.h"
#include "utilities/camera_controller.h"
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
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}

MeshPtr CreatePlane();
std::vector<MeshPtr> mesh;
Vector3 position = Vector3::zero;
Vector3 rotation = Vector3::zero;
Vector3 scale = Vector3::one;
void MainLoop()
{
	printf("%.3f\n", app->GetDeltaTime());

    if (mesh.size() == 0)
    {
		Rasterizer::Initialize();
        Rasterizer::canvas = canvas;
		Rasterizer::camera = CameraPtr(new Camera());
		CameraController::InitCamera(Rasterizer::camera);

		//mesh.push_back(CreatePlane());
		//position = Vector3(0, -20.f, -20.f);
		//rotation = Vector3(0, 0, 0);
		//scale = Vector3(50, 50, 50);
		//MaterialPtr material(new Material());
		//material->diffuseTexture = Texture::LoadTexture("resources/cube/default.png");
		//material->normalTexture = Texture::LoadTexture("resources/sponza/textures/lion_ddn.tga");
		//mesh[0]->materials.push_back(material);

        Rasterizer::light = LightPtr(new Light());
        Rasterizer::light->direction = Vector3(-1.f, -1.f, -1.f).Normalize();
		Mesh::LoadMesh(mesh, "resources/crytek-sponza/sponza.obj");
		position = Vector3(0, 0, 0);
		rotation = Vector3(0, 0, 0);
		scale = Vector3(1, 1, 1);

		//Mesh::LoadMesh(mesh, "resources/head/head.OBJ");
		//position = Vector3(0, 10, -50);
		//rotation = Vector3(10, 10, 0);
		//scale = Vector3(250, 250, 250);
        
        
//        Mesh::LoadMesh(mesh, "resources/cornell-box/CornellBox-Sphere.obj");
//        position = Vector3(0, -1, -2);
//        rotation = Vector3(0, 0, 0);
//        scale = Vector3(1, 1, 1);
//        Rasterizer::light = LightPtr(new Light());
//        Rasterizer::light->direction = Vector3(0.f, -1.f, 0.f);
        

		for (auto& m : mesh)
		{
			if (m->normals.size() <= 0)
				m->RecalculateNormals();
		}
    }

	CameraController::UpdateCamera();

	canvas->Clear();

	Matrix4x4 trans(position, Quaternion(rotation), scale);

	for (auto& m : mesh)
	{
		if (m->materials.size() > 0) Rasterizer::material = m->materials[0];
		Rasterizer::DrawMesh(*m, trans);
        //Rasterizer::DrawMeshWireFrame(*m, trans, Color::red);
        //Rasterizer::DrawMeshPoint(*m, trans, Color::red);
	}

	canvas->Present();
}

MeshPtr CreatePlane()
{
	MeshPtr mesh(new Mesh());
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

	mesh->indices.push_back(0);
	mesh->indices.push_back(1);
	mesh->indices.push_back(2);
	mesh->indices.push_back(2);
	mesh->indices.push_back(3);
	mesh->indices.push_back(0);

	mesh->CalculateTangents();

	return mesh;
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
