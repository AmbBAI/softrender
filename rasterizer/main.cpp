#include "rasterizer.h"
using namespace rasterizer;

Canvas* canvas;
Input* input;

void MainLoop();

int main(int argc, char *argv[])
{
	Application* app = Application::GetInstance();
	app->CreateApplication("rasterizer", 512, 512);
	canvas = app->GetCanvas();
	input = app->GetInput();
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}

Vector3 cPosition = Vector3(0, 0, 100);
Vector3 cTarget = Vector3(0, 0, 0);
CameraPtr CreateCamera();
void UpdateCamera(CameraPtr camera);
MeshPtr CreatePlane();
std::vector<MeshPtr> mesh;
Vector3 position = Vector3::zero;
Vector3 rotation = Vector3::zero;
Vector3 scale = Vector3::one;
void MainLoop()
{
    if (mesh.size() == 0)
    {
		Rasterizer::Initialize();
        Rasterizer::canvas = canvas;
        Rasterizer::camera = CreateCamera();

        //mesh.push_back(CreatePlane());
		//MaterialPtr material(new Material());
		//material->diffuseTexture = Texture::LoadTexture("resources/cube/default.png");
        //mesh[0]->materials.push_back(material);
		Mesh::LoadMesh(mesh, "resources/crytek-sponza/sponza.obj");

		for (auto m : mesh)
		{
			if (m->normals.size() <= 0)
				m->RecalculateNormals();
		}
        
        position = Vector3(0, 0, 0);
        rotation = Vector3(0, 0, 0);
        scale = Vector3(1, 1, 1);

		Rasterizer::fragmentShader = Rasterizer::FS;
    }

	UpdateCamera(Rasterizer::camera);

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
//	//meshes.push_back(Plane());
//	Vector3 position = Vector3(0, 0, -100.3/2);
//	Vector3 rotation = Vector3(45, 45, 0);
//	Vector3 scale = Vector3(50, 50, 50);

	Application* app = Application::GetInstance();
	float startTime = app->GetTime();
	canvas->Clear();

	Matrix4x4 trans(position, Quaternion(rotation), scale);

	for (auto m : mesh)
	{
        if (m->materials.size() > 0) Rasterizer::material = m->materials[0];
		Rasterizer::DrawMesh(*m, trans, Color::white);
        //Rasterizer::DrawMeshWireFrame(*m, trans, Color::red);
	}

	canvas->Present();

	float endTime = app->GetTime();
	printf("\r%.3f\n", endTime - startTime);
}

CameraPtr CreateCamera()
{
	CameraPtr camera(new Camera());
	camera->SetLookAt(cPosition, cTarget, Vector3::up);
	camera->SetPerspective(90, 1, 0.3f, 10000);
	//camera.SetOrthographic(-100, 100, -100, 100, 0.3f, 1000);
	return camera;
}

void UpdateCamera(CameraPtr camera)
{
	bool changed = false;
	if (input->GetKey(GLFW_KEY_W))
	{
		cPosition.z -= 10;
		cTarget.z -= 10;
		changed = true;
	}
	if (input->GetKey(GLFW_KEY_S))
	{
		cPosition.z += 10;
		cTarget.z += 10;
		changed = true;
	}
	if (input->GetKey(GLFW_KEY_A))
	{
		cPosition.x -= 10;
		cTarget.x -= 10;
		changed = true;
	}
	if (input->GetKey(GLFW_KEY_D))
	{
		cPosition.x += 10;
		cTarget.x += 10;
		changed = true;
	}
	if (input->GetKey(GLFW_KEY_Q))
	{
		cPosition.y -= 10;
		cTarget.y -= 10;
		changed = true;
	}
	if (input->GetKey(GLFW_KEY_E))
	{
		cPosition.y += 10;
		cTarget.y += 10;
		changed = true;
	}

	if (changed) camera->SetLookAt(cPosition, cTarget, Vector3::up);
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
