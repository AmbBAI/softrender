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
	Vector2 texCoord;
};

struct VaryingData
{
	Vector4 position;
	Vector2 texCoord;
};

struct PlaneShader : shader::Shader<Vertex, VaryingData>
{
	TexturePtr mainTex;

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.texCoord = input.texCoord;
		return output;
	}

	Color frag(const VaryingData& input) override
	{
		return mainTex->Sample(input.texCoord.x, input.texCoord.y);
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static MeshPtr mesh;
	static Transform trans;
	static std::vector<Vertex> vertices;
	static std::vector<u16> indices;
	static MaterialPtr mat;
	static PlaneShader fx;
	Canvas* canvas = app->GetCanvas();

	if (!isInitilized)
    {
		isInitilized = true;

		Rasterizer::Initialize();
        Rasterizer::canvas = canvas;
		//Rasterizer::camera = std::make_shared<Camera>();
		Rasterizer::camera = CameraPtr(new Camera());
		CameraController::InitCamera(Rasterizer::camera);

		mat = MaterialPtr(new Material());
		mat->diffuseTexture = Texture::LoadTexture("resources/teapot/default.png");
		fx.mainTex = mat->diffuseTexture;
		fx.varyingDecl = {
			{ 0, VaryingDataDecl::VaryingDataDeclType_POSITION, VaryingDataDecl::VaryingDataDeclFormat_Vector4 },
			{ 0, VaryingDataDecl::VaryingDataDeclType_TEXCOORD, VaryingDataDecl::VaryingDataDeclFormat_Vector2 }
		};

		mesh = CreatePlane();

		vertices.clear();
		indices.clear();
		int vertexCount = mesh->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
		{
			vertices.emplace_back(Vertex{
				mesh->vertices[i],
				mesh->texcoords[i] });
		}
		for (auto idx : mesh->indices) indices.emplace_back((u16)idx);
    }

	CameraController::UpdateCamera();

	canvas->Clear();

	Rasterizer::PrepareRender();

	Rasterizer::transform = trans.GetMatrix();
	Rasterizer::renderData.AssignVertexBuffer(vertices);
	Rasterizer::renderData.AssignIndexBuffer(indices);
	Rasterizer::SetShader(&fx);
	Rasterizer::Submit();

	Rasterizer::Render();

    canvas->Present();
}
