#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "Animation.h"


struct Bone
{
	int id = 0;
	std::string name = "";
	glm::mat4 offset = glm::mat4(1.0f);
	glm::mat4 mTransform = glm::mat4(1.0f);
	glm::mat4 globalTransform = glm::mat4(1.0f);
	std::vector<Bone> children = {};
};


class Armature
{
public:
	Bone rootBone;
	glm::mat4 globalInvTransform;
	std::unordered_map<std::string, std::pair<int, glm::mat4>> boneInfoTable = {};
	std::vector<glm::mat4> defaultPose;

	Armature() {};



	int nNodes = 0;
	void processNode(aiNode* node)
	{
		nNodes++;
		for (int i = 0; i < node->mNumChildren; i++)
			processNode(node->mChildren[i]);
	}


	void readSkeleton(Bone& rootBone, aiNode* node, glm::mat4 parentNodeTransform)
	{
		glm::mat4 globalTransform = parentNodeTransform * assimpToGlmMatrix(node->mTransformation);
		if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end())
		{
			rootBone.name = node->mName.data;
			rootBone.id = boneInfoTable[rootBone.name].first;
			rootBone.offset = boneInfoTable[rootBone.name].second;
			//rootBone.mTransform = assimpToGlmMatrix(node->mTransformation);
			rootBone.mTransform = globalTransform;
			rootBone.globalTransform = globalTransform;

			for (int i = 0; i < node->mNumChildren; i++)
			{
				readSkeletonRec(rootBone, node->mChildren[i], globalTransform);
			}
		}
		else
		{
			for (int i = 0; i < node->mNumChildren; i++)
			{
				readSkeleton(rootBone, node->mChildren[i], globalTransform);
			}
		}
	}

	void readSkeletonRec(Bone& parent, aiNode* node, glm::mat4 parentNodeTransform)
	{
		//glm::mat4 globalTransform = parentNodeTransform * assimpToGlmMatrix(node->mTransformation);
		glm::mat4 globalTransform = parentNodeTransform * assimpToGlmMatrix(node->mTransformation);
		glm::mat4 m0 = assimpToGlmMatrix(node->mTransformation);
		/*printf("\nmTransformation: %s\n", node->mName.C_Str());
		printf("%f %f %f %f\n", m0[0][0], m0[1][0], m0[2][0], m0[3][0]);
		printf("%f %f %f %f\n", m0[0][1], m0[1][1], m0[2][1], m0[3][1]);
		printf("%f %f %f %f\n", m0[0][2], m0[1][2], m0[2][2], m0[3][2]);
		printf("%f %f %f %f\n", m0[0][3], m0[1][3], m0[2][3], m0[3][3]);*/
		if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end())
		{
			countBones++;
			Bone child;
			child.name = node->mName.data;
			child.id = boneInfoTable[child.name].first;
			child.offset = boneInfoTable[child.name].second;
			child.mTransform = assimpToGlmMatrix(node->mTransformation);
			child.globalTransform = globalTransform;
			printf("ADDING: To %s, child: %s\n", parent.name.c_str(), child.name.c_str());
			parent.children.push_back(child);
			for (int i = 0; i < node->mNumChildren; i++)
			{
				readSkeletonRec(parent.children[parent.children.size()-1], node->mChildren[i], globalTransform);
			}
		}
		else
		{
			Bone child;
			child.name = "empty";
			child.id = 0;
			child.offset = glm::mat4(1.0f);
			child.mTransform = assimpToGlmMatrix(node->mTransformation);
			child.globalTransform = globalTransform;
			//printf("ADDING empty: To %s, child: %s\n", parent.name.c_str(), child.name.c_str());
			parent.children.push_back(child);
			for (int i = 0; i < node->mNumChildren; i++)
			{
				readSkeletonRec(parent.children[parent.children.size() - 1], node->mChildren[i], globalTransform);
			}
		}
	}

	std::pair<int, float> getTimeFraction(std::vector<float>& times, float& dt) {
		int segment = 0;
		while (dt >= times[segment])
			if (times.size() >= segment+1) segment++;
		float start = times[segment - 1];
		float end = times[segment];
		float frac = (dt - start) / (end - start);
		return { segment, frac };
	}
	int bonesTransformed = 0;
	void getPose(Animation& animation, Bone skeletion, float dt, std::vector<glm::mat4>& output, glm::mat4& parentTransform)
	{
		glm::mat4 localTransform = skeletion.mTransform;

		if (skeletion.name != "empty")
		{
			BoneTransformTrack& btt = animation.boneTransforms[skeletion.name];
			std::pair<unsigned int, float> fp;

			if (btt.positionTimestamps.size() >= 2)
			{
				fp = getTimeFraction(btt.positionTimestamps, dt);
			}
			else fp.first = 0;
			
			//printf("Position size: %d, current fp %d\n", btt.positions.size(), fp.first + 1);
			if (btt.positions.size() > fp.first + 1)
			{
				//printf("If worked");
				glm::vec3 position1 = btt.positions[fp.first];
				glm::vec3 position2 = btt.positions[fp.first + 1];
				//printf(" pos");
				glm::vec3 position = glm::mix(position1, position2, fp.second);
				// calculate interpolated rotation
				fp = getTimeFraction(btt.positionTimestamps, dt);
				glm::quat rotation1 = btt.rotations[fp.first];
				glm::quat rotation2 = btt.rotations[fp.first + 1];
				//printf(" rot");
				glm::quat rotation = glm::slerp(rotation1, rotation2, fp.second);
				//printf(" slerp");
				//calculate interpolated scale
				fp = getTimeFraction(btt.scaleTimestamps, dt);
				glm::vec3 scale1 = btt.scales[fp.first];
				glm::vec3 scale2 = btt.scales[fp.first + 1];
				//printf(" scal");
				glm::vec3 scale = glm::mix(scale1, scale2, fp.second);
				//printf(" mix");
				glm::mat4 positionMat = glm::mat4(1.0);
				glm::mat4 scaleMat = glm::mat4(1.0);
				//printf(" scal");

				// calculate localTransform
				positionMat = glm::translate(positionMat, position);
				glm::mat4 rotationMat = glm::toMat4(rotation);
				scaleMat = glm::scale(scaleMat, scale);
				localTransform = positionMat * rotationMat * scaleMat;
			}
		}
		glm::mat4 globalTransform = parentTransform;

		if (skeletion.name != "empty")
		{
			globalTransform = parentTransform * localTransform;
			output[skeletion.id] = globalInvTransform * globalTransform * skeletion.offset;
		}
		
		//update values for children bones
		for (Bone child : skeletion.children) {
			getPose(animation, child, dt, output, globalTransform);
		}
	}

	void getDefaultPose(Bone& skeletion, glm::mat4 parentTransform = glm::mat4(1.0f))
	{
		defaultPose.resize(boneInfoTable.size(), glm::mat4(1.0f));
		generateDefaultPose(rootBone, parentTransform);
		printf("nBones: %d\n", countBones);
	}
	int countBones = 0;
	void generateDefaultPose(Bone skeletion, glm::mat4 parentTransform)
	{
		glm::mat4 m0;
		glm::mat4 globalTransform = parentTransform;
		//glm::mat4 globalTransform = parentTransform * skeletion.mTransform;

		if (skeletion.name != "empty")
		{
			globalTransform = parentTransform * skeletion.mTransform;
			defaultPose[skeletion.id] = globalInvTransform * globalTransform * skeletion.offset;
		}
		
		

		
		
		//glm::mat4 m0;
		
		/*printf("GEN: BONE NAME: ");
		printf("%s\n", skeletion.name.c_str());
		printf("Children count: %d\n", skeletion.children.size());*/
		
		/*m0 = skeletion.offset;
		printf("offset:\n");
		printf("%f %f %f %f\n", m0[0][0], m0[1][0], m0[2][0], m0[3][0]);
		printf("%f %f %f %f\n", m0[0][1], m0[1][1], m0[2][1], m0[3][1]);
		printf("%f %f %f %f\n", m0[0][2], m0[1][2], m0[2][2], m0[3][2]);
		printf("%f %f %f %f\n", m0[0][3], m0[1][3], m0[2][3], m0[3][3]);

		m0 = globalTransform;
		printf("globalTransform:\n");
		printf("%f %f %f %f\n", m0[0][0], m0[1][0], m0[2][0], m0[3][0]);
		printf("%f %f %f %f\n", m0[0][1], m0[1][1], m0[2][1], m0[3][1]);
		printf("%f %f %f %f\n", m0[0][2], m0[1][2], m0[2][2], m0[3][2]);
		printf("%f %f %f %f\n", m0[0][3], m0[1][3], m0[2][3], m0[3][3]);

		m0 = globalInvTransform;
		printf("globalInvTransform:\n");
		printf("%f %f %f %f\n", m0[0][0], m0[1][0], m0[2][0], m0[3][0]);
		printf("%f %f %f %f\n", m0[0][1], m0[1][1], m0[2][1], m0[3][1]);
		printf("%f %f %f %f\n", m0[0][2], m0[1][2], m0[2][2], m0[3][2]);
		printf("%f %f %f %f\n", m0[0][3], m0[1][3], m0[2][3], m0[3][3]);

		m0 = defaultPose[skeletion.id];
		printf("final pose:\n");
		printf("%f %f %f %f\n", m0[0][0], m0[1][0], m0[2][0], m0[3][0]);
		printf("%f %f %f %f\n", m0[0][1], m0[1][1], m0[2][1], m0[3][1]);
		printf("%f %f %f %f\n", m0[0][2], m0[1][2], m0[2][2], m0[3][2]);
		printf("%f %f %f %f\n", m0[0][3], m0[1][3], m0[2][3], m0[3][3]);
		printf("\n");*/

		for (Bone child : skeletion.children)
		{
			generateDefaultPose(child, globalTransform);
		}
	}

	int getBoneCount()
	{
		return boneInfoTable.size();
	}
	//void calcTransformNext(Bone* parent, glm::mat4 parentTransform)
	//{
	//	Bone* child;
	//	for (int i = 0; i < parent->children.size(); i++)
	//	{
	//		child = parent->children[i];
	//		glm::mat4 childTransform = child->transformMatrix;
	//		//glm::mat4 childTransform = glm::mat4(1.0f);

	//		glm::vec3 scale;
	//		glm::quat rotation;
	//		glm::vec3 position;
	//		glm::vec3 skew;
	//		glm::vec4 perspective;
	//		glm::decompose(childTransform, scale, rotation, position, skew, perspective);



	//		glm::mat4 posMatrix = glm::translate(glm::mat4(1.0f), position);
	//		glm::mat4 rotMatrix = glm::toMat4(glm::quat(0.985, 0.173648, 0, 0));
	//		glm::mat4 scalMatrix = glm::scale(glm::mat4(1.0f), scale);
	//		glm::mat4 m0 = posMatrix * rotMatrix * scalMatrix;
	//		glm::mat4 globalTransform = parentTransform * childTransform;

	//		glm::mat4 finalTransform = globalTransform * child->offsetTransform;
	//		defaultPoseTransform.push_back(finalTransform);

	//		calcTransformNext(child, globalTransform);
	//	}
	//}




private:
	
};