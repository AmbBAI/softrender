#ifndef _RASTERIZER_MATERIAL_HPP_
#define _RASTERIZER_MATERIAL_HPP_

#include "base/header.h"
#include "math/color.h"
#include "rasterizer/texture2d.h"
#include "tinyobjloader/tiny_obj_loader.h"


namespace rasterizer
{

struct Material;
typedef std::shared_ptr<Material> MaterialPtr;

struct Material
{
	std::string name;
    Color ambient = Color::black;
	Color diffuse = Color::white;
	Color specular = Color::white;
	float shininess = 10.f;
	Color emission = Color::white;
	float alpha = 1.f;
	bool isTransparent = false;

	Texture2DPtr ambientTexture;
	Texture2DPtr diffuseTexture;
	Texture2DPtr normalTexture;
	Texture2DPtr specularTexture;
	Texture2DPtr alphaMaskTexture;
	

    static void LoadMaterial(std::vector<MaterialPtr>& materials, const std::vector<tinyobj::material_t>& objMaterials, const char* fileDir);
    
#if _NOCRASH_ && defined(_MSC_VER)
	MEMALIGN_NEW_OPERATOR_OVERRIDE(16)
	MEMALIGN_DELETE_OPERATOR_OVERRIDE
#endif
};

}

#endif //!_RASTERIZER_MATERIAL_HPP_
