#include "rasterizer.h"
#include "transform_controller.hpp"
#include "object_utilities.h"
using namespace rasterizer;

Application* app;

void MainLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("cubemap", 800, 600);
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}

struct Vertex
{
	Vector3 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct VaryingData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	Vector3 worldPos;

	static std::vector<VaryingDataLayout> GetLayout()
	{
		static std::vector<VaryingDataLayout> layout = {
			{ 0, VaryingDataDeclUsage_SVPOSITION, VaryingDataDeclFormat_Vector4 },
			{ 16, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector2 },
			{ 24, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 36, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 }
		};

		return layout;
	}
};

struct PlaneShader : Shader<Vertex, VaryingData>
{
	Vector3 lightDir = Vector3(1.f, 1.f, 1.f).Normalize();

	TexturePtr mainTex;
	TextureCUBEPtr cubeMap;
	Vector2 ddx, ddy;

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.texcoord = input.texcoord;
		output.normal = _Object2World.MultiplyVector(input.normal);
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
		Color color = Tex2D(mainTex, input.texcoord, ddx, ddy);
		float ndotl = Mathf::Max(0.f, input.normal.Dot(lightDir));

		Vector3 viewDir = (_WorldSpaceCameraPos - input.worldPos).Normalize();
		Vector3 reflectDir = Reflect(-viewDir, input.normal).Normalize();
		Color reflectCol = cubeMap->Sample(reflectDir);

		color.rgb = color.rgb * ndotl + reflectCol.rgb * 0.2f;
		return color;
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static Transform objectTrans;
	static Transform cameraTrans;
	static TransformController transCtrl;
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
		camera->SetPerspective(60.f, 1.33333f, 0.3f, 2000.f);
		cameraTrans.position = Vector3(0.f, 0.f, 2.f);
		camera->SetLookAt(cameraTrans);
		Rasterizer::camera = camera;

		material = MaterialPtr(new Material());
		material->diffuseTexture = Texture::LoadTexture("resources/teapot/default.png");
		material->diffuseTexture->GenerateMipmaps();

		shader.mainTex = material->diffuseTexture;
		shader.cubeMap = TextureCUBEPtr(new TextureCUBE());
		for (int i = 0; i < 6; ++i)
		{
			char path[32];
			sprintf(path, "resources/cubemap/%d.png", i);
			shader.cubeMap->cubeMap[i] = Texture::LoadTexture(path);
			shader.cubeMap->cubeMap[i]->xAddressMode = Texture::AddressMode_Clamp;
			shader.cubeMap->cubeMap[i]->yAddressMode = Texture::AddressMode_Clamp;
		}
		shader.varyingDataDecl = VaryingData::GetLayout();
		shader.varyingDataSize = sizeof(VaryingData);
		assert(shader.varyingDataSize == 48);

		std::vector<MeshPtr> meshes;
		Mesh::LoadMesh(meshes, "resources/cubemap/sphere.obj");
		MeshPtr mesh = meshes[0];
		vertices.clear();
		indices.clear();
		int vertexCount = mesh->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
		{
			vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->texcoords[i], mesh->normals[i] });
		}
		for (auto idx : mesh->indices) indices.emplace_back((u16)idx);

		Rasterizer::transform = objectTrans.GetMatrix();
    }

	if (transCtrl.MouseRotate(cameraTrans)
		|| transCtrl.KeyMove(cameraTrans, 1.f))
	{
		Rasterizer::camera->SetLookAt(cameraTrans);
	}

	canvas->Clear();

	Rasterizer::renderData.AssignVertexBuffer(vertices);
	Rasterizer::renderData.AssignIndexBuffer(indices);
	Rasterizer::SetShader(&shader);
	Rasterizer::Submit();

    canvas->Present();
}
