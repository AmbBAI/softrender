#include "input.h"

namespace sr
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
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return Vector2((float)x, (float)y);
}

}
