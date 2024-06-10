#pragma once

#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Pipelines/Method.hpp"
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
 * @brief The SurfaceShader class represents a collection of shader tasks for various components of
 * a rendering pipeline.
 *
 * Currently supported are vertex and fragment shaders.
 */
class SurfaceShader : public dynasma::PolymorphicBase
{
    dynasma::FirmPtr<Method<ShaderTask>> m_vertexMethod;
    dynasma::FirmPtr<Method<ShaderTask>> m_fragmentMethod;

    std::vector<StringId> m_inputParamNames;

  public:
    struct SetupParams
    {
        dynasma::LazyPtr<Method<ShaderTask>> vertexMethod;
        dynasma::LazyPtr<Method<ShaderTask>> fragmentMethod;
    };

    SurfaceShader(const SetupParams &params);
    ~SurfaceShader();

    std::size_t memory_cost() const;

    dynasma::FirmPtr<CompiledShader> compile(const ScopedDict &properties,
                                             ComponentRoot &root) const;
};

struct SurfaceShaderManagerSeed
{
    using Asset = SurfaceShader;

    inline std::size_t load_cost() const { return 1; }

    std::variant<SurfaceShader::SetupParams> kernel;
};

using SurfaceShaderManager = dynasma::AbstractManager<SurfaceShaderManagerSeed>;

/**
 * @brief The CompiledShader class is a base class for all compiled shaders.
 * An instance of this class is returned by calls to SurfaceShader::compile().
 */
class CompiledSurfaceShader : public dynasma::PolymorphicBase
{
  public:
    virtual ~CompiledSurfaceShader() = 0;
    virtual std::size_t memory_cost() const = 0;

    struct CompilationParams
    {
        dynasma::FirmPtr<Method<ShaderTask>> m_vertexMethod;
        dynasma::FirmPtr<Method<ShaderTask>> m_fragmentMethod;
        ComponentRoot *mp_root;

        bool operator==(const CompilationParams &other) const = default;
        bool operator!=(const CompilationParams &other) const = default;
        bool operator<(const CompilationParams &other) const = default;
        bool operator>(const CompilationParams &other) const = default;
        bool operator<=(const CompilationParams &other) const = default;
        bool operator>=(const CompilationParams &other) const = default;
    };
};

struct CompiledShaderCacherSeed
{
    using Asset = CompiledSurfaceShader;

    inline std::size_t load_cost() const { return 1; }
    inline bool operator<(const CompiledShaderCacherSeed &other) const
    {
        return kernel < other.kernel;
    }

    std::variant<ShaderCompilationParams> kernel;
};

using CompiledShaderCacher = dynasma::AbstractCacher<CompiledShaderCacherSeed>;

} // namespace Vitrae