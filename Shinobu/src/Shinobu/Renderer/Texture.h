#pragma once

#include "Shinobu/Core/Core.h"

#include <memory>

namespace sh
{
    class SHINOBU_API Texture;
    using TextureRef = std::shared_ptr<Texture>;

    class SHINOBU_API Texture
    {
    public:
        virtual ~Texture() = default;

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void Unbind() const = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual void SetData(void* data, uint32_t size) = 0;

        // Not sure if this function belongs here, since a path is not necessarily associated
        // with a texture. The texture is just the wrapper around data that our render api uses.
        // This should rather be inside of a resource manager class that maps paths to texture objects
        //
        // TODO: Create a resource manager class (for serialization purposes, right now I just store the path
        // inside of the components)
        //virtual const std::string& GetPath()
    };

    class SHINOBU_API Texture2D : public Texture
    {
    public:
        static TextureRef Create(uint32_t width, uint32_t height);
        static TextureRef Create(const char* path);
    };

    class SHINOBU_API ArrayTexture2D : public Texture
    {
    public:
        static TextureRef Create(uint32_t x, uint32_t y, const char* path);

        virtual void SetLayer(unsigned layer) = 0;
    };
}