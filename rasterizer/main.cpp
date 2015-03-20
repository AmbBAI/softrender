#include "rasterizer.h"
using namespace rasterizer;

Canvas* canvas;

void MainLoop();

int main(int argc, char *argv[])
{
	Application* app = Application::GetInstance();
	app->CreateApplication("rasterizer", 512, 512);
	canvas = app->GetCanvas();
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}

CameraPtr CreateCamera();
MeshPtr CreatePlane();
void LoadTexture();
MeshPtr mesh;
void MainLoop()
{
    if (mesh == nullptr)
    {
        Rasterizer::canvas = canvas;
        Rasterizer::camera = CreateCamera();

        mesh = CreatePlane();

        if (mesh->normals.size() <= 0)
            mesh->RecalculateNormals();
        
        LoadTexture();
    }

//	Vector3 position = Vector3::zero;
//	Vector3 scale = Vector3::one;
//	std::vector<MeshPtr> meshes;
//	//Mesh::LoadMesh(meshes, "resources/cube/cube.obj");
//	//position = Vector3(0, 0, 0);
//	//scale = Vector3(20, 20, 20);
//	//Mesh::LoadMesh(meshes, "resources/teapot/teapot.obj");
//	//position = Vector3(0, -50, 0);
//	//scale = Vector3(1, 1, 1);
//	Mesh::LoadMesh(meshes, "resources/head/head.obj");
//	position = Vector3(0, 0, 0);
//	scale = Vector3(400, 400, 400);
//	//meshes.push_back(Plane());
    Vector3 position = Vector3(0, 0, -100.3/2);
    Vector3 rotation = Vector3(45, 45, 0);
	Vector3 scale = Vector3(50, 50, 50);

	Application* app = Application::GetInstance();
	float startTime = app->GetTime();
	canvas->Clear();

	Matrix4x4 trans(position, Quaternion(rotation), scale);

    Rasterizer::DrawMesh(*(mesh), trans, Color::white);
    
//	for (int i = 0; i < (int)meshes.size(); ++i)
//	{
//		Rasterizer::DrawMesh(*(meshes[i]), trans, Color::white);
//	}

	canvas->Present();

	float endTime = app->GetTime();
	printf("%.3f\n", endTime - startTime);
}

CameraPtr CreateCamera()
{
	CameraPtr camera(new Camera());
	camera->SetLookAt(Vector3(0, 0, 100), Vector3(0, 0, 0), Vector3::up);
	camera->SetPerspective(90, 1, 0.3f, 1000);
	//camera.SetOrthographic(-100, 100, -100, 100, 0.3f, 1000);
	return camera;
}

void LoadTexture()
{
	TexturePtr texture = Texture::LoadTexture("resources/head/lambertian.jpg");
	TexturePtr bump = Texture::LoadTexture("resources/head/bump-lowRes.png");
	bump->ConvertBumpToNormal();

	Rasterizer::texture = texture;
	Rasterizer::normalMap = bump;
}

MeshPtr CreatePlane()
{
	MeshPtr mesh(new Mesh());
	mesh->vertices.push_back(Vector3(-1, -1, 0));
	mesh->vertices.push_back(Vector3(1, -1, 0));
	mesh->vertices.push_back(Vector3(1, 1, 0));
	mesh->vertices.push_back(Vector3(-1, 1, 0));

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
	return mesh;
}
