#include "Vitrae/Assets/Shader.h"
#include <set>

namespace Vitrae
{

Shader::Shader(const ShaderParams &params)
{
    m_vertexTask = params.vertexTask;
    m_fragmentTask = params.fragmentTask;

    std::set<StringId> inputParams;

    for (auto &task : {m_vertexTask, m_fragmentTask}) {
        for (auto &[inputName, inputSpec] : task->getInputSpecs()) {
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
                                                 ComponentRoot &root) const
{
    std::map<StringId, Variant> relevantProperties;
    for (auto &name : m_inputParamNames) {
        if (auto propPtr = properties.getPtr(name)) {
            relevantProperties.insert({name, *propPtr});
        }
    }
    return root.getComponent<CompiledShaderCacher>().retrieve_asset({ShaderCompilationParams(
        m_vertexTask, m_fragmentTask, std::move(relevantProperties), root)});
}

ShaderCompilationParams::ShaderCompilationParams(dynasma::FirmPtr<ShaderTask> vertexTask,
                                                 dynasma::FirmPtr<ShaderTask> fragmentTask,
                                                 std::map<StringId, Variant> &&compileParameters,
                                                 ComponentRoot &root)
    : m_vertexTask(vertexTask), m_fragmentTask(fragmentTask),
      m_compileParameters(std::move(compileParameters)), m_root(root)
{
    updateHash();
}

void ShaderCompilationParams::updateHash()
{
    m_hash = 0;

    auto add_hash = [&]<typename T>(const T &val) {
        std::hash<T> hasher;
        m_hash ^= hasher(val) + 0x9e3779b9 + (m_hash << 6) + (m_hash >> 2);
    };

    add_hash(&*m_vertexTask);
    add_hash(&*m_fragmentTask);

    add_hash(m_compileParameters.size());
    for (auto [name, val] : m_compileParameters) {
        add_hash(name);
        add_hash(val);
    }

    add_hash(&m_root);
}

} // namespace Vitrae