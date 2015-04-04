#ifndef _RASTERIZER_SHADER_H_
#define _RASTERIZER_SHADER_H_

#include "base/header.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"

namespace rasterizer
{

template<typename VSOutputType, typename PSInputType>
struct Shader
{
	Matrix4x4* _MATRIX_MVP = nullptr;
	Matrix4x4* _MATRIX_VIEW = nullptr;
	Matrix4x4* _MATRIX_PROJECTION = nullptr;
	Matrix4x4* _MATRIX_OBJ_TO_WORLD = nullptr;

	virtual void VertexShader(VSOutputType& out) = 0;
	virtual const Color PixelShader(const PSInputType& input) = 0;
};

}

#endif //! _RASTERIZER_SHADER_H_