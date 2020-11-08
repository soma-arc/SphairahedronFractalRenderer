#include "cubeA.h"
#include "cube.h"
#include "../sphere.h"
#include "../sphairahedron.h"

CubeA::CubeA(float _zb, float _zc) : Cube(_zb, _zc) {
    planes = Sphairahedron::PRISM_PLANES_333;
}

void CubeA::computeSpheres() {
    float r2 = 0.5 + (zb * zc) / 3.0;
    float r4 = 0.5 + (zb * zb - zb * zc) / 3.0;
    float r6 = 0.5 + (-zb * zc + zc * zc) / 3.0;
    Sphere s2(1 - r2, 0, 0, r2);
    Sphere s4(-(1 - r4) * 0.5, zb, sqrt(3) * (1 - r4) * 0.5, r4);
    Sphere s6(-(1 - r6) * 0.5, zc, -sqrt(3) * (1 - r6) * 0.5, r6);
    prismSpheres = {s2, s4, s6};
}
