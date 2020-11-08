#include "cube.h"
#include "../sphairahedron.h"
#include "../sphere.h"

Cube::Cube(float _zb, float _zc) : Sphairahedron(_zb, _zc) {
    numFaces = 6;
    numSpheres = 3;
    numPlanes = 3;
    vertexIndexes = {{0, 1, 2}, {0, 3, 4}, {2, 4, 5}, {0, 1, 3},
                     {3, 4, 5}, {1, 2, 5}, {1, 3, 5}, {0, 2, 4}};
    numVertexes = vertexIndexes.size();
}

void Cube::computeGenSpheres() {
    gSpheres.clear();
    gSpheres[0] = inversionSphere.invertOnPlane(planes[0]); // O1
    gSpheres[1] = inversionSphere.invertOnSphere(prismSpheres[0]); // O2
    gSpheres[2] = inversionSphere.invertOnPlane(planes[1]); // O3
    gSpheres[3] = inversionSphere.invertOnSphere(prismSpheres[1]); // O4
    gSpheres[4] = inversionSphere.invertOnPlane(planes[2]); // O5
    gSpheres[5] = inversionSphere.invertOnSphere(prismSpheres[2]); // O6
    
}

void Cube::computeInversionSphere() {
    inversionSphere = Sphere(
        -prismSpheres[2].center.x(),
        -prismSpheres[2].center.y(),
         prismSpheres[2].center.z(),
         prismSpheres[2].r);
}
