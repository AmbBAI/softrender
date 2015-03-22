#include "input.h"

namespace rasterizer
{

Input::Input(GLFWwindow* window)
	: window(window)
{
}

bool Input::GetKey(int key)
{
	return GLFW_PRESS == glfwGetKey(window, key);
}

bool Input::GetMouseButton(int button)
{
	return GLFW_PRESS == glfwGetMouseButton(window, button);
}

Vector2 Input::GetMousePos()
{
	Vector2 pos;
	glfwGetCursorPos(window, (double*)&pos.x, (double*)&pos.y);
	return pos;
}

}
