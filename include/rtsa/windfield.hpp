#pragma once
#include "vec3.hpp"

namespace rtsa {

// For now WindField is constant; future extension can add spatio-temporal fields.
class WindField {
public:
    explicit WindField(const Vec3& constantWind) : wind_{constantWind} {}
    Vec3 wind() const { return wind_; }
    void setWind(const Vec3& w) { wind_ = w; }
private:
    Vec3 wind_;
};

} // namespace rtsa
