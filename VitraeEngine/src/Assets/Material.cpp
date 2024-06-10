#include "Vitrae/Assets/Material.hpp"
#include "Vitrae/ComponentRoot.hpp"

namespace Vitrae
{

Material::Material(const AssimpLoadParams &params)
{
    TextureManager &textureManager = params.root.getComponent<TextureManager>();

    // get shading type
    aiShadingMode aiMode;
    unsigned int aiModeSize = 1;
    params.p_extMaterial->Get(AI_MATKEY_SHADING_MODEL, &aiMode, &aiModeSize);
    if (aiModeSize != 1) {
        throw std::runtime_error("Invalid aiModeSize");
    }
    const ComponentRoot::AiMaterialShadingInfo &shadingInfo =
        params.root.getAiMaterialShadingInfo(aiMode);

    m_vertexMethod = shadingInfo.vertexMethod;
    m_fragmentMethod = shadingInfo.fragmentMethod;

    // Get all textures
    for (auto &textureInfo : params.root.getAiMaterialTextureInfos()) {
        for (int i = 0; i < params.p_extMaterial->GetTextureCount(textureInfo.aiTextureId); i++) {
            aiString path;
            params.p_extMaterial->GetTexture(textureInfo.aiTextureId, i, &path);
            m_textures[textureInfo.textureNameId] = textureManager.register_asset(
                {Texture::FileLoadParams{.filepath = path.C_Str(), .root = params.root}});
        }
    }
}

Material::~Material() {}

std::size_t Material::memory_cost() const
{
    /// TODO: caculate real cost
    return sizeof(Material);
}

dynasma::FirmPtr<Method<ShaderTask>> Material::getVertexMethod() const
{
    return m_vertexMethod;
}

dynasma::FirmPtr<Method<ShaderTask>> Material::getFragmentMethod() const
{
    return m_fragmentMethod;
}

const std::map<StringId, dynasma::LazyPtr<Texture>> &Material::getTextures() const
{
    return m_textures;
}

} // namespace Vitrae