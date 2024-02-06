#include "Vitrae/Renderers/OpenGL/Shader.h"
#include "Vitrae/Renderers/OpenGL.h"
#include "Vitrae/Assets/ShaderSteps/Source.h"
#include "Vitrae/Util/StringConvert.h"
#include "Vitrae/ComponentRoot.h"

#include <fstream>

namespace Vitrae
{

    GLSLShader::GLSLShader()
    {

    }

    GLSLShader::~GLSLShader()
    {

    }
    
    void GLSLShader::setOutputStep(const String& outputName, casted_asset_ptr<ShaderStep> step)
    {
        auto insRes = mNamedOutputData.insert_or_assign(outputName, OutputData{
            step,
            {},
            {},
            {}
        });

        OutputData &od = insRes.first->second;
        step->extractInputPropertyNames(od.inputPropertyNames);
        step->extractInputVariableNames(od.inputVariableNames);
        step->extractOutputVariableNames(od.outputVariableNames);
    }

    const GLSLShader::OutputData &GLSLShader::getOutputData(const String& outputName) const
    {
        return mNamedOutputData.at(outputName);
    }

    CompiledGLSLProgram::CompiledGLSLProgram(
        const GLSLShader::OutputData &geom, const GLSLShader::OutputData &vertex, const GLSLShader::OutputData &fragment,
        const std::map<String, VariantProperty> &properties,
        const OpenGLRenderer &rend, ComponentRoot &resRoot
    )
    {
        struct CompilationHelp
        {
            String filesuffix, inVarPrefix, outVarPrefix;
            const GLSLShader::OutputData *oData;
            
            std::map<String, VariantPropertySpec> uniformVarSpecs;
            std::map<String, VariantPropertySpec> inVarSpecs;
            std::map<String, VariantPropertySpec> outVarSpecs;
            std::map<String, VariantPropertySpec> pipethroughVarSpecs;
            std::map<String, VariantPropertySpec> localVarSpecs;
            std::vector<const SourceShaderStep*> steps;
            GLuint shaderId;
        } vertexHelper, geomHelper, fragHelper;

        vertexHelper.oData = &vertex;
        vertexHelper.filesuffix = "vert";
        vertexHelper.inVarPrefix = "vertex_";
        vertexHelper.outVarPrefix = "geom_";
        geomHelper.oData = &geom;
        geomHelper.filesuffix = "geom";
        geomHelper.inVarPrefix = "geom_";
        geomHelper.outVarPrefix = "frag_";
        fragHelper.oData = &fragment;
        fragHelper.filesuffix = "frag";
        fragHelper.inVarPrefix = "frag_";
        fragHelper.outVarPrefix = "";

        CompilationHelp *helpers[] = {&vertexHelper, &geomHelper, &fragHelper};

        // utils
        auto toGlslTypeString = [](const VariantProperty &prop) {
            return std::visit(
                [](const auto& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    return GLTypeInfo<T>::GLSLTypeName;
                }
            , prop);
        };
        auto toGlslValueString = [](const VariantProperty &prop) {
            return std::visit(
                [](const auto& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    return toStringOrErr<T>(arg);
                }
            , prop);
        };


        // set vertex data for the first stage
        std::map<String, VariantPropertySpec> prevStepOuts;// outputs from the prev step
        String vertexHeader;

        auto exportVertexInputs = [&]<class aiType, class glmType = typename aiTypeCvt<aiType>::glmType>(
            std::vector<VertexBufferSpec<aiType>> vertexBufferSpecs)
        {
            for (auto &spec : vertexBufferSpecs)
            {
                vertexHeader += String("layout(location = ") + std::to_string(spec.layoutInd) + ") in " +
                    String(GLTypeInfo<glmType>::GLSLTypeName) + " " + vertexHelper.inVarPrefix + spec.name + ";\n";

                prevStepOuts[spec.name] = VariantPropertySpec{.exampleValue = VariantProperty(glmType())};
            }
        };
        exportVertexInputs(rend.getVertexBufferSpecs<aiVector2D>());
        exportVertexInputs(rend.getVertexBufferSpecs<aiVector3D>());
        exportVertexInputs(rend.getVertexBufferSpecs<aiColor3D>());
        exportVertexInputs(rend.getVertexBufferSpecs<aiColor4D>());
        
        // extract data from shaders
        for (CompilationHelp *helper : helpers) {
            // separate uniforms and in vars
            for (const auto& reqInputNameSpec : helper->oData->inputVariableNames) {
                if (prevStepOuts.find(reqInputNameSpec.first) == prevStepOuts.end()) {
                    helper->uniformVarSpecs.insert(reqInputNameSpec);
                }
                else {
                    helper->inVarSpecs.insert(reqInputNameSpec);
                }
            }

            // outputs
            helper->outVarSpecs = helper->oData->outputVariableNames;
            for (const auto& recvInputNameSpec : prevStepOuts) {
                if (helper->oData->inputVariableNames.find(recvInputNameSpec.first) == helper->oData->inputVariableNames.end()) {
                    helper->pipethroughVarSpecs.insert(recvInputNameSpec);
                }
            }
            
            prevStepOuts = helper->outVarSpecs;

            // extract flat list
            helper->oData->mainStep->extractPrimitiveSteps(helper->steps, properties);

            // extract local variables
            for (const SourceShaderStep *step: helper->steps) {
                step->extractInputVariableNames(helper->localVarSpecs);
                step->extractOutputVariableNames(helper->localVarSpecs);
            }
        }

        // common header
        String commonHeader = "#version 330 core"
            "#define VITRAE_VARIABLES_DECLARED\n"
            "#define VITRAE_MAIN_BODY_ONLY\n";
        
        // property definition header
        String propertyHeader;
        for (const auto& nameVal : properties) {
            propertyHeader += String("#define ") + "VITRAE_"+nameVal.first + " " + toGlslValueString(nameVal.second) + "\n";
        }

        // shader building
        auto getInsOutsHeader = [&](const CompilationHelp *helper, const String &inDeclSuffix) {
            String str;
            for (const auto& nameSpec: helper->inVarSpecs) {
                str +=  String("in ") + String(toGlslTypeString(nameSpec.second.exampleValue)) + " " +
                        helper->inVarPrefix + nameSpec.first + inDeclSuffix + ";\n";
            }
            for (const auto& nameSpec: helper->outVarSpecs) {
                str +=  String("out ") + String(toGlslTypeString(nameSpec.second.exampleValue)) + " " +
                        helper->outVarPrefix + nameSpec.first + ";\n";
            }
            for (const auto& nameSpec: helper->uniformVarSpecs) {
                str +=  String("uniform ") + String(toGlslTypeString(nameSpec.second.exampleValue)) + " " +
                        nameSpec.first + ";\n";
            }
            return str;
        };
        auto getMainSrcFromBody = [](const String &body) {
            return String("void main()") +
                "{"+ body + "}";
        };
        auto getPipethroughStatement = [](const CompilationHelp *helper, const String &inUsageSuffix, const String &outUsageSuffix) {
            String str;
            for (const auto& nameSpec : helper->pipethroughVarSpecs) {
                str +=  helper->outVarPrefix + nameSpec.first + outUsageSuffix + " = " +
                        helper->inVarPrefix + nameSpec.first + inUsageSuffix + "; ";
            }
            return str;
        };
        auto getInputToLocalStatement = [](const CompilationHelp *helper, const String &inUsageSuffix) {
            String str;
            for (const auto& nameSpec : helper->inVarSpecs) {
                str +=  nameSpec.first + " = " +
                        helper->inVarPrefix + nameSpec.first + inUsageSuffix + "; ";
            }
            return str;
        };
        auto getLocalToOutputStatement = [](const CompilationHelp *helper, const String &outUsageSuffix) {
            String str;
            for (const auto& nameSpec : helper->outVarSpecs) {
                str +=  helper->outVarPrefix + nameSpec.first + outUsageSuffix + " = " +
                        nameSpec.first + "; ";
            }
            return str;
        };
        auto getLocalDeclarationsStatement = [&](const CompilationHelp *helper) {
            String str;
            for (const auto& nameSpec : helper->localVarSpecs) {
                str += String(toGlslTypeString(nameSpec.second.exampleValue)) + nameSpec.first + "; ";
            }
            return str;
        };

        // vertex
        {
            CompilationHelp *helper = &vertexHelper;
            String code;

            std::stringstream mainBodySS;
            mainBodySS <<   String("    ") + getPipethroughStatement(helper, "", "") + "\n" + 
                            String("    ") + getLocalDeclarationsStatement(helper) + "\n" + 
                            String("    ") + getInputToLocalStatement(helper, "") + "\n";
            for (const SourceShaderStep *step: helper->steps) {
                step->extractSource(mainBodySS);
            }
            mainBodySS << String("    ") + getLocalToOutputStatement(helper, "") + "\n";

            code += commonHeader + 
                "\n" + 
                propertyHeader +
                "\n" + 
                vertexHeader +
                getInsOutsHeader(helper, "") +
                "\n" +
                getMainSrcFromBody(
                    mainBodySS.str()
                ) + 
                "\n";

            const char *c_code = code.c_str();
            helper->shaderId = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(helper->shaderId, 1, &c_code, NULL);

            // debugging
            std::ofstream file;
            file.open("shader_concat."+helper->filesuffix);
            file << code;
            file.close();
        }

        // geometry
        {
            CompilationHelp *helper = &geomHelper;
            String code;

            std::stringstream mainBodySS;
            mainBodySS << String("    ") + getLocalDeclarationsStatement(helper) + "\n";
            for (const SourceShaderStep *step: helper->steps) {
                step->extractSource(mainBodySS);
            }

            code += commonHeader + 
                "\n" + 
                propertyHeader +
                "layout(triangles) in;\n" + 
                "layout(triangle_strip, max_vertices=3) out;\n" +
                getInsOutsHeader(helper, "[]") +
                "#define VITRAE_PIPETHROUGH_VERTEX_VALUES(i) " + getPipethroughStatement(helper, "[i]", "") +
                "\n" +
                getMainSrcFromBody(
                    mainBodySS.str()
                ) + 
                "\n";

            const char *c_code = code.c_str();
            helper->shaderId = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(helper->shaderId, 1, &c_code, NULL);

            // debugging
            std::ofstream file;
            file.open("shader_concat."+helper->filesuffix);
            file << code;
            file.close();
        }

        // fragment
        {
            CompilationHelp *helper = &fragHelper;
            String code;

            std::stringstream mainBodySS;
            mainBodySS <<   String("    ") + getPipethroughStatement(helper, "", "") + "\n" + 
                            String("    ") + getLocalDeclarationsStatement(helper) + "\n" + 
                            String("    ") + getInputToLocalStatement(helper, "") + "\n";
            for (const SourceShaderStep *step: helper->steps) {
                step->extractSource(mainBodySS);
            }
            mainBodySS << String("    ") + getLocalToOutputStatement(helper, "") + "\n";

            code += commonHeader + 
                "\n" + 
                propertyHeader +
                "\n" + 
                getInsOutsHeader(helper, "") +
                "\n" +
                getMainSrcFromBody(
                    mainBodySS.str()
                ) + 
                "\n";

            const char *c_code = code.c_str();
            helper->shaderId = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(helper->shaderId, 1, &c_code, NULL);

            // debugging
            std::ofstream file;
            file.open("shader_concat."+helper->filesuffix);
            file << code;
            file.close();
        }

        // compile shaders
	    programGLName = glCreateProgram();
        for (CompilationHelp *helper : helpers) {
            glCompileShader(helper->shaderId);

            int success;
            char cmplLog[1024];
            glGetShaderInfoLog(helper->shaderId, sizeof(cmplLog), nullptr, cmplLog);
            glGetShaderiv(helper->shaderId, GL_COMPILE_STATUS, &success);
            if (!success) {
                resRoot.getErrStream() << "Shader compilation error: " << cmplLog;
            } else {
                resRoot.getInfoStream() << "Shader compiled: " << cmplLog;
            }

	        glAttachShader(programGLName, helper->shaderId);
        }
	    glLinkProgram(programGLName);
        {
            int success;
            char cmplLog[1024];
            glGetProgramInfoLog(programGLName, sizeof(cmplLog), nullptr, cmplLog);
            glGetProgramiv(programGLName, GL_LINK_STATUS, &success);
            if (!success) {
                resRoot.getErrStream() << "Shader linking error: " << cmplLog;
            } else {
                resRoot.getInfoStream() << "Shader linked: " << cmplLog;
            }
        }
        for (CompilationHelp *helper : helpers) {
            glDeleteShader(helper->shaderId);
        }
    }

    CompiledGLSLProgram::~CompiledGLSLProgram()
    {
	    glDeleteProgram(programGLName);
    }
    
}