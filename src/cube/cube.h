#pragma once
#include "../sphairahedron.h"
#include <jinja2cpp/template.h>

class Cube : public Sphairahedron {
public:
    Cube(float _zb, float _zc);
    void computeGenSpheres();
    void computeInversionSphere();
    
    virtual jinja2::ValuesMap getShaderTemplateContext();
    virtual void computeVertexes();
    virtual void computeSeedSpheres();
    virtual void computeDividePlanes();
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

class CubeD : public Cube {
public:
    CubeD(float _zb, float _zc);
    void computeSpheres();
    void update();
};

class CubeE : public Cube {
public:
    CubeE(float _zb, float _zc);
    void computeSpheres();
    void update();
};

class CubeH : public Cube {
public:
    CubeH(float _zb, float _zc);
    void computeSpheres();
    void update();
};

class CubeI : public Cube {
public:
    CubeI(float _zb, float _zc);
    void computeSpheres();
    void update();
};
