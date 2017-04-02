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
			{ 36, VaryingDataDeclUsage_POSITION, VaryingDataDeclFormat_Vector3 }
		};

		return decl;
	}
};

struct MainShader : Shader<Vertex, VaryingData>
{
	CubemapPtr envMap = nullptr;
	float roughness = 0.5f;
	float metallic = 0.5f;

	VaryingData vert(const Vertex& input) override
	{
		VaryingData output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		output.texcoord = input.texcoord;
		output.normal = _Object2World.MultiplyVector(input.normal).Normalize();
		output.worldPos = _Object2World.MultiplyPoint3x4(input.position);
		return output;
	}

	Color frag(const VaryingData& input) override
	{
		PBSInput pbsInput;
		pbsInput.albedo = Color::white.rgb;
		pbsInput.normal = input.normal;
		pbsInput.roughness = roughness;
		pbsInput.metallic = metallic;
		pbsInput.PBSSetup();

		Vector3 lightDir;
		float lightAtten;
		InitLightArgs(input.worldPos, lightDir, lightAtten);
		PBSLight pbsLight;
		pbsLight.color = _LightColor.xyz * lightAtten;
		pbsLight.dir = lightDir;

		Vector3 viewDir = (_WorldSpaceCameraPos - input.worldPos).Normalize();

		Color fragColor = Color::white * 0.1f;
		fragColor.rgb += PBSF::BRDF1(pbsInput, pbsInput.normal, viewDir, pbsLight);
		fragColor.rgb += PBSF::ApproximateSpecularIBL(*envMap, pbsInput.specColor, pbsInput.normal, viewDir, pbsInput.roughness);
		return fragColor;
	}
};

void MainLoop()
{
	static bool isInitilized = false;
	static Transform objectTrans;
	static Transform cameraTrans;
	static TransformController objectCtrl;
	static MeshWrapper<Vertex> objectMesh;
	static std::shared_ptr<MainShader> shader;
	static LightPtr light = std::make_shared<Light>();

	if (!isInitilized)
    {
		isInitilized = true;

		auto camera = CameraPtr(new Camera());
		camera->SetPerspective(30.f, 1.f, 0.3f, 2000.f);
		//camera->SetOrthographic(-3.f, 3.f, -3.f, 3.f, 0.f, 20.f);
		camera->transform.position = Vector3(0.f, 0.f, -20.f);
		SoftRender::camera = camera;

		light->type = Light::LightType_Directional;
		light->transform.position = Vector3(0, 0, 0);
		light->transform.rotation = Quaternion(Vector3(45.f, -45.f, 0.f));
		light->Initilize();
		SoftRender::light = light;

		// Gen IBL lut
		//BitmapPtr bitmap = BitmapPtr(new Bitmap(512, 512, Bitmap::BitmapType_RGB24));
		//for (int i = 0; i < 512; ++i)
		//{
		//	for (int j = 0; j < 512; ++j)
		//	{
		//		Vector2 out = PBSF::IntergrateBRDF((float)j / (float)511, (float)i / (float)511, 1024);
		//		Color color = Color(1.f, out.x, out.y, 0.f);
		//		bitmap->SetPixel(i, j, color);
		//	}
		//}
		//bitmap->SaveToFile("resources/cubemap/lut.png");

		shader = std::make_shared<MainShader>();
		shader->envMap = CubemapPtr(new Cubemap());
		//Texture2DPtr* images = new Texture2DPtr[6];
		//for (int i = 0; i < 6; ++i)
		//{
		//	char path[32];
		//	sprintf(path, "resources/cubemap/%d.jpg", i);
		//	images[i] = Texture2D::LoadTexture(path);
		//	images[i]->xAddressMode = Texture2D::AddressMode_Clamp;
		//	images[i]->yAddressMode = Texture2D::AddressMode_Clamp;
		//}
		//shader->envMap->InitWith6Images(images);
		//shader->envMap->Mapping6ImagesToLatlong(1024);

		auto latlong = Texture2D::LoadTexture("resources/pbr/envmap.png");
		shader->envMap->InitWithLatlong(latlong);
		// PrefilterEnvMap
		//shader->envMap->PrefilterEnvMap(10, 1024);
		//for (int i = 0; i <= latlong->GetMipmapsCount(); ++i)
		//{
		//	BitmapPtr bitmap = latlong->GetBitmap(i);
		//	std::string path = std::string("resources/pbr/envmap_mip") + std::to_string(i) + ".png";
		//	bitmap->SaveToFile(path.c_str());
		//}

		std::vector<BitmapPtr> mipmaps;
		for (int i = 1; i <= 10; ++i)
		{
			char path[256];
			sprintf(path, "resources/pbr/envmap_mip%d.png", i);
			mipmaps.push_back(Texture2D::LoadTexture(path)->GetBitmap(0));
		}
		latlong->SetMipmaps(mipmaps);

		std::vector<MeshPtr> meshes;
		Mesh::LoadMesh(meshes, "resources/sphere.obj");
		auto mesh = meshes[0];
		objectMesh.vertices.clear();
		objectMesh.indices.clear();
		int vertexCount = mesh->GetVertexCount();
		for (int i = 0; i < vertexCount; ++i)
			objectMesh.vertices.emplace_back(Vertex{ mesh->vertices[i], mesh->texcoords[i], mesh->normals[i], mesh->tangents[i] });
		for (auto idx : mesh->indices) objectMesh.indices.emplace_back((uint16_t)idx);

		SoftRender::renderData.AssignVertexBuffer(objectMesh.vertices);
		SoftRender::renderData.AssignIndexBuffer(objectMesh.indices);
	}

	SoftRender::Clear(true, true, Color(1.f, 0.19f, 0.3f, 0.47f));

	objectCtrl.MouseRotate(objectTrans, false);

	//objectTrans.position = Vector3(0.f, 0.f, 2.f);
	//objectTrans.scale = Vector3::one * 4.f;
	//SoftRender::modelMatrix = objectTrans.localToWorldMatrix();
	//shader->roughness = 0.5f;
	//shader->metallic = 0.5f;
	//SoftRender::SetShader(shader);
	//SoftRender::Submit();

	for (int i = 0; i < 11; ++i)
	{
		for (int j = 0; j < 11; ++j)
		{
			objectTrans.position = Vector3(1.f * (i - 5), 1.f * (j - 5), 2.f);
			objectTrans.scale = Vector3::one * 0.45f;
			SoftRender::modelMatrix = objectTrans.localToWorldMatrix();
			shader->roughness = i * 0.1f;
			shader->metallic = j * 0.1f;
			SoftRender::SetShader(shader);
			SoftRender::Submit();
		}
	}

    SoftRender::Present();
}
