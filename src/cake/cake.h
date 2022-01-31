#pragma once
#include "../sphairahedron.h"
#include <jinja2cpp/template.h>

class Cake : public Sphairahedron {
public:
    Cake(float _zb);
    void computeGenSpheres();
    void computeInversionSphere();
    
    virtual jinja2::ValuesMap getShaderTemplateContext();
    virtual void computeVertexes();
    virtual void computeSeedSpheres();
};

class CakeA : public Cake {
public:
    CakeA(float _zb);
    void computeSpheres();
    void update();
};

class CakeB : public Cake {
public:
    CakeB(float _zb);
    void computeSpheres();
    void update();
};

class CakeC : public Cake {
public:
    CakeC(float _zb);
    void computeSpheres();
    void update();
};
