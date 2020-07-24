#pragma once

constexpr float planeVertices[]
{
    -1.f, 1.f, 0.f, 0.f, 1.f,
    -1.f,-1.f, 0.f, 0.f, 0.f,
     1.f,-1.f, 0.f, 1.f, 0.f,
     1.f, 1.f, 0.f, 1.f, 1.f,
};

constexpr unsigned planeIndices[]
{
    0,1,3,
    1,2,3,
};