#ifndef _RASTERIZER_RENDER_DATA_H_
#define _RASTERIZER_RENDER_DATA_H_

#include "base/header.h"
#include "rasterizer/vertex.hpp"
#include "rasterizer/light.hpp"
#include "rasterizer/material.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"

namespace rasterizer
{


template <typename VertexType>
struct RenderVertexData
{
	u32 meshIndex;
	Vector3 position;
	Vector3 normal;
	Vector4 tangent;
	Vector2 texcoord;
	VertexType vertex;
};

template <typename VertexType>
struct RenderTriangleData
{
	u32 meshIndex;
	MaterialPtr material = nullptr;
	Triangle<VertexType> vertices;
	Triangle<Projection> projection;
};

template <typename VertexType>
struct RenderMeshData
{
	Matrix4x4 _MATRIX_MVP;
	Matrix4x4 _MATRIX_MV;
	Matrix4x4 _MATRIX_V;
	Matrix4x4 _MATRIX_P;
	Matrix4x4 _MATRIX_VP;
	Matrix4x4 _Object2World;
	Matrix4x4 _World2Object;
	LightPtr light = nullptr;
	CameraPtr camera = nullptr;

	std::vector<RenderVertexData<VertexType> > vertices;
	std::vector<RenderTriangleData<VertexType> > triangles;
};

template <typename PixelType>
struct RenderPixelData
{
	u32 meshIndex;
	u32 triangleIndex;

	u32 x, y;
	float depth;
	PixelType pixel;
	Vector2 ddx, ddy;
};

template<typename VertexType, typename PixelType, u32 RenderListCount>
struct RenderData
{
	std::vector<RenderMeshData<VertexType> > meshes;
	std::vector<RenderPixelData<PixelType> > renderList[RenderListCount];
};

}

#endif // !_RASTERIZER_RENDER_DATA_H_
