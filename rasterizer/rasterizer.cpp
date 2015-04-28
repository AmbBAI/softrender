#include "rasterizer.h"

namespace rasterizer
{

Canvas* Rasterizer::canvas = nullptr;
CameraPtr Rasterizer::camera;
rasterizer::MaterialPtr Rasterizer::material;
rasterizer::LightPtr Rasterizer::light;
rasterizer::Shader0 Rasterizer::shader;
std::vector<Rasterizer::RenderBlock> Rasterizer::renderList;


bool Rasterizer::isDrawTextured = true;
bool Rasterizer::isDrawWireFrame = false;
bool Rasterizer::isDrawPoint = false;


void Rasterizer::Initialize()
{
    Texture::Initialize();
}
    
void TestColor(Canvas* canvas)
{
	int w = canvas->GetWidth();
	int h = canvas->GetHeight();

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			canvas->SetPixel(x, y,
				Color(1.f, (float)x / w, (float)y / h, 0.f));
		}
	}
}

void TestTexture(Canvas* canvas, const Vector4& rect, const Texture& texture, float lod /*= 0.f*/)
{
	int width = (int)canvas->GetWidth();
	int height = (int)canvas->GetHeight();

	int minX = Mathf::Clamp(Mathf::FloorToInt(rect.x), 0, width);
	int maxX = Mathf::Clamp(Mathf::FloorToInt(rect.z), 0, width);
	int minY = Mathf::Clamp(Mathf::CeilToInt(rect.y), 0, height);
	int maxY = Mathf::Clamp(Mathf::CeilToInt(rect.w), 0, height);

	for (int y = minY; y < maxY; ++y)
	{
		float v = ((float)y - rect.y) / (rect.w - rect.y);
		for (int x = minX; x < maxX; ++x)
		{
			float u = ((float)x - rect.x) / (rect.z - rect.x);
			canvas->SetPixel(x, y, texture.Sample(u, v, lod));
		}
	}
}

void Rasterizer::DrawLine(int x0, int x1, int y0, int y1, const Color32& color)
{
	bool steep = Mathf::Abs(y1 - y0) > Mathf::Abs(x1 - x0);
	if (steep)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int deltaX = x1 - x0;
	int deltaY = Mathf::Abs(y1 - y0);
	int error = deltaX / 2;
	int yStep = -1;
	if (y0 < y1) yStep = 1;

	for (int x = x0, y = y0; x < x1; ++x)
	{
		steep ? canvas->SetPixel(y, x, color) : canvas->SetPixel(x, y, color);
		error = error - deltaY;
		if (error < 0)
		{
			y = y + yStep;
			error = error + deltaX;
		}
	}
}

int Rasterizer::Orient2D(int x0, int y0, int x1, int y1, int x2, int y2)
{
	return (x1 - x0) * (y2 - y1) - (y1 - y0) * (x2 - x1);
}

void Rasterizer::DrawTriangle(const Projection& p0, const Projection& p1, const Projection& p2, const Triangle<VertexStd>& triangle)
{
	int minX = Mathf::Min(p0.x, p1.x, p2.x);
	int minY = Mathf::Min(p0.y, p1.y, p2.y);
	int maxX = Mathf::Max(p0.x, p1.x, p2.x);
	int maxY = Mathf::Max(p0.y, p1.y, p2.y);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();
	if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= width) maxX = width - 1;
	if (maxY >= height) maxY = height - 1;

	if (maxX < minX) return;
	if (maxY < minY) return;

	int dx01 = p0.x - p1.x;
	int dx12 = p1.x - p2.x;
	int dx20 = p2.x - p0.x;

	int dy01 = p0.y - p1.y;
	int dy12 = p1.y - p2.y;
	int dy20 = p2.y - p0.y;

	int startW0 = Orient2D(p0.x, p0.y, p1.x, p1.y, minX, minY);
	int startW1 = Orient2D(p1.x, p1.y, p2.x, p2.y, minX, minY);
	int startW2 = Orient2D(p2.x, p2.y, p0.x, p0.y, minX, minY);

	if (dy01 < 0 || (dy01 == 0 && dx01 < 0)) startW0 += 1;
	if (dy12 < 0 || (dy12 == 0 && dx12 < 0)) startW1 += 1;
	if (dy20 < 0 || (dy20 == 0 && dx20 < 0)) startW2 += 1;

    static int quadX[4] = {0, 1, 0, 1};
    static int quadY[4] = {0, 0, 1, 1};

#if _MATH_SIMD_INTRINSIC_
	static __m128 _mf_one = _mm_set1_ps(1.f);

	__m128i mi_w0_delta = _mm_setr_epi32(0, dy01, -dx01, dy01 - dx01);
	__m128i mi_w1_delta = _mm_setr_epi32(0, dy12, -dx12, dy12 - dx12);
	__m128i mi_w2_delta = _mm_setr_epi32(0, dy20, -dx20, dy20 - dx20);

	__m128 mf_p0_invW = _mm_load1_ps(&(p0.invW));
	__m128 mf_p1_invW = _mm_load1_ps(&(p1.invW));
	__m128 mf_p2_invW = _mm_load1_ps(&(p2.invW));

	SIMD_ALIGN float f_x[4], f_y[4], f_z[4];
	SIMD_ALIGN float f_depth[4];
#else
    int i_w0_delta[4] = {0, dy01, -dx01, dy01 - dx01};
    int i_w1_delta[4] = {0, dy12, -dx12, dy12 - dx12};
    int i_w2_delta[4] = {0, dy20, -dx20, dy20 - dx20};
    
	float f_depth[4];
#endif
	PSInput quad[4];

	for (int y = minY; y <= maxY; y+=2)
	{
		int w0 = startW0;
		int w1 = startW1;
		int w2 = startW2;

		for (int x = minX; x <= maxX; x+=2)
		{
            u8 maskCode = 0x00;
#if _MATH_SIMD_INTRINSIC_
			__m128i mi_w0 = _mm_add_epi32(_mm_set1_epi32(w0), mi_w0_delta);
			__m128i mi_w1 = _mm_add_epi32(_mm_set1_epi32(w1), mi_w1_delta);
			__m128i mi_w2 = _mm_add_epi32(_mm_set1_epi32(w2), mi_w2_delta);
            
            __m128i mi_or_w = _mm_or_si128(_mm_or_si128(mi_w0, mi_w1), mi_w2);
            if (_mm_extract_epi32(mi_or_w, 0) > 0) maskCode |= 0x1;
            if (_mm_extract_epi32(mi_or_w, 1) > 0) maskCode |= 0x2;
            if (_mm_extract_epi32(mi_or_w, 2) > 0) maskCode |= 0x4;
            if (_mm_extract_epi32(mi_or_w, 3) > 0) maskCode |= 0x8;
			if (maskCode != 0)
			{
				__m128 mf_w0 = _mm_cvtepi32_ps(mi_w0);
				__m128 mf_w1 = _mm_cvtepi32_ps(mi_w1);
				__m128 mf_w2 = _mm_cvtepi32_ps(mi_w2);

				__m128 mf_tmp0 = _mm_mul_ps(mf_w1, mf_p0_invW);
				__m128 mf_tmp1 = _mm_mul_ps(mf_w2, mf_p1_invW);
				__m128 mf_tmp2 = _mm_mul_ps(mf_w0, mf_p2_invW);
				//__m128 mf_invw = _mm_rcp_ps(_mm_add_ps(_mm_add_ps(mf_x, mf_y), mf_z));
				__m128 mf_invSum = _mm_div_ps(_mf_one, _mm_add_ps(_mm_add_ps(mf_tmp0, mf_tmp1), mf_tmp2));
				_mm_store_ps(f_x, _mm_mul_ps(mf_tmp0, mf_invSum));
				_mm_store_ps(f_y, _mm_mul_ps(mf_tmp1, mf_invSum));
				_mm_store_ps(f_z, _mm_mul_ps(mf_tmp2, mf_invSum));

				__m128 mf_depth = _mm_mul_ps(_mm_add_ps(mf_w0, _mm_add_ps(mf_w1, mf_w2)), mf_invSum);
				_mm_store_ps(f_depth, mf_depth);
                
                for (int i = 0; i < 4; ++i)
                {
#else
            int i_w0[4], i_w1[4], i_w2[4];
            for (int i = 0; i < 4; ++i)
            {
                i_w0[i] = w0 + i_w0_delta[i];
                i_w1[i] = w1 + i_w1_delta[i];
                i_w2[i] = w2 + i_w2_delta[i];
                if ((i_w0[i] | i_w1[i] | i_w2[i]) > 0) maskCode |= (1<<i);
            }
                
            if (maskCode != 0)
            {
                for (int i=0; i<4; ++i)
                {
					float f_w0 = (float)i_w0[i];
					float f_w1 = (float)i_w1[i];
					float f_w2 = (float)i_w2[i];

					float f_x = f_w1 * p0.invW;
					float f_y = f_w2 * p1.invW;
					float f_z = f_w0 * p2.invW;
					float f_invSum = 1.f / (f_x + f_y + f_z);
					f_x *= f_invSum;
					f_y *= f_invSum;
					f_z *= f_invSum;

					f_depth[i] = (f_w0 + f_w1 + f_w2) * f_invSum;
#endif
					f_depth[i] = camera->GetLinearDepth(f_depth[i]);
                    quad[i] = PSInput(triangle.v0, triangle.v1, triangle.v2, f_x, f_y, f_z);
				}

				Vector2 ddx = Shader0::CalcDDX(quad);
				Vector2 ddy = Shader0::CalcDDY(quad);

				for (int i = 0; i < 4; ++i)
				{
					if (maskCode & (1 << i))
					{
						RenderBlock block;
						block.x = x + quadX[i];
						block.y = y + quadY[i];
						block.material = material;
						block.depth = f_depth[i];
						block.ddx = ddx;
						block.ddy = ddy;
						block.input = quad[i];
						renderList.push_back(block);
					}
				}
			}


			w0 += dy01 * 2;
			w1 += dy12 * 2;
			w2 += dy20 * 2;
		}

		startW0 -= dx01 * 2;
		startW1 -= dx12 * 2;
		startW2 -= dx20 * 2;
	}
}

void Rasterizer::DrawMesh(const Mesh& mesh, const Matrix4x4& transform)
{
	assert(canvas != nullptr);
	assert(camera != nullptr);

	//Matrix4x4 mvp = (*projection).Multiply((*view).Multiply(transform));
	shader._MATRIX_V = *camera->GetViewMatrix();
	shader._MATRIX_P = *camera->GetProjectionMatrix();
	shader._MATRIX_VP = shader._MATRIX_P.Multiply(shader._MATRIX_V);
	shader._Object2World = transform;
	shader._World2Object = transform.Inverse();
	shader._MATRIX_MV = shader._MATRIX_V.Multiply(transform);
	shader._MATRIX_MVP = shader._MATRIX_VP.Multiply(transform);
	shader.material = material;
	shader.light = light;
	shader.camera = camera;

	u32 width = canvas->GetWidth();
	u32 height = canvas->GetHeight();

	int vertexN = (int)mesh.vertices.size();
	std::vector<VertexStd> vertices;
	vertices.resize(vertexN);

	//TODO vertex buff
	int i = 0;
	//#pragma omp parallel for private(i)
	for (i = 0; i < vertexN; ++i)
	{
		shader.position = &mesh.vertices[i];
		shader.normal = &mesh.normals[i];
		if (mesh.tangents.size() != 0) shader.tangent = &mesh.tangents[i];
		if (mesh.texcoords.size() != 0) shader.texcoord = &mesh.texcoords[i];
		shader.VertexShader(vertices[i]);

		if (isDrawPoint)
		{
			if (!vertices[i].clipCode)
			{
				Projection point = Projection::CalculateViewProjection(vertices[i].hc, width, height);
				canvas->SetPixel(point.x, point.y, Color::red);
			}
		}
	}

	//TODO Z pre-pass
	int faceN = (int)mesh.indices.size() / 3;
	//#pragma omp parallel for private(i)
	for (i = 0; i < faceN; ++i)
	{
		int i0 = mesh.indices[i * 3];
		int i1 = mesh.indices[i * 3 + 1];
		int i2 = mesh.indices[i * 3 + 2];

		if (isDrawTextured)
		{
			//TODO Guard-band clipping
			auto triangles = Clipper::ClipTriangle(vertices[i0], vertices[i1], vertices[i2]);
			for (auto& triangle : triangles)
			{
				Projection p0 = triangle.v0.GetViewProjection(width, height);
				Projection p1 = triangle.v1.GetViewProjection(width, height);
				Projection p2 = triangle.v2.GetViewProjection(width, height);

				if (Orient2D(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y) < 0.f) continue;

				DrawTriangle(p0, p1, p2, triangle);

				//DrawLine(p0.x, p1.x, p0.y, p1.y, Color::green);
				//DrawLine(p1.x, p2.x, p1.y, p2.y, Color::green);
				//DrawLine(p2.x, p0.x, p2.y, p0.y, Color::green);
			}
		}

		if (isDrawWireFrame)
		{
			std::vector<Line<VertexStd> > lines;
			auto l1 = Clipper::ClipLine(vertices[i0], vertices[i1]);
			lines.insert(lines.end(), l1.begin(), l1.end());
			auto l2 = Clipper::ClipLine(vertices[i0], vertices[i2]);
			lines.insert(lines.end(), l2.begin(), l2.end());
			auto l3 = Clipper::ClipLine(vertices[i1], vertices[i2]);
			lines.insert(lines.end(), l3.begin(), l3.end());

			for (auto& line : lines)
			{
				Projection p0 = line.v0.GetViewProjection(width, height);
				Projection p1 = line.v1.GetViewProjection(width, height);
				DrawLine(p0.x, p1.x, p0.y, p1.y, Color::blue);
			}
		}
	}

	if (isDrawPoint)
	{
		for (i = 0; i < vertexN; ++i)
		{
			if (!vertices[i].clipCode)
			{
				Projection point = Projection::CalculateViewProjection(vertices[i].hc, width, height);
				canvas->SetPixel(point.x, point.y, Color::red);
			}
		}
	}

	Vector4 hc = shader._MATRIX_VP.MultiplyPoint(light->position);
	u32 clipCode = Clipper::CalculateClipCode(hc);
	if (0 == clipCode)
	{
		Projection point = Projection::CalculateViewProjection(hc, width, height);
		canvas->SetPixel(point.x, point.y, Color::green);
	}
}

void Rasterizer::PrepareRender()
{
	renderList.clear();
}

void Rasterizer::Render()
{
	std::vector<u32> indices(renderList.size(), 0);
	for (u32 i = 0; i < indices.size(); ++i) indices[i] = i;

	std::sort(indices.begin(), indices.end(), [](const u32& a, const u32& b)
	{
		return renderList[a].depth < renderList[b].depth;
	});

	for (auto& idx : indices)
	{
		RenderBlock& block = renderList[idx];
		if (block.depth < canvas->GetDepth(block.x, block.y))
		{
			canvas->SetDepth(block.x, block.y, block.depth);

			shader.material = block.material;
			shader.ddx = block.ddx;
			shader.ddy = block.ddy;
			Color color = shader.PixelShader(block.input);
			canvas->SetPixel(block.x, block.y, color);
		}
	}
}

}