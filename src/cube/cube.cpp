#include "cube.h"
#include "../sphairahedron.h"
#include "../sphere.h"


Cube::Cube(float _zb, float _zc) : Sphairahedron(_zb, _zc) {
    numFaces = 6;
    numSpheres = 3;
    numPlanes = 3;
    vertexIndexes = vector<vector<int>>{{0, 1, 2}, {0, 3, 4}, {2, 4, 5}, {0, 1, 3},
                                        {3, 4, 5}, {1, 2, 5}, {1, 3, 5}, {0, 2, 4}};
    numVertexes = vertexIndexes.size();
}

void Cube::computeGenSpheres() {
    gSpheres.clear();
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[0])); // O1
    gSpheres.push_back(inversionSphere.invertOnSphere(prismSpheres[0])); // O2
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[1])); // O3
    gSpheres.push_back(inversionSphere.invertOnSphere(prismSpheres[1])); // O4
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[2])); // O5
    gSpheres.push_back(inversionSphere.invertOnSphere(prismSpheres[2])); // O6
}

void Cube::computeInversionSphere() {
    inversionSphere = Sphere(
        -prismSpheres[2].center.x(),
        -prismSpheres[2].center.y(),
         prismSpheres[2].center.z(),
         prismSpheres[2].r);
}

jinja2::ValuesMap Cube::getShaderTemplateContext() {
    jinja2::ValuesMap data;
    data["numPrismSpheres"] = numSpheres;
    data["numPrismPlanes"] = numPlanes;
    data["numSphairahedronSpheres"] = numFaces;
    data["numSeedSpheres"] = numVertexes;
    data["numDividePlanes"] = numDividePlanes;
    data["numExcavationSpheres"] = numExcavationSpheres;
    return data;
}

void Cube::computeVertexes() {
    printf("vertexes compute %zu\n", vertexIndexes.size());
    vertexes.clear();
    for(const auto &vert : vertexIndexes) {
        vertexes.push_back(computeIdealVertex(gSpheres[vert[0]],
                                              gSpheres[vert[1]],
                                              gSpheres[vert[2]]));
    }
    printf("Done %zu\n", vertexes.size());
}

void Cube::computeSeedSpheres() {
    seedSpheres.clear();
    printf("num seed sphere %d\n", numVertexes);
    for(int i = 0; i < numVertexes; i++) {
        addSphereIfNotExists(seedSpheres,
                             computeMinSeedSphere(vertexes[i], vertexes,
                                                  gSpheres[vertexIndexes[i][0]],
                                                  gSpheres[vertexIndexes[i][1]],
                                                  gSpheres[vertexIndexes[i][2]]));
    }
}

CubeA::CubeA(float _zb, float _zc) : Cube(_zb, _zc) {
    printf("cubeA d\n");
    planes = Sphairahedron::PRISM_PLANES_333;
    printf("pre update\n");
    update();
}

void CubeA::update() {
    computeSpheres();
    computeInversionSphere();
    computeGenSpheres();
    computeVertexes();
    computeDividePlanes();
    computeExcavationSpheres();
    computeSeedSpheres();
    computeConvexSphere();
    computeBoundingVolume();
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


CubeB::CubeB(float _zb, float _zc) : Cube(_zb, _zc) {
    planes = Sphairahedron::PRISM_PLANES_333;
    update();
}

void CubeB::update() {
    computeSpheres();
    computeInversionSphere();
    computeGenSpheres();
    computeVertexes();
    computeDividePlanes();
    computeExcavationSpheres();
    computeSeedSpheres();
    computeConvexSphere();
    computeBoundingVolume();
}

void CubeB::computeSpheres() {
    float r2 = (3 * SQRT_3 + 2 * SQRT_3 * zb * zc) / 9.0;
    float r4 = (3 * zb * zb - 4 * zb * zc + 3) / 9.0;
    float r6 = (3 * zc * zc - 2 * zb * zc + 6) / 9.0;
    Sphere s2((2 - SQRT_3 * r2) * 0.5, 0, r2 * 0.5, r2);
    Sphere s4(-(1 - r4) * 0.5, zb, SQRT_3 * (1 - r4) * 0.5, r4);
    Sphere s6(-(1 - r6) * 0.5, zc, -SQRT_3 * (1 - r6) * 0.5, r6);
    prismSpheres = {s2, s4, s6};
}

CubeC::CubeC(float _zb, float _zc) : Cube(_zb, _zc) {
    planes = Sphairahedron::PRISM_PLANES_333;
    update();
}

void CubeC::update() {
    computeSpheres();
    computeInversionSphere();
    computeGenSpheres();
    computeVertexes();
    computeDividePlanes();
    computeExcavationSpheres();
    computeSeedSpheres();
    computeConvexSphere();
    computeBoundingVolume();
}

void CubeC::computeSpheres() {
    float r2 = (zb * zb + 2 * zb * zc + 6) / (5 * SQRT_3);
    float r4 = (3 * zb * zb - 4 * zb * zc + 3) / (5 * SQRT_3);
    float r6 = (-zb * zb - 2 * zb * zc + 5 * zc * zc + 9) / 15.0;
    Sphere s2((2 - SQRT_3 * r2) * 0.5, 0, r2 * 0.5, r2);
    Sphere s4(-0.5, zb, SQRT_3 / 2 - r4, r4);
    Sphere s6(-(1 - r6) * 0.5, zc, -SQRT_3 * (1 - r6) * 0.5, r6);

    prismSpheres = {s2, s4, s6};
}

CubeD::CubeD(float _zb, float _zc) : Cube(_zb, _zc) {
    planes = Sphairahedron::PRISM_PLANES_236;
    update();
}

void CubeD::update() {
    computeSpheres();
    computeInversionSphere();
    computeGenSpheres();
    computeVertexes();
    computeDividePlanes();
    computeExcavationSpheres();
    computeSeedSpheres();
    computeConvexSphere();
    computeBoundingVolume();
}

void CubeD::computeSpheres() {
    float r2 = (3 * zb * zb + zc * zc + 6 * zb * zc + 6) / 18;
    float r4 = (15 * zb * zb - zc * zc - 6 * zb * zc + 12) / (18 * SQRT_3);
    float r6 = (-3 * zb * zb + 5 * zc * zc - 6 * zb * zc + 12) / 18.0;
    Sphere s2(1 - r2, 0, 0, r2);
    Sphere s4(0.5, zb, SQRT_3 / 2 - r4, r4);
    Sphere s6(-(1 - r6) * 0.5, zc, -SQRT_3 * (1 - r6) * 0.5, r6);

    prismSpheres = {s2, s4, s6};
}

CubeE::CubeE(float _zb, float _zc) : Cube(_zb, _zc) {
    planes = Sphairahedron::PRISM_PLANES_236;
    update();
}

void CubeE::update() {
    computeSpheres();
    computeInversionSphere();
    computeGenSpheres();
    computeVertexes();
    computeDividePlanes();
    computeExcavationSpheres();
    computeSeedSpheres();
    computeConvexSphere();
    computeBoundingVolume();
}

void CubeE::computeSpheres() {
    float r2 = (zc * zc + 6 * zb * zc + 3) / (7 * SQRT_3);
    float r4 = (7 * zb * zb - zc * zc - 6 * zb * zc + 4) / (14);
    float r6 = (2 * zc * zc - 2 * zb * zc + 6) / 7.0;
    Sphere s2(1 - SQRT_3 * 0.5 * r2, 0, r2 * 0.5, r2);
    Sphere s4((1 + r4) * 0.5, zb, (1 - r4) * SQRT_3 * 0.5, r4);
    Sphere s6(-(1 - r6) * 0.5, zc, -(1 - r6) * SQRT_3 * 0.5, r6);
    prismSpheres = {s2, s4, s6};
}

CubeH::CubeH(float _zb, float _zc) : Cube(_zb, _zc) {
    planes = Sphairahedron::PRISM_PLANES_236;
    update();
}

void CubeH::update() {
    computeSpheres();
    computeInversionSphere();
    computeGenSpheres();
    computeVertexes();
    computeDividePlanes();
    computeExcavationSpheres();
    computeSeedSpheres();
    computeConvexSphere();
    computeBoundingVolume();
}

void CubeH::computeSpheres() {
    float r2 = (3 * zb * zb + 2 * zb * zc + 3) / (5 * SQRT_3);
    float r4 = 2 * (zb * zb - zb * zc + 1) / (5);
    float r6 = (-3 * zb * zb - 2 * zb * zc + 5 * zc * zc + 12) / (10 * SQRT_3);
    Sphere s2(1 - SQRT_3 / 2 * r2, 0, r2 / 2, r2);
    Sphere s4((1 - r4) / 2, zb, (1 - r4) * SQRT_3 / 2, r4);
    Sphere s6(SQRT_3 / 2 * r6 - 0.5, zc, (-SQRT_3 + r6) / 2, r6);

    prismSpheres = {s2, s4, s6};
}

CubeI::CubeI(float _zb, float _zc) : Cube(_zb, _zc) {
    planes = Sphairahedron::PRISM_PLANES_244;
    update();
}

void CubeI::update() {
    computeSpheres();
    computeInversionSphere();
    computeGenSpheres();
    computeVertexes();
    computeDividePlanes();
    computeExcavationSpheres();
    computeSeedSpheres();
    computeConvexSphere();
    computeBoundingVolume();
}

void CubeI::computeSpheres() {
    float r2 = (zc * zc + 2 * zb * zc + 2) / 6;
    float r4 = SQRT_2 * (-zc * zc - 2 * zb * zc + 3 * zb * zb + 4) / 12;
    float r6 = (zc * zc - zb * zc + 2) / (3);
    Sphere s2(1 - r2, 0, 0, r2);
    Sphere s4(r4 / SQRT_2, zb, 1 - r4 / SQRT_2, r4);
    Sphere s6(0, zc, -1 + r6, r6);

    prismSpheres = {s2, s4, s6};
}
