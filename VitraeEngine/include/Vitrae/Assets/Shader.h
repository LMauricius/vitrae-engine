#pragma once

#include "Vitrae/ComponentRoot.h"
#include "Vitrae/Pipelines/Shading/Task.hpp"
#include "Vitrae/Util/ScopedDict.hpp"

#include "dynasma/cachers/abstract.hpp"
#include "dynasma/core_concepts.hpp"
#include "dynasma/keepers/abstract.hpp"
#include "dynasma/managers/abstract.hpp"
#include "dynasma/pointer.hpp"

#include <filesystem>
#include <variant>

namespace Vitrae
{

class Renderer;

/**
 * @brief The CompiledShader class is a base class for all compiled shaders.
 * An instance of this class is returned by calls to Shader::compile().
 */
class CompiledShader : public dynasma::PolymorphicBase
{
  public:
    virtual ~CompiledShader() = 0;
    virtual std::size_t memory_cost() const = 0;
};

/**
 * @brief The Shader class represents a collection of shader tasks for various components of a
 * rendering pipeline.
 *
 * Currently supported are vertex and fragment shaders.
 */
class Shader : public dynasma::PolymorphicBase
{
    dynasma::FirmPtr<ShaderTask> m_vertexTask;
    dynasma::FirmPtr<ShaderTask> m_fragmentTask;

    std::vector<StringId> m_inputParamNames;

  public:
    struct ShaderParams
    {
        dynasma::LazyPtr<ShaderTask> vertexTask;
        dynasma::LazyPtr<ShaderTask> fragmentTask;
    };

    Shader(const ShaderParams &params);
    ~Shader();

    std::size_t memory_cost() const;

    dynasma::FirmPtr<CompiledShader> compile(const ScopedDict &properties,
                                             ComponentRoot &root) const;
};

struct ShaderManagerSeed
{
    using Asset = Shader;

    inline std::size_t load_cost() const
    {
        return 1;
    }

    std::variant<Shader::ShaderParams> kernel;
};

using ShaderManager = dynasma::AbstractManager<ShaderManagerSeed>;

class ShaderCompilationParams
{
    dynasma::FirmPtr<ShaderTask> m_vertexTask;
    dynasma::FirmPtr<ShaderTask> m_fragmentTask;
    std::map<StringId, Variant> m_compileParameters;
    ComponentRoot &m_root;

    std::size_t m_hash;

    void updateHash();

  public:
    ShaderCompilationParams(dynasma::FirmPtr<ShaderTask> vertexTask,
                            dynasma::FirmPtr<ShaderTask> fragmentTask,
                            std::map<StringId, Variant> &&compileParameters, ComponentRoot &root);

    ShaderCompilationParams(const ShaderCompilationParams &) = default;
    ShaderCompilationParams(ShaderCompilationParams &&) = default;
    ShaderCompilationParams &operator=(const ShaderCompilationParams &) = default;
    ShaderCompilationParams &operator=(ShaderCompilationParams &&) = default;

    inline bool operator<(const ShaderCompilationParams &other) const
    {
        return m_hash < other.m_hash;
    };

    inline dynasma::FirmPtr<ShaderTask> getVertexTask() const { return m_vertexTask; }
    inline dynasma::FirmPtr<ShaderTask> getFragmentTask() const { return m_fragmentTask; }

    inline const std::map<StringId, Variant> &getCompileParameters() const
    {
        return m_compileParameters;
    }

    inline ComponentRoot &getRoot() const { return m_root; }

    inline std::size_t getHash() const { return m_hash; }
};

struct CompiledShaderCacherSeed
{
    using Asset = CompiledShader;

    inline std::size_t load_cost() const
    {
        return 1;
    }
    inline bool operator<(const CompiledShaderCacherSeed &other) const
    {
        return kernel < other.kernel;
    }

    std::variant<ShaderCompilationParams> kernel;
};

using CompiledShaderCacher = dynasma::AbstractCacher<CompiledShaderCacherSeed>;

} // namespace Vitrae