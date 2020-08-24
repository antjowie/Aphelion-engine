cmake_minimum_required(VERSION 3.16)

add_library(
    Renderer

    GraphicsContext.cpp
    GraphicsContext.h
    OrthographicCamera.cpp
    OrthographicCamera.h
    OrthographicCameraController.cpp
    OrthographicCameraController.h
    PerspectiveCamera.cpp
    PerspectiveCamera.h
    PerspectiveCameraController.cpp
    PerspectiveCameraController.h
    Primitive.cpp
    Primitive.h
    RenderCommand.cpp
    RenderCommand.h
    Renderer.cpp
    Renderer.h
    Renderer2D.cpp
    Renderer2D.h
    RendererAPI.cpp
    RendererAPI.h
    Shader.cpp
    Shader.h
    Texture.cpp
    Texture.h
    VertexArray.cpp
    VertexArray.h
    VertexBuffer.cpp
    VertexBuffer.h

    # TODO: These depend on current platform so we should move them in the future
    ../../Platform/OpenGL/OpenGLRendererAPI.cpp
    ../../Platform/OpenGL/OpenGLRendererAPI.h
    ../../Platform/OpenGL/OpenGLContext.cpp
    ../../Platform/OpenGL/OpenGLContext.h
    ../../Platform/OpenGL/OpenGLShader.cpp
    ../../Platform/OpenGL/OpenGLShader.h
    ../../Platform/OpenGL/OpenGLTexture.cpp
    ../../Platform/OpenGL/OpenGLTexture.h
    ../../Platform/OpenGL/OpenGLVertexArray.cpp
    ../../Platform/OpenGL/OpenGLVertexArray.h
    ../../Platform/OpenGL/OpenGLVertexBuffer.cpp
    ../../Platform/OpenGL/OpenGLVertexBuffer.h
)

add_library(Aphelion::Renderer ALIAS Renderer)

# target_precompile_headers(Renderer REUSE_FROM Aphelion::Core)
get_target_property(CORE_PCH Aphelion::Core PRECOMPILE_HEADERS)
target_precompile_headers(
    Renderer
    PUBLIC
    ${CORE_PCH}
)

target_include_directories(Renderer PUBLIC ../../)

target_link_libraries(
    Renderer
    PUBLIC
    Aphelion::Core
    PRIVATE
    Glad::glad
    glfw
    stb_image::stb_image
    # Right now window is managed by Core. I do think that should change since
    # it is a graphics thing
    # glfw 
)