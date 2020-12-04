#pragma once
#include "../sphairahedron.h"
#include <jinja2cpp/template.h>

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
    float SQRT_3 = sqrt(3);
    virtual jinja2::ValuesMap getShaderTemplateContext();
    virtual void computeVertexes();
    virtual void computeSeedSpheres();
};

class CubeA : public Cube {
public:
    CubeA(float _zb, float _zc);
    void computeSpheres();
    void update();
};

class CubeB : public Cube {
public:
    CubeB(float _zb, float _zc);
    void computeSpheres();
    void update();
};

class CubeC : public Cube {
public:
    CubeC(float _zb, float _zc);
    void computeSpheres();
    void update();
};
