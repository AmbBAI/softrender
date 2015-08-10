#ifndef _SOFTRENDER_LIGHT_HPP_
#define _SOFTRENDER_LIGHT_HPP_

#include "base/header.h"
#include "math/color.h"
#include "math/vector3.h"

namespace sr
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
    float intensity = 1.f;
    
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

	CameraPtr BuildShadowMapVirtualCamera(const Matrix4x4& cameraVPIM, const std::vector<Vector3>& vertexWorldPos)
	{
		Matrix4x4 lightVM = transform.worldToLocalMatrix();

		Vector3 sceneMin, sceneMax;
		for (int i = 0; i < (int)vertexWorldPos.size(); ++i)
		{
			Vector3 lightSpacePos = lightVM.MultiplyPoint3x4(vertexWorldPos[i]);
			if (i == 0) sceneMin = sceneMax = lightSpacePos;
			else
			{
				sceneMin = Vector3::Min(sceneMin, lightSpacePos);
				sceneMax = Vector3::Max(sceneMax, lightSpacePos);
			}
		}

		static Vector3 frustumCornerProjection[8] =
		{
			Vector3(-1.f, -1.f, 0.f), Vector3(-1.f, -1.f, 1.f),
			Vector3(-1.f, 1.f, 0.f), Vector3(-1.f, 1.f, 1.f),
			Vector3(1.f, -1.f, 0.f), Vector3(1.f, -1.f, 1.f),
			Vector3(1.f, 1.f, 0.f), Vector3(1.f, 1.f, 1.f)
		};

		Vector3 frustumMin, frustumMax;
		for (int i = 0; i < 8; ++i)
		{
			Vector4 fcWorldPos = cameraVPIM.MultiplyPoint(frustumCornerProjection[i]);
			Vector3 fcLightPos = lightVM.MultiplyPoint3x4(fcWorldPos.xyz / fcWorldPos.w);
			if (i == 0) frustumMin = frustumMax = fcLightPos;
			else
			{
				frustumMin = Vector3::Min(frustumMin, fcLightPos);
				frustumMax = Vector3::Max(frustumMax, fcLightPos);
			}
		}

		float zNear = sceneMin.z, zFar = sceneMax.z;
		float left = Mathf::Max(frustumMin.x, sceneMin.x);
		float right = Mathf::Min(frustumMax.x, sceneMax.x);
		float bottom = Mathf::Max(frustumMin.y, sceneMin.y);
		float top = Mathf::Min(frustumMax.y, sceneMax.y);

		CameraPtr out = std::make_shared<Camera>();
		out->transform = transform;
		out->SetOrthographic(left, right, bottom, top, zNear, zFar);
		return out;
	}

#if _NOCRASH_ && defined(_MSC_VER)
	MEMALIGN_NEW_OPERATOR_OVERRIDE(16)
	MEMALIGN_DELETE_OPERATOR_OVERRIDE
#endif
};

}

#endif //!_SOFTRENDER_LIGHT_HPP_
