#pragma once
#include "vec3.hpp"

namespace rtsa {

struct Ray {
    Vec3 origin;
    Vec3 direction; // should be normalized by caller
    Ray() = default;
    Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d) {}
};

} // namespace rtsa
