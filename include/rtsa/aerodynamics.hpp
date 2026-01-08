#pragma once

namespace rtsa {

// Compute scalar drag force magnitude: 0.5 * rho * v^2 * Cd * A
inline double computeDragMagnitude(double rho, double Cd, double v, double A) {
    return 0.5 * rho * v * v * Cd * A;
}

} // namespace rtsa
