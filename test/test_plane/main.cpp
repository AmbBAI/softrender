#include "rasterizer.h"
#include "transform_controller.hpp"
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
	Vector4 color;
	//Vector2 texCoord;
};

struct VaryingData
{
	Vector4 position;
	Vector4 color;
	//Vector2 texCoord;
};

struct PlaneShader : Shader<Vertex, VaryingData>
{
	TexturePtr mainTex;

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		//output.texCoord = input.texCoord;
		output.color = input.color;
		return output;
	}

	Color frag(const VaryingData& input) override
	{
		return Color(input.color);
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static Transform objectTrans;
	static Transform cameraTrans;
	static TransformController objectCtrl;
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

		auto camera = CameraPtr(new Camera());
		camera->SetPerspective(90.f, 1.33333f, 0.3f, 2000.f);
		cameraTrans.position = Vector3(0.f, 0.f, 5.f);
		camera->SetLookAt(cameraTrans);
		Rasterizer::camera = camera;

		material = MaterialPtr(new Material());
		material->diffuseTexture = Texture::LoadTexture("resources/teapot/default.png");
		
		shader.mainTex = material->diffuseTexture;
		shader.varyingDataDecl = {
			{ 0, VaryingDataDeclUsage_POSITION, VaryingDataDeclFormat_Vector4 },
			{ 16, VaryingDataDeclUsage_COLOR, VaryingDataDeclFormat_Vector4 }
		};
		shader.varyingDataSize = sizeof(VaryingData);

		MeshPtr mesh = CreatePlane();
		vertices.clear();
		indices.clear();
		int vertexCount = mesh->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
		{
			vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->colors[i] });
		}
		for (auto idx : mesh->indices) indices.emplace_back((u16)idx);
    }

	canvas->Clear();

	Rasterizer::PrepareRender();

	objectCtrl.MouseRotate(objectTrans, false);
	Rasterizer::transform = objectTrans.GetMatrix();
	Rasterizer::renderData.AssignVertexBuffer(vertices);
	Rasterizer::renderData.AssignIndexBuffer(indices);
	Rasterizer::SetShader(&shader);
	Rasterizer::Submit();

	Rasterizer::Render();

    canvas->Present();
}
