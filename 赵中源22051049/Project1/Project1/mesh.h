#ifndef MESH_H
#define MESH_H

#include<vector>
#include<string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.h"

using std::vector;
using std::string;

// 定义mesh顶点
struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};


// 定义纹理
struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh
{
public:
	//网格数据
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

public:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	void setupMesh();

public:
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void Draw(Shader& shader);
};

#endif // !MESH_H
