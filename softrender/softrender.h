#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include "base/header.h"
#include "base/application.h"
#include "base/input.h"

#include "math/mathf.h"
#include "math/transform.h"

#include "softrender/camera.h"
#include "softrender/mesh.h"
#include "softrender/material.h"
#include "softrender/texture2d.h"
#include "softrender/cubemap.h"
#include "softrender/render_texture.h"
#include "softrender/render_state.hpp"
#include "softrender/render_data.hpp"
#include "softrender/varying_data.h"
#include "softrender/clipper.hpp"
#include "softrender/vertex.hpp"
#include "softrender/light.hpp"
#include "softrender/shader.hpp"
#include "softrender/shaderf.hpp"

namespace sr
{

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

struct SoftRender
{
	static RenderState renderState;
	static RenderData renderData;

	static Matrix4x4 modelMatrix;
	//static Matrix4x4 viewMatrix;
	//static Matrix4x4 projectionMatrix;
	static CameraPtr camera;
    static LightPtr light;

	template<typename Type>
	using Render2x2Func = std::function<void(const Type&, const Rasterizer2x2Info&)>;

	static void Initialize(int width, int height);
	static void SetRenderTarget(RenderTexturePtr target);
	static RenderTexturePtr GetRenderTarget();
	static void SetShader(ShaderPtr shader);

	static void Clear(bool clearDepth, bool clearColor, const Color& backgroundColor, float depth = 1.0f);
	static void Submit(int startIndex = 0, int primitiveCount = 0);
	static void Present();

	static void DrawLine(int x0, int x1, int y0, int y1, const Color32& color);
	template<typename DrawDataType>
	static void RasterizerTriangle(Triangle<Projection> projection, Render2x2Func<DrawDataType> renderFunc, const DrawDataType& renderData);

private:
	static bool InitShaderLightParams(ShaderPtr shader, const LightPtr& light);
	static void RasterizerRenderFunc(const VertexVaryingData& data, const RasterizerInfo& info);
	static void Rasterizer2x2RenderFunc(const Triangle<VertexVaryingData>& data,  const Rasterizer2x2Info& info);

	static VaryingDataBuffer varyingDataBuffer;
	static ShaderPtr shader;

	static RenderTexturePtr defaultRenderTarget;
	static RenderTexturePtr renderTarget;
	static BitmapPtr colorBuffer;
	static BitmapPtr depthBuffer;

};

}

#endif // !_RASTERIZER_H_
