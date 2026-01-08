#pragma once
#include "frontal_area_estimator.hpp"

namespace rtsa {

// Stub estimator that will use ray-traced shadow sampling in future.
// TODO: implement ray-traced sampling: cast rays from a sampling plane,
// accumulate occlusion to compute projected area.
class RayTracedShadowSamplerEstimator : public FrontalAreaEstimator {
public:
    double estimateFrontalArea(const Mesh& mesh,
                               const Vec3& windDir,
                               uint32_t samples) const override;
};

} // namespace rtsa
