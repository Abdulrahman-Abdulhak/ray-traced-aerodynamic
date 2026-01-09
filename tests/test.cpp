#include <cmath>
#include <iostream>
#include <memory>
#include <string>

#include "rtsa/mesh.hpp"
#include "rtsa/mesh_object.hpp"
#include "rtsa/physics_object.hpp"
#include "rtsa/raytraced_shadow_sampler_estimator.hpp"
#include "rtsa/vec3.hpp"

using rtsa::Mesh;
using rtsa::MeshObject;
using rtsa::PhysicsObject;
using rtsa::RayTracedShadowSamplerEstimator;
using rtsa::Vec3;

namespace {

struct TestStats {
    int passed = 0;
    int failed = 0;
};

void reportFailure(const std::string& name, const std::string& detail) {
    std::cerr << "[FAIL] " << name << " - " << detail << "\n";
}

void expectNear(TestStats& stats,
                double value,
                double expected,
                double tolerance,
                const std::string& name) {
    if (std::abs(value - expected) <= tolerance) {
        stats.passed++;
    } else {
        stats.failed++;
        reportFailure(
            name,
            "expected " + std::to_string(expected) +
                " +/- " + std::to_string(tolerance) +
                ", got " + std::to_string(value)
        );
    }
}

void expectEqual(TestStats& stats,
                 double value,
                 double expected,
                 const std::string& name) {
    if (value == expected) {
        stats.passed++;
    } else {
        stats.failed++;
        reportFailure(
            name,
            "expected " + std::to_string(expected) +
                ", got " + std::to_string(value)
        );
    }
}

Mesh makeTriangleMesh() {
    Mesh m;
    m.vertices = {
        {0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0}
    };
    m.indices = {
        {0, 1, 2}
    };
    return m;
}

Mesh makeSquarePlate(double side) {
    Mesh m;
    const double h = 0.5 * side;
    m.vertices = {
        {0.0, -h, -h},
        {0.0,  h, -h},
        {0.0,  h,  h},
        {0.0, -h,  h}
    };
    m.indices = {
        {0, 1, 2},
        {0, 2, 3}
    };
    return m;
}

Mesh translateMesh(const Mesh& base, const Vec3& delta) {
    Mesh m = base;
    for (auto& v : m.vertices) {
        v = v + delta;
    }
    return m;
}

class CustomObject : public PhysicsObject {
public:
    explicit CustomObject(Mesh mesh)
        : mesh_{std::move(mesh)} {}
    void update(double /*dt*/) override {}
    const Mesh* mesh() const override { return &mesh_; }
private:
    Mesh mesh_;
};

} // namespace

int main() {
    TestStats stats{};
    RayTracedShadowSamplerEstimator estimator;
    const Vec3 wind{1.0, 0.0, 0.0};
    const uint32_t samples = 1024;

    {
        auto cubeMesh = Mesh::unitCube();
        auto cubeObj = std::make_shared<MeshObject>(std::make_shared<Mesh>(cubeMesh));
        double area = estimator.estimateFrontalArea(*cubeObj->mesh(), wind, samples);
        expectNear(stats, area, 1.0, 0.1, "unit cube frontal area along +X");
    }

    {
        Mesh triMesh = makeTriangleMesh();
        CustomObject triObj(triMesh);
        double area = estimator.estimateFrontalArea(*triObj.mesh(), wind, samples);
        expectNear(stats, area, 0.5, 0.1, "single triangle frontal area along +X");
    }

    {
        Mesh plate = makeSquarePlate(2.0);
        CustomObject plateObj(plate);
        double area = estimator.estimateFrontalArea(*plateObj.mesh(), wind, samples);
        expectNear(stats, area, 4.0, 0.2, "square plate frontal area along +X");
    }

    {
        Mesh translatedCube = translateMesh(Mesh::unitCube(), Vec3{5.0, -2.0, 3.0});
        CustomObject cubeObj(translatedCube);
        double area = estimator.estimateFrontalArea(*cubeObj.mesh(), wind, samples);
        expectNear(stats, area, 1.0, 0.1, "translated cube frontal area along +X");
    }

    {
        Mesh empty;
        CustomObject emptyObj(empty);
        double area = estimator.estimateFrontalArea(*emptyObj.mesh(), wind, samples);
        expectEqual(stats, area, 0.0, "empty mesh frontal area");
    }

    if (stats.failed == 0) {
        std::cout << "[OK] " << stats.passed << " tests passed.\n";
        return 0;
    }

    std::cerr << "[FAIL] " << stats.failed << " tests failed, "
              << stats.passed << " passed.\n";
    return 1;
}
