#pragma once

#include "Vitrae/Pipelines/Task.hpp"
#include "Vitrae/Util/Hashing.hpp"

#include "dynasma/managers/abstract.hpp"
#include "dynasma/pointer.hpp"
#include "dynasma/util/dynamic_typing.hpp"

#include <map>
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
            for (StringId[outputId, outputSpec] : task->getOutputSpecs()) {
                m_tasks[outputId] = task;
            }
        }
        for (auto method : params.fallbackMethods) {
            m_fallbackMethods.push_back(method);
            method->m_preferenceChangeListeners.push_back(
                [this](dynasma::FirmPtr<Method> changed) { updatePreferenceOrderHash(); })
        }

        updatePreferenceOrderHash();
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
     * Register a new task
     */
    void registerTask(dynasma::FirmPtr<BasicTask> task)
    {
        for (auto [outputId, outputSpec] : task->getOutputSpecs()) {
            m_tasksPerOutput[outputId] = task;
        }
        updatePreferenceOrderHash();
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

    /**
     * Register a new fallback method
     */
    void registerFallbackMethod(dynasma::FirmPtr<Method> method)
    {
        m_fallbackMethods.push_back(method);
        method->m_preferenceChangeListeners.push_back(
            [this](dynasma::FirmPtr<Method> changed) { updatePreferenceOrderHash(); });
    }

    /**
     * Moves the specified fallback method to the top of the list
     */
    void preferFallbackMethod(dynasma::FirmPtr<Method> method)
    {
        auto it = std::find(m_fallbackMethods.begin(), m_fallbackMethods.end(), method);
        if (it != m_fallbackMethods.end()) {
            m_fallbackMethods.erase(it);
            m_fallbackMethods.push_front(method);
            updatePreferenceOrderHash();
        }
    }

    /**
     * @returns A hash depicting the current preference order.
     * If the fallback order changes, or does for any of the fallback methods, the hash gets
     * updated
     */
    inline std::size_t getPreferenceOrderHash() const { return m_preferenceOrderHash; }

  protected:
    std::map<StringId, dynasma::FirmPtr<BasicTask>> m_tasksPerOutput;
    std::vector<dynasma::FirmPtr<Method>> m_fallbackMethods;

    mutable std::size_t m_preferenceOrderHash;
    std::function<void(dynasma::FirmPtr<Method> changed)> m_preferenceChangeListeners;

    void updatePreferenceOrderHash() const
    {
        std::size_t newHash = 0;

        for (auto method : m_fallbackMethods) {
            newHash = combinedHashes(newHash, method->getPreferenceOrderHash());
        }

        if (m_preferenceOrderHash != newHash) {
            m_preferenceOrderHash = newHash;
            for (auto listener : m_preferenceChangeListeners) {
                listener(dynasma::FirmPtr<Method>(this));
            }
        }
    }
};

} // namespace Vitrae