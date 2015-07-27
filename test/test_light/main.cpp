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

	static std::vector<VaryingDataElement> GetDecl()
	{
		static std::vector<VaryingDataElement> decl = {
			{ 0, VaryingDataDeclUsage_SVPOSITION, VaryingDataDeclFormat_Vector4 },
			{ 16, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 28, VaryingDataDeclUsage_POSITION, VaryingDataDeclFormat_Vector3 }
		};

		return decl;
	}
};

struct ForwardBaseShader : Shader<Vertex, VaryingData>
{
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
		lightInput.ambient = Color(1.f, 0.18f, 0.18f, 0.18f);
		lightInput.diffuse = Color(1.f, 0.9f, 0.9f, 0.9f);
		lightInput.specular = Color::white;
		lightInput.shininess = 10.f;

		Color fragColor;

		Vector3 lightDir;
		float lightAtten;
		InitLightArgs(input.worldPos, lightDir, lightAtten);

		Vector3 viewDir = (_WorldSpaceCameraPos - input.worldPos).Normalize();
		fragColor.rgb = ShaderF::LightingPhong(lightInput, input.normal, lightDir, _LightColor.xyz, viewDir, 1.f);

		return fragColor;
	}
};

struct ForwardAdditionShader : Shader<Vertex, VaryingData>
{
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
		lightInput.ambient = Color::black;
		lightInput.diffuse = Color(1.f, 0.9f, 0.9f, 0.9f);
		lightInput.specular = Color::white;
		lightInput.shininess = 10.f;

		Color fragColor;

		Vector3 lightDir;
		float lightAtten;
		InitLightArgs(input.worldPos, lightDir, lightAtten);

		Vector3 viewDir = (_WorldSpaceCameraPos - input.worldPos).Normalize();
		fragColor.rgb = ShaderF::LightingPhong(lightInput, input.normal, lightDir, _LightColor.xyz, viewDir, 1.f);
		return fragColor;
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static Transform objectTrans;
	static Transform cameraTrans;
	static TransformController objectCtrl;
	static MeshWrapper<Vertex> meshW;
	static ShaderPtr forwardBaseShader;
	static ShaderPtr forwardAdditionShader;
	static LightPtr lightRed;
	static LightPtr lightBlue;
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

		lightRed = LightPtr(new Light());
		lightRed->type = Light::LightType_Point;
		lightRed->color = Color::red;
		lightRed->position = Vector3(-0.5f, 0.5f, 0.f);
		lightRed->range = 2.f;
		lightRed->atten0 = 0.1f;
		lightRed->atten1 = 5.0f;
		lightRed->atten2 = 2.0f;
		lightRed->theta = 30.f;
		lightRed->phi = 45.f;
		lightRed->direction = Vector3(0.f, -1.f, 0.f).Normalize();
		lightRed->Initilize();

		lightBlue = LightPtr(new Light());
		lightBlue->type = Light::LightType_Point;
		lightBlue->color = Color::blue;
		lightBlue->position = Vector3(0.5f, 0.5f, 0.f);
		lightBlue->range = 2.f;
		lightBlue->atten0 = 0.1f;
		lightBlue->atten1 = 5.0f;
		lightBlue->atten2 = 2.0f;
		lightBlue->theta = 30.f;
		lightBlue->phi = 45.f;
		lightBlue->direction = Vector3(0.f, -1.f, 0.f).Normalize();
		lightBlue->Initilize();

		MaterialPtr material = MaterialPtr(new Material());
		material->diffuseTexture = Texture2D::LoadTexture("resources/cube/default.png");
		material->diffuseTexture->GenerateMipmaps();

		auto shader0 = std::make_shared<ForwardBaseShader>();
		forwardBaseShader = shader0;
		auto shader1 = std::make_shared<ForwardAdditionShader>();
		forwardAdditionShader = shader1;

		std::vector<MeshPtr> meshes;
		//Mesh::LoadMesh(meshes, "resources/knot.obj");
		meshes.push_back(CreatePlane());
		objectTrans.position.y = -0.5f;

		meshW.vertices.clear();
		meshW.indices.clear();
		for (int meshID = 0; meshID < (int)meshes.size(); ++meshID)
		{
			auto& mesh = meshes[meshID];
			int meshOffset = (int)meshW.vertices.size();
			int vertexCount = mesh->GetVertexCount();
			int indexCount = mesh->indices.size();
			for (int i = 0; i < vertexCount; ++i)
			{
				meshW.vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->normals[i] });
			}
			for (int i = 0; i < indexCount; ++i)
			{
				meshW.indices.emplace_back(mesh->indices[i] + meshOffset);
			}
		}
    }

	canvas->Clear();

	objectCtrl.MouseRotate(objectTrans, false);
	Rasterizer::modelMatrix = objectTrans.GetMatrix();
	Rasterizer::renderData.AssignVertexBuffer(meshW.vertices);
	Rasterizer::renderData.AssignIndexBuffer(meshW.indices);

	Rasterizer::light = lightRed;
	Rasterizer::renderState.alphaBlend = false;
	Rasterizer::SetShader(forwardBaseShader);
	Rasterizer::Submit();

	Rasterizer::light = lightBlue;
	Rasterizer::renderState.alphaBlend = true;
	Rasterizer::renderState.srcFactor = RenderState::BlendFactor_One;
	Rasterizer::renderState.dstFactor = RenderState::BlendFactor_One;
	Rasterizer::SetShader(forwardAdditionShader);
	Rasterizer::Submit();

    canvas->Present();
}
