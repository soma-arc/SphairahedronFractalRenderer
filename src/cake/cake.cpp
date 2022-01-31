#include "cake.h"
#include "../sphairahedron.h"
#include "../sphere.h"

Cake::Cake(float _zb) : Sphairahedron(_zb, 0) {
    numFaces = 6;
    numSpheres = 2;
    numPlanes = 4;
    vertexIndexes = vector<vector<int>>{{0, 3, 5}, {0, 1, 4},
                                        {2, 3, 5},{1, 2, 4},
                                        {1, 4, 5},
                                        {2, 4, 5}, {0, 1, 2}};
    numVertexes = vertexIndexes.size();
    planes = Sphairahedron::PRISM_PLANES_2222_SQUARE;
}

void Cake::computeGenSpheres() {
    gSpheres.clear();
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[0])); // O1
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[1])); // O2
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[2])); // O3
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[3])); // O4
    gSpheres.push_back(inversionSphere.invertOnSphere(prismSpheres[0])); // O5
    gSpheres.push_back(inversionSphere.invertOnSphere(prismSpheres[1])); // O6
}

void Cake::computeInversionSphere() {
    inversionSphere = Sphere(
        prismSpheres[2].center.x(),
        prismSpheres[2].center.y(),
        prismSpheres[2].center.z(),
        0.5);
}

jinja2::ValuesMap Cake::getShaderTemplateContext() {
    jinja2::ValuesMap data;
    data["numPrismSpheres"] = numSpheres;
    data["numPrismPlanes"] = numPlanes;
    data["numSphairahedronSpheres"] = numFaces;
    data["numSeedSpheres"] = numVertexes;
    data["numDividePlanes"] = numDividePlanes;
    data["numExcavationSpheres"] = numExcavationSpheres;
    return data;
}

void Cake::computeVertexes() {
    printf("vertexes compute %zu\n", vertexIndexes.size());
    vertexes.clear();
    for(const auto &vert : vertexIndexes) {
        vertexes.push_back(computeIdealVertex(gSpheres[vert[0]],
                                              gSpheres[vert[1]],
                                              gSpheres[vert[2]]));
    }
    printf("Done %zu\n", vertexes.size());
}

void Cake::computeSeedSpheres() {
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

CakeA::CakeA(float _zb): Cake(_zb) {
    update();
}

void CakeA::update() {
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

void CakeA::computeSpheres() {
     float r5 = (2 + zb * zb) / (4 * SQRT_2);
     float r6 = SQRT_6 * (2 + zb * zb) / 8;
     Sphere s5(-r5 * SIN_PI_12, zb, 1 - r5 * COS_PI_12, r5);
     
     float dx = (1 - r6 / SQRT_2);
     float dy = -r6 / SQRT_2;
     Sphere s6(dx - r6 * COS_5_PI_12, 0, dy + r6 * SIN_5_PI_12,
                              r6);
     prismSpheres = {s5, s6};
     float cx = -r6 / SQRT_2;
     float cy = 1 - r6 / SQRT_2;
     Plane p1(Vec3f(cx, 4, cy),
              Vec3f((cx + dx) * 0.5, 6, (cy + dy) * 0.5),
              Vec3f(dx, -3, dy),
              planes[2].normal);
     planes[2] = p1;
}

CakeB::CakeB(float _zb): Cake(_zb) {
    update();
}

void CakeB::update() {
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

void CakeB::computeSpheres() {
    float r5 = (2 + zb * zb) * 0.25;
    float r6 = r5;
    float adSinPi4 = (2 + zb * zb) * 0.25;
    Sphere s5(0, zb, 1 - adSinPi4, r5);
    Sphere s6(1 - adSinPi4, 0, 0, r6);
    
    prismSpheres = {s5, s6};
    float cx = -adSinPi4;
    float cy = 1 - adSinPi4;
    float dx = cy;
    float dy = cx;
    planes[2] = Plane(Vec3f(cx, 4, cy),
                      Vec3f((cx + dx) * 0.5, 6, (cy + dy) * 0.5),
                      Vec3f(dx, -2, dy),
                      planes[2].normal);
}

CakeC::CakeC(float _zb): Cake(_zb) {
    update();
}

void CakeC::update() {
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

void CakeC::computeSpheres() {
    float r5 = (2 + zb * zb) / (2 * SQRT_6);
    float r6 = r5;
    Sphere s5(r5 * SIN_PI_12, zb, 1 - r5 * COS_PI_12, r5);
    
    float dx = (1 - r5 / SQRT_2);
    float dy = -r5 / SQRT_2;
    Sphere s6(dx - r6 * COS_5_PI_12, 0, dy + r6 * SIN_5_PI_12,
              r6);
    prismSpheres = {s5, s6};
    float cx = -r6 / SQRT_2;
    float cy = 1 - r6 / SQRT_2;
    planes[2] = Plane(Vec3f(cx, 4, cy),
                      Vec3f((cx + dx) * 0.5, 6, (cy + dy) * 0.5),
                      Vec3f(dx, -3, dy),
                      planes[2].normal);
}
