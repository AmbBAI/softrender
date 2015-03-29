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

void CameraController::InitCamera(CameraPtr camera)
{
	CameraController::camera = camera;
	if (camera != nullptr)
	{
		camera->SetPerspective(90, 1, 0.3f, 10000);
		//camera.SetOrthographic(-100, 100, -100, 100, 0.3f, 1000);

		_UpdateCamera(true);
	}
}

void CameraController::UpdateCamera()
{
	Application* app = Application::GetInstance();
	Input* input = app->GetInput();

	if (input->GetKey(GLFW_KEY_W)) MoveCamera(+z);
	if (input->GetKey(GLFW_KEY_S)) MoveCamera(-z);
	if (input->GetKey(GLFW_KEY_A)) MoveCamera(-x);
	if (input->GetKey(GLFW_KEY_D)) MoveCamera(+x);
	if (input->GetKey(GLFW_KEY_Q)) MoveCamera(+y);
	if (input->GetKey(GLFW_KEY_E)) MoveCamera(-y);

	//if (input->GetMouseButton(0))
	//{
	//	Vector2 nowMousePos = input->GetMousePos();
	//	if (isMouseDown)
	//	{
	//		Vector2 moveDelta = nowMousePos - lastMousePos;
	//		Matrix4x4 cameraMat = camera->GetViewMatrix()->Inverse();
	//		Vector3 rotateVector = cameraMat.MultiplyVector(Vector3(moveDelta.y, moveDelta.x, 0.f));
	//		rotation = Quaternion(rotateVector).Multiply(rotation);
	//		x = rotation.Rotate(Vector3::right);
	//		y = rotation.Rotate(Vector3::up);
	//		z = rotation.Rotate(Vector3::front);
	//		dirty = true;
	//	}
	//	lastMousePos = nowMousePos;
	//	isMouseDown = true;
	//}
	//else isMouseDown = false;

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

float CameraController::rotationX = 0.f;
float CameraController::rotationY = 0.f;

}