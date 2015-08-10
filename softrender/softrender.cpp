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
Matrix4x4 SoftRender::modelMatrix;
//Matrix4x4 Rasterizer::viewMatrix;
//Matrix4x4 Rasterizer::projectionMatrix;
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

int Orient2D(int x0, int y0, int x1, int y1, int x2, int y2)
{
	return (x1 - x0) * (y2 - y1) - (y1 - y0) * (x2 - x1);
}

void SoftRender::Submit(int startIndex/* = 0*/, int primitiveCount/* = 0*/)
{
	assert(camera != nullptr);

	int width = renderTarget->GetWidth();
	int height = renderTarget->GetHeight();

	//shader->_MATRIX_V = viewMatrix;
	//shader->_MATRIX_P = projectionMatrix;
	shader->_MATRIX_V = camera->viewMatrix();
	shader->_MATRIX_P = camera->projectionMatrix();
	shader->_MATRIX_VP = shader->_MATRIX_P.Multiply(shader->_MATRIX_V);
	shader->_Object2World = modelMatrix;
	shader->_World2Object = modelMatrix.Inverse();
	shader->_MATRIX_MV = shader->_MATRIX_V.Multiply(modelMatrix);
	shader->_MATRIX_MVP = shader->_MATRIX_VP.Multiply(modelMatrix);

	shader->_WorldSpaceCameraPos = camera->transform.position;
	shader->_ScreenParams = Vector4((float)width, (float)height, 1.f + 1.f / (float)width, 1.f + 1.f / (float)height);

	InitShaderLightParams(shader, light);

	rasterizer.Initlize(width, height);

	int vertexCount = renderData.GetVertexCount();
	varyingDataBuffer.InitVerticesVaryingData(vertexCount);
	for (int i = 0; i < vertexCount; ++i)
	{
		VertexVaryingData& varyingData = varyingDataBuffer.GetVertexVaryingData(i);
		shader->varyingData = varyingData.data;
		rawptr_t vertexData = renderData.GetVertexData<uint8_t>(i);
		shader->_VSMain(vertexData);

		auto decl = varyingDataBuffer.GetVaryingDataDecl();
		varyingData.position = *Buffer::Value<Vector4>(shader->varyingData, decl.positionOffset);
		varyingData.clipCode = Clipper::CalculateClipCode(varyingData.position);
	}

	varyingDataBuffer.InitDynamicVaryingData();
	varyingDataBuffer.InitPixelVaryingData();

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

		varyingDataBuffer.ResetDynamicVaryingData();
		
		auto triangles = Clipper::ClipTriangle(v0, v1, v2);
		for (auto& triangle : triangles)
		{
			Triangle<Projection> projection;
			projection.v0 = Projection::CalculateViewProjection(triangle.v0.position, width, height);
			projection.v1 = Projection::CalculateViewProjection(triangle.v1.position, width, height);
			projection.v2 = Projection::CalculateViewProjection(triangle.v2.position, width, height);

			int ret = Orient2D(projection.v0.x, projection.v0.y, projection.v2.x, projection.v2.y, projection.v1.x, projection.v1.y);
			switch (renderState.cull)
			{
			case RenderState::CullType_Front:
				if (ret >= 0) continue;
			case RenderState::CullType_Off:
				if (ret == 0) continue;
				if (ret < 0)
				{
					std::swap(projection.v1, projection.v2);
					std::swap(triangle.v1, triangle.v2);
				}
				break;
			case RenderState::CullType_Back:
				if (ret <= 0) continue;
				break;
			default:
				break;
			}

			if (camera->projectionMode() == Camera::ProjectionMode_Perspective)
			{
				projection.v0.z = camera->GetLinearDepth(projection.v0.z);
				projection.v1.z = camera->GetLinearDepth(projection.v1.z);
				projection.v2.z = camera->GetLinearDepth(projection.v2.z);
			}

			varyingDataBuffer.ResetPixelVaryingData();
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

	varyingDataBuffer.SetVaryingDataDecl(shader->varyingDataDecl, shader->varyingDataSize);
}

void SoftRender::RasterizerRenderFunc(const VertexVaryingData& data, const RasterizerInfo& info)
{
	int x = info.x;
	int y = info.y;

	if (!renderState.ZTest(info.depth, depthBuffer->GetAlpha(x, y))) return;

	shader->varyingData = data.data;
	shader->isClipped = false;
	Color color = shader->_PSMain();
	if (!shader->isClipped)
	{
		if (renderState.alphaBlend)
		{
			Color dst = colorBuffer->GetPixel(x, y);
			color = renderState.Blend(color, dst);
		}
		colorBuffer->SetPixel(x, y, color);
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
		pixelVaryingDataQuad[i] = VertexVaryingData::TriangleInterp(data.v0, data.v1, data.v2, quad.wx[i], quad.wy[i], quad.wz[i]);
	}
	shader->_PassQuad(pixelVaryingDataQuad);

	for (int i = 0; i < 4; ++i)
	{
		if (!(quad.maskCode & (1 << i))) continue;

		int x = quad.x + quadX[i];
		int y = quad.y + quadY[i];

		if (!renderState.ZTest(quad.depth[i], depthBuffer->GetAlpha(x, y))) continue;

		shader->varyingData = pixelVaryingDataQuad[i];
		shader->isClipped = false;
		Color color = shader->_PSMain();
		if (!shader->isClipped)
		{
			if (renderState.alphaBlend)
			{
				Color dst = colorBuffer->GetPixel(x, y);
				color = renderState.Blend(color, dst);
			}
			colorBuffer->SetPixel(x, y, color);
			if (renderState.zWrite) depthBuffer->SetAlpha(x, y, quad.depth[i]);
		}
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

void SoftRender::Clear(bool clearDepth, bool clearColor, const Color& backgroundColor, float depth /*= 1.0f*/)
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