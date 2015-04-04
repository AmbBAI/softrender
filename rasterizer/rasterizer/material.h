#ifndef _BASE_MATERIAL_H_
#define _BASE_MATERIAL_H_

#include "base/header.h"
#include "base/color.h"
#include "rasterizer/texture.h"

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
};

}

#endif //!_BASE_MATERIAL_H_
