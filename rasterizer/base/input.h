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
	Input(GLFWwindow* window);
	~Input() = default;

public:
	bool GetKey(int key);
	bool GetMouseButton(int button);
	Vector2 GetMousePos();

private:
    GLFWwindow* window = nullptr;
};

}

#endif // !_BASE_INPUT_H_
