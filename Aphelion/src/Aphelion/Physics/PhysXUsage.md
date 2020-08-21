Since this is the first time that I integrate PhysX, I'll write down some API usage notes here

# Approximate workflow
All physics simulation


# Docs notes
## [Startup and Shutdown](https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/Startup.html)
Create the following systems:
```
Foundation
Physics
```


Optional systems
```
Cooking
Extensions
```
The Foundation is a system that all other systems/modules need. It is passed in their respective PxCreateXXX function
When shutting down, release in reversed init order with the `release` function.

The Foundation takes an error callback and an allocator callback

The Physics system takes a transport which is used for debugging and a ToleranceScale which represents your scaling values

## [Geometry](https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/Geometry.html)
Geometries build shapes for rigid bodies, collision triggers and volumes for the scene query system. There are also function to test interections between geometries, raycast against them and sweep one geometry against another.

Each geometry class defines a volume or surface with a fixed position and orientation and is interpeted by a transform.

There are two types of geometries:
* primitives (BoxGeom, SphereGeom). The geometry contains all the data for these
* meshes (ConvexMeshGeom, TriangleMeshGeom). The geometry has a pointer to another object (ConvexMesh, TriangleMesh). The PxGeom has a scale for these. These objects must be created using a cooking process.

Geometry is copied in and out of a PxShape class. You can use a GeometryHolder union which takes any geometry type by value. Each Mesh object has a reference count that tracks the number of PxShapes whose geometry reference that mesh.

[See how to setup different geometry types here](https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/Geometry.html#geometry-types)

