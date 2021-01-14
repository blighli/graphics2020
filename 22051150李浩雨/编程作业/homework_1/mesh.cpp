#include "mesh.h"

Mesh::Mesh(float vertices[])
{
	this->vertices.resize(36);
	//memcpy(&(this->vertices[0]), vertices, 36 * 8 * sizeof(float));
	for (unsigned int i = 0; i < 36; i++) {
		this->vertices[i].position.x = vertices[i * 3];
		this->vertices[i].position.y = vertices[i * 3 + 1];
		this->vertices[i].position.z = vertices[i * 3 + 2];
	}
	//setupMesh();
	setupMeshforSkybox();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	setupMesh();
}

void Mesh::Draw(Material* material)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	for (unsigned int i = 0; i < textures.size(); i++) {
		// 在绑定之前激活相应的纹理单元
		glActiveTexture(GL_TEXTURE0 + i);

		// 获取纹理序号（diffuse_textureN 中的 N）
		string number;
		string name = textures[i].type;

		if (name == "texture_diffuse") {
			number = std::to_string(diffuseNr++);
		}
		else if (name == "texture_specular") {
			number = std::to_string(specularNr++);
		}

		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		material->SetUniform1i(("material." + name + number).c_str(), i);
	}
	glBindVertexArray(this->VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::DrawAcMat(Material* material)
{
	if (material->diffuse != 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->diffuse);
		material->SetUniform1i("material.texture_diffuse1", 0);
	}
	if (material->specular != 0) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material->specular);
		material->SetUniform1i("material.texture_specular1", 1);
	}

	glBindVertexArray(this->VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::DrawSkybox(Material* material, unsigned int textureId)
{
	glBindVertexArray(this->VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertices.size(), &(this->vertices[0]), GL_STATIC_DRAW);

	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * this->indices.size(), &(this->indices[0]), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::texCoords));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);//解绑
}

void Mesh::setupMeshforSkybox()
{
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertices.size(), &(this->vertices[0]), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);//解绑
}