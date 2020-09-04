#include "Aphelion/Physics/PhysicsGeometry.h"
#include "Aphelion/Physics/PhysicsFoundation.h"

namespace ap
{
    //void PhysicsGeometry::ToGeomTransform(glm::mat4& transform)
    //{
    //}

    //void PhysicsGeometry::FromGeomTransform(glm::mat4& transform)
    //{
    //}

    PhysicsGeometry PhysicsGeometry::CreatePlane()
    {
        //auto obj = PhysicsGeometry{ physx::PxPlaneGeometry() };
        //obj.m_isPlane = true;
        //return obj;
        return physx::PxGeometryHolder(physx::PxPlaneGeometry());
    }

    PhysicsGeometry PhysicsGeometry::CreateSphere(float radius)
    {
        return physx::PxGeometryHolder(physx::PxSphereGeometry(radius));
    }

    PhysicsGeometry PhysicsGeometry::CreateBox(const glm::vec3& halfSize)
    {
        // https://gameworksdocs.nvidia.com/PhysX/4.0/documentation/PhysXGuide/Manual/Geometry.html#boxes
        // TODO: Verify if x should be y in our case (to stay consistent with coordinate systems)
        return physx::PxGeometryHolder(physx::PxBoxGeometry(halfSize.x,halfSize.y,halfSize.z));
    }
    PhysicsGeometry PhysicsGeometry::CreateTriangleMesh(
        const std::vector<float>& vertices, 
        const std::vector<uint32_t>& indices, 
        size_t stride)
    {
        // https://gameworksdocs.nvidia.com/PhysX/4.0/documentation/PhysXGuide/Manual/Geometry.html
        using namespace physx;
        PxCookingParams params(PxGetPhysics().getTolerancesScale());
        // disable mesh cleaning - perform mesh validation on development configurations
        //params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
        // disable edge precompute, edges are set for each triangle, slows contact generation
        //params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
        params.meshPreprocessParams = PxMeshPreprocessingFlag::eFORCE_32BIT_INDICES;
        params.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
        params.meshWeldTolerance = 1e-4f;
            // lower hierarchy for internal mesh
        //params.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;
        auto& cooking = ap::PhysicsFoundation::GetCooking();
        cooking.setParams(params);

        PxTriangleMeshDesc meshDesc;
        // The points count refers to the amount of vertices, not the amount of elements
        meshDesc.points.count = vertices.size() / (stride / sizeof(float));
        meshDesc.points.stride = stride;
        meshDesc.points.data = vertices.data();

        meshDesc.triangles.count = indices.size() / 3;
        meshDesc.triangles.stride = 3 * sizeof(PxU32);
        meshDesc.triangles.data = indices.data();

        bool res = cooking.validateTriangleMesh(meshDesc);
        //ap::Log::GetCoreLogger()->flush();
        //AP_CORE_TRACE("Verified mesh");
        //AP_CORE_ASSERT(res, "Cooking mesh is invalid!");

        // TODO: Verify if we need to delete this mesh
        PxTriangleMesh* mesh = cooking.createTriangleMesh(meshDesc,
            PxGetPhysics().getPhysicsInsertionCallback());

        PxTriangleMeshGeometry geom(mesh);

        return PhysicsGeometry(PxGeometryHolder(geom));
    }
}