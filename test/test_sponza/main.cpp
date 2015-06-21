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

void MainLoop()
{
	static bool isInitilized = false;
	static std::vector<MeshPtr> mesh;
	static Transform trans;
	Canvas* canvas = app->GetCanvas();

	if (!isInitilized)
    {
		isInitilized = true;

		Rasterizer::Initialize();
        Rasterizer::canvas = canvas;
		//Rasterizer::camera = std::make_shared<Camera>();
		Rasterizer::camera = CameraPtr(new Camera());
		CameraController::InitCamera(Rasterizer::camera);

		//Rasterizer::light = std::make_shared<Light>();
		Rasterizer::light = LightPtr(new Light());
		Rasterizer::light->type = Light::LightType_Directional;
		Rasterizer::light->position = Vector3(0, 300, 0);
		Rasterizer::light->direction = Vector3(-1.f, -1.f, -1.f).Normalize();
		Rasterizer::light->range = 1000.f;
		Rasterizer::light->atten0 = 2.f;
		Rasterizer::light->atten1 = 2.f;
		Rasterizer::light->atten2 = 1.f;
		Rasterizer::light->Initilize();

        LoadSponzaMesh(mesh, trans);
    }

	CameraController::UpdateCamera();

	canvas->Clear();

	Matrix4x4 transM = trans.GetMatrix();

	Rasterizer::PrepareRender();

	for (auto& m : mesh)
	{
		Rasterizer::DrawMesh(*m, transM);
	}

	Rasterizer::Render();

    canvas->Present();
}
