#include <vector>
#include "sphere.h"
#include "plane.h"
#include "sphairahedron.h"
#include <jinja2cpp/template.h>

const float RT_3 = sqrt(3);
const float RT_3_INV = 1.0 / sqrt(3);

Sphairahedron::Sphairahedron(float _zb, float _zc) {
    zb = _zb;
    zc = _zc;
}

Vec3f Sphairahedron::computeIdealVertex(Sphere a, Sphere b, Sphere c) {
    float AB = (vlengthSq(a.center) - vlengthSq(b.center) - a.rSq + b.rSq) * 0.5 -
        vlengthSq(a.center) + vdot(a.center, b.center);
    float AC = (vlengthSq(a.center) - vlengthSq(c.center) - a.rSq + c.rSq) * 0.5 -
        vlengthSq(a.center) + vdot(a.center, c.center);
    float x = -vlengthSq(a.center) - vlengthSq(b.center) + 2 * vdot(a.center, b.center);
    float y = -vlengthSq(a.center) - vlengthSq(c.center) + 2 * vdot(a.center, c.center);
    float z = -vlengthSq(a.center) + vdot(a.center, b.center) +
        vdot(a.center, c.center) - vdot(b.center, c.center);
    float s = (AB * y - AC * z) / (x * y - z * z);
    float t = (AC * x - AB * z) / (x * y - z * z);
    return a.center + ((b.center - a.center) * s) + ((c.center - a.center) * t);
}

// void Sphairahedron::update() {
//     printf("compute sphere");
//     computeSpheres();
//     printf("compute genSphere");
//     computeGenSpheres();
//     printf("compute vertex");    
//     computeVertexes();
//     printf("compute DividePlanes");
//     computeDividePlanes();
//     // computeExcavationSpheres();
//     // computeSeedSpheres();
//     // computeConvexSphere();
//     // computeBoundingVolume();
// }

void Sphairahedron::computeSpheres() {} // computed by cube.cpp

void Sphairahedron::computeGenSpheres() {} // computed by cube.cpp

// void Sphairahedron::computeVertexes() {
//     printf("vertexes compute %d\n", vertexIndexes.size());
//     vertexes.clear();
//     for(const auto &vert : vertexIndexes) {
//         vertexes.push_back(computeIdealVertex(gSpheres[vert[0]],
//                                               gSpheres[vert[1]],
//                                               gSpheres[vert[2]]));
//     }
//     printf("Done %d\n", vertexes.size());
// }

void Sphairahedron::addSphereIfNotExists(vector<Sphere>& spheres, Sphere sphere) {
    for (Sphere s : spheres) {
        if (abs(s.r - sphere.r) < 0.00001 &&
            vdistance(s.center, sphere.center) < 0.00001) {
            //printf("duplicate\n");
        }
    }
    spheres.push_back(sphere);
}

Sphere Sphairahedron::computeSeedSphere(Vec3f x, Vec3f y,
                                        Sphere a, Sphere b, Sphere c) {
    Vec3f ab = b.center - a.center;
    Vec3f ac = c.center - a.center;
    Vec3f n = vcross(ab, ac);
    float k = vlengthSq(y - x) / (2 * vdot(y - x, n));
    Vec3f center = x + (n * k);
    return Sphere(center.x(), center.y(), center.z(),
                  abs(k) * vlength(n));
}

Sphere Sphairahedron::computeMinSeedSphere(Vec3f x, vector<Vec3f> vertexes,
                                           Sphere a, Sphere b, Sphere c) {
    Sphere minSphere(0, 0, 0, 99999999999);
    for (Vec3f ov : vertexes) {
        if (vdistance(x, ov) < 0.000001) {
            // x === ov
            continue;
        }
        Sphere s = computeSeedSphere(x, ov, a, b, c);
        if (s.r < minSphere.r) {
            minSphere = s;
        }
    }
    return minSphere;
    
}

// void Sphairahedron::computeSeedSpheres() {
//     seedSpheres.clear();
//     printf("num seed sphere %d", numVertexes);
//     for(int i = 0; i < numVertexes; i++) {
//         printf("seed sphere %d", i);
//         addSphereIfNotExists(seedSpheres,
//                              computeMinSeedSphere(vertexes[i], vertexes,
//                                                   gSpheres[vertexIndexes[i][0]],
//                                                   gSpheres[vertexIndexes[i][1]],
//                                                   gSpheres[vertexIndexes[i][2]]));
//     }
// }

void Sphairahedron::computeConvexSphere() {
    convexSpheres.clear();
    for (int i = 0; i < numDividePlanes; i++) {
        convexSpheres.push_back(inversionSphere.invertOnPlane(dividePlanes[i]));
    }
}

void Sphairahedron::computeBoundingVolume() {
    //printf("in compute bounding volume\n");
    boundingPlaneY = -99999999999;
    float boundingPlaneMinY = 99999999999;
    for (Sphere s : prismSpheres) {
        boundingPlaneY = max(boundingPlaneY, s.center.y());
        boundingPlaneMinY = min(boundingPlaneMinY, s.center.y());
    }
    if (inversionSphere.center.y() < boundingPlaneMinY) {
        boundingSphere = inversionSphere.invertOnPlane(
            Plane(Vec3f(1, boundingPlaneMinY, -9),
                  Vec3f(-4, boundingPlaneMinY, -4),
                  Vec3f(10, boundingPlaneMinY, 3),
                  Vec3f(0, 1, 0)));
    } else {
        boundingSphere = inversionSphere.invertOnPlane(
            Plane(Vec3f(1,  boundingPlaneY, -9),
                  Vec3f(-4, boundingPlaneY, -4),
                  Vec3f(10, boundingPlaneY, 3),
                  Vec3f(0, 1, 0)));
    }
    boundingPlaneY += 1.01;
    boundingSphere.r *= 1.01;
    boundingSphere.update();
}

void Sphairahedron::computeInversionSphere() {} // computed by cube.cpp

Plane Sphairahedron::computePlane(int vertexIdx1, int vertexIdx2, int vertexIdx3) {
    Vec3f p1 = inversionSphere.invertOnPoint(vertexes[vertexIdx1]);
    Vec3f p2 = inversionSphere.invertOnPoint(vertexes[vertexIdx2]);
    Vec3f p3 = inversionSphere.invertOnPoint(vertexes[vertexIdx3]);
    Vec3f v1 = p2 - p1;
    Vec3f v2 = p3 - p1;
    Vec3f normal = vnormalize(vcross(v1, v2));
    if (normal.y() < 0.0) {
        normal = normal * -1;
    }
    return Plane(p1, p2, p3, normal);
}

void Sphairahedron::computeDividePlanes() {
    dividePlanes.clear();
    Plane p = computePlane(0, 1, 2);
    dividePlanes.push_back(p);
}

void Sphairahedron::computeExcavationSpheres() {
    excavationPrismSpheres.clear();
    excavationSpheres.clear();
}

jinja2::ValuesMap Sphairahedron::getShaderTemplateContext() {
    jinja2::ValuesMap data;
    return data;
}

vector<Plane> Sphairahedron::PRISM_PLANES_333 = {
    // AB - CA - BC
    Plane(Vec3f(0, 5, RT_3_INV),
          Vec3f(1, 1, 0),
          Vec3f(2, 2, -RT_3_INV),
          vnormalize(Vec3f(RT_3 * 0.5, 0, 1.5))),
    Plane(Vec3f(0, 3, -RT_3_INV),
          Vec3f(1, 3, 0),
          Vec3f(2, 2, RT_3_INV),
          vnormalize(Vec3f(RT_3 * 0.5, 0, -1.5))),
    Plane(Vec3f(-0.5, 0, 1),
          Vec3f(-0.5, 1, 0),
          Vec3f(-0.5, 2, 1),
          Vec3f(-1, 0, 0))
};

vector<Plane> Sphairahedron::PRISM_PLANES_236 = {
    // AB - CA - BC
    Plane(Vec3f(0.5, 5, RT_3 * 0.5),
          Vec3f(1, 1, 0),
          Vec3f(0.75, 2, RT_3 * 0.25),
          vnormalize(Vec3f(1, 0, RT_3_INV))),
    Plane(Vec3f(1, 0, 0),
          Vec3f(0, 5, -RT_3 / 3),
          Vec3f(-0.5, 2, -RT_3 * 0.5),
          vnormalize(Vec3f(1, 0, -RT_3))),
    Plane(Vec3f(0.5, 3, RT_3 * 0.5),
          Vec3f(0, -10, 0),
          Vec3f(-0.5, -3, -RT_3 * 0.5),
          vnormalize(Vec3f(-1, 0, RT_3_INV)))
};

vector<Plane> Sphairahedron::PRISM_PLANES_244 = {
    Plane(Vec3f(0, 5, 1),
          Vec3f(0.5, 1, 0.5),
          Vec3f(1, 2, 0),
          vnormalize(Vec3f(0.5, 0, 0.5))),
    Plane(Vec3f(0, 3, -1),
          Vec3f(0.5, 3, -0.5),
          Vec3f(1, 2, 0),
          vnormalize(Vec3f(0.5, 0, -0.5))),
    Plane(Vec3f(0, -7, 1),
          Vec3f(0, -4, 0),
          Vec3f(0, 8, -1),
          Vec3f(-1, 0, 0))
};

vector<Plane> Sphairahedron::PRISM_PLANES_2222_SQUARE = {
    Plane(Vec3f(0, 5, 1),
          Vec3f(0.5, 1, 0.5),
          Vec3f(1, 2, 0),
          vnormalize(Vec3f(0.5, 0, 0.5))),
    Plane(Vec3f(0, 5, 1),
          Vec3f(-0.5, 1, 0.5),
          Vec3f(-1, 2, 0),
          vnormalize(Vec3f(-0.5, 0, 0.5))),
    Plane(Vec3f(0, 5, -1),
          Vec3f(-0.5, 1, -0.5),
          Vec3f(-1, 2, 0),
          vnormalize(Vec3f(-0.5, 0, -0.5))),
    Plane(Vec3f(0, 3, -1),
          Vec3f(0.5, 3, -0.5),
          Vec3f(1, 2, 0),
          vnormalize(Vec3f(0.5, 0, -0.5)))
};
