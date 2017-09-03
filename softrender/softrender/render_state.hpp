#ifndef _SOFTRENDER_RENDER_STATE_H_
#define _SOFTRENDER_RENDER_STATE_H_

#include "base/header.h"
#include "math/color.h"
#include "blender.hpp"
#include "srtypes.hpp"

namespace sr
{

struct RenderState
{
	enum RenderType
	{
		RenderType_Point,
		RenderType_WireFrame,
		RenderType_Stardand,
		RenderType_ShadowPrePass
	};
	RenderType renderType = RenderType_Stardand;

	bool alphaBlend = false;
	Blender blender;

	Color Blend(const Color& src, const Color& dst) const
	{
		return blender.Blend(src, dst);
	}

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
	};
	CullType cull = CullType_Back;


	enum StencilComparison
	{
		StencilComparison_Greater,
		StencilComparison_GEqual,
		StencilComparison_Less,
		StencilComparison_LEqual,
		StencilComparison_Equal,
		StencilComparison_NotEqual,
		StencilComparison_Always,
		StencilComparison_Never,
	};

	enum StencilOperation
	{
		StencilOperation_Keep,
		StencilOperation_Zero,
		StencilOperation_Replace,
	};

	bool stencilOn = false;
	uint8_t stencilRefValue = 0;
	StencilComparison stencilComp = StencilComparison_Always;
	StencilOperation stencilOp = StencilOperation_Replace;

	bool StencilTest(float stencil) const
	{
		switch (stencilComp)
		{
		case RenderState::StencilComparison_Greater:
			return stencil > stencilRefValue;
		case RenderState::StencilComparison_GEqual:
			return stencil >= stencilRefValue;
		case RenderState::StencilComparison_Less:
			return stencil < stencilRefValue;
		case RenderState::StencilComparison_LEqual:
			return stencil <= stencilRefValue;
		case RenderState::StencilComparison_Equal:
			return stencil == stencilRefValue;
		case RenderState::StencilComparison_NotEqual:
			return stencil != stencilRefValue;
		case RenderState::StencilComparison_Never:
			return false;
		case RenderState::StencilComparison_Always:
		default:
			return true;
		}
	}

	uint8_t WriteStencil(uint8_t stencil) const
	{
		switch (stencilOp)
		{
		case RenderState::StencilOperation_Keep:
			return stencil;
		case RenderState::StencilOperation_Zero:
			return (uint8_t)0;
		case RenderState::StencilOperation_Replace:
		default:
			return stencilRefValue;
		}
	}

	template <typename Type>
	float _DeterminantOfCullingMatrix(const Type& v0, const Type& v1, const Type& v2) const
	{
		return (v1.x - v0.x) * (v2.y - v0.y) * v0.z
				- (v2.x - v0.x) * (v1.y - v0.y) * v0.z
				+ (v2.x - v0.x) * v0.y * (v1.z - v0.z)
				- (v1.x - v0.x) * v0.y * (v2.z - v0.z)
				+ v0.x * (v1.y - v0.y) * (v2.z - v0.z)
				- v0.x * (v2.y - v0.y) * (v1.z - v0.z);
	}

	template <typename Type>
	bool FaceCulling(const Type& v0, Type& v1, Type& v2) const
	{
		float det = _DeterminantOfCullingMatrix(v0.position, v1.position, v2.position);
		switch (cull)
		{
		case RenderState::CullType_Front:
			if (det <= 0.f) return true;
			std::swap(v1, v2);
			return false;
		case RenderState::CullType_Back:
			return det >= 0.f;
		case RenderState::CullType_Off:
			if (det > 0.f) std::swap(v1, v2);
			return false;
		default:
			return false;
		}
	}

	template <typename Type>
	bool FaceCullingSimple(Triangle<Projection>& p, Triangle<Type>& v) const
	{
		int ret = Projection::Orient2D(p.v0, p.v1, p.v2);

		switch (cull)
		{
		case RenderState::CullType_Front:
			if (ret <= 0) return true;
			std::swap(p.v1, p.v2);
			std::swap(v.v1, v.v2);
			return false;
		case RenderState::CullType_Back:
			return ret >= 0;
		case RenderState::CullType_Off:
			if (ret > 0)
			{
				std::swap(p.v1, p.v2);
				std::swap(v.v1, v.v2);
			}
			return false;
		default:
			return false;
		}
	}

	bool ZTest(float zPixel, float zInBuffer) const
	{
		switch (zTest)
		{
		case RenderState::ZTestType_Always:
			return true;
		case RenderState::ZTestType_Less:
			return zPixel < zInBuffer;
		case RenderState::ZTestType_Greater:
			return zPixel > zInBuffer;
		case RenderState::ZTestType_LEqual:
			return zPixel <= zInBuffer;
		case RenderState::ZTestType_GEqual:
			return zPixel >= zInBuffer;
		case RenderState::ZTestType_Equal:
			return zPixel == zInBuffer;
		case RenderState::ZTestType_NotEqual:
			return zPixel != zInBuffer;
		default:
			break;
		}
		return false;
	}
};


} // namespace sr

#endif // !_SOFTRENDER_RENDER_STATE_H_
