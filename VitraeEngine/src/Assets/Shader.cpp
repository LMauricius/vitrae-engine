#include "Vitrae/Assets/Shader.h"
#include <set>

namespace Vitrae
{

Shader::Shader(const ShaderParams &params)
{
    m_vertexTask = params.vertexTask;
    m_fragmentTask = params.fragmentTask;

    std::set<StringId> inputParams;

    for (auto &task : {m_vertexTask, m_fragmentTask})
    {
        for (auto &[inputName, inputSpec] : task->getInputSpecs())
        {
            inputParams.insert(inputName);
        }
    }

    m_inputParamNames.assign(inputParams.begin(), inputParams.end());
}

Shader::~Shader() = default;

std::size_t Shader::memory_cost() const
{
    /// TODO: calculate the real cost
    return sizeof(*this);
}

dynasma::FirmPtr<CompiledShader> Shader::compile(const ScopedDict &properties,
                                                 const ComponentRoot &root) const
{
    std::map<StringId, Property> relevantProperties;
    for (auto &name : m_inputParamNames)
    {
        if (auto propPtr = properties.getPtr(name))
        {
            relevantProperties.insert({name, *propPtr});
        }
    }
    return root.getComponent<CompiledShaderCacher>().retrieve_asset(
        {ShaderCompilationParams(m_vertexTask, m_fragmentTask, std::move(relevantProperties))});
}

ShaderCompilationParams::ShaderCompilationParams(dynasma::FirmPtr<ShaderTask> vertexTask,
                                                 dynasma::FirmPtr<ShaderTask> fragmentTask,
                                                 std::map<StringId, Property> &&compileParameters)
    : m_vertexTask(vertexTask), m_fragmentTask(fragmentTask),
      m_compileParameters(std::move(compileParameters))
{
}

} // namespace Vitrae