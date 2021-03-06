#include "softrender.h"
using namespace sr;

Application* app;

void TestColor(BitmapPtr& canvas)
{
	int w = canvas->GetWidth();
	int h = canvas->GetHeight();

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			canvas->SetPixel(x, y,
				Color(1.f, (float)x / w, (float)y / h, 0.f));
		}
	}
}

void MainLoop()
{
	SoftRender::Clear(false, true, Color(1.f, 0.19f, 0.3f, 0.47f));
	auto& canvas = SoftRender::GetRenderTarget()->GetColorBuffer();
	TestColor(canvas);
	SoftRender::Present();
}

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("hello", 512, 512);
	SoftRender::Initialize(512, 512);
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}
