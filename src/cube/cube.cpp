#include "cube.h"
#include "../sphairahedron.h"
#include "../sphere.h"


Cube::Cube(float _zb, float _zc) : Sphairahedron(_zb, _zc) { 
}

void Cube::computeGenSpheres() {
    printf("plane size %zu\n", planes.size());
    gSpheres.clear();
    printf("plane%d\n", 0);
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[0])); // O1
    printf("plane%d\n", 1);
    gSpheres.push_back(inversionSphere.invertOnSphere(prismSpheres[0])); // O2
    printf("plane%d\n", 2);
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[1])); // O3
    printf("plane%d\n", 3);
    gSpheres.push_back(inversionSphere.invertOnSphere(prismSpheres[1])); // O4
    printf("plane%d\n", 4);
    gSpheres.push_back(inversionSphere.invertOnPlane(planes[2])); // O5
    printf("plane%d\n", 5);
    gSpheres.push_back(inversionSphere.invertOnSphere(prismSpheres[2])); // O6
    printf("plane Done\n");
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
    printf("compute sphere\n");
    computeSpheres();
    computeInversionSphere();
    printf("compute genSpheres\n");
    computeGenSpheres();
    printf("compute vertex\n");    
    computeVertexes();
    printf("compute DividePlanes\n");
    computeDividePlanes();
    printf("compute ExcavationSpheres\n");
    computeExcavationSpheres();
    printf("compute Seed Spheres\n");
    computeSeedSpheres();
    printf("compute convex spheres\n");
    computeConvexSphere();
    printf("compute bounding volume\n");
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
    printf("compute sphere\n");
    computeSpheres();
    computeInversionSphere();
    printf("compute genSpheres\n");
    computeGenSpheres();
    printf("compute vertex\n");    
    computeVertexes();
    printf("compute DividePlanes\n");
    computeDividePlanes();
    printf("compute ExcavationSpheres\n");
    computeExcavationSpheres();
    printf("compute Seed Spheres\n");
    computeSeedSpheres();
    printf("compute convex spheres\n");
    computeConvexSphere();
    printf("compute bounding volume\n");
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
    printf("compute sphere\n");
    computeSpheres();
    computeInversionSphere();
    printf("compute genSpheres\n");
    computeGenSpheres();
    printf("compute vertex\n");    
    computeVertexes();
    printf("compute DividePlanes\n");
    computeDividePlanes();
    printf("compute ExcavationSpheres\n");
    computeExcavationSpheres();
    printf("compute Seed Spheres\n");
    computeSeedSpheres();
    printf("compute convex spheres\n");
    computeConvexSphere();
    printf("compute bounding volume\n");
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
