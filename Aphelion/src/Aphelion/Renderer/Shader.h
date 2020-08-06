#pragma once

#include "Aphelion/Core/Core.h"

#include <string>

namespace ap
{
    class APHELION_API Shader;
    using ShaderRef = std::shared_ptr<Shader>;

    class APHELION_API Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0; 

        virtual void SetInt(const char* name, const int val) const = 0;
        virtual void SetFloat(const char* name, const float val) const = 0;
        virtual void SetBool(const char* name, const bool val) const = 0;
        virtual void SetMat4(const char* name, const float* val) const = 0;

        virtual void SetVec2(const char* name, const float* val) const = 0;
        virtual void SetVec3(const char* name, const float* val) const = 0;
        virtual void SetVec4(const char* name, const float* val) const = 0;

        static ShaderRef Create(const std::string& filepath);
        static ShaderRef Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    };
}