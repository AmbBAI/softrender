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
	camera.SetPerspective(90, 1, 0.3f, 100);
	//camera.SetOrthographic(-100, 100, -100, 100, 0.3f, 1000);
	Vector3 position = Vector3::zero;
	Vector3 scale = Vector3::one;
	std::vector<Mesh> meshes;
	//Mesh::LoadMesh(meshes, "resources/cube/cube.obj");
	//position = Vector3(0, 0, 0);
	//scale = Vector3(20, 20, 20);
	Mesh::LoadMesh(meshes, "resources/teapot/teapot.obj");
	position = Vector3(0, -50, 0);
	scale = Vector3(1, 1, 1);
	//Mesh::LoadMesh(meshes, "resources/head/head.obj");
	//position = Vector3(0, 0, 0);
	//scale = Vector3(400, 400, 400);

	Texture texture;
	Texture::LoadTexture(texture, "resources/teapot/default.png");

	for (int i = 0; i < (int)meshes.size(); ++i)
	{
		if (meshes[i].normals.size() <= 0) meshes[i].BuildNormal();
	}

	Rasterizer::canvas = canvas;
	Rasterizer::camera = &camera;
	Rasterizer::texture = &texture;

	float r = 0;
	float s = 1;
	while (true)
	{
		r += 0.1f;
		//s *= 1.01f;
		clock_t startTime = clock();
		canvas->Clear();

		//int width = texture.GetWidth();
		//int height = texture.GetHeight();
		//for (int y = 0; y < height; ++y)
		//{
		//	for (int x = 0; x < width; ++x)
		//	{
		//		Color32 color = texture.GetColor(x, y);
		//		canvas->SetPixel(x, y, color);
		//	}
		//}
		//for (int y = 0; y < height * 4; ++y)
		//{
		//	for (int x = 0; x < width * 4; ++x)
		//	{
		//		Color32 color = texture.Sample((x - width * 2) * 1.0f / width, (y - height * 2) * 1.0f / height, Texture::Mirror);
		//		canvas->SetPixel(x, y, color);
		//	}
		//}

		//for (int i = 0; i < 512; ++i)
		//{
		//	float p = Mathf::PingPong(i * 0.1, 10);
		//	canvas->SetPixel(i, Mathf::RoundToInt(p * 10), Color32(0xffff0000));
		//}

		Matrix4x4 trans(position, Quaternion(Vector3(r, r, 0)), scale);

		for (int i = 0; i < (int)meshes.size(); ++i)
		{
			//Rasterizer::DrawMeshPoint(meshes[i], trans, Color32(Color::green));
			Rasterizer::DrawMesh(meshes[i], trans, Color32::white);
			//Rasterizer::DrawMeshWireFrame(meshes[i], trans, Color32(0x209999ff));
		}

		//Rasterizer::DrawLine(300, 210, 10, 100, Color32::white);
		//Rasterizer::DrawLine(400, 220, 10, 100, Color32(0x30ff00ff));
		//Rasterizer::DrawLine(400, 220, 100, 100, Color32(0x60ffff00));
		//Rasterizer::DrawLine(220, 399, -100, 400, Color32::white);
		//Rasterizer::DrawLine(270, 270, 150, 400, Color32::white);

		//Rasterizer::DrawSmoothLine(100, 10, 10, 100, Color32::white);
		//Rasterizer::DrawSmoothLine(200, 20, 10, 100, Color32(0x30ff00ff));
		//Rasterizer::DrawSmoothLine(200, 20, 100, 100, Color32(0x60ffff00));
		//Rasterizer::DrawSmoothLine(20, 199, -100, 400, Color32::white);
		//Rasterizer::DrawSmoothLine(70, 70, 150, 400, Color32::white);

		//Rasterizer::DrawTriangle(Vector2(0, 0), Vector2(100, 0), Vector2(0, 100), Color32(0xffff88ff));
		//Rasterizer::DrawTriangle(Vector2(100, 0), Vector2(100, 100), Vector2(0, 100), Color32(0xffffff88));

		canvas->Present();
		clock_t endTime = clock();
		printf("%ld\n", endTime - startTime);
	}

}