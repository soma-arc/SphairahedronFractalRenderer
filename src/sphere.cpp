#include <vector>
#include "sphere.h"
#include "plane.h"
#include "vec3.h"

using namespace std;

Sphere::Sphere(float x, float y, float z, float _r) {
    center = Vec3f(x, y, z);
    r = _r;
    rSq = r * r;
}

int Sphere::setUniformValues(int index,
                             vector<GLuint> uniLocations) {
    glUniform3f(uniLocations[index++],
                center.x(), center.y(), center.z());
    glUniform2f(uniLocations[index++],
                r, rSq);
    return index;
}

Vec3f Sphere::invertOnPoint(Vec3f p) {
    Vec3f d = p - center;
    float len = vlength(d);
    return d * (rSq / (len * len)) + center;
}

void Sphere::Pivoting(vector< vector<double> > &A, int n, int k) {
    int p = k;
    double am = fabs(A[k][k]);
    for(int i = k+1; i < n; ++i){
        if(fabs(A[i][k]) > am){
            p = i;
            am = fabs(A[i][k]);
        }
    }

    if(k != p) swap(A[k], A[p]);
}

Sphere Sphere::FromPoints(Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4) {
    vector<Vec3f> p = {p1, p2, p3, p4};
    int n = 3;
    vector<vector<double>> coefficient;
    for (int i = 0; i < 3; i++) {
        coefficient[i][0] = 2 * (p[i + 1].x() - p[i].x());
        coefficient[i][1] = 2 * (p[i + 1].y() - p[i].y());
        coefficient[i][2] = 2 * (p[i + 1].z() - p[i].z());
        coefficient[i][3] = -((p[i].x() * p[i].x()) +
                              (p[i].y() * p[i].y()) +
                              (p[i].z() * p[i].z())) +
            (p[i + 1].x() * p[i + 1].x()) +
            (p[i + 1].y() * p[i + 1].y()) +
            (p[i + 1].z() * p[i + 1].z());
    }

    for(int k = 0; k < n-1; ++k){
        Sphere::Pivoting(coefficient, n, k);
 
        double akk = coefficient[k][k];
        for(int i = k+1; i < n; ++i){
            double aik = coefficient[i][k];
            for(int j = k; j < n+1; ++j){
                coefficient[i][j] = coefficient[i][j]-aik*(coefficient[k][j]/akk);
            }
        }
    }
 
    // (back substitution)
    coefficient[n-1][n] = coefficient[n-1][n]/coefficient[n-1][n-1];
    for(int i = n-2; i >= 0; --i){
        double ax = 0.0;
        for(int j = i+1; j < n; ++j){
            ax += coefficient[i][j]*coefficient[j][n];
        }
        coefficient[i][n] = (coefficient[i][n]-ax)/coefficient[i][i];
    }

    Vec3f center(coefficient[0][3], coefficient[1][3], coefficient[2][3]);
    float r = vlength(center - (p1));
    return Sphere(coefficient[0][3], coefficient[1][3], coefficient[2][3], r);
}

Sphere Sphere::invertOnSphere(Sphere invertSphere) {
    float r = invertSphere.r;
    float RT_3 = sqrt(3);
    float coeffR = r * RT_3 / 3;
    Vec3f p1 = invertOnPoint(invertSphere.center +  Vec3f(coeffR, coeffR, coeffR));
    Vec3f p2 = invertOnPoint(invertSphere.center +  Vec3f(-coeffR, -coeffR, -coeffR));
    Vec3f p3 = invertOnPoint(invertSphere.center +  Vec3f(coeffR, -coeffR, -coeffR));
    Vec3f p4 = invertOnPoint(invertSphere.center +  Vec3f(coeffR, coeffR, -coeffR));
    return Sphere::fromPoints(p1, p2, p3, p4);
}

Sphere Sphere::invertOnPlane(Plane plane) {
    return Sphere::fromPoints(invertOnPoint(plane.p1),
                              invertOnPoint(plane.p2),
                              invertOnPoint(plane.p3),
                              center);
}
