#pragma once
//#include <glad/glad.h>

//#include "client/gl/Texture.hpp"
//
///**
// * Simple wrapper around a shader to compile and display errors
// * 
// * When setting a uniform the user should bind the shader themselves
// */
//class Shader
//{
//public:
//    Shader(const char* vertexPath, const char* fragmentPath);
//    
//    
//    inline void Bind() const { glUseProgram(m_ID); }
//    inline void Unbind() const { glUseProgram(0); }
//
//    inline void SetInt(const char* name, const int val) const { glUniform1i(glGetUniformLocation(m_ID, name), val); }
//    inline void SetFloat(const char* name, const float val) const { glUniform1f(glGetUniformLocation(m_ID, name), val); }
//    inline void SetBool(const char* name, const bool val) const { glUniform1i(glGetUniformLocation(m_ID, name), val); };
//    inline void SetMatrix(const char* name, const float* val) const { glUniformMatrix4fv(glGetUniformLocation(m_ID, name), 1, GL_FALSE, val); }
//
//    inline void SetVec2(const char* name, const float* val) const { glUniform2fv(glGetUniformLocation(m_ID, name), 1, val); }
//    inline void SetVec3(const char* name, const float* val) const { glUniform3fv(glGetUniformLocation(m_ID, name), 1, val); }
//
//private:
//    unsigned int m_ID;
//};