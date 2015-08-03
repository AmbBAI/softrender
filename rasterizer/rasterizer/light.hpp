#ifndef _RASTERIZER_LIGHT_HPP_
#define _RASTERIZER_LIGHT_HPP_

#include "base/header.h"
#include "base/color.h"
#include "math/vector3.h"

namespace rasterizer
{

struct Light;
typedef std::shared_ptr<Light> LightPtr;

struct Light
{
    enum LightType
    {
        LightType_Directional,
        LightType_Point,
        LightType_Spot
    };
    LightType type = LightType_Directional;
	Color color = Color(1.f, 1.f, 1.f, 1.f);
    float intensity = 0.5f;
    
	Transform transform;
    float range = 10.f;
    float atten0 = 0.f;
    float atten1 = 0.f;
    float atten2 = 1.f;
    float falloff = 1.f;
    float theta = 30.f;
    float phi = 45.f;
    
    float cosHalfTheta;
    float cosHalfPhi;
    
    void Initilize()
    {
        if (type == LightType_Spot)
        {
            cosHalfPhi = Mathf::Cos(phi * Mathf::deg2rad / 2.f);
            cosHalfTheta = Mathf::Cos(theta * Mathf::deg2rad / 2.f);
        }
    }

	Matrix4x4 CalcShadowMapMatrix(const CameraPtr& camera)
	{
		Matrix4x4 viewMatrix = camera->viewMatrix();
		Matrix4x4 projectionMatrix = camera->projectionMatrix();
		Matrix4x4 VPIM = projectionMatrix.Multiply(viewMatrix).Inverse();
		Matrix4x4 LVM = transform.worldToLocalMatrix();

		static Vector3 frustumCornerProjection[8] =
		{
			Vector3(-1.f, -1.f, 0.f), Vector3(-1.f, -1.f, 1.f),
			Vector3(-1.f, 1.f, 0.f), Vector3(-1.f, 1.f, 1.f),
			Vector3(1.f, -1.f, 0.f), Vector3(1.f, -1.f, 1.f),
			Vector3(1.f, 1.f, 0.f), Vector3(1.f, 1.f, 1.f)
		};

		Vector3 minPos, maxPos;
		for (int i = 0; i < 8; ++i)
		{
			Vector4 fcWorldPos = VPIM.MultiplyPoint(frustumCornerProjection[i]);
			Vector3 fcLightPos = LVM.MultiplyPoint3x4(fcWorldPos.xyz / fcWorldPos.w);
			//printf("wp %f %f %f %f\n", fcWorldPos.x, fcWorldPos.y, fcWorldPos.z, fcWorldPos.w);
			//printf("lp %f %f %f\n", fcLightPos.x, fcLightPos.y, fcLightPos.z);
			if (i == 0) minPos = maxPos = fcLightPos;
			else
			{
				minPos = Vector3::Min(minPos, fcLightPos);
				maxPos = Vector3::Max(maxPos, fcLightPos);
			}
		}

		return Matrix4x4::Orthographic(minPos.x, maxPos.x, minPos.y, maxPos.y, minPos.z, maxPos.z);
	}

#if _NOCRASH_ && defined(_MSC_VER)
	MEMALIGN_NEW_OPERATOR_OVERRIDE(16)
	MEMALIGN_DELETE_OPERATOR_OVERRIDE
#endif
};

}

#endif //!_RASTERIZER_LIGHT_HPP_
