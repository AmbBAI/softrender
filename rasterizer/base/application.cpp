#include "application.h"
#include "input.h"

namespace rasterizer
{

Application* Application::GetInstance()
{
	static Application _GlobalApplication;
	return &_GlobalApplication;
}

Application::~Application()
{
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

	glfwWindowHint(GLFW_RESIZABLE, 0);
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
    {
        assert(false);
        return false;
    }
    
    glfwSwapInterval(0);    
    glfwSetTime(0);
    
    glfwMakeContextCurrent(window);
	
    this->width = width;
	this->height = height;
    
	return true;
}

void Application::SetTitle(const char* title)
{
	assert(window != nullptr);
	glfwSetWindowTitle(window, title);
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
        glfwSwapBuffers(window);
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
