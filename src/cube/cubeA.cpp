#include "cubeA.h"
#include "cube.h"
#include "../sphere.h"
#include "../sphairahedron.h"
#include <jinja2cpp/template.h>

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

void CubeA::computeSeedSpheres() {
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

void CubeA::computeVertexes() {
    printf("vertexes compute %zu\n", vertexIndexes.size());
    vertexes.clear();
    for(const auto &vert : vertexIndexes) {
        vertexes.push_back(computeIdealVertex(gSpheres[vert[0]],
                                              gSpheres[vert[1]],
                                              gSpheres[vert[2]]));
    }
    printf("Done %zu\n", vertexes.size());
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
