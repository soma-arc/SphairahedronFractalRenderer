#pragma once
#include "../sphairahedron.h"
#include <jinja2cpp/template.h>

class Cube : public Sphairahedron {
public:
    Cube(float _zb, float _zc);
    void computeGenSpheres();
    void computeInversionSphere();
    
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
