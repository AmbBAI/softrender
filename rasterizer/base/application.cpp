#include "application.h"
#include "canvas.h"

namespace rasterizer
{

Application::Application()
	: loopFunc(NULL)
	, width(0)
	, height(0)
	, isConsoleVisible(false)
{

}

Application* Application::GetInstance()
{
	static Application _GlobalApplication;
	return &_GlobalApplication;
}

bool Application::CreateApplication(int argc, char *argv[], const char* title, int width, int height)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow(title);
	this->width = width;
	this->height = height;
	return true;
}

Canvas* Application::GetCanvas()
{
	Canvas* canvas = new Canvas(width, height);
	return canvas;
}

void Application::RunLoop()
{
	glutDisplayFunc(loopFunc);
	glutMainLoop();
}

}
