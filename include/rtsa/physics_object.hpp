#pragma once
#include "mesh.hpp"
#include <memory>

namespace rtsa {

// Abstract base for physical objects in the world.
// Responsibilities: own geometry, expose update hooks. Objects are expected to be
// non-moving for this initial skeleton (standstill), but update(dt) is present
// for future dynamics and plugin-style polymorphism.
class PhysicsObject {
public:
    virtual ~PhysicsObject() = default;
    virtual void update(double dt) = 0;
    virtual const Mesh* mesh() const = 0;
};

} // namespace rtsa
