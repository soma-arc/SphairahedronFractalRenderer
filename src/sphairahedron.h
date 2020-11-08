#pragma once

#include <vector>
#include "GL/glew.h"
#include "vec3.h"
#include "plane.h"
#include "sphere.h"
#include <map>

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
    void setUniformValues(vector<GLuint>uniLocations, int index);
    Vec3f computeIdealVertex(Sphere a, Sphere b, Sphere c);
    map<string, int> getShaderTemplateContext();
    void addSphereIfNotExists(vector<Sphere>& spheres, Sphere sphere);
    
    float zb, zc;
    int numFaces, numSpheres, numPlanes, numVertexes;
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
