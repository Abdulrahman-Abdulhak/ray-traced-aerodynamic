#pragma once
#include "physics_object.hpp"
#include "windfield.hpp"
#include <vector>
#include <memory>

namespace rtsa {

class World {
public:
    explicit World(WindField wind) : wind_{wind} {}

    void addObject(std::shared_ptr<PhysicsObject> obj) {
        objects_.push_back(std::move(obj));
    }

    void update(double dt) {
        for (auto& o : objects_) o->update(dt);
    }

    const std::vector<std::shared_ptr<PhysicsObject>>& objects() const { return objects_; }
    const WindField& wind() const { return wind_; }
    WindField& wind() { return wind_; }

private:
    std::vector<std::shared_ptr<PhysicsObject>> objects_;
    WindField wind_;
};

} // namespace rtsa
