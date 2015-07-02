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
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}

template<typename Type>
struct MeshWrapper
{
	std::vector<Type> vertices;
	std::vector<u16> indices;
	std::vector<std::tuple<MaterialPtr, int, int> > materials;
};

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
	Vector2 texCoord;
	Vector3 normal;
	Vector4 tangent;
};

struct VaryingData
{
	Vector4 position;
	Vector2 texCoord;
	Vector3 normal;
	Vector3 tangent;
	Vector3 bitangent;
	Vector3 worldPos;

	static std::vector<VaryingDataLayout> GetLayout()
	{
		static std::vector<VaryingDataLayout> layout = {
			{ 0, VaryingDataDeclUsage_SVPOSITION, VaryingDataDeclFormat_Vector4 },
			{ 16, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector2 },
			{ 24, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 36, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 48, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 60, VaryingDataDeclUsage_POSITION, VaryingDataDeclFormat_Vector3 }
		};

		return layout;
	}
};

struct ShaderStand : Shader<Vertex, VaryingData>
{
	TexturePtr mainTex;
	TexturePtr normalTex;
	TexturePtr specularTex;
	float shininess = 10.f;
	Vector2 ddx, ddy;

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.texCoord = input.texCoord;
		output.normal = _Object2World.MultiplyVector(input.normal);
		output.tangent = _Object2World.MultiplyVector(input.tangent.xyz);
		output.bitangent = output.normal.Cross(output.tangent) * input.tangent.w;
		output.worldPos = _Object2World.MultiplyPoint3x4(input.position);
		return output;
	}

	void passQuad(const Quad<VaryingData*>& quad) override
	{
		ddx = quad[1]->texCoord - quad[0]->texCoord;
		ddy = quad[2]->texCoord - quad[0]->texCoord;
	}

	Color frag(const VaryingData& input) override
	{
		Color fragColor = Color::white;
		if (mainTex != nullptr) fragColor = Tex2D(mainTex, input.texCoord, ddx, ddy);

		Vector3 normal = input.normal;
		if (normalTex != nullptr)
		{
			Matrix4x4 tbn = TangentSpaceRotation(input.tangent, input.bitangent, input.normal);
			normal = UnpackNormal(Tex2D(normalTex, input.texCoord, ddx, ddy), tbn);
		}

		Vector3 lightDir;
		ColorRGB lightColor;
		float lightAtten;
		if (InitLightArgs(input.worldPos, lightDir, lightColor, lightAtten))
		{
			LightInput lightInput;
			lightInput.ambient = fragColor;
			lightInput.ambient.rgb *= 0.2f;
			lightInput.diffuse = fragColor;
			//lightInput.diffuse.rgb *= 0.9f;

			if (specularTex != nullptr)
			{
				Color specColor = Tex2D(specularTex, input.texCoord);
				lightInput.specular = specColor;
				lightInput.shininess = shininess;

				Vector3 viewDir = (_WorldSpaceCameraPos - input.worldPos).Normalize();
				fragColor.rgb = ShaderF::LightingPhong(lightInput, normal, lightDir, lightColor, viewDir, lightAtten);
			}
			else
			{
				fragColor.rgb = ShaderF::LightingLambert(lightInput, normal, lightDir, lightColor, lightAtten);
			}
		}
		return fragColor;
	}
};

struct ShaderWithAlphaTest : ShaderStand
{
	TexturePtr alphaMaskTex;

	Color frag(const VaryingData& input) override
	{
		if (alphaMaskTex != nullptr)
		{
			float alpha = Tex2D(alphaMaskTex, input.texCoord).r;
			if (Clip(alpha - 0.2f)) return Color::white;
		}

		return ShaderStand::frag(input);
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static std::vector<MeshWrapper<Vertex> > meshData;
	static ShaderWithAlphaTest shader;
	static Transform sceneTrans;
	static Transform cameraTrans;
	static TransformController transCtrl;
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

		LightPtr light = LightPtr(new Light());
		light->type = Light::LightType_Directional;
		light->position = Vector3(0, 300, 0);
		light->direction = Vector3(-1.f, -1.f, -1.f).Normalize();
		light->range = 1000.f;
		light->atten0 = 2.f;
		light->atten1 = 2.f;
		light->atten2 = 1.f;
		light->Initilize();
		Rasterizer::light = light;

		shader.varyingDataDecl = VaryingData::GetLayout();
		shader.varyingDataSize = sizeof(VaryingData);
		Rasterizer::SetShader(&shader);

		std::vector<MeshPtr> meshes;
        LoadSponzaMesh(meshes, sceneTrans);

		for (auto mesh : meshes)
		{
			MeshWrapper<Vertex> meshWapper;
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

		Rasterizer::transform = sceneTrans.GetMatrix();
    }

	if (transCtrl.MouseRotate(cameraTrans)
		|| transCtrl.KeyMove(cameraTrans))
	{
		Rasterizer::camera->SetLookAt(cameraTrans);
	}

	canvas->Clear();

	for (auto& meshWapper : meshData)
	{
		Rasterizer::renderData.AssignVertexBuffer(meshWapper.vertices);
		Rasterizer::renderData.AssignIndexBuffer(meshWapper.indices);

		for (auto& materialTuple : meshWapper.materials)
		{
			MaterialPtr material = std::get<0>(materialTuple);
			int startIndex = std::get<1>(materialTuple);
			int primitiveCount = std::get<2>(materialTuple);

			shader.mainTex = material->diffuseTexture;
			shader.normalTex = material->normalTexture;
			shader.specularTex = material->specularTexture;
			shader.shininess = shader.shininess;
			shader.alphaMaskTex = material->alphaMaskTexture;

			Rasterizer::Submit(startIndex, primitiveCount);
		}
	}

    canvas->Present();
}
