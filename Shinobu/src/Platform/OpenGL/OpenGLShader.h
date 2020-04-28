#pragma once

#include "Shinobu/Core/Core.h"
#include "Shinobu/Renderer/Shader.h"

namespace sh
{
    class SHINOBU_API OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const char* vertexPath, const char* fragmentPath);

        virtual void Bind() const override final;
        virtual void Unbind() const override final;

        virtual void SetInt(const char* name, const int val) const override final;
        virtual void SetFloat(const char* name, const float val) const override final;
        virtual void SetBool(const char* name, const bool val) const override final;
        virtual void SetMat4(const char* name, const float* val) const override final;

        virtual void SetVec2(const char* name, const float* val) const override final;
        virtual void SetVec3(const char* name, const float* val) const override final;

    private:
        unsigned int m_ID;
    };
}