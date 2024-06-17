#pragma once

#include "Vitrae/Pipelines/Task.hpp"
#include "Vitrae/Util/Hashing.hpp"

#include "dynasma/managers/abstract.hpp"
#include "dynasma/pointer.hpp"
#include "dynasma/util/dynamic_typing.hpp"

#include <map>
#include <optional>
#include <vector>

namespace Vitrae
{

template <TaskChild BasicTask> class Method : public dynasma::PolymorphicBase
{
  public:
    /*
    Management
    */

    struct MethodParams
    {
        std::vector<dynasma::FirmPtr<BasicTask>> tasks;
        std::vector<dynasma::FirmPtr<Method>> fallbackMethods;
    };

    struct MethodManagerSeed
    {
        using Asset = Method;

        std::variant<MethodParams> kernel;

        inline std::size_t load_cost() const { return 1; }
    };

    using MethodManager = dynasma::AbstractManager<MethodManagerSeed>;

    // Constructor
    Method(const MethodParams &params)
    {
        for (auto task : params.tasks) {
            for (auto [outputId, outputSpec] : task->getOutputSpecs()) {
                m_tasksPerOutput[outputId] = task;
            }
        }
        for (auto method : params.fallbackMethods) {
            m_fallbackMethods.push_back(method);
        }

        m_hash = 0;
        for (auto [nameId, p_task] : m_tasksPerOutput) {
            m_hash = combinedHashes<3>(
                {{m_hash, std::hash<StringId>{}(nameId), static_cast<std::size_t>(&*p_task)}});
        }
        for (auto method : m_fallbackMethods) {
            m_hash = combinedHashes<2>({{m_hash, method->getHash()}});
        }
    }

    // Destructor
    ~Method() = default;

    std::size_t memory_cost() const
    {
        std::size_t calcd = sizeof(*this);
        for (auto task : m_tasksPerOutput) {
            calcd += task.second->memory_cost();
        }
        for (auto method : m_fallbackMethods) {
            calcd += method->memory_cost();
        }
        return calcd;
    }

    /**
     * @returns the task generating the desired output, if found
     */
    std::optional<dynasma::FirmPtr<BasicTask>> getTask(StringId outputNameId) const
    {
        auto it = m_tasksPerOutput.find(outputNameId);
        if (it != m_tasksPerOutput.end()) {
            return it->second;
        }
        for (auto method : m_fallbackMethods) {
            auto task = method->getTask(outputNameId);
            if (task.has_value()) {
                return task;
            }
        }
        return std::nullopt;
    }

    std::size_t getHash() const { return m_hash; }

  protected:
    std::map<StringId, dynasma::FirmPtr<BasicTask>> m_tasksPerOutput;
    std::vector<dynasma::FirmPtr<Method>> m_fallbackMethods;
    std::size_t m_hash;
};

} // namespace Vitrae

namespace std
{
template <typename BasicTask> struct hash<Vitrae::Method<BasicTask>>
{
    std::size_t operator()(const Vitrae::Method<BasicTask> &method) const
    {
        return method.getHash();
    }
};
} // namespace std