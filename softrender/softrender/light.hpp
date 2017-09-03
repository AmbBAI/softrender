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
    float atten0 = 1.f;
    float atten1 = 0.f;
    float atten2 = 25.f;
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

	Matrix4x4 GetProjectionToWorldSpaceMatrix(const CameraPtr& camera)
	{
		return transform.worldToLocalMatrix() * (camera->projectionMatrix() * camera->viewMatrix()).Inverse();
	}

	void GetSceneBoundsInLightSpace(std::vector<Vector3>& worldPoints, Vector3& sceneMin, Vector3& sceneMax)
	{
		Matrix4x4 worldToLightSpace = transform.worldToLocalMatrix();
		for (int i = 0; i < (int)worldPoints.size(); ++i)
		{
			Vector3 lightSpacePos = worldToLightSpace.MultiplyPoint3x4(worldPoints[i]);
			if (i == 0)
			{
				sceneMin = sceneMax = lightSpacePos;
			}
			else
			{
				sceneMin = Vector3::Min(sceneMin, lightSpacePos);
				sceneMax = Vector3::Max(sceneMax, lightSpacePos);
			}
		}
	}

	void GetFrustumBoundsInLightSpace(const Matrix4x4& cameraProjectionToLightSpace, Vector3& frustumMin, Vector3& frustumMax)
	{
		static Vector3 frustumCornerProjection[8] =
		{
			Vector3(-1.f, -1.f, 0.f), Vector3(-1.f, -1.f, 1.f),
			Vector3(-1.f, 1.f, 0.f), Vector3(-1.f, 1.f, 1.f),
			Vector3(1.f, -1.f, 0.f), Vector3(1.f, -1.f, 1.f),
			Vector3(1.f, 1.f, 0.f), Vector3(1.f, 1.f, 1.f)
		};

		for (int i = 0; i < 8; ++i)
		{
			Vector4 lightSpacePosHC = cameraProjectionToLightSpace.MultiplyPoint(frustumCornerProjection[i]);
			Vector3 lightSpacePos = lightSpacePosHC.xyz / lightSpacePosHC.w;
			if (i == 0) frustumMin = frustumMax = lightSpacePos;
			else
			{
				frustumMin = Vector3::Min(frustumMin, lightSpacePos);
				frustumMax = Vector3::Max(frustumMax, lightSpacePos);
			}
		}
	}

	CameraPtr BuildShadowMapCamera(const Vector3& frustumMin, const Vector3& frustumMax, const Vector3& sceneMin, const Vector3& sceneMax)
	{
		float zNear = sceneMin.z;
		float zFar = Mathf::Min(sceneMax.z, frustumMax.z);
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
