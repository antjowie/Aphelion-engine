#include "Platform/OpenGL/OpenGLTexture.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace ap
{
    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
        : m_width(width), m_height(height)
    {
        m_internalFormat = GL_RGBA8;
        m_dataFormat = GL_RGBA;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
        glTextureStorage2D(m_id, 1, m_internalFormat, m_width, m_height);

        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    OpenGLTexture2D::OpenGLTexture2D(const char* path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = nullptr;
		{
			data = stbi_load(path, &width, &height, &channels, 0);
		}
		AP_CORE_ASSERT(data, "Failed to load image!");
		m_width = width;
		m_height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_internalFormat = internalFormat;
		m_dataFormat = dataFormat;

		AP_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
		glTextureStorage2D(m_id, 1, internalFormat, m_width, m_height);

		glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

        OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_id);
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_id);
    }

    void OpenGLTexture2D::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    }

    uint32_t OpenGLTexture2D::GetWidth() const
    {
        return m_width;
    }
    
    uint32_t OpenGLTexture2D::GetHeight() const
    {
        return m_height;
    }

    void OpenGLTexture2D::SetData(void* data, uint32_t size)
    {
		uint32_t bpp = m_dataFormat == GL_RGBA ? 4 : 3;
		AP_CORE_ASSERT(size == m_width * m_height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
    }

    OpenGLArrayTexture2D::OpenGLArrayTexture2D(
        uint32_t x, uint32_t y, const char* path)
            : m_layer(0)
    {
        int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = nullptr;
		{
			data = stbi_load(path, &width, &height, &channels, 0);
		}
		AP_CORE_ASSERT(data, "Failed to load image!");
		m_width = width;
		m_height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_internalFormat = internalFormat;
		m_dataFormat = dataFormat;

		AP_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

        // Convert raw texture data to data that we can interpret
        const unsigned texSize = width / x;
        auto image{ std::make_unique<unsigned char[]>(width * height * channels) };

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
                        &data[(yI * width * texSize * channels) +
                              (xI * texSize * channels) +
                               row * width * channels],
                        texSize * channels);
                    offset += texSize * channels;
                }
            }
        }

        const auto size = x * y;
        // Create the storage
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_id);
        glTextureStorage3D(m_id, 4, m_internalFormat, texSize, texSize, size);

        glTextureSubImage3D(
            m_id,
            0,
            0, 0, 0,
            texSize, texSize, size,
            m_dataFormat,
            GL_UNSIGNED_BYTE,
            image.get());

        stbi_image_free(data);

        glGenerateTextureMipmap(m_id);

        // Set texture parameters
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    OpenGLArrayTexture2D::~OpenGLArrayTexture2D()
    {
        glDeleteTextures(1, &m_id);
    }

    void OpenGLArrayTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_id);
    }

    void OpenGLArrayTexture2D::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    }

    uint32_t OpenGLArrayTexture2D::GetWidth() const
    {
        return m_width;
    }
    
    uint32_t OpenGLArrayTexture2D::GetHeight() const
    {
        return m_height;
    }

    void OpenGLArrayTexture2D::SetData(void* data, uint32_t size)
    {
		uint32_t bpp = m_dataFormat == GL_RGBA ? 4 : 3;
		AP_CORE_ASSERT(size == m_width * m_height * bpp, "Data must be entire texture!");
		glTextureSubImage3D(m_id, 0, 0, 0, 0, m_width, m_height, m_layer, m_dataFormat, GL_UNSIGNED_BYTE, data);
    }

    void OpenGLArrayTexture2D::SetLayer(unsigned layer)
    {
        m_layer = layer;
    }

    //Texture::Texture(const char* path)
    //{
    //    Bind();
    //
    //    int width, height, nrChannels;
    //    stbi_set_flip_vertically_on_load(true);
    //
    //    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, STBI_rgb_alpha);
    //    
    //    if (data)
    //    {
    //        glTexImage2D(
    //            GL_TEXTURE_2D,
    //            0,
    //            GL_RGBA,
    //            width,
    //            height,
    //            0,
    //            GL_RGBA,
    //            GL_UNSIGNED_BYTE,
    //            data);
    //        glGenerateMipmap(GL_TEXTURE_2D);
    //    }
    //    else
    //    {
    //        std::cout << "Failed to load texture: " << path << std::endl;
    //    }
    //    stbi_image_free(data);
    //
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //}
    //
    //TextureAtlas::TextureAtlas(const char* path, unsigned x, unsigned y)
    //{
    //    Bind();
    //
    //    int width, height, nrChannels;
    //    stbi_set_flip_vertically_on_load(true);
    //
    //    // Since we put our textures in a grid like pattern, we have to convert the data
    //    // to be in a format that emulates all the textures in one columns
    //    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, STBI_rgb_alpha);
    //    const unsigned texSize = width / x;
    //
    //    if (data)
    //    {
    //        auto image{ std::make_unique<unsigned char[]>(width * height * nrChannels) };
    //
    //        unsigned offset = 0;
    //        for (unsigned yI = 0; yI < height / texSize; yI++)
    //        {
    //            for (unsigned xI = 0; xI < width / texSize; xI++)
    //            {
    //                // Read 64 rows
    //                for (unsigned row = 0; row < texSize; row++)
    //                {
    //                    memcpy(
    //                        &image[offset],
    //                        &data
    //                        [(yI * width * texSize * nrChannels) +
    //                    (xI * texSize * nrChannels) +
    //                        row * width * nrChannels],
    //                        texSize * nrChannels);
    //                    offset += texSize * nrChannels;
    //                }
    //            }
    //        }
    //
    //        const auto size = x * y;
    //        // Create the storage
    //        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 4, GL_RGBA8, texSize, texSize, size);
    //
    //        glTexSubImage3D(
    //            GL_TEXTURE_2D_ARRAY,
    //            0,
    //            0, 0, 0,
    //            texSize, texSize, size,
    //            GL_RGBA,
    //            GL_UNSIGNED_BYTE,
    //            image.get());
    //
    //        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    //    }
    //    else
    //    {
    //        std::cout << "Failed to load texture: " << path << std::endl;
    //    }
    //    stbi_image_free(data);
    //
    //    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //}
    //
    //TextureCubemap::TextureCubemap(const char* dir, const char* extension)
    //{
    //    Bind();
    //
    //    //#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
    //    //#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
    //    //#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
    //    //#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
    //    //#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
    //    //#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
    //
    //    const std::string image[6]
    //    {
    //        dir + std::string("/right.") + extension,
    //        dir + std::string("/left.") + extension,
    //        dir + std::string("/top.") + extension,
    //        dir + std::string("/bottom.") + extension,
    //        dir + std::string("/front.") + extension,
    //        dir + std::string("/back.") + extension,
    //    };
    //
    //    stbi_set_flip_vertically_on_load(false);
    //    for (int i = 0; i < 6; i++)
    //    {
    //        int width, height, nrChannels;
    //
    //        unsigned char* data = stbi_load(image[i].c_str(), &width, &height, &nrChannels, STBI_rgb);
    //        std::cout << nrChannels << '\n';
    //        if (data)
    //        {
    //            glTexImage2D(
    //                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
    //                0,
    //                GL_RGB,
    //                width,
    //                height,
    //                0,
    //                GL_RGB,
    //                GL_UNSIGNED_BYTE,
    //                data);
    //        }
    //        else
    //        {
    //            std::cout << "Failed to load texture: " << image[i] << std::endl;
    //        }
    //        stbi_image_free(data);
    //    }
    //
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //
    //    Unbind();
    //}
}