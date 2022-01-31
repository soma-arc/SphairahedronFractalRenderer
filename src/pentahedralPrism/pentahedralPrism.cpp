#include "pentahedralPrism.h"
#include "../sphairahedron.h"
#include "../sphere.h"

PentahedralPrism::PentahedralPrism(float _zb) : Sphairahedron(_zb, 0) {
    numFaces = 5;
    numSpheres = 2;
    numPlanes = 3;
    vertexIndexes = vector<vector<int>>{{0, 1, 2}, {2, 3, 4},
                                        {0, 3, 4}, {1, 2, 3},
                                        {0, 1, 3}, {0, 2, 4}};
    numVertexes = vertexIndexes.size();
}

void PentahedralPrism::computeGenSpheres() {
    gSpheres.clear();
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[0])); // O1
    gSpheres.push_back(inversionSphere.invertOnSphere(prismSpheres[0])); // O2
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[1])); // O3
    gSpheres.push_back(inversionSphere.invertOnSphere(prismSpheres[1])); // O4
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[2])); // O5
}

void PentahedralPrism::computeInversionSphere() {
    inversionSphere = Sphere(0.1, 4, -0.1, 0.5);
}

void PentahedralPrism::computeDividePlanes() {
    dividePlanes.clear();
    Plane p = computePlane(0, 1, 2);
    dividePlanes.push_back(p);
}

jinja2::ValuesMap PentahedralPrism::getShaderTemplateContext() {
    jinja2::ValuesMap data;
    data["numPrismSpheres"] = numSpheres;
    data["numPrismPlanes"] = numPlanes;
    data["numSphairahedronSpheres"] = numFaces;
    data["numSeedSpheres"] = numVertexes;
    data["numDividePlanes"] = numDividePlanes;
    data["numExcavationSpheres"] = numExcavationSpheres;
    return data;
}

void PentahedralPrism::computeVertexes() {
    printf("vertexes compute %zu\n", vertexIndexes.size());
    vertexes.clear();
    for(const auto &vert : vertexIndexes) {
        vertexes.push_back(computeIdealVertex(gSpheres[vert[0]],
                                              gSpheres[vert[1]],
                                              gSpheres[vert[2]]));
    }
    printf("Done %zu\n", vertexes.size());
}

void PentahedralPrism::computeSeedSpheres() {
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

PentahedralPrismA::PentahedralPrismA(float _zb): PentahedralPrism(_zb) {
    planes = PRISM_PLANES_333;
    update();
}

void PentahedralPrismA::update() {
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

void PentahedralPrismA::computeSpheres() {
    float r4 = 1;
    float r2 = (zb * zb) / 3;
    Sphere s2(1 - r2, 0, 0, r2);
    Sphere s4(0, zb, 0, r4);
    prismSpheres = {s2, s4};
}

PentahedralPrismB::PentahedralPrismB(float _zb): PentahedralPrism(_zb) {
    planes = PRISM_PLANES_333;
    update();
}

void PentahedralPrismB::update() {
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

void PentahedralPrismB::computeSpheres() {
    float r4 = SQRT_3 * 0.5;
    float r2 = (3 + 2 * zb * zb) / 6;
    Sphere s2(1 - r2, 0, 0, r2);
    Sphere s4(-0.5, zb, 0, r4);
    prismSpheres = {s2, s4};
}

PentahedralPrismC::PentahedralPrismC(float _zb): PentahedralPrism(_zb) {
    planes = PRISM_PLANES_333;
    update();
}

void PentahedralPrismC::update() {
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

void PentahedralPrismC::computeSpheres() {
    float r4 = SQRT_3;
    float r2 = (zb * zb - 3) / 3;
    Sphere s2(1 - r2, 0, 0, r2);
    Sphere s4(1, zb, 0, r4);
    prismSpheres = {s2, s4};
}

PentahedralPrismD::PentahedralPrismD(float _zb): PentahedralPrism(_zb) {
    planes = PRISM_PLANES_236;
    update();
}

void PentahedralPrismD::update() {
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

void PentahedralPrismD::computeSpheres() {
    float r4 = 1;
    float r2 = (zb * zb) / SQRT_3;
    Sphere s2(1 - r2 * SQRT_3 * 0.5, 0, r2 * 0.5, r2);
    Sphere s4(0, zb, 0, r4);
    prismSpheres = {s2, s4};
}

PentahedralPrismE::PentahedralPrismE(float _zb): PentahedralPrism(_zb) {
    planes = PRISM_PLANES_244;
    update();
}

void PentahedralPrismE::update() {
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

void PentahedralPrismE::computeSpheres() {
    float r4 = SQRT_2;
    float r2 = (zb * zb - 2) / 2;
    Sphere s2(1 - r2, 0, 0, r2);
    Sphere s4(1, zb, 0, r4);
    prismSpheres = {s2, s4};
}

PentahedralPrismF::PentahedralPrismF(float _zb): PentahedralPrism(_zb) {
    planes = PRISM_PLANES_244;
    update();
}

void PentahedralPrismF::update() {
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

void PentahedralPrismF::computeSpheres() {
    float r4 = 1;
    float r2 = (zb * zb) / 2;
    Sphere s2(1 - r2, 0, 0, r2);
    Sphere s4(0, zb, 0, r4);
    prismSpheres = {s2, s4};
}

