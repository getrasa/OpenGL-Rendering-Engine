#pragma once

// Variables
const unsigned int NUM_BONES_PER_VEREX = 12;
const std::string WHITE_MAP_PATH = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/white_map.png";



// Functions
inline glm::mat4 assimpToGlmMatrix(aiMatrix4x4 mat) {
	glm::mat4 m;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			m[x][y] = mat[y][x];
		}
	}
	return m;
}

inline glm::vec3 assimpToGlmVec3(aiVector3D vec) {
	return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::quat assimpToGlmQuat(aiQuaternion quat) {
	glm::quat q;
	q.x = quat.x;
	q.y = quat.y;
	q.z = quat.z;
	q.w = quat.w;

	return q;
}

void decomposeMatrix(const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale)
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