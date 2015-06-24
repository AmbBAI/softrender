#include "rasterizer.h"
#include "camera_controller.h"
#include "object_utilities.h"
using namespace rasterizer;

Application* app;

void MainLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("plane", 800, 600);
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

struct PlaneShader : Shader<Vertex, VaryingData>
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
	static MaterialPtr material;
	static PlaneShader shader;
	Canvas* canvas = app->GetCanvas();

	if (!isInitilized)
    {
		isInitilized = true;

		Rasterizer::Initialize();
        Rasterizer::canvas = canvas;
		//Rasterizer::camera = std::make_shared<Camera>();
		Rasterizer::camera = CameraPtr(new Camera());
		CameraController::InitCamera(Rasterizer::camera);

		material = MaterialPtr(new Material());
		material->diffuseTexture = Texture::LoadTexture("resources/teapot/default.png");
		shader.mainTex = material->diffuseTexture;
		shader.varyingDataDecl = {
			{ 0, VaryingDataDeclUsage_POSITION, VaryingDataDeclFormat_Vector4 },
			{ 16, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector2 }
		};
		shader.varyingDataSize = sizeof(VaryingData);

		mesh = CreatePlane();

		vertices.clear();
		indices.clear();
		int vertexCount = mesh->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
		{
			vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->texcoords[i] });
		}
		for (auto idx : mesh->indices) indices.emplace_back((u16)idx);
    }

	CameraController::UpdateCamera();

	canvas->Clear();

	Rasterizer::PrepareRender();

	Rasterizer::transform = trans.GetMatrix();
	Rasterizer::renderData.AssignVertexBuffer(vertices);
	Rasterizer::renderData.AssignIndexBuffer(indices);
	Rasterizer::SetShader(&shader);
	Rasterizer::Submit();

	Rasterizer::Render();

    canvas->Present();
}
