#include "softrender.h"
using namespace sr;

void decompose(std::string file, int size, BitmapPtr& lfMap, BitmapPtr& hfMap)
{
	std::string prefix = "resources/";
	prefix += file;
	Texture2DPtr tex = Texture2D::LoadTexture((prefix + ".png").c_str());
	auto input = tex->GetBitmap(0);
	float gap = (1.0 / size) * 0.5;

	BitmapPtr erodeMap = std::make_shared<Bitmap>(size, size, Bitmap::BitmapType_RGBA32);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			float x = gap * (i * 2 + 1) * 2.0 - 1.0;
			float y = gap * (j * 2 + 1) * 2.0 - 1.0;
			if (x * x + y * y > 1.0) erodeMap->SetPixel(i, j, tex->GetBitmap(0)->GetPixel(i, j));
			else
			{
				Color outColor = Color::white;
				Vector3 np(x, y, Mathf::Sqrt(1.0 - x * x - y * y));
				for (int ii = 0; ii < size; ii++)
				{
					for (int jj = 0; jj < size; jj++)
					{
						float xx = gap * (ii * 2 + 1) * 2.0 - 1.0;
						float yy = gap * (jj * 2 + 1) * 2.0 - 1.0;
						if (xx * xx + yy * yy > 1.0) continue;
						Vector3 nq(xx, yy, Mathf::Sqrt(1.0 - xx * xx - yy * yy));

						float np_dot_nq = np.Dot(nq);
						if (np_dot_nq <= 0) continue;
						Color qColor = input->GetPixel(ii, jj);
						Color tmpColor = qColor * (1.0 / np_dot_nq);
						outColor = Color::Min(tmpColor, outColor);
					}
				}
				erodeMap->SetPixel(i, j, outColor);
			}
		}
	}

	input = erodeMap;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			float x = gap * (i * 2 + 1) * 2.0 - 1.0;
			float y = gap * (j * 2 + 1) * 2.0 - 1.0;
			if (x * x + y * y > 1.0) lfMap->SetPixel(i, j, tex->GetBitmap(0)->GetPixel(i, j));
			else
			{
				Color outColor = Color::black;
				Vector3 np(x, y, Mathf::Sqrt(1.0 - x * x - y * y));
				for (int ii = 0; ii < size; ii++)
				{
					for (int jj = 0; jj < size; jj++)
					{
						float xx = gap * (ii * 2 + 1) * 2.0 - 1.0;
						float yy = gap * (jj * 2 + 1) * 2.0 - 1.0;
						if (xx * xx + yy * yy > 1.0) continue;
						Vector3 nq(xx, yy, Mathf::Sqrt(1.0 - xx * xx - yy * yy));

						float np_dot_nq = np.Dot(nq);
						if (np_dot_nq <= 0) continue;
						Color qColor = input->GetPixel(ii, jj);
						Color tmpColor = qColor * np_dot_nq;
						outColor = Color::Max(tmpColor, outColor);
					}
				}
				lfMap->SetPixel(i, j, outColor);
			}
		}
	}
	lfMap->SaveToFile((prefix + "_lf.png").c_str());

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			float x = gap * (i * 2 + 1) * 2.0 - 1.0;
			float y = gap * (j * 2 + 1) * 2.0 - 1.0;
			if (x * x + y * y > 1.0) hfMap->SetPixel(i, j, tex->GetBitmap(0)->GetPixel(i, j));
			else
			{
				Color outColor = tex->GetBitmap(0)->GetPixel(i, j) - input->GetPixel(i, j);
				outColor.a = 1.f;
				hfMap->SetPixel(i, j, outColor);
			}
		}
	}
	hfMap->SaveToFile((prefix + "_hf.png").c_str());
}

void balance(int size, BitmapPtr& texMap)
{
	float gap = (1.0 / size) * 0.5;
	Color a0 = Color::black;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			float x = gap * (i * 2 + 1) * 2.0 - 1.0;
			float y = gap * (j * 2 + 1) * 2.0 - 1.0;
			if (x * x + y * y > 1.0) continue;
			else
			{
				a0 = Color::Max(texMap->GetPixel(i, j), a0);
			}
		}
	}
	a0.a = 1.0;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			float x = gap * (i * 2 + 1) * 2.0 - 1.0;
			float y = gap * (j * 2 + 1) * 2.0 - 1.0;
			if (x * x + y * y > 1.0) continue;
			else
			{
				texMap->SetPixel(i, j, texMap->GetPixel(i, j) / a0);
			}
		}
	}
}

void balance(std::string file, int size, BitmapPtr& lfMap, BitmapPtr& hfMap)
{
	std::string prefix = "resources/";
	prefix += file;
	balance(size, lfMap);
	balance(size, hfMap);
	lfMap->SaveToFile((prefix + "_lfb.png").c_str());
	hfMap->SaveToFile((prefix + "_hfb.png").c_str());
}

int main(int argc, char *argv[])
{
	BitmapPtr lfMap = std::make_shared<Bitmap>(128, 128, Bitmap::BitmapType_RGBA32);
	BitmapPtr hfMap = std::make_shared<Bitmap>(128, 128, Bitmap::BitmapType_RGBA32);
	decompose("matcap", 128, lfMap, hfMap);
	balance("matcap", 128, lfMap, hfMap);
	return 0;
}


