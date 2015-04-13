#include "camera_controller.h"

namespace rasterizer
{

CameraPtr CameraController::camera = nullptr;
Vector3 CameraController::position = Vector3(0.f, 0.f, 0.f);
Quaternion CameraController::rotation = Quaternion(0.f, 0.f, 0.f, 1.f);
Vector3 CameraController::x = Vector3(1.f, 0.f, 0.f);
Vector3 CameraController::y = Vector3(0.f, 1.f, 0.f);
Vector3 CameraController::z = Vector3(0.f, 0.f, -1.f);
bool CameraController::dirty = false;
bool CameraController::isMouseDown = false;
rasterizer::Vector2 CameraController::lastMousePos = Vector2::zero;
float CameraController::moveScale = 5.f;

void CameraController::InitCamera(CameraPtr camera)
{
	CameraController::camera = camera;
	if (camera != nullptr)
	{
		camera->SetPerspective(90.f, 1.33333f, 0.3f, 2000.f);
		//camera->SetOrthographic(-100.f, 100.f, -100.f, 100.f, 0.3f, 1000.f);

		_UpdateCamera(true);
	}
}

void CameraController::UpdateCamera()
{
	Application* app = Application::GetInstance();
	Input* input = app->GetInput();

	if (input->GetKey(GLFW_KEY_W)) MoveCamera(+z * moveScale);
	if (input->GetKey(GLFW_KEY_S)) MoveCamera(-z * moveScale);
	if (input->GetKey(GLFW_KEY_A)) MoveCamera(-x * moveScale);
	if (input->GetKey(GLFW_KEY_D)) MoveCamera(+x * moveScale);
	if (input->GetKey(GLFW_KEY_Q)) MoveCamera(+y * moveScale);
	if (input->GetKey(GLFW_KEY_E)) MoveCamera(-y * moveScale);

	if (input->GetMouseButton(0))
	{
		Vector2 nowMousePos = input->GetMousePos();
		if (isMouseDown)
		{
			Vector2 moveDelta = nowMousePos - lastMousePos;
			rotation = rotation.Multiply(Vector3(moveDelta.y, moveDelta.x, 0));
			z = rotation.Rotate(Vector3::front).Normalize();
			x = Vector3::down.Cross(z).Normalize();
			y = x.Cross(z);
			dirty = true;
		}
		lastMousePos = nowMousePos;
		isMouseDown = true;
	}
	else isMouseDown = false;

	_UpdateCamera();
}

void CameraController::MoveCamera(Vector3 delta)
{
	position += delta;
	dirty = true;
}

void CameraController::_UpdateCamera(bool force /*= false*/)
{
	if (dirty || force)
	{
		camera->SetLookAt(position, position + z, y);
		dirty = false;
	}
}

}