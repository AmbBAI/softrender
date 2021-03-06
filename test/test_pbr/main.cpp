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

	static const std::vector<Mesh::VertexElement>& elements()
	{
		static std::vector<Mesh::VertexElement> _elements
		{
			Mesh::VertexElement_Position,
			Mesh::VertexElement_Texcoord,
			Mesh::VertexElement_Normal,
			Mesh::VertexElement_Tangent
		};

		return _elements;
	}
};

struct V2F
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 tspace0;
	Vector3 tspace1;
	Vector3 tspace2;
	Vector3 worldPos;
};

struct MainShader : Shader<Vertex, V2F>
{
	CubemapPtr envMap = nullptr;

	Texture2DPtr albedoMap;
	Texture2DPtr normalMap;
	Texture2DPtr paramMap;

	V2F vert(const Vertex& input) override
	{
		V2F output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.texcoord = input.texcoord;
		Vector3 normal = _Object2World.MultiplyVector(input.normal).Normalize();
		Vector3 tangent = _Object2World.MultiplyVector(input.tangent.xyz).Normalize();
		Vector3 bitangent = normal.Cross(tangent) * input.tangent.w;
		output.tspace0 = Vector3(tangent.x, bitangent.x, normal.x);
		output.tspace1 = Vector3(tangent.y, bitangent.y, normal.y);
		output.tspace2 = Vector3(tangent.z, bitangent.z, normal.z);

		output.worldPos = _Object2World.MultiplyPoint3x4(input.position);
		return output;
	}

	void frag(const V2F& input) override
	{
		PBSInput pbsInput;
		pbsInput.albedo = Tex2D(*albedoMap, input.texcoord).rgb;

		Vector3 normal = UnpackNormal(Tex2D(*normalMap, input.texcoord));
		pbsInput.normal.x = input.tspace0.Dot(normal);
		pbsInput.normal.y = input.tspace1.Dot(normal);
		pbsInput.normal.z = input.tspace2.Dot(normal);

		Vector3 param = Tex2D(*paramMap, input.texcoord).rgb;
		pbsInput.roughness = param.x;
		pbsInput.metallic = param.y;
		pbsInput.PBSSetup();

		Vector3 lightDir;
		Color lightColor;
		InitLightArgs(input.worldPos, lightDir, lightColor);
		PBSLight pbsLight;
		pbsLight.color = lightColor.rgb;
		pbsLight.dir = lightDir;

		Vector3 viewDir = (_WorldSpaceCameraPos - input.worldPos).Normalize();

		Color fragColor = Color::white * 0.1f;
		fragColor.rgb += PBSF::BRDF1(pbsInput, pbsInput.normal, viewDir, pbsLight);
		fragColor.rgb += PBSF::ApproximateSpecularIBL(*envMap, pbsInput.specColor, pbsInput.normal, viewDir, pbsInput.roughness);
		SV_Target0 = fragColor;
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static Transform objectTrans;
	static Transform cameraTrans;
	static TransformController objectCtrl;
	static std::shared_ptr<MainShader> shader;
	static LightPtr light = std::make_shared<Light>();

	if (!isInitilized)
    {
		isInitilized = true;

		auto camera = CameraPtr(new Camera());
		camera->SetPerspective(30.f, 1.f, 0.3f, 1000.f);
		//camera->SetOrthographic(-3.f, 3.f, -3.f, 3.f, 0.f, 20.f);
		camera->transform.position = Vector3(0.f, 0.f, -20.f);
		SoftRender::camera = camera;

		light->type = Light::LightType_Directional;
		light->transform.position = Vector3(0, 0, 0);
		light->transform.rotation = Quaternion(Vector3(45.f, -45.f, 0.f));
		light->Initilize();
		SoftRender::light = light;

		shader = std::make_shared<MainShader>();
		shader->albedoMap = Texture2D::LoadTexture("resources/pbr/knife_albedo.png");
		shader->normalMap = Texture2D::LoadTexture("resources/pbr/knife_normal.png");
		shader->paramMap = Texture2D::LoadTexture("resources/pbr/knife_param.png");

		shader->envMap = CubemapPtr(new Cubemap());
		auto latlong = Texture2D::LoadTexture("resources/pbr/envmap.png");
		std::vector<BitmapPtr> mipmaps;
		for (int i = 1; i <= 10; ++i)
		{
			char path[256];
			sprintf(path, "resources/pbr/envmap_mip%d.png", i);
			mipmaps.push_back(Texture2D::LoadTexture(path)->GetBitmap(0));
		}
		latlong->SetMipmaps(mipmaps);
		shader->envMap->InitWithLatlong(latlong);

		auto mesh = LoadMesh("resources/pbr/knife.obj");
		mesh->CalculateTangents();
		SoftRender::renderData.AssetVerticesIndicesBuffer<Vertex>(*mesh);
		objectTrans.rotation = Quaternion(Vector3(0.f, -110.f, -20.f));
	}

	SoftRender::Clear(true, true, Color(1.f, 0.19f, 0.3f, 0.47f));

	objectCtrl.MouseRotate(objectTrans, false);

	objectTrans.position = Vector3(0.f, 0.f, 2.f);
	objectTrans.scale = Vector3::one * 2.f;
	SoftRender::modelMatrix = objectTrans.localToWorldMatrix();
	SoftRender::SetShader(shader);
	SoftRender::Submit();

    SoftRender::Present();
}
