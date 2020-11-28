#pragma once
#include "vec3.h"
#include <vector>
#include <GL/glew.h>
#include <math.h>

class Camera {
public:
    Camera(Vec3f _pos, Vec3f _target, float _fov, Vec3f _up){
        pos = _pos;
        target = _target;
        fov = _fov * M_PI / 180.;
        up = _up;
    }

    void getUniformLocations(GLuint programID,
                             std::vector<GLuint>& uniLocations) {
        uniLocations.push_back(glGetUniformLocation(programID,
                                                    "u_camera.pos"));
        uniLocations.push_back(glGetUniformLocation(programID,
                                                    "u_camera.target"));
        uniLocations.push_back(glGetUniformLocation(programID,
                                                    "u_camera.fov"));
        uniLocations.push_back(glGetUniformLocation(programID,
                                                    "u_camera.up"));
    }

    int setUniformValues(int index,
                         std::vector<GLuint> uniLocations) {
        glUniform3f(uniLocations[index++],
                    pos.x(), pos.y(), pos.z());
        glUniform3f(uniLocations[index++],
                    target.x(), target.y(), target.z());
        glUniform1f(uniLocations[index++], fov);
        glUniform3f(uniLocations[index++],
                    up.x(), up.y(), up.z());
        return index;
    }

    Vec3f pos, target, up;
    float fov;
};
