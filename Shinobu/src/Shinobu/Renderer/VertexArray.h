#pragma once

#include "Shinobu/Core/Core.h"

namespace sh
{
    class SHINOBU_API VertexArray
    {
    public:
        virtual void Bind();
        virtual void Unbind();

        virtual void AddVertexBuffer();
        virtual void SetIndexBuffer()
    };
}