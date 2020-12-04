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

    void getUniformLocations(GLuint programID) {
        posID = glGetUniformLocation(programID,
                                     "u_camera.pos");
        targetID = glGetUniformLocation(programID,
                                        "u_camera.target");
        fovID = glGetUniformLocation(programID,
                                     "u_camera.fov");
        upID = glGetUniformLocation(programID,
                                    "u_camera.up");
    }

    void setUniformValues() {
        glUniform3f(posID,
                    pos.x(), pos.y(), pos.z());
        glUniform3f(targetID,
                    target.x(), target.y(), target.z());
        glUniform1f(fovID, fov);
        glUniform3f(upID,
                    up.x(), up.y(), up.z());
    }

    GLuint posID, targetID, fovID, upID;
    Vec3f pos, target, up;
    float fov;
};
