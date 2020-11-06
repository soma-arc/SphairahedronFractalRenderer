#include <vector>
#include "sphere.h"
#include "plane.h"
#include "vec3.h"

using namespace std;

Plane::Plane(Vec3f _p1, Vec3f _p2, Vec3f _p3, Vec3f _normal) {
    p1 = _p1;
    p2 = _p2;
    p3 = _p3;
    normal = _normal;
}

Plane Plane::invertOnPlane(Plane plane) {
    float dp = vdot(normal, plane.normal);
    Vec3f nNormal = vnormalize(plane.normal - (normal * 2 * dp));
    return Plane(invertOnPoint(plane.p1),
                 invertOnPoint(plane.p2),
                 invertOnPoint(plane.p3),
                 nNormal);
}

Vec3f Plane::invertOnPoint(Vec3f p){
    Vec3f pos = p - p1;
    float dp = vdot(normal, pos);
    pos = pos - (normal * 2 * dp);;
    return pos + p1;
}

Sphere Plane::invertOnSphere(Sphere sphere) {
    Vec3f nc = invertOnPoint(sphere.center);
    return Sphere(nc.x(), nc.y(), nc.z(), sphere.r);
}

Vec3f Plane::ComputeIntersection(Plane l1, Plane l2) {
    Vec3f l1Eq = Plane::ComputeLineEquation(Vec3f(l1.p1.x(),
                                                  l1.p1.z(), 0),
                                            Vec3f(l1.p2.x(),
                                                  l1.p2.z(), 0));
    Vec3f l2Eq = Plane::ComputeLineEquation(Vec3f(l2.p1.x(), l2.p1.z(), 0),
                                            Vec3f(l2.p3.x(), l2.p3.z(), 0));
    if (l1Eq.x() == 1 && l1Eq.y() == 0) {
        return Vec3f(l1Eq.z(), 0, Plane::CalcY(l2Eq, l1Eq.z()));
    } else if (l1Eq.x() == 0 && l1Eq.y() == 1) {
        return Vec3f(Plane::CalcX(l2Eq, l1Eq.z()), 0, l1Eq.z());
    } else if (l2Eq.x() == 1 && l2Eq.y() == 0) {
        return Vec3f(l2Eq.z(), 0, Plane::CalcY(l1Eq, l2Eq.z()));
    } else if (l2Eq.x() == 0 && l2Eq.y() == 1) {
        return Vec3f(Plane::CalcX(l1Eq, l2Eq.z()), 0, l2Eq.z());
    } else {
        float x1 = 1.0;
        float x2 = 5.0;
        float y1 = Plane::CalcY(l1Eq, x1);
        float y2 = Plane::CalcY(l1Eq, x2);

        float x3 = 4.0;
        float x4 = 8.0;
        float y3 = Plane::CalcY(l2Eq, x3);
        float y4 = Plane::CalcY(l2Eq, x4);

        // http://mf-atelier.sakura.ne.jp/mf-atelier/modules/tips/program/algorithm/a1.html
        float ksi   = ( y4-y3 )*( x4-x1 ) - ( x4-x3 )*( y4-y1 );
        float eta   = ( x2-x1 )*( y4-y1 ) - ( y2-y1 )*( x4-x1 );
        float delta = ( x2-x1 )*( y4-y3 ) - ( y2-y1 )*( x4-x3 );

        float lambda = ksi / delta;
        float mu    = eta / delta;

        return Vec3f(x1 + lambda*( x2-x1 ), 0, y1 + lambda*( y2-y1 ));
    }
}

Vec3f Plane::ComputeLineEquation(Vec3f p1, Vec3f p2) {
    float xDiff = p2.x() - p1.x();
    float yDiff = p2.y() - p1.y();
    if (abs(xDiff) < 0.000001) {
        // x = c
        return Vec3f(1, 0, p1.x());
    } else if (abs(yDiff) < 0.000001) {
        // y = c
        return Vec3f(0, 1, p1.y());
    } else {
        // y = ax + b
        return Vec3f(yDiff / xDiff,
                     p1.y() - p1.x() * (yDiff / xDiff),
                     0);
    }
}

float Plane::CalcX(Vec3f line, float y) {
    if (line.z() == 0) {
        return (y - line.y()) / line.x();
    } else {
        return line.z();
    }
}

float Plane::CalcY(Vec3f line, float x) {
    if (line.z() == 0) {
        return line.x() * x + line.y();
    } else {
        return line.z();
    }
}
