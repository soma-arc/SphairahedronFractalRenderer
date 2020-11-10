#pragma once
#include "cube.h"
#include <jinja2cpp/template.h>

class CubeA : public Cube {
public:
    CubeA(float _zb, float _zc);
    void computeSpheres();
    jinja2::ValuesMap getShaderTemplateContext();
    void getUniformLocations(GLuint programID,
                             vector<GLuint> &uniLocations);
    int setUniformValues(int uniI, vector<GLuint>uniLocations);
    void update();
    void computeVertexes();
    void computeSeedSpheres();
};
