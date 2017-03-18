#include "softrender.h"
#include "transform_controller.hpp"
#include "object_utilities.h"
using namespace sr;

Application* app;

void MainLoop();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("sponza", 800, 600);
	SoftRender::Initialize(800, 600);
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

struct SMVaryingData
{
	Vector4 position;
	static std::vector<VaryingDataElement> GetDecl()
	{
		static std::vector<VaryingDataElement> decl = {
			{ 0, VaryingDataDeclUsage_SVPOSITION, VaryingDataDeclFormat_Vector4 }
		};

		return decl;
	}
};

struct ShadowMapPrePass : Shader<Vertex, SMVaryingData>
{
	SMVaryingData vert(const Vertex& input) override
	{
		SMVaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		return output;
	}
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

	Texture2DPtr shadowMap = nullptr;
	Matrix4x4 lightVPM;

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

		Vector4 proj = lightVPM.MultiplyPoint(input.worldPos);
		Vector2 smuv = Vector2((proj.x / proj.w + 1.f) / 2.f, (proj.y / proj.w + 1.f) / 2.f);
		float shadow = Tex2DProjInterpolated(shadowMap, smuv, proj.z / proj.w, 0.0025f);

		Vector3 lightDir;
		float lightAtten;
		InitLightArgs(input.worldPos, lightDir, lightAtten);
		lightAtten *= (1.f - shadow);

		LightInput lightInput;
		lightInput.ambient = Color::white * 0.25f;
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
	static std::shared_ptr<SceneShader> sceneShader = std::make_shared<SceneShader>();
	static std::shared_ptr<ShadowMapPrePass> smPrePass = std::make_shared<ShadowMapPrePass>();
	static Transform sceneTrans;
	static TransformController transCtrl;
	static CameraPtr camera = std::make_shared<Camera>();
	static LightPtr light = std::make_shared<Light>();
	static std::vector<Vector3> sceneWorldBox;
	static RenderTexturePtr shadowMap = std::make_shared<RenderTexture>(1024, 1024);

	if (!isInitilized)
    {
		isInitilized = true;

		camera->SetPerspective(60.f, 1.33333f, 0.3f, 4000.f);
		camera->transform.position = Vector3(800.f, 400.f, 0.f);
		camera->transform.rotation = Quaternion(Vector3(0.f, -90.f, 0.f));
		//camera->SetOrthographic(-800.f, 800.f, -600.f, 600.f, 0.3f, 4000.f);
		//camera->transform = Vector3(0.f, 400.f, 0.f);
		//camera->transform = Quaternion(Vector3(0.f, -90.f, 0.f));
		SoftRender::camera = camera;

		light->type = Light::LightType_Directional;
		light->transform.position = Vector3(0, 300, 0);
		light->transform.rotation = Quaternion(Vector3(60.f, -45.f, 0.f));
		light->Initilize();
		SoftRender::light = light;

		SoftRender::SetShader(sceneShader);

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

		SoftRender::modelMatrix = sceneTrans.localToWorldMatrix();
		sceneWorldBox.emplace_back(-1000.f, 0.f, -2000.f);
		sceneWorldBox.emplace_back(-1000.f, 0.f, 2000.f);
		sceneWorldBox.emplace_back(-1000.f, 1000.f, -2000.f);
		sceneWorldBox.emplace_back(-1000.f, 1000.f, 2000.f);
		sceneWorldBox.emplace_back(1000.f, 0.f, -2000.f);
		sceneWorldBox.emplace_back(1000.f, 0.f, 2000.f);
		sceneWorldBox.emplace_back(1000.f, 1000.f, -2000.f);
		sceneWorldBox.emplace_back(1000.f, 1000.f, 2000.f);
    }

	static char title[32];
	sprintf(title, "sponza - %f", app->GetDeltaTime());
	app->SetTitle(title);

	transCtrl.MouseRotate(camera->transform);
	transCtrl.KeyMove(camera->transform);

	Matrix4x4 cameraVM = camera->viewMatrix();
	Matrix4x4 cameraPM = camera->projectionMatrix();

	SoftRender::SetRenderTarget(shadowMap);
	SoftRender::Clear(true, false, Color::black);

	Matrix4x4 cameraVPIM = (cameraPM * cameraVM).Inverse();
	CameraPtr virtualCamera = light->BuildShadowMapVirtualCamera(cameraVPIM, sceneWorldBox);
	Matrix4x4 lightVM = virtualCamera->viewMatrix();
	Matrix4x4 lightPM = virtualCamera->projectionMatrix();

	SoftRender::camera = virtualCamera;
	
	SoftRender::SetShader(smPrePass);
	SoftRender::renderState.cull = RenderState::CullType_Off;

	for (auto& meshWapper : meshData)
	{
		SoftRender::renderData.AssignVertexBuffer(meshWapper.vertices);
		SoftRender::renderData.AssignIndexBuffer(meshWapper.indices);

		for (auto& materialTuple : meshWapper.materials)
		{
			MaterialPtr material = std::get<0>(materialTuple);
			int startIndex = std::get<1>(materialTuple);
			int primitiveCount = std::get<2>(materialTuple);

			SoftRender::Submit(startIndex, primitiveCount);
		}
	}


	SoftRender::SetRenderTarget(nullptr);
	SoftRender::Clear(true, true, Color(1.f, 0.19f, 0.3f, 0.47f));

	SoftRender::camera = camera;
	SoftRender::SetShader(sceneShader);
	BitmapPtr bitmap = shadowMap->GetDepthBuffer();
	sceneShader->shadowMap = Texture2D::CreateWithBitmap(bitmap);
	sceneShader->lightVPM = lightPM * lightVM;
	SoftRender::renderState.cull = RenderState::CullType_Back;

	for (auto& meshWapper : meshData)
	{
		SoftRender::renderData.AssignVertexBuffer(meshWapper.vertices);
		SoftRender::renderData.AssignIndexBuffer(meshWapper.indices);

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

			SoftRender::Submit(startIndex, primitiveCount);
		}
	}

    SoftRender::Present();
	//Rasterizer::GetRenderTarget()->GetDepthBuffer()->SaveToFile("depth.tiff");
	//Rasterizer::GetRenderTarget()->GetColorBuffer()->SaveToFile("color.png");
}
