#include "softrender.h"
#include "transform_controller.hpp"
#include "object_utilities.h"
using namespace sr;

Application* app;

void MainLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("light", 800, 600);
	SoftRender::Initialize(800, 600);
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
		lightInput.ambient = Color::white * 0.18f;
		lightInput.diffuse = Color(1.f, 0.9f, 0.9f, 0.9f);
		lightInput.specular = Color::white;
		lightInput.shininess = 10.f;

		Color fragColor;

		Vector3 lightDir;
		float lightAtten;
		InitLightArgs(input.worldPos, lightDir, lightAtten);

		Vector3 viewDir = (_WorldSpaceCameraPos - input.worldPos).Normalize();
		fragColor.rgb = ShaderF::LightingPhong(lightInput, input.normal, lightDir, _LightColor.xyz, viewDir, lightAtten);

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
		fragColor.rgb = ShaderF::LightingPhong(lightInput, input.normal, lightDir, _LightColor.xyz, viewDir, lightAtten);
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

	if (!isInitilized)
    {
		isInitilized = true;

		auto camera = CameraPtr(new Camera());
		camera->SetPerspective(60.f, 1.33333f, 0.3f, 2000.f);
		camera->transform.position = Vector3(0.f, 0.f, -2.f);
		SoftRender::camera = camera;

		lightRed = LightPtr(new Light());
		lightRed->type = Light::LightType_Point;
		lightRed->color = Color::red;
		lightRed->transform.position = Vector3(-0.5f, 1.f, 0.f);
		lightRed->transform.rotation = Quaternion(Vector3(90.f,0.f, 0.f));
		lightRed->intensity = 2.f;
		lightRed->range = 5.f;
		lightRed->atten0 = 0.1f;
		lightRed->atten1 = 5.0f;
		lightRed->atten2 = 2.0f;
		lightRed->theta = 30.f;
		lightRed->phi = 45.f;
		lightRed->Initilize();

		lightBlue = LightPtr(new Light());
		lightBlue->type = Light::LightType_Point;
		lightBlue->color = Color::blue;
		lightBlue->transform.position = Vector3(0.5f, 1.f, 0.f);
		lightBlue->transform.rotation = Quaternion(Vector3(90.f, 0.f, 0.f));
		lightBlue->intensity = 3.f;
		lightBlue->range = 5.f;
		lightBlue->atten0 = 0.1f;
		lightBlue->atten1 = 5.0f;
		lightBlue->atten2 = 2.0f;
		lightBlue->theta = 30.f;
		lightBlue->phi = 45.f;
		lightBlue->Initilize();

		auto shader0 = std::make_shared<ForwardBaseShader>();
		forwardBaseShader = shader0;
		auto shader1 = std::make_shared<ForwardAdditionShader>();
		forwardAdditionShader = shader1;


		MeshPtr mesh = CreatePlane();
		mesh->CalculateTangents();
		meshW.vertices.clear();
		meshW.indices.clear();
		int vertexCount = mesh->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
		{
			meshW.vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->normals[i] });
		}
		for (auto idx : mesh->indices) meshW.indices.emplace_back((uint16_t)idx);
    }

	SoftRender::Clear(true, true, Color(1.f, 0.19f, 0.3f, 0.47f));

	objectCtrl.MouseRotate(objectTrans, false);
	SoftRender::modelMatrix = objectTrans.localToWorldMatrix();
	SoftRender::renderData.AssignVertexBuffer(meshW.vertices);
	SoftRender::renderData.AssignIndexBuffer(meshW.indices);

	SoftRender::light = lightRed;
	SoftRender::renderState.alphaBlend = false;
	SoftRender::SetShader(forwardBaseShader);
	SoftRender::Submit();

	SoftRender::light = lightBlue;
	SoftRender::renderState.alphaBlend = true;
	SoftRender::renderState.srcFactor = RenderState::BlendFactor_One;
	SoftRender::renderState.dstFactor = RenderState::BlendFactor_One;
	SoftRender::SetShader(forwardAdditionShader);
	SoftRender::Submit();

    SoftRender::Present();
}
