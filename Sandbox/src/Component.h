#pragma once

#include <Shinobu/Renderer/Texture.h>
#include <glm/vec2.hpp>
#include <string>

#include <entt/entt.hpp>

// This si an invalid component type since it isnt' move constructibel tho I thought it could be used this way
//using Player = entt::tag<"Player"_hs>();
struct Player {};

struct Transform
{
    glm::vec2 pos;
};

template <typename S>
void serialize(S& s, Transform& o)
{
    s.value4b(o.pos.x);
    s.value4b(o.pos.y);
}

struct Sprite
{
    void LoadTexture() { tex = sh::Texture2D::Create(image.c_str()); }
    
    std::string image;
    
    // This should be a Texture2D object but not sure if there is a callback after serialize
    sh::TextureRef tex;
};

template <typename S>
void serialize(S& s, Sprite& o)
{
    constexpr int pathSize = 128;
    SH_ASSERT(o.image.size() < pathSize, "Image path is too long");
    s.text1b(o.image,pathSize);

    if (!tex) { o.LoadTexture(); }
}

// Test stuff
// ------------------------------------------

struct Foo
{
    int x;
    int y;

    template <typename S>
    void serialize(S& s) {
        s.value4b(x);
        s.value4b(y);
    }
};

struct Bar
{
    float val;
};
template <typename S>
void serialize(S& s, Bar& o) {
    s.value4b(o.val);
}