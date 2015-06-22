#ifndef _RASTERIZER_SHADER_H_
#define _RASTERIZER_SHADER_H_

#include "base/header.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"
#include "shaderf.hpp"

namespace rasterizer
{
namespace shader
{

struct ShaderBase
{
	RenderData* renderData;

	VaryingDataDecl decl;
	VertexOutData* vertexOut = nullptr;
	PixelInData* pixelIn = nullptr;

	Matrix4x4 _MATRIX_MVP;
	Matrix4x4 _MATRIX_MV;
	Matrix4x4 _MATRIX_V;
	Matrix4x4 _MATRIX_P;
	Matrix4x4 _MATRIX_VP;
	Matrix4x4 _Object2World;
	Matrix4x4 _World2Object;

	virtual void vsMain(const void* input) = 0;
	virtual Color psMain() = 0;
};

template <typename VSInputType, typename VaryingDataType>
struct Shader : ShaderBase
{
	void vsMain(const void* input) override
	{
		VSInputType* vertexInput = (VSInputType*)input;
		*((VaryingDataType*)vertexOut->data) = vs(*vertexInput);
		vertexOut->clipCode = Clipper::CalculateClipCode(vertexOut->GetPosition());
	}

	Color psMain() override
	{
		return frag(*(VaryingDataType*)(pixelIn->data));
	}

	virtual VaryingDataType vert(const VSInputType& input)
	{
		VaryingDataType output;
		output.position = Rasterizer::_MATRIX_MVP.Multiply(input.position);
		return output;
	}

	virtual Color frag(const VaryingDataType& input)
	{
		return Color::white;
	}
};

} // namespace shader
} // namespace rasterizer

#endif //! _RASTERIZER_SHADER_H_