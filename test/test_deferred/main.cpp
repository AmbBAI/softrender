#include "softrender.h"
#include "transform_controller.hpp"
#include "object_utilities.h"
using namespace sr;

Application* app;

void Start();
void Update();

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("course3", 800, 600);
	SoftRender::Initialize(800, 600);
	Start();
	app->SetRunLoop(Update);
	app->RunLoop();
	return 0;
}

struct Vertex
{
	Vector3 position;
	Vector3 normal;
	Vector4 tangent;
	Vector2 texcoord;

	static const std::vector<Mesh::VertexElement>& elements()
	{
		static std::vector<Mesh::VertexElement> _elements
		{
			Mesh::VertexElement_Position,
			Mesh::VertexElement_Normal,
			Mesh::VertexElement_Tangent,
			Mesh::VertexElement_Texcoord
		};

		return _elements;
	}

};

struct V2F
{
	Vector4 position;
	Vector3 worldPos;
	Vector3 tSpace0;
	Vector3 tSpace1;
	Vector3 tSpace2;
	Vector2 texcoord;
};

struct GBufferPass : Shader<Vertex, V2F>
{
	Texture2DPtr diffuseMap;
	Texture2DPtr normalMap;

	V2F vert(const Vertex& input) override
	{
		V2F output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.worldPos = _Object2World.MultiplyPoint3x4(input.position);
		Vector3 normal = _Object2World.MultiplyVector(input.normal).Normalize();
		Vector3 tangent = _Object2World.MultiplyVector(input.tangent.xyz).Normalize();
		Vector3 bitangent = normal.Cross(tangent) * input.tangent.w;
		output.tSpace0 = Vector3(tangent.x, bitangent.x, normal.x);
		output.tSpace1 = Vector3(tangent.y, bitangent.y, normal.y);
		output.tSpace2 = Vector3(tangent.z, bitangent.z, normal.z);
		output.texcoord = input.texcoord;
		return output;
	}

	void frag(const V2F& input) override
	{
		Color diffuseColor = Color::white;// Tex2D(*diffuseMap, input.texcoord);
		Vector3 normal = Vector3::front;// UnpackNormal(Tex2D(*normalMap, input.texcoord));
		Vector3 worldNormal = Vector3::up;
		worldNormal.x = input.tSpace0.Dot(normal);
		worldNormal.y = input.tSpace1.Dot(normal);
		worldNormal.z = input.tSpace2.Dot(normal);

		SV_Target1 = diffuseColor;
		SV_Target2 = Color::white * 0.6f;
		SV_Target3 = Color(worldNormal * 0.5 + Vector3::one * 0.5, 0.f);
		SV_Target0 = diffuseColor * 0.3f;
	}
};

struct LightVertex
{
	Vector3 position;

	static const std::vector<Mesh::VertexElement>& elements()
	{
		static std::vector<Mesh::VertexElement> _elements
		{
			Mesh::VertexElement_Position,
		};

		return _elements;
	}
};

struct LightV2F
{
	Vector4 position;
};

struct LightShadeV2F
{
	Vector4 position;
	Vector3 ray;
};

struct LightShadePass : Shader<LightVertex, LightShadeV2F>
{
	Texture2DPtr diffuseGBuffer;
	Texture2DPtr specularGBuffer;
	Texture2DPtr normalGBuffer;
	Texture2DPtr _CameraDepthTexture;

	LightShadeV2F vert(const LightVertex& input) override
	{
		LightShadeV2F output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.ray = -_MATRIX_MV.MultiplyPoint(input.position).xyz;
		return output;
	}

	void frag(const LightShadeV2F& input) override
	{
		Vector2 screenCoord;
		screenCoord.x = input.position.x * 0.5f / input.position.w + 0.5f;
		screenCoord.y = input.position.y * 0.5f / input.position.w + 0.5f;

		LightInput lightInput;
		lightInput.ambient = Color::black;
		lightInput.diffuse = Tex2D(*diffuseGBuffer, screenCoord);
		lightInput.specular = Tex2D(*specularGBuffer, screenCoord);
		lightInput.shininess = 10.f;
		Vector3 worldNormal = UnpackNormal(Tex2D(*normalGBuffer, screenCoord));
		float depth = Tex2D(*_CameraDepthTexture, screenCoord).a;
		Vector3 viewPos = input.ray * (depth * _ZBufferParams.x / input.ray.z);
		Vector3 worldPos = _CameraToWorld.MultiplyPoint(viewPos).xyz;
		Vector3 worldView = (_WorldSpaceCameraPos - worldPos).Normalize();

		Vector3 lightDir;
		Color lightColor;
		InitLightArgs(worldPos, lightDir, lightColor);

		Color fragColor = Color::clear;
		fragColor.rgb = ShaderF::LightingBlinnPhong(lightInput, worldNormal, lightDir, lightColor.rgb, worldView);
		SV_Target0 = fragColor;
	}
};

int n = 10;
float planeH = -0.5f;
float lightH = 0.8f;
Transform objectTrans;
CameraPtr camera;
TransformController cameraCtrl;
std::shared_ptr<GBufferPass> gbufferPass;
std::shared_ptr<Shader<LightVertex, LightV2F>> lightPrePass;
std::shared_ptr<LightShadePass> lightShadePass;
LightPtr light;
std::vector<std::pair<Color, float>> lightColorIntensity;
MeshPtr pointLightVolume;
MeshPtr plane;
MeshPtr cube;
BitmapPtr diffuseGBuffer;
BitmapPtr specularGBuffer;
BitmapPtr normalGBuffer;

void Start()
{
	camera = CameraPtr(new Camera());
	camera->SetPerspective(60.f, 1.33333f, 0.3f, 100.f);
	camera->transform.position = Vector3(-5.f, 10.f, -5.f);
	camera->transform.rotation = Quaternion(Vector3(30.f, 45.f, 0.f));
	SoftRender::camera = camera;

	pointLightVolume = LoadMesh("resources/point_light_volume.obj");

	light = std::make_shared<Light>();
	light->type = Light::LightType_Point;
	light->Initilize();
	SoftRender::light = light;
	for (int i = 0; i < n * n; ++i)
	{
		lightColorIntensity.emplace_back(Color(1, Mathf::Random(0.f, 1.f), Mathf::Random(0.f, 1.f), Mathf::Random(0.f, 1.f)), Mathf::Random(4.f, 5.f));
	}

	diffuseGBuffer = SoftRender::GetRenderTarget()->CreateGBuffer(0, Bitmap::BitmapType_RGB24);
	specularGBuffer = SoftRender::GetRenderTarget()->CreateGBuffer(1, Bitmap::BitmapType_RGB24);
	normalGBuffer = SoftRender::GetRenderTarget()->CreateGBuffer(2, Bitmap::BitmapType_RGB24);

	gbufferPass = std::make_shared<GBufferPass>();
	gbufferPass->diffuseMap = Texture2D::LoadTexture("resources/bric.tga");
	gbufferPass->diffuseMap->GenerateMipmaps();
	gbufferPass->normalMap = Texture2D::LoadTexture("resources/bric_n.tga");
	gbufferPass->normalMap->GenerateMipmaps();
	lightPrePass = std::make_shared<Shader<LightVertex, LightV2F>>();
	lightShadePass = std::make_shared<LightShadePass>();
	lightShadePass->diffuseGBuffer = Texture2D::CreateWithBitmap(diffuseGBuffer);
	lightShadePass->diffuseGBuffer->filterMode = Texture2D::FilterMode_Point;
	lightShadePass->specularGBuffer = Texture2D::CreateWithBitmap(specularGBuffer);
	lightShadePass->specularGBuffer->filterMode = Texture2D::FilterMode_Point;
	lightShadePass->normalGBuffer = Texture2D::CreateWithBitmap(normalGBuffer);
	lightShadePass->normalGBuffer->filterMode = Texture2D::FilterMode_Point;
	auto& linearDepthBuffer = SoftRender::GetRenderTarget()->GetDepthBuffer();
	lightShadePass->_CameraDepthTexture = Texture2D::CreateWithBitmap(linearDepthBuffer);
	lightShadePass->_CameraDepthTexture->filterMode = Texture2D::FilterMode_Point;

	plane = CreatePlane();
	cube = CreateCube();
}

void Update()
{
	cameraCtrl.MouseRotate(camera->transform);
	cameraCtrl.KeyMove(camera->transform);

	SoftRender::Clear(true, true, Color::clear);
	SoftRender::GetRenderTarget()->SetGBuffer(0, diffuseGBuffer);
	diffuseGBuffer->Fill(Color::clear);
	SoftRender::GetRenderTarget()->SetGBuffer(1, specularGBuffer);
	specularGBuffer->Fill(Color::clear);
	SoftRender::GetRenderTarget()->SetGBuffer(2, normalGBuffer);
	normalGBuffer->Fill(Color::clear);

	// GBuffer Pass
	SoftRender::renderState.alphaBlend = false;
	SoftRender::renderState.stencilOn = false;
	SoftRender::renderState.cull = RenderState::CullType_Back;
	SoftRender::renderState.zTest = RenderState::ZTestType_LEqual;
	SoftRender::renderState.zWrite = true;
	SoftRender::SetShader(gbufferPass);
	SoftRender::renderData.AssetVerticesIndicesBuffer<Vertex>(*plane);
	objectTrans.position = Vector3(0.f, planeH, 0.f);
	objectTrans.rotation = Quaternion(Vector3(90.f, 0.f, 0.f));
	objectTrans.scale = Vector3::one * 100.f;
	SoftRender::modelMatrix = objectTrans.localToWorldMatrix();
	SoftRender::Submit();
	SoftRender::renderData.AssetVerticesIndicesBuffer<Vertex>(*cube);
	for (int i = 0; i < n * n; ++i)
	{
		objectTrans.position = Vector3((i / n) * 2.f, 0.f, (i % n) * 2.f);
		objectTrans.rotation = Quaternion::identity;
		objectTrans.scale = Vector3::one;
		SoftRender::modelMatrix = objectTrans.localToWorldMatrix();
		SoftRender::Submit();
	}

	SoftRender::GetRenderTarget()->SetGBuffer(0, nullptr);
	SoftRender::GetRenderTarget()->SetGBuffer(1, nullptr);
	SoftRender::GetRenderTarget()->SetGBuffer(2, nullptr);

	// Light Pass
	SoftRender::renderData.AssetVerticesIndicesBuffer<LightVertex>(*pointLightVolume);

	for (int i = 0; i < n * n; ++i)
	{
		light->color = lightColorIntensity[i].first;
		light->intensity = lightColorIntensity[i].second;
		light->range = 5.f; //20.f
		light->transform.position = Vector3((i / n) * 2.f, lightH, (i % n) * 2.f);
		light->transform.scale = Vector3::one * light->range;
		SoftRender::modelMatrix = light->transform.localToWorldMatrix();
		Vector3 lightInCameraSpace = (camera->viewMatrix() * SoftRender::modelMatrix).MultiplyPoint3x4(light->transform.position);
		if (lightInCameraSpace.z - light->range < camera->zNear())
		{
			SoftRender::renderState.stencilOn = false;
			SoftRender::renderState.alphaBlend = true;
			SoftRender::renderState.blender.SetColorBlendMode(Blender::BlendMode_One, Blender::BlendMode_One);
			SoftRender::renderState.blender.SetAlphaBlendMode(Blender::BlendMode_Zero, Blender::BlendMode_One);
			SoftRender::renderState.zWrite = false;
			SoftRender::renderState.cull = RenderState::CullType_Front;
			SoftRender::renderState.zTest = RenderState::ZTestType_GEqual;
			SoftRender::SetShader(lightShadePass);
			SoftRender::Submit();
		}
		else
		{
			SoftRender::ClearStencilBuffer(0x00);
			SoftRender::renderState.stencilOn = true;
			SoftRender::renderState.stencilComp = RenderState::StencilComparison_Always;
			SoftRender::renderState.stencilOp = RenderState::StencilOperation_Replace;
			SoftRender::renderState.stencilRefValue = 0xff;
			SoftRender::renderState.alphaBlend = true;
			SoftRender::renderState.blender.SetColorBlendMode(Blender::BlendMode_Zero, Blender::BlendMode_One);
			SoftRender::renderState.blender.SetAlphaBlendMode(Blender::BlendMode_Zero, Blender::BlendMode_One);
			SoftRender::renderState.zWrite = false;
			SoftRender::renderState.cull = RenderState::CullType_Front;
			SoftRender::renderState.zTest = RenderState::ZTestType_GEqual;
			SoftRender::SetShader(lightPrePass);
			SoftRender::Submit();

			// Shade Pass
			SoftRender::renderState.stencilOn = true;
			SoftRender::renderState.stencilComp = RenderState::StencilComparison_Equal;
			SoftRender::renderState.stencilOp = RenderState::StencilOperation_Zero;
			SoftRender::renderState.stencilRefValue = 0xff;
			SoftRender::renderState.alphaBlend = true;
			SoftRender::renderState.blender.SetColorBlendMode(Blender::BlendMode_One, Blender::BlendMode_One);
			SoftRender::renderState.blender.SetAlphaBlendMode(Blender::BlendMode_Zero, Blender::BlendMode_One);
			SoftRender::renderState.zWrite = false;
			SoftRender::renderState.cull = RenderState::CullType_Back;
			SoftRender::renderState.zTest = RenderState::ZTestType_LEqual;
			SoftRender::SetShader(lightShadePass);
			SoftRender::Submit();
		}
	}
	
	SoftRender::Present();

	if (app->GetInput()->GetKey(GLFW_KEY_ENTER))
	{
		diffuseGBuffer->SaveToFile("gbuffer0.png");
		specularGBuffer->SaveToFile("gbuffer1.png");
		normalGBuffer->SaveToFile("gbuffer2.png");
		SoftRender::GetStencilBuffer()->SaveToFile("stencil.png");
		SoftRender::GetRenderTarget()->GetDepthBuffer()->SaveToFile("depth.tiff");
		SoftRender::GetRenderTarget()->GetColorBuffer()->SaveToFile("result.png");
	}
	app->SetTitle(std::to_string(app->GetDeltaTime()).c_str());
}
