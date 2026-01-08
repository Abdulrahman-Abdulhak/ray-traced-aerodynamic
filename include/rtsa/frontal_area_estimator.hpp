#pragma once
#include "mesh.hpp"
#include "vec3.hpp"
#include <cstdint>

namespace rtsa {

// Interface for frontal area estimators. Implementations must be deterministic
// given the same RNG seed and parameters.
class FrontalAreaEstimator {
public:
    virtual ~FrontalAreaEstimator() = default;

    // Estimate projected frontal area of `mesh` when wind direction is `windDir`.
    // `samples` controls sampling resolution; `rngSeed` controls determinism.
    virtual double estimateFrontalArea(const Mesh& mesh,
                                       const Vec3& windDir,
                                       int samples,
                                       uint32_t rngSeed) const = 0;
};

} // namespace rtsa
