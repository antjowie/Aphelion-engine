#include "Platform/OpenGL/OpenGLShader.h"
#include "Shinobu/Core/Log.h"

#include <glad/glad.h>
#include <fstream>

namespace sh
{
    // https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h
    void CheckCompileErrors(unsigned int shader, const char* type)
    {
        int success;
        char infoLog[1024];
        if (strcmp(type,  "Program"))
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                SH_CORE_ERROR("{0} shader could compile : {1}", type, infoLog);
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                SH_CORE_ERROR("{0} shader could not link : {1}", type, infoLog);
            }
        }
    }

    GLenum StringToType(const std::string& string)
    {
        if (string == "vertex") return GL_VERTEX_SHADER;
        else if (string == "fragment") return GL_FRAGMENT_SHADER;
        
        SH_CORE_WARN("string {0} can't be converted", string);
        return 0;
    }

    std::string TypeToString(GLenum type)
    {
        if (type == GL_VERTEX_SHADER) return "vertex";
        else if (type == GL_FRAGMENT_SHADER) return "fragment";

        SH_CORE_WARN("type {0} can't be converted", type);
        return "NO NAME";
    }

    std::string ReadFile(const std::string& filepath)
    {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size != -1)
            {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], size);
                in.close();
            }
            else
            {
                SH_CORE_ERROR("Could not read from file '{0}'", filepath);
            }
        }
        else
        {
            SH_CORE_ERROR("Could not open file '{0}'", filepath);
        }

        return result;
    }

    std::unordered_map<unsigned, std::string> PreProcess(const std::string& source)
    {
        std::unordered_map<GLenum, std::string> shaderSources;

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
            SH_CORE_ASSERT_REL(eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
            std::string type = source.substr(begin, eol - begin);
            SH_CORE_ASSERT_REL(StringToType(type) != 0, "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
            SH_CORE_ASSERT_REL(nextLinePos != std::string::npos, "Syntax error");
            pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

            shaderSources[StringToType(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSources;
    }

    void OpenGLShader::Compile(const std::unordered_map<unsigned, std::string>& shaders)
    {
        m_id = glCreateProgram();
        std::vector<unsigned> ids;
        for (const auto& shader : shaders)
        {
            const char* source = shader.second.c_str();
            SH_CORE_TRACE("Created shader {0}", TypeToString(shader.first));
            unsigned id = glCreateShader(shader.first);
            glShaderSource(id, 1, &source, NULL);
            glCompileShader(id);
            CheckCompileErrors(id, TypeToString(shader.first).c_str());
            glAttachShader(m_id, id);

            ids.push_back(id);
        }

        glLinkProgram(m_id);
        CheckCompileErrors(m_id, "Program");
        // delete the shaders as they're linked into our program now and no longer necessary
        for (auto id : ids)
        {
            glDetachShader(m_id, id);
            glDeleteShader(id);
        }
    }

    OpenGLShader::OpenGLShader(const char* filepath)
    {
        // Extract file name
        std::string path = filepath;
        auto lastSlash = path.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        auto lastDot = path.rfind('.');
        auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
        m_name = path.substr(lastSlash, count);

        std::string source = ReadFile(filepath);
        auto shaderSources = PreProcess(source);
        Compile(shaderSources);
    }

    OpenGLShader::OpenGLShader(const char* name, const char* vertexSrc, const char* fragmentSrc)
    {
        m_name = name;
        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        Compile(sources);
    }

    OpenGLShader::~OpenGLShader()
    {
        glDeleteProgram(m_id);
    }
    
    void OpenGLShader::Bind() const
    { 
        glUseProgram(m_id); 
    }
    void OpenGLShader::Unbind() const
    { 
        glUseProgram(0); 
    }
    
    void OpenGLShader::SetInt(const char* name, const int val) const
    { 
        glUniform1i(glGetUniformLocation(m_id, name), val); 
    }
    void OpenGLShader::SetFloat(const char* name, const float val) const
    { 
        glUniform1f(glGetUniformLocation(m_id, name), val); 
    }
    void OpenGLShader::SetBool(const char* name, const bool val) const
    { 
        glUniform1i(glGetUniformLocation(m_id, name), val); 
    }
    void OpenGLShader::SetMat4(const char* name, const float* val) const
    { 
        glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, val); 
    }
    
    void OpenGLShader::SetVec2(const char* name, const float* val) const
    { 
        glUniform2fv(glGetUniformLocation(m_id, name), 1, val); 
    }
    void OpenGLShader::SetVec3(const char* name, const float* val) const
    { 
        glUniform3fv(glGetUniformLocation(m_id, name), 1, val); 
    }
}