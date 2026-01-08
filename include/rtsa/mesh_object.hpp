#pragma once
#include "physics_object.hpp"
#include <memory>

namespace rtsa {

class MeshObject : public PhysicsObject {
public:
    explicit MeshObject(std::shared_ptr<const Mesh> meshPtr)
        : mesh_{std::move(meshPtr)} {}
    void update(double /*dt*/) override {
        // Standstill object for now; placeholder for future rigid-body updates.
    }
    const Mesh* mesh() const override { return mesh_.get(); }
private:
    std::shared_ptr<const Mesh> mesh_;
};

} // namespace rtsa
