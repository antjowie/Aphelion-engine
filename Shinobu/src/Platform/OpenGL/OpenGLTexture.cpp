#include "vcpch.hpp"
#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const char* path)
{
    Bind();

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, STBI_rgb_alpha);
    
    if (data)
    {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

TextureAtlas::TextureAtlas(const char* path, unsigned x, unsigned y)
{
    Bind();

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    // Since we put our textures in a grid like pattern, we have to convert the data
    // to be in a format that emulates all the textures in one columns
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, STBI_rgb_alpha);
    const unsigned texSize = width / x;

    if (data)
    {
        auto image{ std::make_unique<unsigned char[]>(width * height * nrChannels) };

        unsigned offset = 0;
        for (unsigned yI = 0; yI < height / texSize; yI++)
        {
            for (unsigned xI = 0; xI < width / texSize; xI++)
            {
                // Read 64 rows
                for (unsigned row = 0; row < texSize; row++)
                {
                    memcpy(
                        &image[offset],
                        &data
                        [(yI * width * texSize * nrChannels) +
                    (xI * texSize * nrChannels) +
                        row * width * nrChannels],
                        texSize * nrChannels);
                    offset += texSize * nrChannels;
                }
            }
        }

        const auto size = x * y;
        // Create the storage
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 4, GL_RGBA8, texSize, texSize, size);

        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            0, 0, 0,
            texSize, texSize, size,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            image.get());

        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }
    else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

TextureCubemap::TextureCubemap(const char* dir, const char* extension)
{
    Bind();

    //#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
    //#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
    //#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
    //#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
    //#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
    //#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A

    const std::string image[6]
    {
        dir + std::string("/right.") + extension,
        dir + std::string("/left.") + extension,
        dir + std::string("/top.") + extension,
        dir + std::string("/bottom.") + extension,
        dir + std::string("/front.") + extension,
        dir + std::string("/back.") + extension,
    };

    stbi_set_flip_vertically_on_load(false);
    for (int i = 0; i < 6; i++)
    {
        int width, height, nrChannels;

        unsigned char* data = stbi_load(image[i].c_str(), &width, &height, &nrChannels, STBI_rgb);
        std::cout << nrChannels << '\n';
        if (data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data);
        }
        else
        {
            std::cout << "Failed to load texture: " << image[i] << std::endl;
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    Unbind();
}