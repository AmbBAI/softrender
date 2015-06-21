#ifndef _CAMERA_CONTROLLER_HPP_
#define _CAMERA_CONTROLLER_HPP_

#include "rasterizer.h"

class CameraController
{
public:
	static void InitCamera(rasterizer::CameraPtr camera);
	static void UpdateCamera();

	static float moveScale;
	static void MoveCamera(rasterizer::Vector3 delta);

private:
	static void _UpdateCamera(bool force = false);

private:
	static rasterizer::CameraPtr camera;

	static rasterizer::Vector3 position;
	static rasterizer::Quaternion rotation;
	static rasterizer::Vector3 x, y, z;

	static bool dirty;

	static bool isMouseDown;
	static rasterizer::Vector2 lastMousePos;
};

#endif // !_CAMERA_CONTROLLER_HPP_