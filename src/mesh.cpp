#include "rtsa/mesh.hpp"

namespace rtsa {

Mesh Mesh::unitCube() {
    Mesh m;
    // 8 vertices of cube centered at origin, side length 1
    m.vertices = {
        {-0.5,-0.5,-0.5}, {0.5,-0.5,-0.5}, {0.5,0.5,-0.5}, {-0.5,0.5,-0.5},
        {-0.5,-0.5,0.5},  {0.5,-0.5,0.5},  {0.5,0.5,0.5},  {-0.5,0.5,0.5}
    };
    // 12 triangles (two per face)
    m.indices = {
        {0,1,2},{0,2,3}, {4,6,5},{4,7,6}, // front/back
        {0,4,5},{0,5,1}, {1,5,6},{1,6,2},
        {2,6,7},{2,7,3}, {3,7,4},{3,4,0}
    };
    return m;
}

} // namespace rtsa
