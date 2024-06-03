#include "Vitrae/Renderers/OpenGL/Shader.h"
#include "Vitrae/ComponentRoot.h"
#include "Vitrae/Renderers/OpenGL.h"

#include <fstream>

namespace Vitrae
{

CompiledGLSLShader::CompiledGLSLShader(const ShaderCompilationParams &params)
{
    ComponentRoot &root = params.getRoot();
    OpenGLRenderer &rend = static_cast<OpenGLRenderer &>(root.getComponent<Renderer>());

    // util lambdas
    auto specToGlName = [&](const TypeInfo &typeInfo) -> std::string_view {
        return rend.getTypeConversion(typeInfo).glTypeSpec.glTypeName;
    };

    // uniforms are global variables given to all shader steps
    String uniVarPrefix = "uniform_";
    std::map<StringId, PropertySpec> uniformVarSpecs;

    // mesh vertex element data is given to the vertex shader and passed through to other steps
    String elemVarPrefix = "vertex_";
    std::map<StringId, PropertySpec> elemVarSpecs;

    struct CompilationHelp
    {
        // the specified ShaderTask
        dynasma::FirmPtr<ShaderTask> p_task;
        std::vector<const ShaderTask *> subTaskPtrSet;

        // prefix for output variables
        String outVarPrefix;

        // input variables are those required by the ShaderStep.
        // Can be uniforms, layout vars or GLSL inputs from a previous step
        const std::map<StringId, PropertySpec> &inVarSpecs;

        // output variables are those returned by the ShaderStep. Always passed as GLSL outputs
        const std::map<StringId, PropertySpec> &outVarSpecs;

        // pipethrough variables are those that are just passed from inputs to outputs,
        // bypassing the ShaderStep. Always passed as GLSL outputs
        std::map<StringId, PropertySpec> pipethroughVarSpecs;

        // gl shader type
        GLenum shaderType;

        // id of the compiled shader
        GLuint shaderId;
    };

    CompilationHelp vertexHelper{.p_task = params.getVertexTask(),
                                 .outVarPrefix = "fragment_",
                                 .inVarSpecs = params.getVertexTask()->getInputSpecs(),
                                 .outVarSpecs = params.getVertexTask()->getOutputSpecs(),
                                 .shaderType = GL_VERTEX_SHADER};

    CompilationHelp fragHelper{.p_task = params.getFragmentTask(),
                               .outVarPrefix = "shaded_",
                               .inVarSpecs = params.getFragmentTask()->getInputSpecs(),
                               .outVarSpecs = params.getFragmentTask()->getOutputSpecs(),
                               .shaderType = GL_FRAGMENT_SHADER};

    CompilationHelp *(helperOrder[]) = {&vertexHelper, &fragHelper};
    CompilationHelp *(invHelperOrder[]) = {&fragHelper, &vertexHelper};

    // extract sub tasks so that each appears once
    for (auto p_helper : helperOrder)
    {
        std::set<dynasma::LazyPtr<Task>> abstractTaskSet;
        p_helper->p_task->extractSubTasks(abstractTaskSet);
        abstractTaskSet.insert(p_helper->p_task);

        for (dynasma::LazyPtr<Task> p_task : abstractTaskSet)
        {
            p_helper->subTaskPtrSet.push_back(
                static_cast<const ShaderTask *>(&*p_task.getLoaded()));
        }
    }

    // which variables are required to either be output or passed through
    {
        std::map<StringId, PropertySpec> passedVarSpecs = {};

        for (auto p_helper : invHelperOrder)
        {
            for (auto [reqNameId, reqSpec] : passedVarSpecs)
            {
                if (p_helper->outVarSpecs.find(reqNameId) == p_helper->outVarSpecs.end())
                {
                    p_helper->pipethroughVarSpecs.insert({reqNameId, reqSpec});
                }
            }

            passedVarSpecs = p_helper->inVarSpecs;
            passedVarSpecs.merge(p_helper->pipethroughVarSpecs);
        }

        // now separate required variables in first step to uniforms and mesh layout variables
        for (auto [reqNameId, reqSpec] : passedVarSpecs)
        {
            if (rend.getAllVertexBufferSpecs().find(reqNameId) ==
                rend.getAllVertexBufferSpecs().end())
            {
                uniformVarSpecs.insert({reqNameId, reqSpec});
            }
            else
            {
                elemVarSpecs.insert({reqNameId, reqSpec});
            }
        }
    }

    // make a list of all types we need to define
    std::vector<const GLTypeSpec *> typeDeclarationOrder;

    {
        std::set<const GLTypeSpec *> mentionedTypes;

        std::function<void(const GLTypeSpec &)> processTypeNameId =
            [&](const GLTypeSpec &glTypeSpec) -> void {
            if (mentionedTypes.find(&glTypeSpec) != mentionedTypes.end())
            {
                mentionedTypes.insert(&glTypeSpec);

                for (auto p_dependencyTypeSpec : glTypeSpec.memberTypeDependencies)
                {
                    processTypeNameId(*p_dependencyTypeSpec);
                }

                typeDeclarationOrder.push_back(&glTypeSpec);
            }
        };

        for (auto varSpecs : {uniformVarSpecs, elemVarSpecs})
        {
            for (auto [varNameId, varSpec] : varSpecs)
            {
                processTypeNameId(rend.getTypeConversion(varSpec.typeInfo).glTypeSpec);
            }
        }

        for (auto p_helper : helperOrder)
        {
            std::set<const TypeInfo *> usedTypeSet;
            p_helper->p_task->extractUsedTypes(usedTypeSet);

            for (auto p_type : usedTypeSet)
            {
                processTypeNameId(rend.getTypeConversion(*p_type).glTypeSpec);
            }
        }
    }

    // build the source code
    {
        std::map<StringId, PropertySpec> passedVarSpecs = {};
        std::string_view passedVarPrefix;

        for (auto p_helper : helperOrder)
        {
            // code output
            std::stringstream ss;
            ShaderBuildContext context{.output = ss};
            std::map<StringId, String> inputParametersToGlobalVars;
            std::map<StringId, String> outputParametersToGlobalVars;

            // boilerplate stuff
            ss << "#version 330 core\n";
            ss << "\n";

            // type definitions
            for (auto p_glType : typeDeclarationOrder)
            {
                if (!p_glType->glslDefinitionSnippet.empty())
                {
                    // skip structs with FAMs because they would cause issues, and they are only
                    // used in SSBO blocks
                    if (!p_glType->flexibleMemberSpec.has_value())
                    {
                        ss << p_glType->glslDefinitionSnippet << "\n";
                    }
                }
            }

            ss << "\n";

            // uniforms and SSBOs
            for (auto [uniNameId, uniSpec] : uniformVarSpecs)
            {
                const GLTypeSpec &glTypeSpec = rend.getTypeConversion(uniSpec.typeInfo).glTypeSpec;

                std::string_view glslMemberList = "";
                if (glTypeSpec.glslDefinitionSnippet.find_first_of("struct") != std::string::npos)
                {
                    glslMemberList =
                        std::string_view(glTypeSpec.glslDefinitionSnippet)
                            .substr(glTypeSpec.glslDefinitionSnippet.find_first_of('{') + 1,
                                    glTypeSpec.glslDefinitionSnippet.find_last_of('}') -
                                        glTypeSpec.glslDefinitionSnippet.find_first_of('{') - 1);
                }

                switch (rend.getGpuStorageMethod(glTypeSpec))
                {
                case OpenGLRenderer::GpuValueStorageMethod::Uniform:
                    ss << "uniform " << specToGlName(uniSpec.typeInfo) << " " << uniVarPrefix
                       << uniSpec.name << ";\n";

                    inputParametersToGlobalVars.emplace(uniNameId,
                                                        std::string(uniVarPrefix) + uniSpec.name);
                    break;
                case OpenGLRenderer::GpuValueStorageMethod::UBO:
                    if (glslMemberList.empty())
                    {
                        ss << "uniform " << uniSpec.name << " {\n";
                        ss << "\t" << specToGlName(uniSpec.typeInfo) << " value" << ";\n";
                        ss << "} " << uniVarPrefix << uniSpec.name << ";\n";

                        inputParametersToGlobalVars.emplace(uniNameId, std::string(uniVarPrefix) +
                                                                           uniSpec.name + ".value");
                    }
                    else
                    {
                        ss << "uniform " << uniSpec.name << " {\n";
                        ss << glslMemberList << "\n";
                        ss << "} " << uniVarPrefix << uniSpec.name << ";\n";

                        inputParametersToGlobalVars.emplace(uniNameId, std::string(uniVarPrefix) +
                                                                           uniSpec.name);
                    }
                    break;
                case OpenGLRenderer::GpuValueStorageMethod::SSBO:
                    if (glslMemberList.empty())
                    {
                        ss << "buffer " << uniSpec.name << " {\n";
                        ss << "\t" << specToGlName(uniSpec.typeInfo) << " value" << ";\n";
                        ss << "} " << uniVarPrefix << uniSpec.name << ";\n";

                        inputParametersToGlobalVars.emplace(uniNameId, std::string(uniVarPrefix) +
                                                                           uniSpec.name + ".value");
                    }
                    else
                    {
                        ss << "buffer " << uniSpec.name << " {\n";
                        ss << glslMemberList << "\n";
                        ss << "} " << uniVarPrefix << uniSpec.name << ";\n";

                        inputParametersToGlobalVars.emplace(uniNameId, std::string(uniVarPrefix) +
                                                                           uniSpec.name);
                    }
                    break;
                }
            }

            // mesh vertex element
            if (p_helper == &vertexHelper)
            {
                ss << "\n";
                for (auto [elemNameId, elemSpec] : elemVarSpecs)
                {
                    ss << "layout(location = " << rend.getVertexBufferLayoutIndex(elemNameId)
                       << ") in " << specToGlName(elemSpec.typeInfo) << " " << elemVarPrefix
                       << elemSpec.name << ";\n";

                    inputParametersToGlobalVars.emplace(elemNameId,
                                                        std::string(elemVarPrefix) + elemSpec.name);
                }
            }

            // input variables
            for (auto [passedNameId, passedSpec] : passedVarSpecs)
            {
                ss << "in " << specToGlName(passedSpec.typeInfo) << " " << passedVarPrefix
                   << passedSpec.name << ";\n";

                inputParametersToGlobalVars.emplace(passedNameId,
                                                    std::string(passedVarPrefix) + passedSpec.name);
            }

            ss << "\n";

            // output variables
            passedVarSpecs.clear();
            for (auto specs : {p_helper->outVarSpecs, p_helper->pipethroughVarSpecs})
            {
                for (auto [nameId, spec] : specs)
                {
                    ss << "out " << specToGlName(spec.typeInfo) << " " << p_helper->outVarPrefix
                       << spec.name << ";\n";
                    outputParametersToGlobalVars.emplace(
                        nameId, std::string(p_helper->outVarPrefix) + spec.name);
                }
                passedVarSpecs.merge(specs);
            }
            passedVarPrefix = p_helper->outVarPrefix;

            ss << "\n";

            // p_task function declarations
            for (auto p_task : p_helper->subTaskPtrSet)
            {
                p_task->outputDeclarationCode(context);
                ss << "\n\n";
            }

            // p_task function definitions
            for (auto p_task : p_helper->subTaskPtrSet)
            {
                p_task->outputDefinitionCode(context);
                ss << "\n\n";
            }

            ss << "void main() {\n";
            p_helper->p_task->outputUsageCode(context, inputParametersToGlobalVars,
                                              outputParametersToGlobalVars);
            ss << "}\n";

            // create the shader with the source
            std::string srcCode = ss.str();
            const char *c_code = srcCode.c_str();
            p_helper->shaderId = glCreateShader(p_helper->shaderType);
            glShaderSource(p_helper->shaderId, 1, &c_code, NULL);

            // debug
            std::ofstream file;
            file.open(std::string("concat.") + p_helper->outVarPrefix + "shader" +
                      std::to_string(params.getHash()) + ".glsl");
            file << srcCode;
            file.close();
        }
    }

    // compile shaders
    {
        int success;
        char cmplLog[1024];

        programGLName = glCreateProgram();
        for (auto p_helper : helperOrder)
        {
            glCompileShader(p_helper->shaderId);

            glGetShaderInfoLog(p_helper->shaderId, sizeof(cmplLog), nullptr, cmplLog);
            glGetShaderiv(p_helper->shaderId, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                root.getErrStream() << "Shader compilation error: " << cmplLog;
            }
            else
            {
                root.getInfoStream() << "Shader compiled: " << cmplLog;
            }

            glAttachShader(programGLName, p_helper->shaderId);
        }
        glLinkProgram(programGLName);

        glGetProgramInfoLog(programGLName, sizeof(cmplLog), nullptr, cmplLog);
        glGetProgramiv(programGLName, GL_LINK_STATUS, &success);
        if (!success)
        {
            root.getErrStream() << "Shader linking error: " << cmplLog;
        }
        else
        {
            root.getInfoStream() << "Shader linked: " << cmplLog;
        }

        for (auto p_helper : helperOrder)
        {
            glDeleteShader(p_helper->shaderId);
        }
    }

    // store uniform indices
    for (auto [uniNameId, uniSpec] : uniformVarSpecs)
    {
        std::string uniFullName = std::string(uniVarPrefix) + uniSpec.name;

        switch (rend.getGpuStorageMethod(rend.getTypeConversion(uniSpec.typeInfo).glTypeSpec))
        {
        case OpenGLRenderer::GpuValueStorageMethod::Uniform:
            uniformSpecs.emplace(uniNameId, VariableSpec{.srcSpec = uniSpec.typeInfo,
                                                         .glNameId = glGetUniformLocation(
                                                             programGLName, uniFullName.c_str())});
            break;
        case OpenGLRenderer::GpuValueStorageMethod::UBO:
            uboSpecs.emplace(uniNameId, VariableSpec{.srcSpec = uniSpec.typeInfo,
                                                     .glNameId = (GLint)glGetUniformBlockIndex(
                                                         programGLName, uniFullName.c_str())});
            break;
        case OpenGLRenderer::GpuValueStorageMethod::SSBO:
            ssboSpecs.emplace(uniNameId, VariableSpec{.srcSpec = uniSpec.typeInfo,
                                                      .glNameId = (GLint)glGetProgramResourceIndex(
                                                          programGLName, GL_SHADER_STORAGE_BLOCK,
                                                          uniSpec.name.c_str())});
            break;
        }
    }
}

CompiledGLSLShader::~CompiledGLSLShader()
{
    glDeleteProgram(programGLName);
}

} // namespace Vitrae