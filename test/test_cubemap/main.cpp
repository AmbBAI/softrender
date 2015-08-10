#include "softrender.h"
#include "transform_controller.hpp"
#include "object_utilities.h"
using namespace sr;

Application* app;

void MainLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("cubemap", 800, 600);
	SoftRender::Initialize(800, 600);
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

	static std::vector<VaryingDataElement> GetDecl()
	{
		static std::vector<VaryingDataElement> decl = {
			{ 0, VaryingDataDeclUsage_SVPOSITION, VaryingDataDeclFormat_Vector4 },
			{ 16, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector2 },
			{ 24, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 },
			{ 36, VaryingDataDeclUsage_TEXCOORD, VaryingDataDeclFormat_Vector3 }
		};

		return decl;
	}
};

struct SkyShader : Shader<Vertex, VaryingData>
{
	CubemapPtr cubeMap;

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.normal = _Object2World.MultiplyVector(input.normal);
		output.worldPos = _Object2World.MultiplyPoint3x4(input.position);
		return output;
	}

	Color frag(const VaryingData& input) override
	{
		return TexCUBE(cubeMap, (input.worldPos - _WorldSpaceCameraPos).Normalize());
	}
};

struct ObjShader : Shader<Vertex, VaryingData>
{
	Vector3 lightDir = Vector3(1.f, 1.f, -1.f).Normalize();

	Texture2DPtr mainTex;
	CubemapPtr cubeMap;
	float reflection = 0.2f;
	float fresnelPow = 0.4f;
	float fresnelFalloff = 0.6f;

	Vector2 ddx, ddy;

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.texcoord = input.texcoord * Vector2(5.f, 1.f);
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
		Color reflectCol = TexCUBE(cubeMap, reflectDir);
		float fresnel = Mathf::Pow(1.f - Mathf::Clamp01(Mathf::Abs(Vector3::Dot(viewDir, input.normal))), fresnelFalloff) * fresnelPow;
		reflectCol *= Mathf::Clamp01(fresnel + reflection);

		color.rgb = color.rgb * ndotl + reflectCol.rgb + reflectCol.rgb * fresnel;
		return color;
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static TransformController transCtrl;
	static CameraPtr camera;
	static Transform objTrans;
	static Transform skyTrans;
	static MeshWrapper<Vertex> objMesh;
	static MeshWrapper<Vertex> skyMesh;
	static std::shared_ptr<ObjShader> objShader;
	static std::shared_ptr<SkyShader> skyShader;

	if (!isInitilized)
    {
		isInitilized = true;

		camera = CameraPtr(new Camera());
		camera->SetPerspective(60.f, 1.33333f, 0.3f, 2000.f);
		camera->transform.position = Vector3(0.f, 0.f, -3.f);
		SoftRender::camera = camera;

		MaterialPtr material = MaterialPtr(new Material());
		material->diffuseTexture = Texture2D::LoadTexture("resources/teapot/default.png");
		material->diffuseTexture->GenerateMipmaps();

		objShader = std::make_shared<ObjShader>();
		objShader->mainTex = material->diffuseTexture;
		objShader->cubeMap = CubemapPtr(new Cubemap());
		for (int i = 0; i < 6; ++i)
		{
			char path[32];
			sprintf(path, "resources/cubemap/%d.png", i);
			Texture2DPtr tex = Texture2D::LoadTexture(path);
			tex->xAddressMode = Texture2D::AddressMode_Clamp;
			tex->yAddressMode = Texture2D::AddressMode_Clamp;
			objShader->cubeMap->SetTexture((Cubemap::CubemapFace)i, tex);
		}
		assert(objShader->varyingDataSize == 48);
		skyShader = std::make_shared<SkyShader>();
		skyShader->cubeMap = objShader->cubeMap;

		std::vector<MeshPtr> meshes;
		Mesh::LoadMesh(meshes, "resources/knot.obj");
		objMesh.vertices.clear();
		objMesh.indices.clear();
		int vertexCount = meshes[0]->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
		{
			objMesh.vertices.emplace_back(Vertex{ meshes[0]->vertices[i], meshes[0]->texcoords[i], meshes[0]->normals[i] });
		}
		for (auto idx : meshes[0]->indices) objMesh.indices.emplace_back(idx);

		skyTrans.scale = Vector3::one * 1000.f;

		meshes.clear();
		Mesh::LoadMesh(meshes, "resources/cubemap/sphere.obj");
		skyMesh.vertices.clear();
		skyMesh.indices.clear();
		vertexCount = meshes[0]->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
		{
			skyMesh.vertices.emplace_back(Vertex{ meshes[0]->vertices[i], meshes[0]->texcoords[i], meshes[0]->normals[i] });
		}
		for (auto idx : meshes[0]->indices) skyMesh.indices.emplace_back(idx);

    }

	transCtrl.MouseRotateAround(camera->transform, objTrans.position);
	objTrans.Rotate(Vector3(0.f, 2.f, 0.f));

	SoftRender::Clear(true, true, Color(1.f, 0.19f, 0.3f, 0.47f));

	SoftRender::modelMatrix = objTrans.localToWorldMatrix();
	SoftRender::SetShader(objShader);
	SoftRender::renderData.AssignVertexBuffer(objMesh.vertices);
	SoftRender::renderData.AssignIndexBuffer(objMesh.indices);
	SoftRender::renderState.cull = RenderState::CullType_Back;
	SoftRender::Submit();

	SoftRender::modelMatrix = skyTrans.localToWorldMatrix();
	SoftRender::SetShader(skyShader);
	SoftRender::renderData.AssignVertexBuffer(skyMesh.vertices);
	SoftRender::renderData.AssignIndexBuffer(skyMesh.indices);
	SoftRender::renderState.cull = RenderState::CullType_Front;
	SoftRender::Submit();

	SoftRender::Present();
}
