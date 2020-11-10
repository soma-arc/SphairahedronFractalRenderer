#pragma once

#include <vector>
#include "GL/glew.h"
#include "vec3.h"
#include "plane.h"
#include "sphere.h"
#include <jinja2cpp/template.h>

using namespace std;

class Sphairahedron {
public:
    Sphairahedron();
    Sphairahedron(float _zb, float _zc);

    void update();
    void computeSpheres();
    void computeInversionSphere();
    void computeGenSpheres();
    void computeVertexes();
    void computeDividePlanes();
    void computeExcavationSpheres();
    void computeSeedSpheres();
    void computeConvexSphere();
    void computeBoundingVolume();
    Sphere computeMinSeedSphere(Vec3f x, vector<Vec3f> vertexes,
                                Sphere a, Sphere b, Sphere c);
    Sphere computeSeedSphere(Vec3f x, Vec3f y,
                             Sphere a, Sphere b, Sphere c);

    Plane computePlane(int vertexIdx1, int vertexIdx2, int vertexIdx3);
    
    void getUniformLocations(GLuint programID,
                             vector<GLuint> &uniLocations);
    int setUniformValues(int uniI, vector<GLuint>uniLocations);
    Vec3f computeIdealVertex(Sphere a, Sphere b, Sphere c);
    void addSphereIfNotExists(vector<Sphere>& spheres, Sphere sphere);
    jinja2::ValuesMap getShaderTemplateContext();
    
    float zb, zc;
    int numFaces, numSpheres, numPlanes, numVertexes;
    int numExcavationSpheres = 0;
    int numDividePlanes = 1;
    vector<vector<int>> vertexIndexes;
    vector<Plane> dividePlanes;
    vector<Vec3f> vertexes;
    vector<Sphere> gSpheres;
    vector<Sphere> seedSpheres;
    vector<Sphere> convexSpheres;
    Sphere inversionSphere;
    Sphere boundingSphere;
    float boundingPlaneY;
    vector<Sphere>prismSpheres;
    vector<Plane> planes;
    vector<Sphere> excavationPrismSpheres;
    vector<Sphere> excavationSpheres;
    
    static vector<Plane> PRISM_PLANES_333;
    static vector<Plane> PRISM_PLANES_236;
    static vector<Plane> PRISM_PLANES_244;
    static vector<Plane> PRISM_PLANES_2222_SQUARE;
};
