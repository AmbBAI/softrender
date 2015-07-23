#include "material.h"

namespace rasterizer {

void Material::LoadMaterial(std::vector<MaterialPtr>& materials, const std::vector<tinyobj::material_t>& objMaterials, const char* fileDir)
{
    materials.clear();
    for (auto& m : objMaterials)
    {
        //MaterialPtr newM = std::make_shared<Material>();
		MaterialPtr newM = MaterialPtr(new Material());
		newM->name = m.name;
		newM->ambient = Color(1.f, m.ambient[0], m.ambient[1], m.ambient[2]);
        newM->diffuse = Color(1.f, m.diffuse[0], m.diffuse[1], m.diffuse[2]);
        newM->specular = Color(1.f, m.specular[0], m.specular[1], m.specular[2]);
        newM->shininess = m.shininess;
        newM->emission = Color(1.f, m.emission[0], m.emission[1], m.emission[2]);
        if (m.diffuse_texname.size() > 0)
        {
            std::string texPath = fileDir + m.diffuse_texname;
            std::replace(texPath.begin(), texPath.end(), '\\', '/');
            newM->diffuseTexture = Texture2D::LoadTexture(texPath.c_str());
            newM->diffuseTexture->GenerateMipmaps();
            //newM->diffuseTexture->filterMode = Texture::FilterMode_Trilinear;
        }
        if (m.normal_texname.size() > 0)
        {
            std::string texPath = fileDir + m.normal_texname;
            std::replace(texPath.begin(), texPath.end(), '\\', '/');
            newM->normalTexture = Texture2D::LoadTexture(texPath.c_str());
        }
        else
        { // check bump
            auto paramItor = m.unknown_parameter.find("map_bump");
            if (paramItor == m.unknown_parameter.end()) paramItor = m.unknown_parameter.find("bump");
            if (paramItor != m.unknown_parameter.end())
            {
                float bumpMultiply = 1.f;
                std::string bumpPath;
                
                std::string param = paramItor->second;
                if (param.find("-bm") == 0)
                {
                    std::stringstream ss(param.substr(4));
                    ss >> bumpMultiply >> bumpPath;
                }
                else
                {
                    bumpPath = param;
                }
                
                if (bumpPath.length() > 0)
                {
                    bumpPath = fileDir + bumpPath;
                    std::replace(bumpPath.begin(), bumpPath.end(), '\\', '/');
                    newM->normalTexture = Texture2D::LoadTexture(bumpPath.c_str());
                    if (newM->normalTexture)
                    {
                        newM->normalTexture->ConvertBumpToNormal();
                    }
                }
            }
        }
		if (newM->normalTexture != nullptr) newM->normalTexture->GenerateMipmaps();

        if (m.specular_texname.size() > 0)
        {
            std::string texPath = fileDir + m.specular_texname;
            std::replace(texPath.begin(), texPath.end(), '\\', '/');
            newM->specularTexture = Texture2D::LoadTexture(texPath.c_str());
        }

		// alpha_mask
		auto paramItor = m.unknown_parameter.find("map_d");
		if (paramItor != m.unknown_parameter.end())
		{
			std::string texPath = fileDir + paramItor->second;
			std::replace(texPath.begin(), texPath.end(), '\\', '/');
			newM->alphaMaskTexture = Texture2D::LoadTexture(texPath.c_str());
		}
		//newM->alpha = m.dissolve;

		newM->isTransparent = false;
		if (newM->alpha < 1.f - Mathf::epsilon || newM->alphaMaskTexture != nullptr)
		{
			newM->isTransparent = true;
		}
        
        materials.push_back(newM);
    }
}

}