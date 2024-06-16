#include "Vitrae/Assets/Material.hpp"
#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Util/StringProcessing.hpp"

namespace Vitrae
{

Material::Material(const AssimpLoadParams &params)
{
    TextureManager &textureManager = params.root.getComponent<TextureManager>();

    std::filesystem::path parentDirPath = params.sceneFilepath.parent_path();

    // get shading type
    aiShadingMode aiMode;
    if (params.p_extMaterial->Get(AI_MATKEY_SHADING_MODEL, aiMode) != aiReturn_SUCCESS) {
        aiMode = aiShadingMode_Phong;
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

            String relconvPath =
                searchAndReplace(searchAndReplace(path.C_Str(), "\\", "/"), "//", "/");

            m_textures[textureInfo.textureNameId] = textureManager.register_asset(
                {Texture::FileLoadParams{.root = params.root,
                                         .filepath = parentDirPath / relconvPath,
                                         .useMipMaps = true}});
        }
    }
}

Material::~Material() {}

std::size_t Material::memory_cost() const
{
    /// TODO: caculate real cost
    return sizeof(Material);
}

void Material::setMethods(dynasma::FirmPtr<Method<ShaderTask>> vertexMethod,
                          dynasma::FirmPtr<Method<ShaderTask>> fragmentMethod)
{
    m_vertexMethod = vertexMethod;
    m_fragmentMethod = fragmentMethod;
}

dynasma::FirmPtr<Method<ShaderTask>> Material::getVertexMethod() const
{
    return m_vertexMethod;
}

dynasma::FirmPtr<Method<ShaderTask>> Material::getFragmentMethod() const
{
    return m_fragmentMethod;
}

const std::map<StringId, dynasma::FirmPtr<Texture>> &Material::getTextures() const
{
    return m_textures;
}

} // namespace Vitrae