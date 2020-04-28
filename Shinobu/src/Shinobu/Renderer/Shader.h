#pragma once

#include "Shinobu/Core/Core.h"

#include <string>

namespace sh
{
    class SHINOBU_API Shader
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

        static std::shared_ptr<Shader> Create(const std::string& filepath);
        static std::shared_ptr<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    };
}