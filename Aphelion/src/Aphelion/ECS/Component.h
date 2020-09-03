#pragma once
#include "Aphelion/Core/Transform.h"
#include "Aphelion/Physics/RigidBody.h"
#include "Aphelion/Physics/PhysicsScene.h"

#include <bitsery/traits/string.h>
#include <string>

//#define EMPTY_COMPONENT(Component)

namespace ap
{
    struct APHELION_API TransformComponent
    {
        Transform t;
    };
    template<typename S> void serialize(S& s, TransformComponent& v) { serialize(s, v.t); }
    inline bool operator==(const TransformComponent& lhs, const TransformComponent& rhs) { return lhs.t == rhs.t; }

    struct APHELION_API TagComponent
    {
        std::string tag;
        operator std::string() const { return tag; }
    };
    template<typename S> void serialize(S& s, TagComponent& v) { s.text1b(v.tag,32); }
    inline bool operator==(const TagComponent& lhs, const TagComponent& rhs) { return lhs.tag == rhs.tag; }

    struct APHELION_API GUIDComponent
    {
        unsigned guid;
        operator unsigned() const { return guid; }
    };
    template<typename S> void serialize(S& s, GUIDComponent& v) { s.value4b(v.guid); }
    inline bool operator==(const GUIDComponent& lhs, const GUIDComponent& rhs) { return lhs.guid == rhs.guid; }

    class APHELION_API RigidBodyComponent
    {
    public:
        /// These are called in Scene::HandleComponentCreate and Scene::HandleComponentRemove
        void OnCreate(PhysicsScene& scene, unsigned guid) { m_scene = &scene; m_guid = guid; }
        /// NOTE: not sure if this correctly deletes the integer
        void OnRemove() { if (m_rb) { delete reinterpret_cast<int*>(m_rb.GetUserData());  m_scene->RemoveActor(m_rb); } }

        void CreateStatic(const glm::mat4& transform);
        void CreateDynamic(float density, const glm::mat4& transform);

        RigidBody& GetRigidBody() { return m_rb; }
        const RigidBody& GetRigidBody() const { return m_rb; }

    private:
        unsigned m_guid;
        RigidBody m_rb;
        PhysicsScene* m_scene = nullptr;
    };
    template<typename S> void serialize(S& s, RigidBodyComponent& v) { }
    inline bool operator==(const RigidBodyComponent& lhs, const RigidBodyComponent& rhs) { return true; }

    void APHELION_API RegisterECSComponents();
}