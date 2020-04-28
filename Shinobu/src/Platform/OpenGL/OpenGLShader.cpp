#include "Platform/OpenGL/OpenGLShader.h"
#include "Shinobu/Core/Log.h"

#include <glad/glad.h>
#include <fstream>

namespace sh
{
    // https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h
    void CheckCompileErrors(unsigned int shader, const std::string& type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
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
    
    OpenGLShader::OpenGLShader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            SH_ERROR("Shader files could not be read. \nVertex: {0}\nFragment: {1}", vertexPath, fragmentPath);
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        CheckCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        CheckCompileErrors(fragment, "FRAGMENT");
        // shader Program
        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertex);
        glAttachShader(m_ID, fragment);
        glLinkProgram(m_ID);
        CheckCompileErrors(m_ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    void OpenGLShader::Bind() const
    { 
        glUseProgram(m_ID); 
    }
    void OpenGLShader::Unbind() const
    { 
        glUseProgram(0); 
    }
    
    void OpenGLShader::SetInt(const char* name, const int val) const
    { 
        glUniform1i(glGetUniformLocation(m_ID, name), val); 
    }
    void OpenGLShader::SetFloat(const char* name, const float val) const
    { 
        glUniform1f(glGetUniformLocation(m_ID, name), val); 
    }
    void OpenGLShader::SetBool(const char* name, const bool val) const
    { 
        glUniform1i(glGetUniformLocation(m_ID, name), val); 
    };
    void OpenGLShader::SetMat4(const char* name, const float* val) const
    { 
        glUniformMatrix4fv(glGetUniformLocation(m_ID, name), 1, GL_FALSE, val); 
    }
    
    void OpenGLShader::SetVec2(const char* name, const float* val) const
    { 
        glUniform2fv(glGetUniformLocation(m_ID, name), 1, val); 
    }
    void OpenGLShader::SetVec3(const char* name, const float* val) const
    { 
        glUniform3fv(glGetUniformLocation(m_ID, name), 1, val); 
    }
}