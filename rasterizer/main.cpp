#include "rasterizer.h"
using namespace rasterizer;

Application* app;
Canvas* canvas;

void MainLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication(argc, argv, "rasterizer", 512, 512);
	canvas = app->GetCanvas();
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}

void MainLoop()
{
	Camera camera;
	camera.SetLookAt(Vector3(0, 0, 100), Vector3(0, 0, 0), Vector3::up);
	camera.SetPerspective(90, 1, 0.3f, 1000);
	//camera.SetOrthographic(-100, 100, -100, 100, 0.3f, 1000);
	Vector3 position = Vector3::zero;
	Vector3 scale = Vector3::one;
	std::vector<Mesh> meshes;
	//Mesh::LoadMesh(meshes, "resources/cube/cube.obj");
	//position = Vector3(0, 0, 0);
	//scale = Vector3(10, 10, 10);
	Mesh::LoadMesh(meshes, "resources/teapot/teapot.obj");
	position = Vector3(0, -50, 0);
	scale = Vector3(1, 1, 1);

	float r = 0;
	float s = 1;
	while (true)
	{
		r += 0.1f;
		//s *= 1.01f;
		canvas->BeginDraw();

		Matrix4x4 trans(position, Quaternion(Vector3(r, r, 0)), scale);

		for (int i = 0; i < (int)meshes.size(); ++i)
		{
			Rasterizer::DrawMeshPoint(canvas, camera, meshes[i], trans, Color::green);
		}

		//Rasterizer::Line(canvas, Color(1, 1, 1, 1), 300, 210, 10, 100);
		//Rasterizer::Line(canvas, Color(0.6f, 0.5f, 1, 1), 400, 220, 10, 100);
		//Rasterizer::Line(canvas, Color(0.2f, 1, 1, 1), 400, 220, 100, 100);
		//Rasterizer::Line(canvas, Color(1, 1, 1, 1), 220, 399, -100, 400);
		//Rasterizer::Line(canvas, Color(1, 1, 0.5f, 1), 270, 270, 150, 400);

		//Rasterizer::SmoothLine(canvas, Color(1, 1, 1, 1), 100, 10, 10, 100);
		//Rasterizer::SmoothLine(canvas, Color(0.6f, 0.5f, 1, 1), 200, 20, 10, 100);
		//Rasterizer::SmoothLine(canvas, Color(0.2f, 1, 1, 1), 200, 20, 100, 100);
		//Rasterizer::SmoothLine(canvas, Color(1, 1, 1, 1), 20, 199, -100, 400);
		//Rasterizer::SmoothLine(canvas, Color(1, 1, 0.5f, 1), 70, 70, 150, 400);



		canvas->EndDraw();
	}

}