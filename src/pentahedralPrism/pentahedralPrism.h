#pragma once
#include "../sphairahedron.h"
#include <jinja2cpp/template.h>

class PentahedralPrism : public Sphairahedron {
public:
    PentahedralPrism(float _zb);
    void computeGenSpheres();
    void computeInversionSphere();
    
    virtual jinja2::ValuesMap getShaderTemplateContext();
    virtual void computeVertexes();
    virtual void computeSeedSpheres();
    virtual void computeDividePlanes();
};

class PentahedralPrismA : public PentahedralPrism {
public:
    PentahedralPrismA(float _zb);
    void computeSpheres();
    void update();
};

class PentahedralPrismB : public PentahedralPrism {
public:
    PentahedralPrismB(float _zb);
    void computeSpheres();
    void update();
};

class PentahedralPrismC : public PentahedralPrism {
public:
    PentahedralPrismC(float _zb);
    void computeSpheres();
    void update();
};

class PentahedralPrismD : public PentahedralPrism {
public:
    PentahedralPrismD(float _zb);
    void computeSpheres();
    void update();
};

class PentahedralPrismE : public PentahedralPrism {
public:
    PentahedralPrismE(float _zb);
    void computeSpheres();
    void update();
};

class PentahedralPrismF : public PentahedralPrism {
public:
    PentahedralPrismF(float _zb);
    void computeSpheres();
    void update();
};
