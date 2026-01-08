#include "rtsa/raytraced_shadow_sampler_estimator.hpp"
#include "rtsa/mesh.hpp"
#include "rtsa/ray.hpp"
#include <random>
#include <algorithm>
#include <vector>

namespace rtsa {

// Simple axis-aligned bounding box (AABB) hitbox helper.
// Returns pair<min, max> corners. If mesh has no vertices, returns zero box.
static std::pair<Vec3, Vec3> computeMeshAABB(const Mesh& mesh) {
    Vec3 minV{0.0, 0.0, 0.0};
    Vec3 maxV{0.0, 0.0, 0.0};
    if (mesh.vertices.empty()) return {minV, maxV};

    minV = mesh.vertices[0];
    maxV = mesh.vertices[0];
    for (const auto& v : mesh.vertices) {
        if (v.x < minV.x) minV.x = v.x;
        if (v.y < minV.y) minV.y = v.y;
        if (v.z < minV.z) minV.z = v.z;

        if (v.x > maxV.x) maxV.x = v.x;
        if (v.y > maxV.y) maxV.y = v.y;
        if (v.z > maxV.z) maxV.z = v.z;
    }
    return {minV, maxV};
}

// Sampling square placed on plane orthogonal to windDir that sits flush with
// the mesh on the upwind side (distance 0). The square is centered and sized
// to contain the mesh projection; axes u and v form an orthonormal basis on
// the plane. Normal points opposite the wind direction (i.e. -windDir).
struct SamplingSquare {
    Vec3 center; // 3D center position on the plane
    Vec3 normal; // plane normal (should be -windDir.normalized())
    Vec3 axis_u; // first in-plane axis (unit)
    Vec3 axis_v; // second in-plane axis (unit)
    double halfSize; // half-extent of the square (square is 2*halfSize x 2*halfSize)
};

static SamplingSquare computeSamplingSquare(const Mesh& mesh, const Vec3& windDir) {
    SamplingSquare sq{};
    if (mesh.vertices.empty()) {
        sq.center = Vec3{0,0,0};
        sq.normal = -windDir.normalized();
        sq.axis_u = Vec3{1,0,0};
        sq.axis_v = Vec3{0,1,0};
        sq.halfSize = 0.5;
        return sq;
    }

    Vec3 u = windDir.normalized();
    // plane normal should be opposite the wind direction
    sq.normal = -u;

    auto [minV, maxV] = computeMeshAABB(mesh);
    // Find extreme along wind direction (max projection) to place plane flush with mesh
    double maxP = mesh.vertices.front().dot(u);
    for (const auto& v : mesh.vertices) maxP = std::max(maxP, v.dot(u));
    // choose a point on plane: u * maxP (plane passes through this point)
    Vec3 planePoint = u * maxP;

    // Build orthonormal basis for the plane
    Vec3 temp{0.0, 1.0, 0.0};
    if (std::abs(u.y) > 0.999) temp = Vec3{1.0, 0.0, 0.0};
    // axis_u = temp cross u
    Vec3 axis_u = temp.cross(u).normalized();
    Vec3 axis_v = u.cross(axis_u).normalized();

    // Compute mesh centroid to align square center 'level' with mesh center
    Vec3 centroid{0.0, 0.0, 0.0};
    for (const auto& v : mesh.vertices) centroid = centroid + v;
    centroid = centroid / static_cast<double>(mesh.vertices.size());

    // Project centroid into plane coordinates relative to planePoint
    Vec3 dcent = centroid - planePoint;
    double cx = dcent.dot(axis_u);
    double cy = dcent.dot(axis_v);

    // Side length equals distance between minV and maxV (AABB diagonal length)
    Vec3 diag = maxV - minV;
    double side = diag.length();
    if (side <= 0.0) side = 1.0; // fallback

    sq.halfSize = 0.5 * side;
    sq.axis_u = axis_u;
    sq.axis_v = axis_v;
    // center is at same 'level' as centroid projected onto the flush plane
    sq.center = planePoint + axis_u * cx + axis_v * cy;
    return sq;
}

static std::vector<Vec3> computeSamplingPoints(
    const SamplingSquare& square,
    uint32_t samples
) {
    std::size_t pointsCount = samples * samples;
    std::vector<Vec3> samplePoints;
    samplePoints.reserve(pointsCount);

    // Generate an samples x samples grid evenly across the square (in plane coordinates).
    if (samples == 1) {
        samplePoints.push_back(square.center);
    } else {
        double half = square.halfSize;
        double span = 2.0 * half;
        double step = span / static_cast<double>(samples - 1);
        // grid centered, i/j in [0..samples-1]
        double originOffset = -half;
        for (uint32_t j = 0; j < samples; ++j) {
            double oy = originOffset + j * step;
            for (uint32_t i = 0; i < samples; ++i) {
                double ox = originOffset + i * step;
                Vec3 p = square.center + square.axis_u * ox + square.axis_v * oy;
                samplePoints.push_back(p);
            }
        }
    }

    return samplePoints;
}

// Create rays originating from a single distant point such that the
// direction from that origin to the plane center equals `windDir`.
// Rays point toward each sample point on the plane.
static std::vector<Ray> createRaysFromDistantPoint(
    const SamplingSquare& plane,
    const std::vector<Vec3>& points,
    const Vec3& windDir,
    double distance
) {
    std::vector<Ray> rays;
    rays.reserve(points.size());

    Vec3 dir = windDir.normalized();
    Vec3 origin = plane.center - dir * distance;

    for (const auto& p : points) {
        Vec3 rd = (p - origin).normalized();
        rays.push_back(Ray{origin, rd});
    }
    return rays;
};

static bool hitMeshBrute(const Ray& r, const Mesh& m, double tMin = 1e-6) {
    // Brute-force ray-triangle intersection using Möller–Trumbore.
    // Returns true if any triangle is intersected at t > tMin.
    const double EPS = 1e-9;
    for (const auto& idx : m.indices) {
        // validate indices
        int i0 = idx[0];
        int i1 = idx[1];
        int i2 = idx[2];
        if (i0 < 0 || i1 < 0 || i2 < 0) continue;
        if (static_cast<size_t>(i0) >= m.vertices.size()) continue;
        if (static_cast<size_t>(i1) >= m.vertices.size()) continue;
        if (static_cast<size_t>(i2) >= m.vertices.size()) continue;

        const Vec3& v0 = m.vertices[static_cast<size_t>(i0)];
        const Vec3& v1 = m.vertices[static_cast<size_t>(i1)];
        const Vec3& v2 = m.vertices[static_cast<size_t>(i2)];

        Vec3 edge1 = v1 - v0;
        Vec3 edge2 = v2 - v0;

        Vec3 h = r.direction.cross(edge2);
        double a = edge1.dot(h);
        if (std::abs(a) < EPS) continue; // ray parallel to triangle

        double f = 1.0 / a;
        Vec3 s = r.origin - v0;
        double u = f * s.dot(h);
        if (u < 0.0 || u > 1.0) continue;

        Vec3 q = s.cross(edge1);
        double v = f * r.direction.dot(q);
        if (v < 0.0 || (u + v) > 1.0) continue;

        double t = f * edge2.dot(q);
        if (t > tMin) {
            return true;
        }
    }
    return false;
}

static uint32_t countHits(
    const std::vector<Ray>& rays,
    const Mesh& mesh,
    double tMin = 1e-6
) {
    uint32_t hits = 0;
    for (const auto& r : rays) {
        if (hitMeshBrute(r, mesh, tMin)) {
            hits++;
        }
    }
    return hits;
}

double RayTracedShadowSamplerEstimator::estimateFrontalArea(
    const Mesh& mesh,
    const Vec3& windDir,
    uint32_t samples
) const {
    auto plane = computeSamplingSquare(mesh, windDir);
    auto samplePoints = computeSamplingPoints(plane, samples);
    auto farDist = 1e6;
    auto rays = createRaysFromDistantPoint(plane, samplePoints, windDir, farDist);
    auto hits = countHits(rays, mesh);
    double planeLengthSide = plane.halfSize * 2.0;
    double planeSize = planeLengthSide * planeLengthSide;
    double hitRatio = rays.empty() ? 0.0 : (static_cast<double>(hits) / static_cast<double>(rays.size()));
    double meshAreaEstimate = planeSize * hitRatio;
    return meshAreaEstimate;
}

} // namespace rtsa
