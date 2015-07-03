#include "rasterizer.h"
using namespace rasterizer;

Application* app;

void DrawTexture(Canvas* canvas, const Vector4& rect, const Texture& texture, float lod /*= 0.f*/)
{
	int width = (int)canvas->GetWidth();
	int height = (int)canvas->GetHeight();

	int minX = Mathf::Clamp(Mathf::FloorToInt(rect.x), 0, width);
	int maxX = Mathf::Clamp(Mathf::FloorToInt(rect.z), 0, width);
	int minY = Mathf::Clamp(Mathf::CeilToInt(rect.y), 0, height);
	int maxY = Mathf::Clamp(Mathf::CeilToInt(rect.w), 0, height);

	for (int y = minY; y < maxY; ++y)
	{
		float v = ((float)y - rect.y) / (rect.w - rect.y);
		for (int x = minX; x < maxX; ++x)
		{
			float u = ((float)x - rect.x) / (rect.z - rect.x);
			Color color = texture.Sample(u, v, lod);
			canvas->SetPixel(x, y, color);
		}
	}
}

void MainLoop()
{
	static TexturePtr tex = nullptr;

	Canvas* canvas = app->GetCanvas();

	if (tex == nullptr)
	{
		tex = Texture::LoadTexture("resources/crytek-sponza/textures/background.tga");
		tex->filterMode = Texture::FilterMode_Bilinear;
		tex->xAddressMode = Texture::AddressMode_Clamp;
		tex->yAddressMode = Texture::AddressMode_Clamp;
		// tex->CompressTexture();
		// tex->ConvertBumpToNormal(10);
		// tex->GenerateMipmaps();
	}

	canvas->Clear();

	DrawTexture(canvas, Vector4(0, 0, 512, 512), *tex, 0);
	//TestTexture(canvas, Vector4(256, 256, 512, 512), *tex, 1);
	canvas->Present();
}

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("image", 512, 512);
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}


