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

	static std::vector<VaryingDataLayout> GetLayout()
	{
		static std::vector<VaryingDataLayout> layout = {
			{ 0, VaryingDataDeclUsage_SVPOSITION, VaryingDataDeclFormat_Vector4 },
			{ 16, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector2 },
			{ 24, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 36, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 48, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 }
		};

		return layout;
	}
};

struct PlaneShader : Shader<Vertex, VaryingData>
{
	Vector3 lightDir = Vector3(1.f, 1.f, 1.f).Normalize();

	TexturePtr mainTex;
	TexturePtr normalTex;
	Vector2 ddx, ddy;

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.texCoord = input.texCoord;
		output.normal = _Object2World.MultiplyVector(input.normal);
		output.tangent = _Object2World.MultiplyVector(input.tangent.xyz);
		output.bitangent = output.normal.Cross(output.tangent) * input.tangent.w;
		return output;
	}

	void passQuad(const Quad<VaryingData*>& quad) override
	{
		ddx = quad[1]->texCoord - quad[0]->texCoord;
		ddy = quad[2]->texCoord - quad[0]->texCoord;
	}

	Color frag(const VaryingData& input) override
	{
		//Color color = Tex2D(mainTex, input.texCoord, ddx, ddy);

		Matrix4x4 tbn = GetTangentSpace(input.tangent, input.bitangent, input.normal);
		Vector3 normal = UnpackNormal(Tex2D(normalTex, input.texCoord, ddx, ddy), tbn);
		
		Color color;
		color.rgb = ColorRGB((normal.x + 1.f) / 2.f, (normal.y + 1.f) / 2.f, (normal.z + 1.f) / 2.f);
		return color;
		//color.rgb *= Mathf::Max(0.f, lightDir.Dot(normal));
		//return color;
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
		camera->SetPerspective(60.f, 1.33333f, 0.3f, 2000.f);
		cameraTrans.position = Vector3(0.f, 0.f, 2.f);
		camera->SetLookAt(cameraTrans);
		Rasterizer::camera = camera;

		material = MaterialPtr(new Material());
		material->diffuseTexture = Texture::LoadTexture("resources/crytek-sponza/textures/spnza_bricks_a_diff.tga");
		material->diffuseTexture->GenerateMipmaps();
		material->normalTexture = Texture::LoadTexture("resources/crytek-sponza/textures/spnza_bricks_a_bump.png");
		material->normalTexture->ConvertBumpToNormal(8.f);

		shader.mainTex = material->diffuseTexture;
		shader.normalTex = material->normalTexture;
		shader.varyingDataDecl = VaryingData::GetLayout();
		shader.varyingDataSize = sizeof(VaryingData);
		assert(shader.varyingDataSize == 60);

		//objectTrans.scale = Vector3(0.01f, 0.01f, 0.01f);

		std::vector<MeshPtr> meshes;
		Mesh::LoadMesh(meshes, "resources/cube/cube.obj");
		//Mesh::LoadMesh(meshes, "resources/teapot/teapot.obj");

		vertices.clear();
		indices.clear();
		for (int meshID = 0; meshID < (int)meshes.size(); ++meshID)
		{
			auto& mesh = meshes[meshID];
			mesh->RecalculateNormals();
			int meshOffset = (int)vertices.size();
			int vertexCount = mesh->GetVertexCount();
			int indexCount = mesh->indices.size();
			for (int i = 0; i < vertexCount; ++i)
			{
				vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->texcoords[i], mesh->normals[i], mesh->tangents[i] });
			}
			for (int i = 0; i < indexCount; ++i)
			{
				indices.emplace_back(mesh->indices[i] + meshOffset);
			}
		}
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
