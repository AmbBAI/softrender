#include "rasterizer.h"
#include "utilities/camera_controller.h"
#include "utilities/object_utilities.h"
#include "base/ui.h"
using namespace rasterizer;

Application* app;

void MainLoop();
void TestTextureLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("rasterizer", 800, 600);
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
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

		//CameraController::moveScale = 0.5f;
		////MaterialPtr material = std::make_shared<Material>();
		//MaterialPtr material = MaterialPtr(new Material());
		//material->diffuseTexture = Texture::LoadTexture("resources/crytek-sponza/textures/gi_flag.tga");
		//mesh.push_back(CreatePlane(material));
		//trans.position = Vector3(0, 0, -20);
		//trans.rotation = Vector3(90, 0, 0);
		//trans.scale = Vector3(10, 10, 10);

        LoadSponzaMesh(mesh, trans);
        
		//CameraController::moveScale = 0.5f;
		//Mesh::LoadMesh(mesh, "resources/cornell-box/CornellBox-Sphere.obj");
		//trans.position = Vector3(0, -1, -2);
		//trans.rotation = Vector3(0, 0, 0);
		//trans.scale = Vector3(1, 1, 1);
		//Rasterizer::light = LightPtr(new Light());
		//Rasterizer::light->type = Light::LightType_Point;
		//Rasterizer::light->color = Color::white;
		////Rasterizer::light->position = Vector3(5.f, 0.f, -2.f);
		//Rasterizer::light->position = Vector3(0.f, 5.f, -2.f);
		//Rasterizer::light->direction = Vector3(0.f, -1.f, 0.f);
		//Rasterizer::light->range = 10.f;
		//Rasterizer::light->atten0 = 2.f;
		//Rasterizer::light->atten1 = 2.f;
		//Rasterizer::light->atten2 = 1.f;
		//Rasterizer::light->Initilize();

		UI::SetFont("resources/DejaVuSans.ttf");
		UI::SetIconImage("resources/blender_icons16.png");
    }

	//Rasterizer::light->position -= Vector3(0.01f, 0, 0);
	//Rasterizer::light->position -= Vector3(0, 0.01f, 0);
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
//
//    UI::Begin();
//
//	char fs[512];
//	sprintf(fs, "%.3f", app->GetDeltaTime());
//
//	uiBeginLayout();
//
//	int root = UI::Root();
//	int label = UI::Label(root, -1, fs, Color::red, UI_HFILL | UI_TOP);
//
//	int panel = UI::Panel(root, UI_TOP | UI_RIGHT, 200, 0);
//	uiSetBox(panel, UI_COLUMN);
//
//	int box = UI::Box(panel, UI_ROW, UI_HFILL | UI_TOP);
//	static int menu_status = -1;
//	int menu1 = UI::Radio(box, BND_ICON_MESH_MONKEY, NULL, &menu_status);
//	int menu2 = UI::Radio(box, BND_ICON_CAMERA_DATA, NULL, &menu_status);
//	int menu3 = UI::Radio(box, BND_ICON_TEXTURE, NULL, &menu_status);
//	int menu4 = UI::Radio(box, BND_ICON_MATERIAL, NULL, &menu_status);
//	int menu5 = UI::Radio(box, BND_ICON_LAMP, NULL, &menu_status);
//	int menu6 = UI::Radio(box, BND_ICON_TEXTURE_SHADED, NULL, &menu_status);
//
//	if (menu_status == menu1)
//	{
//		static int isMeshDrawPoint = (int)Rasterizer::isDrawPoint;
//		static int isMeshDrawWireframe = (int)Rasterizer::isDrawWireFrame;
//		static int isMeshDrawTextured = (int)Rasterizer::isDrawTextured;
//
//		int box1 = UI::Box(panel, UI_COLUMN, UI_HFILL | UI_TOP);
//		uiSetMargins(box1, 10, 10, 10, 10);
//		UI::Check(box1, "Point", &isMeshDrawPoint);
//		UI::Check(box1, "WireFrame", &isMeshDrawWireframe);
//		UI::Check(box1, "Textured", &isMeshDrawTextured);
//
//		Rasterizer::isDrawPoint = (isMeshDrawPoint != 0);
//		Rasterizer::isDrawWireFrame = (isMeshDrawWireframe != 0);
//		Rasterizer::isDrawTextured = (isMeshDrawTextured != 0);
//	}
//
//	uiEndLayout();
//
//	UI::DrawUI(root, BND_CORNER_TOP);
//	uiProcess((int)(glfwGetTime()*1000.0));
//    
//    UI::End();
}

void TestTextureLoop()
{
	static TexturePtr tex = nullptr;

	Canvas* canvas = app->GetCanvas();

	if (tex == nullptr)
	{
		tex = Texture::LoadTexture("resources/crytek-sponza/textures/background.tga");
		tex->filterMode = Texture::FilterMode_Bilinear;
		tex->xAddressMode = Texture::AddressMode_Clamp;
		tex->yAddressMode = Texture::AddressMode_Clamp;
		tex->CompressTexture();
//		tex->ConvertBumpToNormal(10);
//		tex->GenerateMipmaps();
	}

	canvas->Clear();

	TestTexture(canvas, Vector4(0, 0, 512, 512), *tex, 0);
	//TestTexture(canvas, Vector4(256, 256, 512, 512), *tex, 1);
	canvas->Present();
}
