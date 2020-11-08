#pragma once

#include <GL/glew.h>
#include <vector>
#include <map>

#include "vec3.h"

using namespace std;

class Plane;

class Sphere {
public:
    Sphere();
    Sphere(float x, float y, float z, float _r);
    void update();
    int setUniformValues(int index,
                         vector<GLuint> uniLocations);
    Vec3f invertOnPoint(Vec3f p);
    static void Pivoting(vector< vector<double> > &A, int n, int k);
    static Sphere FromPoints(Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4);
    Sphere invertOnSphere(Sphere invertSphere);
    Sphere invertOnPlane(Plane plane);
    //std::map<std::string, int> toJson();
    
    Vec3f center;
    float r, rSq;
};
