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
	Rasterizer::Initialize(800, 600);
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

struct ObjShader : Shader<Vertex, VaryingData>
{
	Color ambientColor = Color(0.f, 0.1f, 0.1f, 0.1f);
	Texture2DPtr shadowMap = nullptr;

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
		Color color = Color::white;

		Vector3 lightDir;
		float lightAtten;
		InitLightArgs(input.worldPos, lightDir, lightAtten);

		color.rgb *= Mathf::Max(0.f, lightDir.Dot(input.normal)) * lightAtten;
		return color + ambientColor;
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static Transform objectTrans;
	static Transform planeTrans;
	static TransformController objectCtrl;
	static MeshWrapper<Vertex> objectMesh;
	static MeshWrapper<Vertex> planeMesh;
	static std::shared_ptr<ObjShader> objectShader;
	static LightPtr light;
	static CameraPtr camera;
	static RenderTexturePtr shadowMap;

	if (!isInitilized)
    {
		isInitilized = true;

		camera = CameraPtr(new Camera());
		camera->SetPerspective(60.f, 1.33333f, 0.3f, 20.f);
		camera->transform.position = Vector3(0.f, 0.f, -2.f);
		Rasterizer::camera = camera;

		light = LightPtr(new Light());
		light->type = Light::LightType_Directional;
		light->transform.position = Vector3(0, 0, 0);
		light->transform.rotation = Quaternion(Vector3(30.f, -45.f, 0.f));
		light->Initilize();
		Rasterizer::light = light;

		shadowMap = std::make_shared<RenderTexture>(512, 512);

		objectShader = std::make_shared<ObjShader>();

		planeTrans.position = Vector3(0.f, -1.f, 0.f);
		planeTrans.rotation = Quaternion(Vector3(90.f, 0.f, 0.f));
		planeTrans.scale = Vector3(10.f, 10.f, 1.f);
		MeshPtr mesh = CreatePlane();
		planeMesh.vertices.clear();
		planeMesh.indices.clear();
		int vertexCount = mesh->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
			planeMesh.vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->normals[i] });
		for (auto idx : mesh->indices) planeMesh.indices.emplace_back((uint16_t)idx);

		objectTrans.position = Vector3(0.f, 0.f, 0.f);
		objectTrans.scale = Vector3::one * 0.8f;
		std::vector<MeshPtr> meshes;
		Mesh::LoadMesh(meshes, "resources/knot.obj");
		mesh = meshes[0];
		objectMesh.vertices.clear();
		objectMesh.indices.clear();
		vertexCount = mesh->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
			objectMesh.vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->normals[i] });
		for (auto idx : mesh->indices) objectMesh.indices.emplace_back((uint16_t)idx);
    }

	Rasterizer::SetRenderTarget(shadowMap);
	Rasterizer::Clear(true, false, Color::black);

	Rasterizer::viewMatrix = light->transform.worldToLocalMatrix();
	Rasterizer::projectionMatrix = light->CalcShadowMapMatrix(camera);
	Rasterizer::renderState.cull = RenderState::CullType_Off;
	Rasterizer::renderState.renderType = RenderState::RenderType_ShadowPrePass;

	Rasterizer::SetShader(objectShader);
	Rasterizer::modelMatrix = objectTrans.localToWorldMatrix();
	Rasterizer::renderData.AssignVertexBuffer(objectMesh.vertices);
	Rasterizer::renderData.AssignIndexBuffer(objectMesh.indices);
	Rasterizer::Submit();

	//shadowMap->GetDepthBuffer()->SaveToFile("depth.tiff");

	//Rasterizer::SetRenderTarget(nullptr);
	//Rasterizer::Clear(true, true, Color(1.f, 0.19f, 0.3f, 0.47f));

	//Rasterizer::viewMatrix = camera->viewMatrix();
	//Rasterizer::projectionMatrix = camera->projectionMatrix();

	//objectShader->shadowMap = Texture2D::CreateWithBitmap(shadowMap->GetDepthBuffer());
	//Rasterizer::SetShader(objectShader);
	//Rasterizer::renderState.cull = RenderState::CullType_Back;
	//Rasterizer::renderState.renderType = RenderState::RenderType_Stardand;

	//Rasterizer::modelMatrix = planeTrans.localToWorldMatrix();
	//Rasterizer::renderData.AssignVertexBuffer(planeMesh.vertices);
	//Rasterizer::renderData.AssignIndexBuffer(planeMesh.indices);
	//Rasterizer::Submit();

	//Rasterizer::modelMatrix = objectTrans.localToWorldMatrix();
	//Rasterizer::renderData.AssignVertexBuffer(objectMesh.vertices);
	//Rasterizer::renderData.AssignIndexBuffer(objectMesh.indices);
	//Rasterizer::Submit();

    Rasterizer::Present();
}
