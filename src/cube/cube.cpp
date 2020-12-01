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
