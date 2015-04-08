#ifndef _RASTERIZER_CAMERA_CONTROLLER_HPP_
#define _RASTERIZER_CAMERA_CONTROLLER_HPP_

#include "rasterizer.h"

namespace rasterizer
{

class CameraController
{
public:
	static void InitCamera(CameraPtr camera);
	static void UpdateCamera();

	static float moveScale;
	static void MoveCamera(Vector3 delta);

private:
	static void _UpdateCamera(bool force = false);

private:
	static CameraPtr camera;

	static Vector3 position;
	static Vector3 x, y, z;
	static Quaternion rotation;

	static bool dirty;

	static bool isMouseDown;
	static Vector2 lastMousePos;
};

} // namespace rasterizer

#endif // !_RASTERIZER_CAMERA_CONTROLLER_HPP_