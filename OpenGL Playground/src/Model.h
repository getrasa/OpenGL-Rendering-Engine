#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"
#include "Mesh.h"
#include "Armature.h"
#include "Animation.h"
#include "Shader.h"
#include "Globals.h"

#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include "Textures.h"

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);
unsigned int TextureFromFileAbsolute(std::string path);

class Model
{
public:
	// model data
	std::vector<Mesh>		meshes;
	std::vector<Mesh>		transparentMeshes;
	Armature				armature;
	std::vector<Animation>	animations;
	int animationIndex;
	std::map<std::string, int> animationNameToIndex;
	std::vector<Texture>	textures_loaded;	// stores all the textures loaded so far, optimization to make sure textues aren't loaded more than once.
	std::vector<glm::mat4>	currentPose;
	glm::mat4 globalInverseTransform;
	std::vector<std::string> meshNames;

	std::string directory;
	bool gammaCorrection;

	Model() {};

	// constructor, expects a filepath to a 3D model.
	Model(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
	{
		this->animationIndex = -1;
		loadModel(path);
		currentPose = armature.defaultPose;
	}

	void updatePose(float timeInSeconds)
	{
		glm::mat4 identity = glm::mat4(1.0f);
		int animIndex = animationIndex;
		if (animations.size() > 0 && animIndex < animations.size() && animIndex >= 0)
		{
			float timeInTicks = timeInSeconds * animations[animIndex].ticksPerSecond;
			float dt = fmod(timeInTicks, animations[animIndex].duration - 1.0f);

			armature.getPose(animations[animIndex], armature.rootBone, dt, currentPose, identity);
		}
	}

	// draw the model, and thus all its meshes
	void Draw(Shader& shader, float timeInSeconds)
	{	
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(shader, currentPose);
		}


		for (unsigned int i = 0; i < transparentMeshes.size(); i++)
			transparentMeshes[i].Draw(shader, currentPose);
	}

	void setAnimation(std::string name)
	{
		if (animationNameToIndex.find(name) != animationNameToIndex.end())
		{
			printf("Index %d\n", animationNameToIndex[name]);
			this->animationIndex = animationNameToIndex[name];
		}
		else
		{
			this->animationIndex = -1;
		}
	}

	void setAnimationIndex(int index)
	{
		printf("Index %d, anim size: %d", index, animations.size());
		this->animationIndex = index;
	}


private:
	void loadModel(std::string const& path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		//const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			std::cout << "ERROR:ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		//Global Inverse Transformation
		glm::mat4 globalTransform = assimpToGlmMatrix(scene->mRootNode->mTransformation);
		armature.globalInvTransform = glm::inverse(globalTransform);
		glm::mat4 m0 = armature.globalInvTransform;

		// process ASSIMP's root node recurisvely
		processNode(scene->mRootNode, scene);
		//sortTransparentMeshesLast();

		armature.processNode(scene->mRootNode);
		armature.readSkeleton(armature.rootBone, scene->mRootNode, glm::mat4(1.0f));
		printf("nNodes: %d\n", armature.nNodes);
		printf("Skeleton loaded\n");
		if (armature.rootBone.children.size() != 0)
		{
			armature.getDefaultPose(armature.rootBone, glm::mat4(1.0f));
			printf("Generated default pose\n");
		}



		// animations
		loadAnimations(scene);

		printf("Nodes count: %d\n", nodesCount);
		//printf("Bone count: %d\n", armature.getBoneCount());
		for (int i = 0; i < armature.rootBone.children.size(); i++)
		{
			printf("CHILDREN: %s\n", armature.rootBone.children[i].name.c_str());
		}
		printf("Bone count: %d\n", boneCount);
		printf("aiMesh count: %d\n", scene->mNumMeshes);
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];
			printf("Mesh: %d, Name %s, nVertices: %d, nBones: %d\n", i, mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumBones);
		}
	}


	// process a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	int nodesCount = 0;
	int boneCount = 0;
	int overflows = 0;
	void processNode(aiNode* node, const aiScene* scene)
	{
		nodesCount++;
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene.
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(mesh, scene);
			//meshes.push_back(processMesh(mesh, scene));
		}


		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}


	void processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// data to fill
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		// walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			// assign vertex parameters
			vertex.Position = assimpToGlmVec3(mesh->mVertices[i]);

			vertex.IDs = glm::ivec4(0);
			vertex.IDs2 = glm::ivec4(0);
			vertex.IDs3 = glm::ivec4(0);
			vertex.Weights = glm::vec4(0.0f);
			vertex.Weights2 = glm::vec4(0.0f);
			vertex.Weights3 = glm::vec4(0.0f);

			if (mesh->HasNormals())
				vertex.Normal = assimpToGlmVec3(mesh->mNormals[i]);

			if (mesh->mTextureCoords[0])
			{
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
				// use models where a vertex can have multiple texture coordinates so we always take the first (0).
				vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
				// tangent
				vertex.Tangent = assimpToGlmVec3(mesh->mTangents[i]);
				vertex.Bitangent = assimpToGlmVec3(mesh->mBitangents[i]);

			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}


		// now walk throug each of the mesh's bones and add it's weight to coresponding vertexes.
		for (int i = 0; i < mesh->mNumBones; i++)
		{
			aiBone* bone = mesh->mBones[i];

			if (armature.boneInfoTable.find(bone->mName.C_Str()) == armature.boneInfoTable.end())
			{
				glm::mat4 m = assimpToGlmMatrix(bone->mOffsetMatrix);
				armature.boneInfoTable[bone->mName.C_Str()] = { i, m };
				boneCount++;
			}


			for (int j = 0; j < bone->mNumWeights; j++)
			{
				aiVertexWeight weight = bone->mWeights[j];

				for (int k = 0; k < NUM_BONES_PER_VEREX; k++)
				{
					if (k < 4 && vertices[weight.mVertexId].Weights[k] == 0.0f)
					{
						vertices[weight.mVertexId].IDs[k] = i;
						vertices[weight.mVertexId].Weights[k] = weight.mWeight;
						break;
					}
					else if (k >= 4 && k < 8 && vertices[weight.mVertexId].Weights2[k - 4] == 0.0f)
					{
						vertices[weight.mVertexId].IDs2[k - 4] = i;
						vertices[weight.mVertexId].Weights2[k - 4] = weight.mWeight;
						break;
					}
					else if (k >= 8 && vertices[weight.mVertexId].Weights3[k - 8] == 0.0f)
					{
						if (k == 11) overflows++;
						vertices[weight.mVertexId].IDs3[k - 8] = i;
						vertices[weight.mVertexId].Weights3[k - 8] = weight.mWeight;
						break;
					}
				}
			}
		}

		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "albedoMap");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "normalMap");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// roughness maps
		std::vector<Texture> roughnessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "roughnessMap");
		textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
		//// metallic maps
		std::vector<Texture> metallicMaps = loadMaterialTextures(material, aiTextureType_METALNESS, "metallicMap");
		textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
		//// opacity maps
		std::vector<Texture> opacityMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "opacityMap");
		textures.insert(textures.end(), opacityMaps.begin(), opacityMaps.end());
		if (opacityMaps.size() > 0)
		{
			transparentMeshes.push_back(Mesh(vertices, indices, textures, mesh->mName.data));
		}
		else
			meshes.push_back(Mesh(vertices, indices, textures, mesh->mName.data));

		// return a mesh object created from the extracted mesh data
		//return Mesh(vertices, indices, textures, mesh->mName.data);
	}



	void loadAnimations(const aiScene* scene)
	{
		printf("Total of %d animations: \n", scene->mNumAnimations);
		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			aiAnimation* anim = scene->mAnimations[i];
			
			Animation animation(anim->mName.data);
			printf("- %s\n", animation.Name.c_str());
			animationNameToIndex.insert(std::pair<std::string, int>(animation.Name.c_str(), i));

			if (anim->mTicksPerSecond != 0.0f)
				animation.ticksPerSecond = anim->mTicksPerSecond;
			else
				animation.ticksPerSecond = 1;

			animation.ticksPerSecond = anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f;
			animation.duration = anim->mDuration;

			animation.boneTransforms = {};

			//load positions rotations and scales for each bone
			// each channel represents each bone
			for (int i = 0; i < anim->mNumChannels; i++) {
				aiNodeAnim* channel = anim->mChannels[i];

				BoneTransformTrack track;
				for (int j = 0; j < channel->mNumPositionKeys; j++) {
					track.positionTimestamps.push_back(channel->mPositionKeys[j].mTime);
					track.positions.push_back(assimpToGlmVec3(channel->mPositionKeys[j].mValue));
				}
				for (int j = 0; j < channel->mNumRotationKeys; j++) {
					track.rotationTimestamps.push_back(channel->mRotationKeys[j].mTime);
					track.rotations.push_back(assimpToGlmQuat(channel->mRotationKeys[j].mValue));

				}
				for (int j = 0; j < channel->mNumScalingKeys; j++) {
					track.scaleTimestamps.push_back(channel->mScalingKeys[j].mTime);
					track.scales.push_back(assimpToGlmVec3(channel->mScalingKeys[j].mValue));

				}
				std::string boneName = channel->mNodeName.C_Str();
				animation.boneTransforms[boneName] = track;
			}
			animations.push_back(animation);
		}
	}



	// checks all material textuers of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		printf("Texture count: %d\n", mat->GetTextureCount(type));
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory, false);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}

	/*void sortTransparentMeshesLast()
	{
		std::vector<Mesh> opaqueMeshes;
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			bool isTransparent = false;
			for (unsigned j = 0; j < meshes[i].textures.size(); j++)
			{
				if (meshes[i].textures[j].type == "opacityMap")
				{
					isTransparent = true;
					transparentMeshes.push_back(meshes[i]);
					break;
				}
			}
			if (!isTransparent)
			{
			std::vector<Texture> textureList = setMaterialTextures(WHITE_MAP_PATH, "opacityMap");
			meshes[i].textures.insert(meshes[i].textures.end(), textureList.begin(), textureList.end());
			opaqueMeshes.push_back(meshes[i]);
			}
		}
		meshes = opaqueMeshes;
	}*/

	/*std::vector<Texture> setMaterialTextures(std::string path, std::string typeName)
	{
		std::vector<Texture> textures;
		Texture texture;
		texture.id = TextureFromFile(path);
		texture.type = typeName;
		texture.path = path;
		textures.push_back(texture);
		textures_loaded.push_back(texture);

		return textures;
	}*/

};

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	std::string base_filename = filename.substr(filename.find_last_of("/\\") + 1);
	filename = directory + "/textures/" + base_filename;
	printf("filename: %s\n", filename.c_str());

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int TextureFromFileAbsolute(std::string path)
{
	std::string filename = path;
	printf("filename: %s\n", filename.c_str());

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
#endif