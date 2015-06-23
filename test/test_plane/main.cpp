#include "rasterizer.h"
#include "camera_controller.h"
#include "object_utilities.h"
using namespace rasterizer;

Application* app;

void MainLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("sponza", 800, 600);
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}

struct Vertex
{
	Vector3 position;
	Vector3 normal;
	Vector2 texCoord;
};

void MainLoop()
{
	static bool isInitilized = false;
	static std::vector<MeshPtr> mesh;
	static Transform trans;
	static std::vector<Vertex> vertices;
	static std::vector<u16> indices;
	Canvas* canvas = app->GetCanvas();

	if (!isInitilized)
    {
		isInitilized = true;

		Rasterizer::Initialize();
        Rasterizer::canvas = canvas;
		//Rasterizer::camera = std::make_shared<Camera>();
		Rasterizer::camera = CameraPtr(new Camera());
		CameraController::InitCamera(Rasterizer::camera);

		MaterialPtr mat = MaterialPtr(new Material());
		mesh.push_back(CreatePlane(mat));

		vertices.clear();
		indices.clear();

		auto& m = mesh[0];
		int vertexCount = m->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
		{
			vertices.emplace_back(Vertex{ m->vertices[i], m->normals[i], m->texcoords[i] });
		}
		for (auto idx : m->indices) indices.emplace_back((u16)idx);
    }

	CameraController::UpdateCamera();

	canvas->Clear();

	Rasterizer::PrepareRender();

	Rasterizer::matrix = trans.GetMatrix();
	Rasterizer::renderData.AssignVertexBuffer(vertices);
	Rasterizer::renderData.AssignIndexBuffer(indices);

	Rasterizer::Render();

    canvas->Present();
}
