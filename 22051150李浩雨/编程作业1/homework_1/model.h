#ifndef _MODEL_H_
#define _MODEL_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

#include "stb_image.h"
#include "mesh.h"

class Model
{
public:
	Model(std::string path, glm::vec3 position = glm::vec3(0, 0, 0));


	std::vector<Mesh> meshes;
	std::string directory;
	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Texture> textures_loaded;

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);
	glm::mat4 GetModelMatrix();

	void Draw(Material* material);
	void DrawAcMat(Material* material);

private:
	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	unsigned int LoadTextureFromFile(const char* path, const string& directory);

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

#endif // !_MODEL_H_
