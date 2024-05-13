#include "Vitrae/Pipelines/Rendering/Group.hpp"

namespace Vitrae
{
void RenderGroup::run(RenderRunContext args)
{
    /// TODO: optimize this

    // setup local variables
    ScopedDict locals(&args.properties);

    for (auto [inputName, p_input] : args.inputPropertyPtrs)
    {
        locals.set(inputName, *p_input);
    }

    for (auto &item : m_items)
    {
        // prepare item inputs and outputs
        std::map<StringId, const Property *> itemInputPropertyPtrs;
        std::map<StringId, Property *> itemOutputPropertyPtrs;
        std::vector<std::pair<StringId, Property>> itemOutputProperties;
        itemOutputProperties.reserve(item.outputParamsToSharedVariables.size());
        for (auto [inputName, localName] : item.inputParamsToSharedVariables)
        {
            itemInputPropertyPtrs.insert({inputName, &locals.get(localName)});
        }
        for (auto [outputName, localName] : item.outputParamsToSharedVariables)
        {
            itemOutputProperties.emplace_back(localName, Property());
            itemOutputPropertyPtrs.insert({outputName, &itemOutputProperties.back().second});
        }

        // run
        item.task->run(RenderRunContext{locals, itemInputPropertyPtrs, itemOutputPropertyPtrs});

        // save item outputs
        for (auto [localName, prop] : itemOutputProperties)
        {
            locals.set(localName, prop);
        }
    }

    // save group outputs
    for (auto [outputName, p_output] : args.outputPropertyPtrs)
    {
        *p_output = locals.get(outputName);
    }
}
} // namespace Vitrae