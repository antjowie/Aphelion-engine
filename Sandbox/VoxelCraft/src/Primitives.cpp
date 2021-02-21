#include "Primitives.h"

FaceVertices GenerateFaceVertices(FaceDir dir, float x, float y, float z, float texIndex)
{
    // Pos3 Nor3 Tex2 Index1
    switch (dir)
    {
    case FaceDir::Top:
        return {
            x - 0.5f, y + 0.5f, z - 0.5f, 0.f, 1.f, 0.f, 0.f, 1.f, texIndex,
            x - 0.5f, y + 0.5f, z + 0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, texIndex,
            x + 0.5f, y + 0.5f, z + 0.5f, 0.f, 1.f, 0.f, 1.f, 0.f, texIndex,
            x + 0.5f, y + 0.5f, z - 0.5f, 0.f, 1.f, 0.f, 1.f, 1.f, texIndex,
        };

    case FaceDir::Bottom:
        return {
            x + 0.5f, y - 0.5f, z - 0.5f, 0.f, -1.f, 0.f, 1.f, 1.f, texIndex,
            x + 0.5f, y - 0.5f, z + 0.5f, 0.f, -1.f, 0.f, 1.f, 0.f, texIndex,
            x - 0.5f, y - 0.5f, z + 0.5f, 0.f, -1.f, 0.f, 0.f, 0.f, texIndex,
            x - 0.5f, y - 0.5f, z - 0.5f, 0.f, -1.f, 0.f, 0.f, 1.f, texIndex,
        };

    case FaceDir::Left:
        return {
            x - 0.5f, y + 0.5f, z - 0.5f, -1.f, 0.f, 0.f, 1.f, 1.f, texIndex,
            x - 0.5f, y - 0.5f, z - 0.5f, -1.f, 0.f, 0.f, 1.f, 0.f, texIndex,
            x - 0.5f, y - 0.5f, z + 0.5f, -1.f, 0.f, 0.f, 0.f, 0.f, texIndex,
            x - 0.5f, y + 0.5f, z + 0.5f, -1.f, 0.f, 0.f, 0.f, 1.f, texIndex,
        };

    case FaceDir::Right:
        return {
            x + 0.5f, y + 0.5f, z + 0.5f, 1.f, 0.f, 0.f, 0.f, 1.f, texIndex,
            x + 0.5f, y - 0.5f, z + 0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, texIndex,
            x + 0.5f, y - 0.5f, z - 0.5f, 1.f, 0.f, 0.f, 1.f, 0.f, texIndex,
            x + 0.5f, y + 0.5f, z - 0.5f, 1.f, 0.f, 0.f, 1.f, 1.f, texIndex,
        };

    case FaceDir::Back:
        return {
            x + 0.5f, y + 0.5f, z - 0.5f, 0.f, 0.f, -1.f, 1.f, 1.f, texIndex,
            x + 0.5f, y - 0.5f, z - 0.5f, 0.f, 0.f, -1.f, 1.f, 0.f, texIndex,
            x - 0.5f, y - 0.5f, z - 0.5f, 0.f, 0.f, -1.f, 0.f, 0.f, texIndex,
            x - 0.5f, y + 0.5f, z - 0.5f, 0.f, 0.f, -1.f, 0.f, 1.f, texIndex,
        };

    case FaceDir::Front:
        return {
            x - 0.5f, y + 0.5f, z + 0.5f, 0.f, 0.f, 1.f, 0.f, 1.f, texIndex,
            x - 0.5f, y - 0.5f, z + 0.5f, 0.f, 0.f, 1.f, 0.f, 0.f, texIndex,
            x + 0.5f, y - 0.5f, z + 0.5f, 0.f, 0.f, 1.f, 1.f, 0.f, texIndex,
            x + 0.5f, y + 0.5f, z + 0.5f, 0.f, 0.f, 1.f, 1.f, 1.f, texIndex,
        };
    }
}