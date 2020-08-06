#pragma once

#include "Aphelion/Renderer/Texture.h"

namespace ap
{
    class APHELION_API OpenGLTexture2D : public Texture2D
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

    class APHELION_API OpenGLArrayTexture2D : public ArrayTexture2D
    {
    public:
        OpenGLArrayTexture2D(uint32_t x, uint32_t y, const char* path);
        virtual ~OpenGLArrayTexture2D();

        virtual void Bind(uint32_t slot) const override final;
        virtual void Unbind() const override final;

        virtual uint32_t GetWidth() const override final;
        virtual uint32_t GetHeight() const override final;

        virtual void SetData(void* data, uint32_t size) override final;
        virtual void SetLayer(unsigned layer) override final;

    private:
        uint32_t m_width, m_height;
        uint32_t m_id;
        unsigned m_internalFormat, m_dataFormat;
        unsigned m_layer;
    };
}