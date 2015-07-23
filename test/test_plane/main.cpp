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

	static std::vector<VaryingDataElement> GetDecl()
	{
		static std::vector<VaryingDataElement> decl = {
			{ 0, VaryingDataDeclUsage_SVPOSITION, VaryingDataDeclFormat_Vector4 },
			{ 16, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector2 },
			{ 24, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 36, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 48, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 }
		};

		return decl;
	}
};

struct ObjShader : Shader<Vertex, VaryingData>
{
	Vector3 lightDir = Vector3(1.f, 1.f, 1.f).Normalize();

	TexturePtr mainTex;
	TexturePtr normalTex;
	Vector2 ddx, ddy;

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.texcoord = input.texcoord;
		output.normal = _Object2World.MultiplyVector(input.normal);
		output.tangent = _Object2World.MultiplyVector(input.tangent.xyz);
		output.bitangent = output.normal.Cross(output.tangent) * input.tangent.w;
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

		Matrix4x4 tbn = TangentSpaceRotation(input.tangent, input.bitangent, input.normal);
		Vector3 normal = UnpackNormal(Tex2D(normalTex, input.texcoord, ddx, ddy), tbn);
		
		color.rgb *= Mathf::Max(0.f, lightDir.Dot(normal));
		return color;
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static Transform objectTrans;
	static Transform cameraTrans;
	static TransformController objectCtrl;
	static MeshWrapper<Vertex> meshW;
	static std::shared_ptr<ObjShader> objectShader;
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

		MaterialPtr material = MaterialPtr(new Material());
		material->diffuseTexture = Texture::LoadTexture("resources/crytek-sponza/textures/spnza_bricks_a_diff.tga");
		material->diffuseTexture->GenerateMipmaps();
		material->normalTexture = Texture::LoadTexture("resources/crytek-sponza/textures/spnza_bricks_a_bump.png");
		material->normalTexture->ConvertBumpToNormal(8.f);

		objectShader = std::make_shared<ObjShader>();
		objectShader->mainTex = material->diffuseTexture;
		objectShader->normalTex = material->normalTexture;
		assert(objectShader->varyingDataSize == 60);

		MeshPtr mesh = CreatePlane();
		mesh->CalculateTangents();
		meshW.vertices.clear();
		meshW.indices.clear();
		int vertexCount = mesh->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
		{
			meshW.vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->texcoords[i], mesh->normals[i], mesh->tangents[i] });
		}
		for (auto idx : mesh->indices) meshW.indices.emplace_back((uint16_t)idx);
    }

	canvas->Clear();

	objectCtrl.MouseRotate(objectTrans, false);
	Rasterizer::transform = objectTrans.GetMatrix();
	Rasterizer::renderData.AssignVertexBuffer(meshW.vertices);
	Rasterizer::renderData.AssignIndexBuffer(meshW.indices);
	Rasterizer::SetShader(objectShader);
	Rasterizer::Submit();

    canvas->Present();
}
