#pragma once
#include "../sphairahedron.h"

class Cube : public Sphairahedron {
public:
    Cube(float _zb, float _zc);
    void computeGenSpheres();
    void computeInversionSphere();
    
    int numFaces = 6;
    int numSpheres = 3;
    int numPlanes = 3;
    vector<vector<int>>vertexIndexes = {{0, 1, 2}, {0, 3, 4}, {2, 4, 5}, {0, 1, 3},
                                        {3, 4, 5}, {1, 2, 5}, {1, 3, 5}, {0, 2, 4}};
    int numVertexes = vertexIndexes.size();
};
