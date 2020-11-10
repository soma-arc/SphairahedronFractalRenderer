#include "cube.h"
#include "../sphairahedron.h"
#include "../sphere.h"

Cube::Cube(float _zb, float _zc) : Sphairahedron(_zb, _zc) {
    printf("cube cpp\n");
    numFaces = 6;
    numSpheres = 3;
    numPlanes = 3;
    vertexIndexes = {{0, 1, 2}, {0, 3, 4}, {2, 4, 5}, {0, 1, 3},
                     {3, 4, 5}, {1, 2, 5}, {1, 3, 5}, {0, 2, 4}};
    numVertexes = vertexIndexes.size();
    
}

void Cube::computeGenSpheres() {
    printf("plane size %d\n", planes.size());
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
