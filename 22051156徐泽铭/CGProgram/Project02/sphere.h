//
// Created by Mirocos on 2021/1/11.
//

#ifndef CGPROGRAM_SPHERE_H
#define CGPROGRAM_SPHERE_H

#include <vector>
#include "shader.h"
class Sphere {

public:
    Sphere(float radius = 1.f, int sectorCount = 36, int stackCount = 18, bool smooth = true);
    ~Sphere() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    float getRadius() const { return radius; }
    float getSectorCount() const { return sectorCount; }
    float getStackCount() const { return stackCount; }

    void set(float radius, int sectorCount, int stackCount, bool smooth = true);
    void setShader(std::string vs, std::string fs);
    void setRadius(float radius);
    void setSectorCount(int sectorCount);
    void setStackCount(int stackCount);
    void setSmooth(bool smooth);
    void setTexture(const char* texturePath);
    void init();
    void use() { shader.use(); }


    // for vertex data
    unsigned int getVertexCount() const     { return (unsigned int)vertices.size() / 3; }
    unsigned int getNormalCount() const     { return (unsigned int)normals.size() / 3; }
    unsigned int getTexCoordCount() const   { return (unsigned int)texCoords.size() / 2; }
    unsigned int getIndexCount() const      { return (unsigned int)indices.size(); }
//    unsigned int getLineIndexCount() const  { return (unsigned int)lineIndices.size(); }
    unsigned int getTriangleCount() const   { return getIndexCount() / 3; }
    unsigned int getVertexSize() const      { return (unsigned int)vertices.size() * sizeof(float); }
    unsigned int getNormalSize() const      { return (unsigned int)normals.size() * sizeof(float); }
    unsigned int getTexCoordSize() const    { return (unsigned int)texCoords.size() * sizeof(float); }
    unsigned int getIndexSize() const       { return (unsigned int)indices.size() * sizeof(unsigned int); }
//    unsigned int getLineIndexSize() const   { return (unsigned int)lineIndices.size() * sizeof(unsigned int); }
    const float* getVertices() const        { return vertices.data(); }
    const float* getNormals() const         { return normals.data(); }
    const float* getTexCoords() const       { return texCoords.data(); }
    const unsigned int* getIndices() const  { return indices.data(); }
//    const unsigned int* getLineIndices() const  { return lineIndices.data(); }




    void draw();


    Shader shader;
private:
    // member functions
    void buildVerticesSmooth();
    void buildVerticesFlat();
    void buildInterleavedVertices();
    void clearArrays();
//    void addVertex(float x, float y, float z);
//    void addNormal(float x, float y, float z);
//    void addTexCoord(float s, float t);
//    void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);
//    std::vector<float> computeFaceNormal(float x1, float y1, float z1,
//                                         float x2, float y2, float z2,
//                                         float x3, float y3, float z3);




    // member variables
    float radius;
    int sectorCount;
    int stackCount;
    bool smooth;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> lineIndices;

    std::vector<float> interleavedVertices;
    int interleavedStride;


    unsigned int texture;
    unsigned int VAO, VBO, EBO;

};


#endif //CGPROGRAM_SPHERE_H
