#ifndef _TRANSFORM_CONTROLLER_HPP_
#define _TRANSFORM_CONTROLLER_HPP_

#include "softrender.h"

class TransformController
{
public:
	bool MouseRotate(sr::Transform& trans, bool isLocal = true)
	{
		sr::Application* app = sr::Application::GetInstance();
		sr::Input* input = app->GetInput();

		bool isDirty = false;

		if (input->GetMouseButton(0))
		{
			sr::Vector2 nowMousePos = input->GetMousePos();
			if (isMouseDown)
			{
				sr::Vector2 moveDelta = nowMousePos - lastMousePos;
				trans.Rotate(sr::Vector3(-moveDelta.y, -moveDelta.x, 0), isLocal);
				isDirty = true;
			}
			lastMousePos = nowMousePos;
			isMouseDown = true;
		}
		else isMouseDown = false;

		return isDirty;
	}

	bool MouseRotateAround(sr::Transform& trans, const sr::Vector3& center)
	{
		sr::Application* app = sr::Application::GetInstance();
		sr::Input* input = app->GetInput();

		bool isDirty = false;

		if (input->GetMouseButton(0))
		{
			sr::Vector2 nowMousePos = input->GetMousePos();
			if (isMouseDown)
			{
				sr::Vector3 delta = center - trans.position;
				float deltaLength = delta.Length();
				sr::Vector2 moveDelta = nowMousePos - lastMousePos;
				trans.Rotate(sr::Vector3(moveDelta.y, moveDelta.x, 0), true);
				trans.position = center - trans.forward() * deltaLength;
				isDirty = true;
			}
			lastMousePos = nowMousePos;
			isMouseDown = true;
		}
		else isMouseDown = false;
		return isDirty;
	}

	bool KeyMove(sr::Transform& trans, float moveScale = 5.f, bool isLocal = true)
	{
		sr::Application* app = sr::Application::GetInstance();
		sr::Input* input = app->GetInput();

		bool isDirty = false;

		sr::Vector3 x, y, z;
		if (isLocal) trans.GetAxis(x, y, z);
		else
		{
			x = sr::Vector3::right;
			y = sr::Vector3::up;
			z = sr::Vector3::front;
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
	sr::Vector2 lastMousePos;
};

#endif // !_TRANSFORM_CONTROLLER_HPP_