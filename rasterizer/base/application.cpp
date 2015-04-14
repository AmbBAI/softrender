#include "application.h"
#include "canvas.h"
#include "input.h"
#include "ui.h"

namespace rasterizer
{

Application* Application::GetInstance()
{
	static Application _GlobalApplication;
	return &_GlobalApplication;
}

Application::~Application()
{
	if (canvas != nullptr)
	{
		delete canvas;
		canvas = nullptr;
	}

    //UI::Finalize();
    
	if (input != nullptr)
	{
		delete input;
		input = nullptr;
	}
    
    if (window != nullptr)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }
}

bool Application::CreateApplication(const char* title, int width, int height)
{
    if (!glfwInit())
    {
        assert(false);
        return false;
    }

	glfwWindowHint(GLFW_DOUBLEBUFFER, 0);
	glfwWindowHint(GLFW_RESIZABLE, 0);
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
    {
        assert(false);
        return false;
    }
    
    glfwMakeContextCurrent(window);
	
    this->width = width;
	this->height = height;
    
    //bool ret = UI::Initialize(window, width, height);
    //assert(ret);
    
	return true;
}

Canvas* Application::GetCanvas()
{
	assert(width > 0 && height > 0);
	if (canvas == nullptr)
	{
		canvas = new Canvas(width, height);
	}
	return canvas;
}

Input* Application::GetInput()
{
	assert(window != nullptr);
	if (input == nullptr)
	{
		input = new Input(window);
	}
	return input;
}



void Application::RunLoop()
{
    assert(window != nullptr);
	while (!glfwWindowShouldClose(window))
    {
		lastFrameTime = thisFrameTime;
		thisFrameTime = GetTime();
		deltaTime = thisFrameTime - lastFrameTime;
        if (loopFunc != nullptr) loopFunc();
		glfwPollEvents();
    }
    glfwTerminate();
}

float Application::GetTime()
{
    return (float) glfwGetTime();
}

float Application::GetDeltaTime()
{
	return deltaTime;
}

}
