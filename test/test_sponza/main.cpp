#include "rasterizer.h"
#include "transform_controller.hpp"
#include "object_utilities.h"
using namespace rasterizer;

Application* app;

void MainLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("sponza", 800, 600);
	Rasterizer::Initialize(800, 600);
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}

void LoadSponzaMesh(std::vector<MeshPtr>& mesh, Transform& trans)
{
	Mesh::LoadMesh(mesh, "resources/crytek-sponza/sponza.obj");
	//Mesh::LoadMesh(mesh, "resources/teapot/teapot.obj");
	trans.position = Vector3(0, 0, 0);
	trans.rotation = Vector3(0, 0, 0);
	trans.scale = Vector3(1, 1, 1);

	for (auto& m : mesh)
	{
		if (m->normals.size() <= 0)
			m->RecalculateNormals();
	}
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

struct SceneShader : Shader<Vertex, VaryingData>
{
	Texture2DPtr mainTex;
	Texture2DPtr normalTex;
	Texture2DPtr specularTex;
	float shininess = 10.f;
	Texture2DPtr alphaMaskTex;
	Vector2 ddx, ddy;

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.texcoord = input.texcoord;
		output.normal = _Object2World.MultiplyVector(input.normal);
		output.tangent = _Object2World.MultiplyVector(input.tangent.xyz);
		output.bitangent = output.normal.Cross(output.tangent) * input.tangent.w;
		output.worldPos = _Object2World.MultiplyPoint3x4(input.position);
		return output;
	}

	void passQuad(const Quad<VaryingData*>& quad) override
	{
		ddx = quad[1]->texcoord - quad[0]->texcoord;
		ddy = quad[2]->texcoord - quad[0]->texcoord;
	}

	Color frag(const VaryingData& input) override
	{
		if (alphaMaskTex != nullptr)
		{
			float alpha = Tex2D(alphaMaskTex, input.texcoord).r;
			if (Clip(alpha - 0.2f)) return Color::white;
		}

		Color fragColor = Color::white;
		if (mainTex != nullptr) fragColor = Tex2D(mainTex, input.texcoord, ddx, ddy);

		Vector3 normal = input.normal;
		if (normalTex != nullptr)
		{
			Matrix4x4 tbn = TangentSpaceRotation(input.tangent, input.bitangent, input.normal);
			normal = UnpackNormal(Tex2D(normalTex, input.texcoord, ddx, ddy), tbn);
		}

		Vector3 lightDir;
		float lightAtten;
		InitLightArgs(input.worldPos, lightDir, lightAtten);

		LightInput lightInput;
		lightInput.ambient = fragColor;
		lightInput.ambient.rgb *= 0.2f;
		lightInput.diffuse = fragColor;
		//lightInput.diffuse.rgb *= 0.9f;

		if (specularTex != nullptr)
		{
			Color specColor = Tex2D(specularTex, input.texcoord);
			lightInput.specular = specColor;
			lightInput.shininess = shininess;

			Vector3 viewDir = (_WorldSpaceCameraPos - input.worldPos).Normalize();
			fragColor.rgb = ShaderF::LightingPhong(lightInput, normal, lightDir, _LightColor.xyz, viewDir, lightAtten);
		}
		else
		{
			fragColor.rgb = ShaderF::LightingLambert(lightInput, normal, lightDir, _LightColor.xyz, lightAtten);
		}

		return fragColor;
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static std::vector<MeshWrapper<Vertex> > meshData;
	static std::shared_ptr<SceneShader> sceneShader;
	static Transform sceneTrans;
	static Transform cameraTrans;
	static TransformController transCtrl;

	if (!isInitilized)
    {
		isInitilized = true;

		auto camera = CameraPtr(new Camera());
		camera->SetPerspective(60.f, 1.33333f, 0.3f, 4000.f);
		cameraTrans.position = Vector3(800.f, 400.f, 0.f);
		cameraTrans.rotation = Quaternion(Vector3(0.f, -90.f, 0.f));
		//camera->SetOrthographic(-800.f, 800.f, -600.f, 600.f, 0.3f, 4000.f);
		//cameraTrans.position = Vector3(0.f, 400.f, 0.f);
		//cameraTrans.rotation = Quaternion(Vector3(0.f, -90.f, 0.f));
		camera->SetLookAt(cameraTrans);
		Rasterizer::camera = camera;

		LightPtr light = LightPtr(new Light());
		light->type = Light::LightType_Directional;
		light->position = Vector3(0, 300, 0);
		light->direction = Vector3(-1.f, -1.f, 1.f).Normalize();
		light->range = 1000.f;
		light->atten0 = 2.f;
		light->atten1 = 2.f;
		light->atten2 = 1.f;
		light->Initilize();
		Rasterizer::light = light;

		sceneShader = std::make_shared<SceneShader>();
		Rasterizer::SetShader(sceneShader);

		std::vector<MeshPtr> meshes;
        LoadSponzaMesh(meshes, sceneTrans);

		for (auto mesh : meshes)
		{
			MeshWrapper<Vertex> meshWapper;
			meshWapper.name = mesh->name;

			for (int i = 0; i < (int)mesh->vertices.size(); ++i)
			{
				Vertex vertex = {
					mesh->vertices[i],
					mesh->texcoords[i],
					mesh->normals[i],
					mesh->tangents[i]
				};
				meshWapper.vertices.push_back(vertex);
			}

			for (int i = 0; i < (int)mesh->indices.size(); ++i)
			{
				meshWapper.indices.push_back(mesh->indices[i]);
			}

			meshWapper.materials = mesh->materials;

			meshData.push_back(meshWapper);
		}

		Rasterizer::modelMatrix = sceneTrans.GetMatrix();
    }

	static char title[32];
	sprintf(title, "sponza - %f", app->GetDeltaTime());
	app->SetTitle(title);

	if (transCtrl.MouseRotate(cameraTrans)
		|| transCtrl.KeyMove(cameraTrans))
	{
		Rasterizer::camera->SetLookAt(cameraTrans);
	}

	Rasterizer::Clear(true, true, Color(1.f, 0.19f, 0.3f, 0.47f));

	for (auto& meshWapper : meshData)
	{
		Rasterizer::renderData.AssignVertexBuffer(meshWapper.vertices);
		Rasterizer::renderData.AssignIndexBuffer(meshWapper.indices);

		for (auto& materialTuple : meshWapper.materials)
		{
			MaterialPtr material = std::get<0>(materialTuple);
			int startIndex = std::get<1>(materialTuple);
			int primitiveCount = std::get<2>(materialTuple);

			sceneShader->mainTex = material->diffuseTexture;
			sceneShader->normalTex = material->normalTexture;
			sceneShader->specularTex = material->specularTexture;
			sceneShader->shininess = sceneShader->shininess;
			sceneShader->alphaMaskTex = material->alphaMaskTexture;

			Rasterizer::Submit(startIndex, primitiveCount);
		}
	}

    Rasterizer::Present();
	Rasterizer::GetRenderTarget()->GetDepthBuffer()->SaveToFile("depth.tiff");
	//Rasterizer::GetRenderTarget()->GetColorBuffer()->SaveToFile("color.png");
}
