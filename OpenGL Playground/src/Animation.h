#pragma once

#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Keyframe.h"

struct BoneTransformTrack {
	std::vector<float> positionTimestamps = {};
	std::vector<float> rotationTimestamps = {};
	std::vector<float> scaleTimestamps = {};

	std::vector<glm::vec3> positions = {};
	std::vector<glm::quat> rotations = {};
	std::vector<glm::vec3> scales = {};
};

class Animation
{
public:
	std::string Name;
	std::unordered_map<std::string, BoneTransformTrack> boneTransforms = {};
	std::map<int, Keyframe> keyframes;
	float duration = 0.0f;
	float ticksPerSecond = 0.0f;

	Animation(std::string Name)
	{
		this->Name = Name;
	}


	void setBoneTransform(std::string boneName, int time, glm::vec3 position, glm::quat rotation, glm::vec3 scale)
	{
		Keyframe currentFrame = getKeyframe(time);
		currentFrame.setTransform(boneName, position, rotation, scale);
	}

	void setBonePosition(std::string boneName, int time, glm::vec3 position)
	{
		Keyframe currentFrame = getKeyframe(time);
		currentFrame.setPosition(boneName, position);
	}

	void setBoneRotation(std::string boneName, int time, glm::quat rotation)
	{
		Keyframe currentFrame = getKeyframe(time);
		currentFrame.setRotation(boneName, rotation);
	}

	void setBoneScale(std::string boneName, int time, glm::vec3 scale)
	{
		Keyframe currentFrame = getKeyframe(time);
		currentFrame.setScale(boneName, scale);
	}

private:
	Keyframe getKeyframe(int time)
	{
		if (keyframes.find(time) == keyframes.end())
		{
			Keyframe newFrame(time);
			keyframes[time] = newFrame;
		}
		return keyframes[time];
	}


};