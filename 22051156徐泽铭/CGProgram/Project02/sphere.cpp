//
// Created by Mirocos on 2021/1/11.
//

#include "sphere.h"
#include <math.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;

static unsigned int loadTexture(char const * path){
    unsigned int textureId;
    glGenTextures(1, &textureId);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if(data){
        GLenum format;
        if(nrComponents == 1)
            format = GL_RED;
        else if(nrComponents == 3)
            format = GL_RGB;
        else if(nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cerr << "Texture failed to load at path: " << path << std:: endl;
        stbi_image_free(data);
    }

    return textureId;
}

Sphere::Sphere(float radius, int sectorCount, int stackCount, bool smooth): interleavedStride(32) {
    set(radius,sectorCount, stackCount, smooth);
}






void Sphere::set(float radius, int sectorCount, int stackCount, bool smooth) {
    this->radius = radius;
    this->sectorCount = sectorCount;
    if(sectorCount < MIN_SECTOR_COUNT)
        this->sectorCount = MIN_SECTOR_COUNT;
    this->stackCount = stackCount;
    if(stackCount < MIN_STACK_COUNT)
        this->stackCount = MIN_STACK_COUNT;

    this->smooth = smooth;
    if(smooth)
        buildVerticesSmooth();
    else
        buildVerticesFlat();
}


void Sphere::setShader(std::string vs, std::string fs){
    shader  = Shader(vs.c_str(), fs.c_str());

}


void Sphere::setRadius(float radius) {
    this->radius = radius;
}

void  Sphere::setSectorCount(int sectorCount) {
    this->sectorCount = sectorCount;
}

void  Sphere::setStackCount(int stackCount) {
    this->stackCount = stackCount;
}

void Sphere::setSmooth(bool smooth) {
    this->smooth = smooth;
}


void Sphere::setTexture(const char * texturePath) {
    this->texture = loadTexture(texturePath);
}



void Sphere::clearArrays() {
    std::vector<float>().swap(vertices);
    std::vector<float>().swap(normals);
    std::vector<float>().swap(texCoords);
    std::vector<unsigned int>().swap(indices);
}


void Sphere::buildVerticesSmooth() {

    // x = (r * cos(phi)) * cos(theta)
    // y = (r * cos(phi)) * sin(theta)
    // z = r * sin(phi)

    // theta(sectorAngle) = 2 * PI - sectorStep / sectorCount;
    // phi(stackAngle)    = 2 * PI - stackStep  / stackCount;
    const float PI = acosf(-1.f);

    clearArrays();

    float x, y, z, xy;                             // position
    float nx, ny, nz, lengthInv = 1.0f / radius;   // normal
    float s, t;                                    // texCoord

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i  <= stackCount; i++){

        stackAngle = PI / 2.f - i * stackStep;              // PI / 2 -> -PI / 2
        xy =  radius * cosf(stackAngle);                    // r * cos(phi)
        z = radius * sinf(stackAngle);                      // r * sin(phi)
        for(int j = 0; j <= sectorCount; j++){
            sectorAngle = j * sectorStep;
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            // vertex position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);


            // normal
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);


            // texCoord
            s = (float) j / sectorCount;
            t = (float) i / stackCount;
            texCoords.push_back(s);
            texCoords.push_back(t);

        }
    }




    // build indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1

    unsigned  int k1, k2;
    for(int i = 0; i < stackCount; i++){
        k1 = i * (sectorCount + 1);         // beginning of current stack
        k2 = k1 + sectorCount + 1;          // beginning of next stack

        for(int j = 0; j < sectorCount; j++, k1++, k2++){
            if(i != 0){
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if(i != (stackCount - 1)){
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }

    }


    buildInterleavedVertices();


}


void Sphere::buildInterleavedVertices() {
    std::vector<float>().swap(interleavedVertices);
    std::size_t i, j;
    std::size_t count = vertices.size();
    for(int i = 0, j = 0; i < count; i += 3, j+= 2){
        interleavedVertices.push_back(vertices[i]);
        interleavedVertices.push_back(vertices[i + 1]);
        interleavedVertices.push_back(vertices[i + 2]);

        interleavedVertices.push_back(normals[i]);
        interleavedVertices.push_back(normals[i + 1]);
        interleavedVertices.push_back(normals[i + 2]);

        interleavedVertices.push_back(texCoords[j]);
        interleavedVertices.push_back(texCoords[j + 1]);
    }
}


void Sphere::buildVerticesFlat() {

}


void Sphere::init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

//    std::cout << "======================data=====================" << std::endl;
//    for(int i = 0 ; i < interleavedVertices.size(); i += 8){
//        for(int j = 0; j < 8; j++){
//            printf("%f%s", interleavedVertices[i + j], j==7?"\n":" ");
//        }
//    }
//    printf("%d\n", sizeof(float) * interleavedVertices.size());
//    printf("%d\b", sizeof(float) * indices.size());
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * interleavedVertices.size(), interleavedVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, interleavedStride, (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, interleavedStride, (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, interleavedStride, (void*) (6 * sizeof(float)));


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * indices.size(), indices.data(), GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);




}

void Sphere::draw() {
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    shader.use();

//    glActiveTexture(GL_TEXTURE0 + texture);


    // TODO set uniform variables

    glDrawElements(GL_TRIANGLES, (unsigned  int)indices.size(), GL_UNSIGNED_INT, 0);

}

