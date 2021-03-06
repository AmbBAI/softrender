#include "softrender.h"

namespace sr
{

CameraPtr SoftRender::camera = nullptr;
LightPtr SoftRender::light = nullptr;
ShaderPtr SoftRender::shader = nullptr;
RenderTexturePtr SoftRender::defaultRenderTarget = nullptr;
RenderTexturePtr SoftRender::renderTarget = nullptr;
BitmapPtr SoftRender::colorBuffer = nullptr;
BitmapPtr SoftRender::depthBuffer = nullptr;
StencilBufferPtr SoftRender::stencilBuffer = nullptr;
Matrix4x4 SoftRender::modelMatrix;
RenderState SoftRender::renderState;
RenderData SoftRender::renderData;
VaryingDataBuffer SoftRender::varyingDataBuffer;
Rasterizer SoftRender::rasterizer;

void SoftRender::Initialize(int width, int height)
{
    Texture2D::Initialize();

	defaultRenderTarget = std::make_shared<RenderTexture>(width, height);
	SetRenderTarget(defaultRenderTarget);
}

void SoftRender::SetRenderTarget(RenderTexturePtr target)
{
	if (target == nullptr) renderTarget = defaultRenderTarget;
	else renderTarget = target;

	colorBuffer = renderTarget->GetColorBuffer();
	depthBuffer = renderTarget->GetDepthBuffer();
}

RenderTexturePtr SoftRender::GetRenderTarget()
{
	return renderTarget;
}

void SoftRender::ClearStencilBuffer(uint8_t stencil)
{
	if (stencilBuffer == nullptr)
	{
		stencilBuffer = std::make_shared<StencilBuffer>(renderTarget->GetWidth(), renderTarget->GetHeight());
	}
	stencilBuffer->Fill(stencil);
}

StencilBufferPtr SoftRender::GetStencilBuffer()
{
	return stencilBuffer;
}

void SoftRender::Submit(int startIndex/* = 0*/, int primitiveCount/* = 0*/)
{
	assert(camera != nullptr);

	int width = renderTarget->GetWidth();
	int height = renderTarget->GetHeight();

	shader->_WorldToCamera = camera->viewMatrix();
	shader->_CameraToWorld = camera->transform.localToWorldMatrix();

	shader->_MATRIX_P = camera->projectionMatrix();
	shader->_MATRIX_VP = shader->_MATRIX_P.Multiply(shader->_WorldToCamera);
	shader->_Object2World = modelMatrix;
	shader->_World2Object = modelMatrix.Inverse();
	shader->_MATRIX_MV = shader->_WorldToCamera.Multiply(modelMatrix);
	shader->_MATRIX_MVP = shader->_MATRIX_VP.Multiply(modelMatrix);

	shader->_WorldSpaceCameraPos = camera->transform.position;
	shader->_ScreenParams = Vector4((float)width, (float)height, 1.f + 1.f / (float)width, 1.f + 1.f / (float)height);
	shader->_ZBufferParams = Vector4(camera->zFar(), camera->zNear(), 0.f, 0.f);

	InitShaderLightParams(shader, light);

	rasterizer.Initlize(width, height);
	varyingDataBuffer.InitVaryingDataBuffer(shader->varyingDataSize);

	int vertexCount = renderData.GetVertexCount();
	varyingDataBuffer.InitVerticesVaryingData(vertexCount);
	for (int i = 0; i < vertexCount; ++i)
	{
		VertexVaryingData& varyingData = varyingDataBuffer.GetVertexVaryingData(i);
		shader->varyingData = varyingData.data;
		rawptr_t vertexData = renderData.GetVertexData<uint8_t>(i);
		shader->_VSMain(vertexData);

		varyingData.position = *Buffer::Value<Vector4>(shader->varyingData, 0);
		varyingData.clipCode = Clipper::CalculateClipCode(varyingData.position);
	}

	varyingDataBuffer.InitDynamicVaryingData();
	varyingDataBuffer.InitPixelVaryingData(4);

	if (primitiveCount <= 0) primitiveCount = renderData.GetPrimitiveCount() - startIndex;
	for (int i = 0; i < primitiveCount; ++i)
	{
		int primitiveIndex = i + startIndex;
		Triangle<uint16_t> triangleIdx;
		if (!renderData.GetTrianglePrimitive(primitiveIndex, triangleIdx))
		{
			assert(false);
			continue;
		}

		auto v0 = varyingDataBuffer.GetVertexVaryingData(triangleIdx.v0);
		auto v1 = varyingDataBuffer.GetVertexVaryingData(triangleIdx.v1);
		auto v2 = varyingDataBuffer.GetVertexVaryingData(triangleIdx.v2);

		//if (renderState.FaceCulling(v0, v1, v2)) continue;

		varyingDataBuffer.ResetDynamicVaryingData();
		auto triangles = Clipper::ClipTriangle(v0, v1, v2);
		for (auto& triangle : triangles)
		{
			Triangle<Projection> projection;
			projection.v0 = Projection::CalculateViewProjection(triangle.v0.position, width, height);
			projection.v1 = Projection::CalculateViewProjection(triangle.v1.position, width, height);
			projection.v2 = Projection::CalculateViewProjection(triangle.v2.position, width, height);
			if (renderState.FaceCullingSimple(projection, triangle)) continue;

			if (camera->projectionMode() == Camera::ProjectionMode_Perspective)
			{
				projection.v0.z = camera->GetLinearDepth(projection.v0.z);
				projection.v1.z = camera->GetLinearDepth(projection.v1.z);
				projection.v2.z = camera->GetLinearDepth(projection.v2.z);
			}

			rasterizer.RasterizerTriangle<Triangle<VertexVaryingData> >(projection, Rasterizer2x2RenderFunc, triangle);
		}


		/* // draw wireframe
		std::vector<Line<VertexVaryingData> > lines;
		auto l1 = Clipper::ClipLine(v0, v1);
		lines.insert(lines.end(), l1.begin(), l1.end());
		auto l2 = Clipper::ClipLine(v0, v2);
		lines.insert(lines.end(), l2.begin(), l2.end());
		auto l3 = Clipper::ClipLine(v1, v2);
		lines.insert(lines.end(), l3.begin(), l3.end());

		for (auto& line : lines)
		{
			Projection p0 = Projection::CalculateViewProjection(line.v0.position, width, height);
			Projection p1 = Projection::CalculateViewProjection(line.v1.position, width, height);
			rasterizer.DrawLine(p0.x, p1.x, p0.y, p1.y, Color::blue);
		}
		*/
	}

	/* draw point
	for (int i = 0; i < vertexCount; ++i)
	{
		auto data = varyingDataBuffer.GetVertexVaryingData(i);
		if (data.clipCode != 0) continue;

		Projection proj = Projection::CalculateViewProjection(data.position, width, height);
		canvas->SetPixel(proj.x, proj.y, Color::red);
	}
	*/
}

void SoftRender::SetShader(ShaderPtr shader)
{
	assert(shader != nullptr);
	SoftRender::shader = shader;
}

void SoftRender::RasterizerRenderFunc(const VertexVaryingData& data, const RasterizerInfo& info)
{
	int x = info.x;
	int y = info.y;

	if (!renderState.ZTest(info.depth, depthBuffer->GetAlpha(x, y))) return;

	if (renderState.stencilOn) 
	{
		uint8_t stencilContent = stencilBuffer->GetStencil(x, y);
		if (!renderState.StencilTest(stencilContent)) return;
		stencilBuffer->SetStencil(x, y, renderState.WriteStencil(stencilContent));
	}

	shader->varyingData = data.data;
	shader->isClipped = false;
	shader->SV_Target0 = Color::clear;
	shader->SV_Target1 = Color::clear;
	shader->SV_Target2 = Color::clear;
	shader->SV_Target3 = Color::clear;
	shader->_PSMain();
	if (!shader->isClipped)
	{
		if (renderState.alphaBlend)
		{
			auto buffer = renderTarget->GetColorBuffer();
			buffer->SetPixel(x, y, renderState.Blend(shader->SV_Target0, buffer->GetPixel(x, y)));

			for (int k = 0; k < 3; ++k)
			{
				buffer = renderTarget->GetGBuffer(k);
				if (buffer)
				{
					buffer->SetPixel(x, y, renderState.Blend(ShaderGBufferOutput(shader, k), buffer->GetPixel(x, y)));
				}
			}
		}
		else
		{
			auto buffer = renderTarget->GetColorBuffer();
			buffer->SetPixel(x, y, shader->SV_Target0);

			for (int k = 0; k < 3; ++k)
			{
				buffer = renderTarget->GetGBuffer(k);
				if (buffer)
				{
					buffer->SetPixel(x, y, ShaderGBufferOutput(shader, k));
				}
			}
		}
		if (renderState.zWrite) depthBuffer->SetAlpha(x, y, info.depth);
	}
}

void SoftRender::Rasterizer2x2RenderFunc(const Triangle<VertexVaryingData>& data, const Rasterizer2x2Info& quad)
{
	static int quadX[4] = { 0, 1, 0, 1 };
	static int quadY[4] = { 0, 0, 1, 1 };

	rawptr_t pixelVaryingDataQuad[4];
	for (int i = 0; i < 4; ++i)
	{
		pixelVaryingDataQuad[i] = VertexVaryingData::TriangleInterp(i, data.v0, data.v1, data.v2, quad.wx[i], quad.wy[i], quad.wz[i]);
	}
	shader->_PassQuad(pixelVaryingDataQuad);

	for (int i = 0; i < 4; ++i)
	{
		if (!(quad.maskCode & (1 << i))) continue;

		int x = quad.x + quadX[i];
		int y = quad.y + quadY[i];

		if (!renderState.ZTest(quad.depth[i], depthBuffer->GetAlpha(x, y))) continue;

		if (renderState.stencilOn)
		{
			uint8_t stencilContent = stencilBuffer->GetStencil(x, y);
			if (!renderState.StencilTest(stencilContent)) return;
			stencilBuffer->SetStencil(x, y, renderState.WriteStencil(stencilContent));
		}

		shader->varyingData = pixelVaryingDataQuad[i];
		shader->isClipped = false;
		shader->SV_Target0 = Color::clear;
		shader->SV_Target1 = Color::clear;
		shader->SV_Target2 = Color::clear;
		shader->SV_Target3 = Color::clear;
		shader->_PSMain();
		if (!shader->isClipped)
		{
			if (renderState.alphaBlend)
			{
				auto buffer = renderTarget->GetColorBuffer();
				buffer->SetPixel(x, y, renderState.Blend(shader->SV_Target0, buffer->GetPixel(x, y)));

				for (int k = 0; k < 3; ++k)
				{
					buffer = renderTarget->GetGBuffer(k);
					if (buffer)
					{
						buffer->SetPixel(x, y, renderState.Blend(ShaderGBufferOutput(shader, k), buffer->GetPixel(x, y)));
					}
				}
			}
			else
			{
				auto buffer = renderTarget->GetColorBuffer();
				buffer->SetPixel(x, y, shader->SV_Target0);

				for (int k = 0; k < 3; ++k)
				{
					buffer = renderTarget->GetGBuffer(k);
					if (buffer)
					{
						buffer->SetPixel(x, y, ShaderGBufferOutput(shader, k));
					}
				}
			}
			if (renderState.zWrite) depthBuffer->SetAlpha(x, y, quad.depth[i]);
		}
	}
}

const Color& SoftRender::ShaderGBufferOutput(ShaderPtr& shader, int index)
{
	switch (index)
	{
	case 0:
		return shader->SV_Target1;
	case 1:
		return shader->SV_Target2;
	case 2:
		return shader->SV_Target3;
	default:
		throw std::out_of_range("g-buffer index out of range!");
	}
}

bool SoftRender::InitShaderLightParams(ShaderPtr shader, const LightPtr& light)
{
	if (light == nullptr)
	{
		shader->_WorldSpaceLightPos = Vector4(0.f, 0.f, 1.f, 0.f);
		shader->_LightColor = Color::black;
		return false;
	}

	light->Initilize();
	if (light->type == Light::LightType_Directional)
	{
		shader->_WorldSpaceLightPos = Vector4(light->transform.forward(), 0.f);
	}
	else
	{
		shader->_WorldSpaceLightPos = Vector4(light->transform.position, 1.f);
		if (light->type == Light::LightType_Spot)
		{
			shader->_SpotLightDir = Vector3(light->transform.forward());
			shader->_SpotLightParams = Vector3(light->cosHalfPhi, light->cosHalfTheta, light->falloff);
		}
		else
		{
			shader->_SpotLightParams = Vector3(-1.f, -1.f, 0.f);
		}
	}
	shader->_LightColor = light->color;
	shader->_LightColor *= light->intensity;
	shader->_LightAtten = Vector4(light->atten0, light->atten1, light->atten2, light->range);
	return true;
}

void SoftRender::Clear(bool clearColor, bool clearDepth, const Color& backgroundColor, float depth /*= 1.0f*/)
{
	if (clearColor) colorBuffer->Fill(backgroundColor);
	if (clearDepth) depthBuffer->Fill(Color(depth, 0.f, 0.f, 0.f));
}

void SoftRender::Present()
{
	int width = colorBuffer->GetWidth();
	int height = colorBuffer->GetHeight();
	rawptr_t bytes = colorBuffer->GetBytes();
	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glFlush();
}

}