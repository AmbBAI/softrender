#include "rasterizer.h"
#include "transform_controller.hpp"
#include "object_utilities.h"
using namespace rasterizer;

Application* app;

void MainLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("light", 800, 600);
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}

struct Vertex
{
	Vector3 position;
	Vector3 normal;
};

struct VaryingData
{
	Vector4 position;
	Vector3 normal;
	Vector3 worldPos;

	static std::vector<VaryingDataLayout> GetLayout()
	{
		static std::vector<VaryingDataLayout> layout = {
			{ 0, VaryingDataDeclUsage_SVPOSITION, VaryingDataDeclFormat_Vector4 },
			{ 16, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 28, VaryingDataDeclUsage_POSITION, VaryingDataDeclFormat_Vector3 }
		};

		return layout;
	}
};

struct TestShader : Shader<Vertex, VaryingData>
{
	Vector3 lightDir = Vector3(1.f, 1.f, 1.f).Normalize();

	void Init()
	{
		varyingDataDecl = VaryingData::GetLayout();
		varyingDataSize = sizeof(VaryingData);
	}

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.normal = _Object2World.MultiplyVector(input.normal).Normalize();
		output.worldPos = _Object2World.MultiplyPoint3x4(input.position);
		return output;
	}

	Color frag(const VaryingData& input) override
	{
		LightInput lightInput;
		lightInput.ambient = Color(1.f, 0.18f, 0.04f, 0.18f);
		lightInput.diffuse = Color(1.f, 0.9f, 0.2f, 0.9f);
		lightInput.specular = Color::white;
		lightInput.shininess = 10.f;

		Color fragColor;

		Vector3 lightDir;
		ColorRGB lightColor;
		float lightAtten;
		if (InitLightArgs(input.worldPos, lightDir, lightColor, lightAtten))
		{
			Vector3 viewDir = (_WorldSpaceCameraPos - input.worldPos).Normalize();
			fragColor.rgb = ShaderF::LightingPhong(lightInput, input.normal, lightDir, lightColor, viewDir, 1.f);
		}
		else
		{
			fragColor = Color(1.f, 0.f, 1.f, 1.f);
			fragColor.rgb += lightInput.ambient.rgb;
			fragColor.rgb += lightInput.diffuse.rgb;
		}

		//fragColor.rgb = (input.normal + Vector3::one) / 2.f;
		return fragColor;
	}
};


void MainLoop()
{
	static bool isInitilized = false;
	static Transform objectTrans;
	static Transform cameraTrans;
	static TransformController objectCtrl;
	static std::vector<Vertex> vertices;
	static std::vector<uint16_t> indices;
	static MaterialPtr material;
	static TestShader shader;
	Canvas* canvas = app->GetCanvas();

	if (!isInitilized)
    {
		isInitilized = true;

		Rasterizer::Initialize();
        Rasterizer::canvas = canvas;

		auto camera = CameraPtr(new Camera());
		camera->SetPerspective(60.f, 1.33333f, 0.3f, 2000.f);
		cameraTrans.position = Vector3(0.f, 0.f, 2.f);
		camera->SetLookAt(cameraTrans);
		Rasterizer::camera = camera;

		auto light = LightPtr(new Light());
		light->type = Light::LightType_Point;
		light->position = Vector3(0.f, 0.5f, 0.f);
		light->range = 1.f;
		light->atten0 = 0.1f;
		light->atten1 = 5.0f;
		light->atten2 = 2.0f;
		light->theta = 30.f;
		light->phi = 45.f;
		light->direction = Vector3(0.f, -1.f, 0.f).Normalize();
		light->Initilize();
		Rasterizer::light = light;

		material = MaterialPtr(new Material());
		material->diffuseTexture = Texture::LoadTexture("resources/cube/default.png");
		material->diffuseTexture->GenerateMipmaps();

		shader.Init();

		std::vector<MeshPtr> meshes;
		//Mesh::LoadMesh(meshes, "resources/knot.obj");
		meshes.push_back(CreatePlane());
		objectTrans.position.y = -0.5f;

		vertices.clear();
		indices.clear();
		for (int meshID = 0; meshID < (int)meshes.size(); ++meshID)
		{
			auto& mesh = meshes[meshID];
			int meshOffset = (int)vertices.size();
			int vertexCount = mesh->GetVertexCount();
			int indexCount = mesh->indices.size();
			for (int i = 0; i < vertexCount; ++i)
			{
				vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->normals[i] });
			}
			for (int i = 0; i < indexCount; ++i)
			{
				indices.emplace_back(mesh->indices[i] + meshOffset);
			}
		}
    }

	canvas->Clear();

	objectCtrl.MouseRotate(objectTrans, false);
	Rasterizer::transform = objectTrans.GetMatrix();
	Rasterizer::renderData.AssignVertexBuffer(vertices);
	Rasterizer::renderData.AssignIndexBuffer(indices);
	Rasterizer::SetShader(&shader);
	Rasterizer::Submit();

    canvas->Present();
}
