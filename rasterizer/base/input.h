#ifndef _BASE_INPUT_H_
#define _BASE_INPUT_H_

#include "header.h"
#include "application.h"
#include "math/vector2.h"

namespace rasterizer
{

class Input
{
	friend Application;
	explicit Input(GLFWwindow* window);

public:
	bool GetKey(int key);
	bool GetMouseButton(int button);
	Vector2 GetMousePos();

private:
    GLFWwindow* window = nullptr;
};

}

#endif // !_BASE_INPUT_H_
