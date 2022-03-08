#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Shader.h"

#include <string>
#include <vector>
#include "Globals.h"

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
	// Bone information
	glm::ivec4 IDs;
	glm::ivec4 IDs2;
	glm::vec4 Weights;
	glm::vec4 Weights2;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};


class Mesh {
public:
	// mesh Data
	std::string name;
	std::vector<Vertex>		  vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture>	  textures;
	unsigned int VAO;
	float printed = false;

	// constructor
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, std::string name = "") 
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->name = name;

		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
		//fillMissingTextures();
	}

	// render the mesh
	void Draw(Shader& shader, std::vector<glm::mat4>& currentPose)
	{
		// bind appropriate textures
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			std::string name = textures[i].type;

			glUniform1i(glGetUniformLocation(shader.ID, name.c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		std::vector<glm::mat4> boneIdentityTransform;
		for (int i = 0; i < 200; i++) boneIdentityTransform.push_back(glm::mat4(1.0f));
		//glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))

		int arrayLoc = glGetUniformLocation(shader.ID, "gBones");
		glUniformMatrix4fv(arrayLoc, currentPose.size(), GL_FALSE, &currentPose.at(0)[0][0]);


		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

private:
	// render data
	unsigned int VBO, EBO;

	// initializes all the buffer objects/arrays
	void setupMesh()
	{
		// create buffer/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// bone IDs
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)(offsetof(Vertex, IDs)));
		glEnableVertexAttribArray(6);
		glVertexAttribIPointer(6, 4, GL_INT, sizeof(Vertex), (void*)(offsetof(Vertex, IDs2)));
		//// bone weights
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Weights)));
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Weights2)));

		glBindVertexArray(0);
	}

	void fillMissingTextures()
	{
		bool isAlbedo = false;
		bool isRoughness = false;
		bool isNormal = false;
		bool isMetallic = false;
		bool isOpacity = false;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			std::string name = textures[i].type;
			if (name == "albedoMap") isAlbedo = true;
			if (name == "roughnessMap") isRoughness = true;
			if (name == "normalMap") isNormal = true;
			if (name == "metallicMap") isMetallic = true;
			if (name == "opacityMap") isOpacity = true;
		}

		// Fill missing textures
		if (!isAlbedo) 
		{
			unsigned int textureMap = createMultiValueTexture(255);
			Texture texture;
			texture.id = textureMap;
			texture.type = "albedoMap";
			textures.push_back(texture);
		}
		if (!isRoughness) 
		{
			GLuint textureMap = createSingleValueTexture(127);
			Texture texture;
			texture.id = textureMap;
			texture.type = "roughnessMap";
			textures.push_back(texture);
		}
		if (!isNormal) {}
		if (!isMetallic)
		{
			GLuint textureMap = createSingleValueTexture(0);
			Texture texture;
			texture.id = textureMap;
			texture.type = "metallicMap";
			textures.push_back(texture);
		}
		if (!isOpacity) 
		{
			GLuint textureMap = createSingleValueTexture(255);
			Texture texture;
			texture.id = textureMap;
			texture.type = "opacityMap";
			textures.push_back(texture);
		}
	}

	GLuint createSingleValueTexture(unsigned int value)
	{

		int width;
		int height;
		width = height = 128;
		unsigned char* data = new unsigned char[width * height * sizeof(unsigned char)];

		for (int i = 0; i < (int)(width * height * sizeof(unsigned char)); i++)
		{
			data[i] = value;
		}

		// Generate OpenGL texture.
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
		return textureID;
	}

	unsigned int createMultiValueTexture(unsigned int value)
	{
		
		int width;
		int height;
		width = height = 128;
		unsigned char* data = new unsigned char[width * height * 4];
		for (int i = 0; i < width * height * 4; i++)data[i] = 255;

		// Generate OpenGL texture.
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA8, GL_UNSIGNED_BYTE, data);
		return textureID;
	}
};
#endif


