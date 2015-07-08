#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include "base/header.h"
#include "base/application.h"
#include "base/canvas.h"
#include "base/input.h"
#include "base/camera.h"
#include "math/mathf.h"
#include "math/transform.h"

#include "rasterizer/mesh.h"
#include "rasterizer/material.h"
#include "rasterizer/texture.h"
#include "rasterizer/texture_cube.h"
#include "rasterizer/render_data.h"
#include "rasterizer/clipper.hpp"
#include "rasterizer/vertex.hpp"
#include "rasterizer/light.hpp"
#include "rasterizer/shader.hpp"
#include "rasterizer/shaderf.hpp"

namespace rasterizer
{

struct RenderState
{
	bool alphaBlend = false;
	enum BlendFactor
	{
		BlendFactor_One,
		BlendFactor_Zero,
		BlendFactor_SrcColor,
		BlendFactor_SrcAlpha,
		BlendFactor_DstColor,
		BlendFactor_DstAlpha,
		BlendFactor_OneMinusSrcColor,
		BlendFactor_OneMinusSrcAlpha,
		BlendFactor_OneMinusDstColor,
		BlendFactor_OneMinusDstAlpha
	};
	BlendFactor srcFactor = BlendFactor_SrcAlpha;
	BlendFactor dstFactor = BlendFactor_OneMinusSrcAlpha;
	const Color Blend(const Color& src, const Color& dst) const;

	enum ZTestType
	{
		ZTestType_Always = 0,
		ZTestType_Less,
		ZTestType_Greater,
		ZTestType_LEqual,
		ZTestType_GEqual,
		ZTestType_Equal,
		ZTestType_NotEqual
	};
	ZTestType zTest = ZTestType_LEqual;
	bool zWrite = true;

	enum CullType
	{
		CullType_Off = 0,
		CullType_Back,
		CullType_Front,
	} cull = CullType_Back;
	bool ZTest(float zPixel, float zInBuffer) const;
};

struct RasterizerInfo
{
	int x, y;
	float depth;
	float wx, wy, wz;
};

struct Rasterizer2x2Info
{
	int x, y;
	uint8_t maskCode;
#if _MATH_SIMD_INTRINSIC_
	SIMD_ALIGN float depth[4];
	SIMD_ALIGN float wx[4], wy[4], wz[4];
#else
	float depth[4];
	float wx[4], wy[4], wz[4];
#endif
};

struct Rasterizer
{
	static RenderState renderState;
	static RenderData renderData;
	static VaryingDataBuffer varyingDataBuffer;

	static Matrix4x4 transform;
	static Canvas* canvas;
	static CameraPtr camera;
    static LightPtr light;
	static MaterialPtr material;
	static ShaderBase* shader;

	static bool isDrawPoint;
	static bool isDrawWireFrame;
	static bool isDrawTextured;
	static int pixelDrawCount;
	static int triangleDrawCount;

	template<typename Type>
	using Render2x2Func = std::function<void(const Type&, const Rasterizer2x2Info&)>;

    static void Initialize();
    
	static void SetCamera(CameraPtr camera);
	static void SetMainLight(LightPtr light);
	static void SetShader(ShaderBase* shader);
	static void SetTransform(const Matrix4x4& transform);
	static void Submit(int startIndex = 0, int primitiveCount = 0);

	static void DrawLine(int x0, int x1, int y0, int y1, const Color32& color);
	template<typename DrawDataType>
	static void RasterizerTriangle(Triangle<Projection> projection, Render2x2Func<DrawDataType> renderFunc, const DrawDataType& renderData);

	static void Rasterizer2x2RenderFunc(const Triangle<VertexVaryingData>& data,  const Rasterizer2x2Info& info);
};

}

#endif // !_RASTERIZER_H_
