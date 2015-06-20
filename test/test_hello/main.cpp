#include "rasterizer.h"
using namespace rasterizer;

Application* app;

void TestColor(Canvas* canvas)
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
	Canvas* canvas = app->GetCanvas();
	canvas->Clear();
	TestColor(canvas);
	canvas->Present();
}

int main(int argc, char *argv[])
{
	app = Application::GetInstance();
	app->CreateApplication("hello", 512, 512);
	app->SetRunLoop(MainLoop);
	app->RunLoop();
	return 0;
}
