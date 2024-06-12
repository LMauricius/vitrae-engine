#include "Vitrae/Renderers/OpenGL/ShaderCompilation.hpp"
#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Renderers/OpenGL.hpp"
#include "Vitrae/Renderers/OpenGL/Shading/Task.hpp"

#include <fstream>

namespace Vitrae
{
CompiledGLSLShader::CompiledGLSLShader(const SurfaceShaderSpec &params)
    : CompiledGLSLShader({{CompilationSpec{.p_method = params.vertexMethod,
                                           .outVarPrefix = "fragment_",
                                           .shaderType = GL_VERTEX_SHADER},
                           CompilationSpec{.p_method = params.fragmentMethod,
                                           .outVarPrefix = "shaded_",
                                           .shaderType = GL_FRAGMENT_SHADER}}},
                         params.root)
{}

CompiledGLSLShader::CompiledGLSLShader(std::span<const CompilationSpec> compilationSpecs,
                                       ComponentRoot &root)
{
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

    struct ConvPipeItemReferences
    {
        dynasma::FirmPtr<OpenGLShaderTask> p_task;
        const std::map<StringId, StringId> &inputToLocalVariables;
        const std::map<StringId, StringId> &outputToLocalVariables;
    };

    struct CompilationHelp
    {
        // the specified shading method
        dynasma::FirmPtr<Method<ShaderTask>> p_method;

        // Pipeline variables
        std::map<StringId, PropertySpec> localSpecs, inputSpecs, outputSpecs;
        std::set<StringId> pipethroughInputNames;

        // items converted to OpenGLShaderTask
        std::vector<ConvPipeItemReferences> convertedPipeItems;

        // prefix for output variables
        String outVarPrefix;

        // gl shader type
        GLenum shaderType;

        // id of the compiled shader
        GLuint shaderId;
    };

    std::vector<CompilationHelp> helpers;
    std::vector<CompilationHelp *> helperOrder;
    std::vector<CompilationHelp *> invHelperOrder;
    for (auto &comp_spec : compilationSpecs) {
        helpers.push_back(CompilationHelp{.p_method = comp_spec.p_method,
                                          .outVarPrefix = comp_spec.outVarPrefix,
                                          .shaderType = comp_spec.shaderType});
    }
    for (int i = 0; i < helpers.size(); i++) {
        helperOrder.push_back(&helpers[i]);
        invHelperOrder.push_back(&helpers[helpers.size() - 1 - i]);
    };

    // generate pipelines, from the end result to the first pipeline
    {
        std::vector<PropertySpec> passedVarSpecs = {
            {.name = StandardShaderOutputNames::FRAGMENT_SHADER_OUTPUT,
             .typeInfo = StandardShaderOutputTypes::FRAGMENT_SHADER_OUTPUT_TYPE}};

        for (auto p_helper : invHelperOrder) {
            if (p_helper->shaderType == GL_VERTEX_SHADER) {
                passedVarSpecs.push_back(
                    PropertySpec{.name = StandardShaderOutputNames::VERTEX_SHADER_OUTPUT,
                                 .typeInfo = StandardShaderOutputTypes::VERTEX_SHADER_OUTPUT_TYPE});
            }
            std::span<const PropertySpec> passedVarSpecsSpan(passedVarSpecs);

            Pipeline<ShaderTask> pipeline(p_helper->p_method, passedVarSpecsSpan);

            passedVarSpecs.clear();
            for (auto [reqNameId, reqSpec] : pipeline.inputSpecs) {
                passedVarSpecs.push_back(reqSpec);
            }

            // store useful pipeline data
            p_helper->inputSpecs = std::move(pipeline.inputSpecs);
            p_helper->outputSpecs = std::move(pipeline.outputSpecs);
            p_helper->localSpecs = std::move(pipeline.localSpecs);
            p_helper->pipethroughInputNames = std::move(pipeline.pipethroughInputNames);

            // Convert PipeItems
            for (auto &item : pipeline.items) {
                p_helper->convertedPipeItems.push_back(ConvPipeItemReferences{
                    .p_task = dynasma::dynamic_pointer_cast<OpenGLShaderTask>(item.p_task),
                    .inputToLocalVariables = std::move(item.inputToLocalVariables),
                    .outputToLocalVariables = std::move(item.outputToLocalVariables),
                });
            }
        }
    }

    // make a list of all types we need to define
    std::vector<const GLTypeSpec *> typeDeclarationOrder;

    {
        std::set<const GLTypeSpec *> mentionedTypes;

        std::function<void(const GLTypeSpec &)> processTypeNameId =
            [&](const GLTypeSpec &glTypeSpec) -> void {
            if (mentionedTypes.find(&glTypeSpec) != mentionedTypes.end()) {
                mentionedTypes.insert(&glTypeSpec);

                for (auto p_dependencyTypeSpec : glTypeSpec.memberTypeDependencies) {
                    processTypeNameId(*p_dependencyTypeSpec);
                }

                typeDeclarationOrder.push_back(&glTypeSpec);
            }
        };

        for (auto varSpecs : {uniformVarSpecs, elemVarSpecs}) {
            for (auto [varNameId, varSpec] : varSpecs) {
                processTypeNameId(rend.getTypeConversion(varSpec.typeInfo).glTypeSpec);
            }
        }

        for (auto p_helper : helperOrder) {
            std::set<const TypeInfo *> usedTypeSet;

            for (auto &pipeItem : p_helper->convertedPipeItems) {
                pipeItem.p_task->extractUsedTypes(usedTypeSet);
            }

            for (auto p_type : usedTypeSet) {
                processTypeNameId(rend.getTypeConversion(*p_type).glTypeSpec);
            }
        }
    }

    // build the source code
    {
        std::map<StringId, PropertySpec> passedVarSpecs = {};
        std::string_view passedVarPrefix;

        for (auto p_helper : helperOrder) {
            // code output
            std::stringstream ss;
            OpenGLShaderTask::BuildContext context{.output = ss, .renderer = rend};
            std::map<StringId, String> inputParametersToGlobalVars;
            std::map<StringId, String> outputParametersToGlobalVars;

            // boilerplate stuff
            ss << "#version 330 core\n";
            ss << "\n";

            // type definitions
            for (auto p_glType : typeDeclarationOrder) {
                if (!p_glType->glslDefinitionSnippet.empty()) {
                    // skip structs with FAMs because they would cause issues, and they are only
                    // used in SSBO blocks
                    if (!p_glType->flexibleMemberSpec.has_value()) {
                        ss << p_glType->glslDefinitionSnippet << "\n";
                    }
                }
            }

            ss << "\n";

            // uniforms and SSBOs
            for (auto [uniNameId, uniSpec] : uniformVarSpecs) {
                const GLTypeSpec &glTypeSpec = rend.getTypeConversion(uniSpec.typeInfo).glTypeSpec;

                std::string_view glslMemberList = "";
                if (glTypeSpec.glslDefinitionSnippet.find_first_of("struct") != std::string::npos) {
                    glslMemberList =
                        std::string_view(glTypeSpec.glslDefinitionSnippet)
                            .substr(glTypeSpec.glslDefinitionSnippet.find_first_of('{') + 1,
                                    glTypeSpec.glslDefinitionSnippet.find_last_of('}') -
                                        glTypeSpec.glslDefinitionSnippet.find_first_of('{') - 1);
                }

                switch (rend.getGpuStorageMethod(glTypeSpec)) {
                case OpenGLRenderer::GpuValueStorageMethod::Uniform:
                    ss << "uniform " << specToGlName(uniSpec.typeInfo) << " " << uniVarPrefix
                       << uniSpec.name << ";\n";

                    inputParametersToGlobalVars.emplace(uniNameId,
                                                        std::string(uniVarPrefix) + uniSpec.name);
                    break;
                case OpenGLRenderer::GpuValueStorageMethod::UBO:
                    if (glslMemberList.empty()) {
                        ss << "uniform " << uniSpec.name << " {\n";
                        ss << "\t" << specToGlName(uniSpec.typeInfo) << " value" << ";\n";
                        ss << "} " << uniVarPrefix << uniSpec.name << ";\n";

                        inputParametersToGlobalVars.emplace(uniNameId, std::string(uniVarPrefix) +
                                                                           uniSpec.name + ".value");
                    } else {
                        ss << "uniform " << uniSpec.name << " {\n";
                        ss << glslMemberList << "\n";
                        ss << "} " << uniVarPrefix << uniSpec.name << ";\n";

                        inputParametersToGlobalVars.emplace(uniNameId, std::string(uniVarPrefix) +
                                                                           uniSpec.name);
                    }
                    break;
                case OpenGLRenderer::GpuValueStorageMethod::SSBO:
                    if (glslMemberList.empty()) {
                        ss << "buffer " << uniSpec.name << " {\n";
                        ss << "\t" << specToGlName(uniSpec.typeInfo) << " value" << ";\n";
                        ss << "} " << uniVarPrefix << uniSpec.name << ";\n";

                        inputParametersToGlobalVars.emplace(uniNameId, std::string(uniVarPrefix) +
                                                                           uniSpec.name + ".value");
                    } else {
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
            if (p_helper->shaderType == GL_VERTEX_SHADER) {
                ss << "\n";
                for (auto [elemNameId, elemSpec] : elemVarSpecs) {
                    ss << "layout(location = " << rend.getVertexBufferLayoutIndex(elemNameId)
                       << ") in " << specToGlName(elemSpec.typeInfo) << " " << elemVarPrefix
                       << elemSpec.name << ";\n";

                    inputParametersToGlobalVars.emplace(elemNameId,
                                                        std::string(elemVarPrefix) + elemSpec.name);
                }
            }

            // input variables
            for (auto [passedNameId, passedSpec] : passedVarSpecs) {
                ss << "in " << specToGlName(passedSpec.typeInfo) << " " << passedVarPrefix
                   << passedSpec.name << ";\n";

                inputParametersToGlobalVars.emplace(passedNameId,
                                                    std::string(passedVarPrefix) + passedSpec.name);
            }

            ss << "\n";

            // output variables
            passedVarSpecs.clear();
            for (auto [nameId, spec] : p_helper->outputSpecs) {
                ss << "out " << specToGlName(spec.typeInfo) << " " << p_helper->outVarPrefix
                   << spec.name << ";\n";
                outputParametersToGlobalVars.emplace(nameId, std::string(p_helper->outVarPrefix) +
                                                                 spec.name);
            }
            passedVarPrefix = p_helper->outVarPrefix;

            ss << "\n";

            // p_task function declarations
            for (auto &pipeItem : p_helper->convertedPipeItems) {
                pipeItem.p_task->outputDeclarationCode(context);
                ss << "\n\n";
            }

            // p_task function definitions
            for (auto &pipeItem : p_helper->convertedPipeItems) {
                pipeItem.p_task->outputDefinitionCode(context);
                ss << "\n\n";
            }

            ss << "void main() {\n";
            for (auto &pipeItem : p_helper->convertedPipeItems) {
                pipeItem.p_task->outputUsageCode(context, inputParametersToGlobalVars,
                                                 outputParametersToGlobalVars);
                ss << "\n\n";
            }
            ss << "}\n";

            // create the shader with the source
            std::string srcCode = ss.str();
            const char *c_code = srcCode.c_str();
            p_helper->shaderId = glCreateShader(p_helper->shaderType);
            glShaderSource(p_helper->shaderId, 1, &c_code, NULL);

            // debug
            std::ofstream file;
            file.open(std::string("concat.") + p_helper->outVarPrefix + "shader" +
                      std::to_string((std::size_t) & *p_helper->p_method) + ".glsl");
            file << srcCode;
            file.close();
        }
    }

    // compile shaders
    {
        int success;
        char cmplLog[1024];

        programGLName = glCreateProgram();
        for (auto p_helper : helperOrder) {
            glCompileShader(p_helper->shaderId);

            glGetShaderInfoLog(p_helper->shaderId, sizeof(cmplLog), nullptr, cmplLog);
            glGetShaderiv(p_helper->shaderId, GL_COMPILE_STATUS, &success);
            if (!success) {
                root.getErrStream() << "Shader compilation error: " << cmplLog;
            } else {
                root.getInfoStream() << "Shader compiled: " << cmplLog;
            }

            glAttachShader(programGLName, p_helper->shaderId);
        }
        glLinkProgram(programGLName);

        glGetProgramInfoLog(programGLName, sizeof(cmplLog), nullptr, cmplLog);
        glGetProgramiv(programGLName, GL_LINK_STATUS, &success);
        if (!success) {
            root.getErrStream() << "Shader linking error: " << cmplLog;
        } else {
            root.getInfoStream() << "Shader linked: " << cmplLog;
        }

        for (auto p_helper : helperOrder) {
            glDeleteShader(p_helper->shaderId);
        }
    }

    // store uniform indices
    for (auto [uniNameId, uniSpec] : uniformVarSpecs) {
        std::string uniFullName = std::string(uniVarPrefix) + uniSpec.name;

        switch (rend.getGpuStorageMethod(rend.getTypeConversion(uniSpec.typeInfo).glTypeSpec)) {
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