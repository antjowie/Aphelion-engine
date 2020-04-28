#pragma once

#include "Shinobu/Core/Core.h"
#include "Shinobu/Renderer/RendererAPI.h"

namespace sh
{
    class SHINOBU_API OpenGLRendererAPI : public RendererAPI
    {
    public:
        virtual void Init() override final;
        virtual void SetViewport(unsigned x, unsigned y, unsigned width, unsigned height) override final;
        virtual void SetClearColor(float r, float g, float b, float a) override final;

        virtual void Clear() override final;
    };
}