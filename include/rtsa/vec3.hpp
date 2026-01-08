#pragma once
#include <cmath>
#include <ostream>

namespace rtsa {

struct Vec3 {
    double x{}, y{}, z{};
    constexpr Vec3() = default;
    constexpr Vec3(double x_, double y_, double z_) : x{x_}, y{y_}, z{z_} {}

    Vec3 operator+(const Vec3& o) const { return Vec3{x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return Vec3{x-o.x, y-o.y, z-o.z}; }
    Vec3 operator-() const { return Vec3{-x, -y, -z}; }
    Vec3 operator*(double s) const { return Vec3{x*s, y*s, z*s}; }
    Vec3 operator/(double s) const { return Vec3{x/s, y/s, z/s}; }

    double dot(const Vec3& o) const { return x*o.x + y*o.y + z*o.z; }
    Vec3 cross(const Vec3& o) const {
        return Vec3{
            y*o.z - z*o.y,
            z*o.x - x*o.z,
            x*o.y - y*o.x
        };
    }
    double length() const { return std::sqrt(dot(*this)); }
    Vec3 normalized() const {
        double len = length();
        return len > 0.0 ? (*this) / len : Vec3{0,0,0};
    }
};

inline std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    os << v.x << " " << v.y << " " << v.z;
    return os;
}

} // namespace rtsa
