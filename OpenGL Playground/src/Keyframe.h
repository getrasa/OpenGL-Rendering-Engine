#pragma once
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

// Keyframe transformations are in Bone/Parent space.
class Keyframe
{
public:
	unsigned int timeStamp;
	std::map<std::string, glm::mat4> boneTransform;

	Keyframe() {}
	Keyframe(int timeStamp)
	{
		this->timeStamp = timeStamp;
	};


	void setTransform(std::string boneName, glm::mat4 transMatrix)
	{
		boneTransform[boneName] = transMatrix;
	}

	void setTransform(std::string boneName, glm::vec3 position, glm::quat rotation, glm::vec3 scale)
	{
		glm::mat4 posMatrix = glm::translate(glm::mat4(), position);
		glm::mat4 rotMatrix = glm::toMat4(rotation);
		glm::mat4 scalMatrix = glm::scale(glm::mat4(), scale);
		glm::mat4 transMatrix = posMatrix * rotMatrix * scalMatrix;

		boneTransform[boneName] = transMatrix;
	}

	void setPosition(std::string boneName, glm::vec3 newPosition)
	{
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 position;
		glm::mat4 transMatrix = boneTransform[boneName];
		decomposeMtx(transMatrix, scale, rotation, position);
		//rotation = glm::conjugate(rotation);]

		setTransform(boneName, newPosition, rotation, scale);
	}

	void setRotation(std::string boneName, glm::quat newRotation)
	{
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 position;
		glm::mat4 transMatrix = boneTransform[boneName];
		decomposeMtx(transMatrix, scale, rotation, position);

		setTransform(boneName, position, newRotation, scale);
	}

	void setScale(std::string boneName, glm::vec3 ourScale)
	{
		glm::mat4 transMatrix = boneTransform[boneName];
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 position;
		decomposeMtx(transMatrix, scale, rotation, position);
		//rotation = glm::conjugate(rotation);]

		setTransform(boneName, position, rotation, ourScale);
	}

	void decomposeMtx(const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale)
	{
		pos = m[3];
		for (int i = 0; i < 3; i++)
			scale[i] = glm::length(glm::vec3(m[i]));
		const glm::mat3 rotMtx(
			glm::vec3(m[0]) / scale[0],
			glm::vec3(m[1]) / scale[1],
			glm::vec3(m[2]) / scale[2]);
		rot = glm::quat_cast(rotMtx);
	}
	
};