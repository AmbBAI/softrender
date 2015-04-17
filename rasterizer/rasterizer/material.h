#ifndef _RASTERIZER_MATERIAL_HPP_
#define _RASTERIZER_MATERIAL_HPP_

#include "base/header.h"
#include "base/color.h"
#include "rasterizer/texture.h"
#include "tinyobjloader/tiny_obj_loader.h"


namespace rasterizer
{

struct Material;
typedef std::shared_ptr<Material> MaterialPtr;

struct Material
{
	Color ambient;
	Color diffuse;
	Color specular;
	float shininess;
	Color emission;
	
	TexturePtr ambientTexture;
	TexturePtr diffuseTexture;
	TexturePtr normalTexture;
	TexturePtr specularTexture;

    static void LoadMaterial(std::vector<MaterialPtr>& materials, const std::vector<tinyobj::material_t>& objMaterials, const char* fileDir);
    
#if _MATH_SIMD_INTRINSIC_ && defined(_MSC_VER)
	MEMALIGN_NEW_OPERATOR_OVERRIDE(16)
	MEMALIGN_DELETE_OPERATOR_OVERRIDE
#endif
};

}

#endif //!_RASTERIZER_MATERIAL_HPP_
