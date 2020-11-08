#pragma once
#include "cube.h"

class CubeA : public Cube {
public:
    CubeA(float _zb, float _zc);
    void computeSpheres();
};
