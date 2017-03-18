#ifndef _SOFTRENDER_H_
#define _SOFTRENDER_H_

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
#include "softrender/srtypes.hpp"
#include "softrender/light.hpp"
#include "softrender/shader.hpp"
#include "softrender/shaderf.hpp"
#include "softrender/pbsf.hpp"
#include "softrender/rasterizer.hpp"

namespace sr
{

struct SoftRender
{
	static RenderState renderState;
	static RenderData renderData;

	static Matrix4x4 modelMatrix;
	//static Matrix4x4 viewMatrix;
	//static Matrix4x4 projectionMatrix;
	static CameraPtr camera;
    static LightPtr light;

	static void Initialize(int width, int height);
	static void SetRenderTarget(RenderTexturePtr target);
	static RenderTexturePtr GetRenderTarget();
	static void SetShader(ShaderPtr shader);

	static void Clear(bool clearDepth, bool clearColor, const Color& backgroundColor, float depth = 1.0f);
	static void Submit(int startIndex = 0, int primitiveCount = 0);
	static void Present();

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

	static Rasterizer rasterizer;
};

}

#endif // !_SOFTRENDER_H_
