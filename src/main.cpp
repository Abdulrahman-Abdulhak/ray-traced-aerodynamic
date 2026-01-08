#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

#include "rtsa/vec3.hpp"
#include "rtsa/mesh.hpp"
#include "rtsa/mesh_object.hpp"
#include "rtsa/world.hpp"
#include "rtsa/raytraced_shadow_sampler_estimator.hpp"
#include "rtsa/aerodynamics.hpp"

using namespace rtsa;

static bool parseVec3(int argc, char** argv, int& i, Vec3& out) {
    if (i+3 >= argc) return false;
    out.x = std::atof(argv[++i]);
    out.y = std::atof(argv[++i]);
    out.z = std::atof(argv[++i]);
    return true;
}

int main_cli(int argc, char** argv) {
    // Defaults
    std::string meshPath;
    int samples = 1024;
    uint32_t seed = 1337;
    double rho = 1.225;
    double Cd = 1.0;
    Vec3 wind{1.0, 0.0, 0.0};
    int steps = 10;
    double dt = 0.1;

    // Simple CLI parsing
    for (int i=1;i<argc;i++) {
        std::string a = argv[i];
        if (a=="--mesh" && i+1<argc) meshPath = argv[++i];
        else if (a=="--samples" && i+1<argc) samples = std::atoi(argv[++i]);
        else if (a=="--seed" && i+1<argc) seed = static_cast<uint32_t>(std::atoi(argv[++i]));
        else if (a=="--rho" && i+1<argc) rho = std::atof(argv[++i]);
        else if (a=="--cd" && i+1<argc) Cd = std::atof(argv[++i]);
        else if (a=="--wind") { if (!parseVec3(argc, argv, i, wind)) { std::cerr<<"Invalid --wind args\n"; return 1; } }
        else if (a=="--steps" && i+1<argc) steps = std::atoi(argv[++i]);
        else if (a=="--dt" && i+1<argc) dt = std::atof(argv[++i]);
        else { std::cerr << "Unknown arg: " << a << "\n"; }
    }

    // Build world
    World world{WindField(wind)};

    // Load mesh if provided (not implemented) else use unit cube.
    std::shared_ptr<Mesh> meshPtr;
    if (meshPath.empty()) {
        meshPtr = std::make_shared<Mesh>(Mesh::unitCube());
    } else {
        // Mesh loading is out of scope for this skeleton. Future: implement OBJ/PLY loader.
        std::cerr << "Mesh loading not implemented; using unit cube instead.\n";
        meshPtr = std::make_shared<Mesh>(Mesh::unitCube());
    }

    auto obj = std::make_shared<MeshObject>(meshPtr);
    world.addObject(obj);

    RayTracedShadowSamplerEstimator estimator;

    // CSV header
    std::cout << "step,time,wind_x,wind_y,wind_z,area_est,drag_mag\n";

    double time = 0.0;
    for (int step=0; step<steps; ++step) {
        // update world (objects are standstill)
        world.update(dt);

        Vec3 w = world.wind().wind();
        double v = w.length();
        const Mesh* m = obj->mesh();

        double area = estimator.estimateFrontalArea(*m, w.normalized(), samples, seed);
        double drag = computeDragMagnitude(rho, Cd, v, area);

        std::cout << step << "," << time << "," << w.x << "," << w.y << "," << w.z
                  << "," << area << "," << drag << "\n";

        time += dt;
    }

    return 0;
}

int main(int argc, char** argv) {
    return main_cli(argc, argv);
}
