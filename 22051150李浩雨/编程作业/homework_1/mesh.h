#ifndef _MESH_H_
#define _MESH_H_

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Material.h"
#include <GL/glew.h>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	Mesh(float vertices[]);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	void Draw(Material* material);
	void DrawAcMat(Material* material);
	void DrawSkybox(Material* material, unsigned int textureId);

private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();
	void setupMeshforSkybox();
};

#endif // !_MESH_H_
