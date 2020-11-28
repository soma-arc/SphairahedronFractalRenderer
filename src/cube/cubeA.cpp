#include "cubeA.h"
#include "cube.h"
#include "../sphere.h"
#include "../sphairahedron.h"
#include <jinja2cpp/template.h>

CubeA::CubeA(float _zb, float _zc) : Cube(_zb, _zc) {
    printf("cubeA d\n");
    planes = Sphairahedron::PRISM_PLANES_333;
    printf("pre update\n");
    update();
}

void CubeA::update() {
    printf("compute sphere\n");
    computeSpheres();
    computeInversionSphere();
    printf("compute genSpheres\n");
    computeGenSpheres();
    printf("compute vertex\n");    
    computeVertexes();
    printf("compute DividePlanes\n");
    computeDividePlanes();
    printf("compute ExcavationSpheres\n");
    computeExcavationSpheres();
    printf("compute Seed Spheres\n");
    computeSeedSpheres();
    printf("compute convex spheres\n");
    computeConvexSphere();
    printf("compute bounding volume\n");
    computeBoundingVolume();
}

void CubeA::computeSeedSpheres() {
    seedSpheres.clear();
    printf("num seed sphere %d\n", numVertexes);
    for(int i = 0; i < numVertexes; i++) {
        addSphereIfNotExists(seedSpheres,
                             computeMinSeedSphere(vertexes[i], vertexes,
                                                  gSpheres[vertexIndexes[i][0]],
                                                  gSpheres[vertexIndexes[i][1]],
                                                  gSpheres[vertexIndexes[i][2]]));
    }
}

void CubeA::computeVertexes() {
    printf("vertexes compute %d\n", vertexIndexes.size());
    vertexes.clear();
    for(const auto &vert : vertexIndexes) {
        vertexes.push_back(computeIdealVertex(gSpheres[vert[0]],
                                              gSpheres[vert[1]],
                                              gSpheres[vert[2]]));
    }
    printf("Done %d\n", vertexes.size());
}

void CubeA::computeSpheres() {
    float r2 = 0.5 + (zb * zc) / 3.0;
    float r4 = 0.5 + (zb * zb - zb * zc) / 3.0;
    float r6 = 0.5 + (-zb * zc + zc * zc) / 3.0;
    Sphere s2(1 - r2, 0, 0, r2);
    Sphere s4(-(1 - r4) * 0.5, zb, sqrt(3) * (1 - r4) * 0.5, r4);
    Sphere s6(-(1 - r6) * 0.5, zc, -sqrt(3) * (1 - r6) * 0.5, r6);
    prismSpheres = {s2, s4, s6};
}

jinja2::ValuesMap CubeA::getShaderTemplateContext() {
    jinja2::ValuesMap data;
    data["numPrismSpheres"] = numSpheres;
    data["numPrismPlanes"] = numPlanes;
    data["numSphairahedronSpheres"] = numFaces;
    data["numSeedSpheres"] = numVertexes;
    data["numDividePlanes"] = numDividePlanes;
    data["numExcavationSpheres"] = numExcavationSpheres;
    return data;
}

void CubeA::getUniformLocations(GLuint programID,
                                vector<GLuint> &uniLocations) {
    printf("getUniformLocations\n");
    uniLocations.push_back(glGetUniformLocation(programID, "u_zbzc"));

    uniLocations.push_back(glGetUniformLocation(programID, "u_inversionSphere.center"));
    uniLocations.push_back(glGetUniformLocation(programID, "u_inversionSphere.r"));
    printf("dividePlanes\n");
    for (int i = 0; i < numDividePlanes; i++) {
        std::string s = "u_dividePlanes[";
        s += i;
        std::string s1 = s + "].origin";
        std::string s2 = s + "].normal";
        uniLocations.push_back(glGetUniformLocation(programID, s1.c_str()));
        uniLocations.push_back(glGetUniformLocation(programID, s2.c_str()));
        s = "u_convexSpheres[";
        s += i;
        s1 = s + "].center";
        s2 = s + "].r";
        uniLocations.push_back(glGetUniformLocation(programID, s1.c_str()));
        uniLocations.push_back(glGetUniformLocation(programID, s2.c_str()));
    }
    printf("excavationSpheres\n");
    for (int i = 0; i < numExcavationSpheres; i++) {
        std::string s = "u_excavationPrismSpheres[";
        s += i;
        std::string s1 = s + "].center";
        std::string s2 = s + "].r";
        uniLocations.push_back(glGetUniformLocation(programID, s1.c_str()));
        uniLocations.push_back(glGetUniformLocation(programID, s2.c_str()));
        s = "u_excavationSpheres[";
        s += i;
        s1 = s + "].center";
        s2 = s + "].r";
        uniLocations.push_back(glGetUniformLocation(programID, s1.c_str()));
        uniLocations.push_back(glGetUniformLocation(programID, s2.c_str()));
    }
    printf("num\n");
    uniLocations.push_back(glGetUniformLocation(programID, "u_numPrismSpheres"));
    uniLocations.push_back(glGetUniformLocation(programID, "u_numPrismPlanes"));
    uniLocations.push_back(glGetUniformLocation(programID, "u_numSeedSpheres"));
    uniLocations.push_back(glGetUniformLocation(programID, "u_numGenSpheres"));

    uniLocations.push_back(glGetUniformLocation(programID, "u_boundingPlaneY"));
    uniLocations.push_back(glGetUniformLocation(programID, "u_boundingSphere.center"));
    uniLocations.push_back(glGetUniformLocation(programID, "u_boundingSphere.r"));
    printf("prismSpheres\n");
    for (int i = 0; i < numSpheres; i++) {
        std::string s = "u_prismSpheres[";
        s += i;
        std::string s1 = s + "].center";
        std::string s2 = s + "].r";
        uniLocations.push_back(glGetUniformLocation(programID, s1.c_str()));
        uniLocations.push_back(glGetUniformLocation(programID, s2.c_str()));
    }
    printf("prismPlanes\n");
    printf("%d\n", numPlanes);
    for (int i = 0; i < numPlanes; i++) {
        std::string s = "u_prismPlanes[";
        s += i;
        std::string s1 = s + "].origin";
        std::string s2 = s + "].normal";
        uniLocations.push_back(glGetUniformLocation(programID, s1.c_str()));
        uniLocations.push_back(glGetUniformLocation(programID, s2.c_str()));
    }
    printf("sphairahedronSpheres\n");
    for (int i = 0; i < numFaces; i++) {
        std::string s = "u_sphairahedronSpheres[";
        s += i;
        std::string s1 = s + "].center";
        std::string s2 = s + "].r";
        uniLocations.push_back(glGetUniformLocation(programID, s1.c_str()));
        uniLocations.push_back(glGetUniformLocation(programID, s2.c_str()));
    }
    printf("seedSpheres\n");
    for (int i = 0; i < numVertexes; i++) {
        std::string s = "u_seedSpheres[";
        s += i;
        std::string s1 = s + "].center";
        std::string s2 = s + "].r";
        uniLocations.push_back(glGetUniformLocation(programID, s1.c_str()));
        uniLocations.push_back(glGetUniformLocation(programID, s2.c_str()));
    }
    printf("Done\n");
    // for (int i = 0; i < 12; i++) {
    //     std::string s = "u_boundingPlanes[";
    //     s += i;
    //     std::string s1 = s + "].origin";
    //     std::string s2 = s + "].normal";
    //     uniLocations.push_back(glGetUniformLocation(programID, s1.c_str()));
    //     uniLocations.push_back(glGetUniformLocation(programID, s2.c_str()));
    // }
}

int CubeA::setUniformValues(int uniI, vector<GLuint>uniLocations) {
    printf("set uniform values\n");
    
    glUniform2f(uniLocations[uniI++],
                zb, zc);

    glUniform3f(uniLocations[uniI++],
                inversionSphere.center.x(),
                inversionSphere.center.y(),
                inversionSphere.center.z());
    glUniform2f(uniLocations[uniI++],
                 inversionSphere.r, inversionSphere.rSq);
    printf("dividePlanes num%d\n",  numDividePlanes);
    for (int i = 0; i < numDividePlanes; i++) {
        printf("numDividePlanes 1\n");
        glUniform3f(uniLocations[uniI++],
                    dividePlanes[i].p1.x(),
                    dividePlanes[i].p1.y(),
                    dividePlanes[i].p1.z());
        printf("numDividePlanes 2\n");
        glUniform3f(uniLocations[uniI++],
                    dividePlanes[i].normal.x(),
                    dividePlanes[i].normal.y(),
                    dividePlanes[i].normal.z());
        printf("numDividePlanes 3\n");
        glUniform3f(uniLocations[uniI++],
                    convexSpheres[i].center.x(),
                    convexSpheres[i].center.y(),
                    convexSpheres[i].center.z());
        printf("numDividePlanes 4\n");
        glUniform2f(uniLocations[uniI++],
                     convexSpheres[i].r, convexSpheres[i].rSq);
    }
    printf("excavationSpheres\n");
    for (int i = 0; i < numExcavationSpheres; i++) {
        glUniform3f(uniLocations[uniI++],
                    excavationPrismSpheres[i].center.x(),
                    excavationPrismSpheres[i].center.y(),
                    excavationPrismSpheres[i].center.z());
        glUniform2f(uniLocations[uniI++],
                     excavationPrismSpheres[i].r, excavationPrismSpheres[i].rSq);
        
        glUniform3f(uniLocations[uniI++],
                    excavationSpheres[i].center.x(),
                    excavationSpheres[i].center.y(),
                    excavationSpheres[i].center.z());
        glUniform2f(uniLocations[uniI++],
                     excavationSpheres[i].r, excavationSpheres[i].rSq);
    }
    printf("others\n");
    glUniform1i(uniLocations[uniI++], numSpheres);
    glUniform1i(uniLocations[uniI++], numPlanes);
    glUniform1i(uniLocations[uniI++], numVertexes);
    glUniform1i(uniLocations[uniI++], numFaces);
    glUniform1f(uniLocations[uniI++], boundingPlaneY);
    glUniform3f(uniLocations[uniI++],
                boundingSphere.center.x(),
                boundingSphere.center.y(), 
                boundingSphere.center.z());
    glUniform2f(uniLocations[uniI++],
                 boundingSphere.r, boundingSphere.rSq);
    printf("spheres\n");
    for (int i = 0; i < numSpheres; i++) {
        glUniform3f(uniLocations[uniI++],
                    prismSpheres[i].center.x(),
                    prismSpheres[i].center.y(),
                    prismSpheres[i].center.z());
        glUniform2f(uniLocations[uniI++],
                     prismSpheres[i].r, prismSpheres[i].rSq);
    }
    printf("planes\n");
    for (int i = 0; i < numPlanes; i++) {
        glUniform3f(uniLocations[uniI++],
                    planes[i].p1.x(),
                    planes[i].p1.y(),
                    planes[i].p1.z());
        glUniform3f(uniLocations[uniI++],
                    planes[i].normal.x(),
                    planes[i].normal.y(),
                    planes[i].normal.z());
        printf("prism planes %d (%f, %f, %f) normal(%f, %f, %f)\n",i,
               planes[i].p1.x(),
               planes[i].p1.y(),
               planes[i].p1.z(),
               planes[i].normal.x(),
               planes[i].normal.y(),
               planes[i].normal.z());
    }
    printf("faces\n");
    for (int i = 0; i < numFaces; i++) {
        glUniform3f(uniLocations[uniI++],
                    gSpheres[i].center.x(),
                    gSpheres[i].center.y(),
                    gSpheres[i].center.z());
        glUniform2f(uniLocations[uniI++],
                     gSpheres[i].r, gSpheres[i].rSq);
    }
    printf("vertexes\n");
    for (int i = 0; i < numVertexes; i++) {
        glUniform3f(uniLocations[uniI++],
                    seedSpheres[i].center.x(),
                    seedSpheres[i].center.y(),
                    seedSpheres[i].center.z());
        glUniform2f(uniLocations[uniI++],
                    seedSpheres[i].r, seedSpheres[i].rSq);
    }

    // for (int i = 0; i < boundingPlanes.length; i++) {
    //     glUniform3f(uniLocations[uniI++],
    //                 boundingPlanes[i].p1.x(),
    //                 boundingPlanes[i].p1.y(),
    //                 boundingPlanes[i].p1.z());
    //     glUniform3f(uniLocations[uniI++],
    //                 boundingPlanes[i].normal.x(),
    //                 boundingPlanes[i].normal.y(),
    //                 boundingPlanes[i].normal.z());
    // }

    return uniI;
}
