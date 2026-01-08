#pragma once
#include "vec3.hpp"
#include <array>

namespace rtsa {

struct Triangle {
    std::array<Vec3,3> v;
    Triangle() = default;
    Triangle(const Vec3& a, const Vec3& b, const Vec3& c) { v = {a,b,c}; }
};

} // namespace rtsa
