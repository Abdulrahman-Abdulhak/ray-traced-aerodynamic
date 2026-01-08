#pragma once
#include "vec3.hpp"
#include "triangle.hpp"
#include <vector>
#include <array>
#include <memory>

namespace rtsa {

struct Mesh {
    // Simple triangle mesh: vertices + triangle indices
    std::vector<Vec3> vertices;
    std::vector<std::array<int,3>> indices;

    Mesh() = default;

    // Build a unit cube centered at origin (12 triangles)
    static Mesh unitCube();
};

} // namespace rtsa
