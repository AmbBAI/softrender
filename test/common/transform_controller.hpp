#ifndef _TRANSFORM_CONTROLLER_HPP_
#define _TRANSFORM_CONTROLLER_HPP_

#include "rasterizer.h"

class TransformController
{
public:
	bool MouseRotate(rasterizer::Transform& trans, bool isLocal = true)
	{
		rasterizer::Application* app = rasterizer::Application::GetInstance();
		rasterizer::Input* input = app->GetInput();

		bool isDirty = false;

		if (input->GetMouseButton(0))
		{
			rasterizer::Vector2 nowMousePos = input->GetMousePos();
			if (isMouseDown)
			{
				rasterizer::Vector2 moveDelta = nowMousePos - lastMousePos;
				trans.Rotate(rasterizer::Vector3(moveDelta.y, moveDelta.x, 0), isLocal);
				isDirty = true;
			}
			lastMousePos = nowMousePos;
			isMouseDown = true;
		}
		else isMouseDown = false;

		return isDirty;
	}

	bool KeyMove(rasterizer::Transform& trans, float moveScale = 5.f, bool isLocal = true)
	{
		rasterizer::Application* app = rasterizer::Application::GetInstance();
		rasterizer::Input* input = app->GetInput();

		bool isDirty = false;

		rasterizer::Vector3 x, y, z;
		if (isLocal) trans.GetAxis(x, y, z);
		else
		{
			x = rasterizer::Vector3::right;
			y = rasterizer::Vector3::up;
			z = rasterizer::Vector3::front;
		}
		if (input->GetKey(GLFW_KEY_W)) { trans.Translate(+z * moveScale); isDirty = true; }
		if (input->GetKey(GLFW_KEY_S)) { trans.Translate(-z * moveScale); isDirty = true; }
		if (input->GetKey(GLFW_KEY_A)) { trans.Translate(-x * moveScale); isDirty = true; }
		if (input->GetKey(GLFW_KEY_D)) { trans.Translate(+x * moveScale); isDirty = true; }
		if (input->GetKey(GLFW_KEY_Q)) { trans.Translate(+y * moveScale); isDirty = true; }
		if (input->GetKey(GLFW_KEY_E)) { trans.Translate(-y * moveScale); isDirty = true; }

		return isDirty;
	}

private:
	bool isMouseDown = false;
	rasterizer::Vector2 lastMousePos;
};

#endif // !_TRANSFORM_CONTROLLER_HPP_