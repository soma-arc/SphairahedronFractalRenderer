#pragma once
#include "cube.h"
#include <jinja2cpp/template.h>

class CubeA : public Cube {
public:
    CubeA(float _zb, float _zc);
    void computeSpheres();
    void update();
    void computeVertexes();
    void computeSeedSpheres();
};
