#pragma once

#include <Shinobu/Renderer/Texture.h>
#include <Shinobu/ECS/Registry.h>
#include <glm/vec2.hpp>
#include <string>


// This si an invalid component type since it isnt' move constructibel tho I thought it could be used this way
//using Player = entt::tag<"Player"_hs>();
struct Player 
{
    // Components can't be empty. I don't know how to check for it so this
    // is a work around for the time being
    char empty;
    bool operator==(const Player& rhs) const { return true; }
};
template <typename S> void serialize(S& s, Player& o) {}

struct Transform
{
    glm::vec2 pos;

    bool operator==(const Transform& rhs) const { return pos == rhs.pos; }
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

    bool operator==(const Sprite& rhs) const { return image == rhs.image; }
};

template <typename S>
void serialize(S& s, Sprite& o)
{
    constexpr int pathSize = 128;
    SH_ASSERT(o.image.size() < pathSize, "Image path is too long");
    s.text1b(o.image, pathSize);

    if (!o.tex) { o.LoadTexture(); }
}

struct SpawnEntity
{
    enum Type
    {
        Player
    };

    Type type;
    Transform t;
    Sprite sprite;

    bool operator==(const SpawnEntity& rhs) const { return true; }
};

template <typename S>
void serialize(S& s, SpawnEntity& o)
{
    s.value4b(o.type);
    serialize(s, o.t);
    serialize(s, o.sprite);
}