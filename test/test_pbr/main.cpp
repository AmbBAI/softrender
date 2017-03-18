#include "softrender.h"
#include "transform_controller.hpp"
#include "object_utilities.h"
using namespace sr;

Application* app;

void MainLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("pbr", 800, 800);
	SoftRender::Initialize(800, 800);
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}

struct Vertex
{
	Vector3 position;
	Vector2 texcoord;
	Vector3 normal;
	Vector4 tangent;
};

struct VaryingData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	Vector3 tangent;
	Vector3 bitangent;
	Vector3 worldPos;

	static std::vector<VaryingDataElement> GetDecl()
	{
		static std::vector<VaryingDataElement> decl = {
			{ 0, VaryingDataDeclUsage_SVPOSITION, VaryingDataDeclFormat_Vector4 },
			{ 16, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector2 },
			{ 24, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 36, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 48, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 60, VaryingDataDeclUsage_POSITION, VaryingDataDeclFormat_Vector3 }
		};

		return decl;
	}
};

struct MainShader : Shader<Vertex, VaryingData>
{
	float roughness = 0.5f;
	float specular = 0.5f;

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.texcoord = input.texcoord;
		output.normal = _Object2World.MultiplyVector(input.normal).Normalize();
		output.tangent = _Object2World.MultiplyVector(input.tangent.xyz);
		output.bitangent = output.normal.Cross(output.tangent) * input.tangent.w;
		output.worldPos = _Object2World.MultiplyPoint3x4(input.position);
		return output;
	}

	Color frag(const VaryingData& input) override
	{
		PBSInput pbsInput;
		pbsInput.albedo = Color::red.rgb;
		pbsInput.normal = input.normal;
		pbsInput.roughness = roughness;
		pbsInput.specular = specular;

		Vector3 lightDir;
		float lightAtten;
		InitLightArgs(input.worldPos, lightDir, lightAtten);

		Color fragColor;
		Vector3 viewDir = (_WorldSpaceCameraPos - input.worldPos).Normalize();

		fragColor.rgb = PBSF::BRDF(pbsInput, lightDir, viewDir);
		return fragColor;
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static Transform objectTrans;
	static Transform cameraTrans;
	static TransformController objectCtrl;
	static MeshWrapper<Vertex> objectMesh;
	static std::shared_ptr<MainShader> shader;
	static LightPtr light = std::make_shared<Light>();

	if (!isInitilized)
    {
		isInitilized = true;

		auto camera = CameraPtr(new Camera());
		camera->SetPerspective(30.f, 1.f, 0.3f, 2000.f);
		//camera->SetOrthographic(-3.f, 3.f, -3.f, 3.f, 0.f, 20.f);
		camera->transform.position = Vector3(0.f, 0.f, -20.f);
		SoftRender::camera = camera;

		light->type = Light::LightType_Directional;
		light->transform.position = Vector3(0, 0, 0);
		light->transform.rotation = Quaternion(Vector3(45.f, -45.f, 0.f));
		light->Initilize();
		SoftRender::light = light;

		shader = std::make_shared<MainShader>();

		std::vector<MeshPtr> meshes;
		Mesh::LoadMesh(meshes, "resources/cubemap/sphere.obj");
		auto mesh = meshes[0];
		objectMesh.vertices.clear();
		objectMesh.indices.clear();
		int vertexCount = mesh->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
			objectMesh.vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->texcoords[i], mesh->normals[i], mesh->tangents[i] });
		for (auto idx : mesh->indices) objectMesh.indices.emplace_back((uint16_t)idx);

		SoftRender::renderData.AssignVertexBuffer(objectMesh.vertices);
		SoftRender::renderData.AssignIndexBuffer(objectMesh.indices);
		objectTrans.scale = Vector3::one * 0.4f;
	}

	SoftRender::Clear(true, true, Color(1.f, 0.19f, 0.3f, 0.47f));

	objectCtrl.MouseRotate(objectTrans, false);

	for (int i = 0; i < 11; ++i)
	{
		for (int j = 0; j < 11; ++j)
		{
			objectTrans.position = Vector3(1.f * (i - 5), 1.f * (j - 5), 2.f);
			SoftRender::modelMatrix = objectTrans.localToWorldMatrix();
			shader->roughness = i * 0.1f;
			shader->specular = j * 0.1f;
			SoftRender::SetShader(shader);
			SoftRender::Submit();
		}
	}

    SoftRender::Present();
}
