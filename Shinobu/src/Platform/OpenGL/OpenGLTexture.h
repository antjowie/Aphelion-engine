#pragma once

#include "Shinobu/Renderer/Texture.h"

namespace sh
{
    class SHINOBU_API OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height);
        OpenGLTexture2D(const char* path);
        virtual ~OpenGLTexture2D();

        virtual void Bind(uint32_t slot) const override final;
        virtual void Unbind() const override final;

        virtual uint32_t GetWidth() const override final;
        virtual uint32_t GetHeight() const override final;

        virtual void SetData(void* data, uint32_t size) override final;

    private:
        uint32_t m_width, m_height;
        uint32_t m_id;
        unsigned m_internalFormat, m_dataFormat;
    };
}