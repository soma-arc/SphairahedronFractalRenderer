#pragma once
#include <map>
#include "vec3.h"

class Sphere;

class Plane {
public:
    Plane(Vec3f _p1, Vec3f _p2, Vec3f _p3, Vec3f _normal);
    Plane invertOnPlane(Plane plane);
    Sphere invertOnSphere(Sphere sphere);
    Vec3f invertOnPoint(Vec3f p);
    static Vec3f ComputeIntersection(Plane l1, Plane l2);
    static Vec3f ComputeLineEquation(Vec3f p1, Vec3f p2);
    static float CalcX(Vec3f line, float y);
    static float CalcY(Vec3f line, float x);
    //std::map<std::string, int> toJson();

    Vec3f p1, p2, p3, normal;
};
