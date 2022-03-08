#pragma once
#include <fstream>
#include <strstream>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <string>

bool LoadFromObjectFile(std::string sFileName, std::vector<float> &vertices)
{

	std::ifstream f(sFileName);
	if (!f.is_open())
	{
		std::cout << "Unable to open file";
		return false;
	}
	// Local cache of verts and vertices
	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> uvs;
	//std::vector<float> vertices;
	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		char junk;

		if (line[0] == 'v')
		{
			if (line[1] == 't')
			{
				glm::vec2 t;
				s >> junk >> junk >> t.x >> t.y;
				/*t.x = 1.0f - t.x;
				t.y = 1.0f - t.y;*/
				uvs.push_back(t);
			}
			else
			{
				glm::vec3 v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}
		}
		
		
		if (uvs.size() == 0)
		{
			if (line[0] == 'f')
			{
				
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];

				vertices.push_back(verts[f[0] - 1].x);
				vertices.push_back(verts[f[0] - 1].y);
				vertices.push_back(verts[f[0] - 1].z);
				vertices.push_back(0.0f);
				vertices.push_back(0.0f);

				vertices.push_back(verts[f[1] - 1].x);
				vertices.push_back(verts[f[1] - 1].y);
				vertices.push_back(verts[f[1] - 1].z);
				vertices.push_back(0.0f);
				vertices.push_back(0.0f);

				vertices.push_back(verts[f[2] - 1].x);
				vertices.push_back(verts[f[2] - 1].y);
				vertices.push_back(verts[f[2] - 1].z);
				vertices.push_back(0.0f);
				vertices.push_back(0.0f);
			}
		}
		else
		{
			if (line[0] == 'f')
			{
				s >> junk;
				std::string tokens[6];
				int nTokenCount = -1;
				
				while (!s.eof())
				{
					char c = s.get();
					if (c == ' ' || c == '/') nTokenCount++;
					else tokens[nTokenCount].append(1, c);
				}
				tokens[nTokenCount].pop_back();

				//printf("%f\n", verts[std::stoi(tokens[0]) - 1].x);

				vertices.push_back(verts[std::stoi(tokens[0]) - 1].x);
				vertices.push_back(verts[std::stoi(tokens[0]) - 1].y);
				vertices.push_back(verts[std::stoi(tokens[0]) - 1].z);
				vertices.push_back(uvs[std::stoi(tokens[1]) - 1].x);
				vertices.push_back(uvs[std::stoi(tokens[1]) - 1].y);

				vertices.push_back(verts[std::stoi(tokens[2]) - 1].x);
				vertices.push_back(verts[std::stoi(tokens[2]) - 1].y);
				vertices.push_back(verts[std::stoi(tokens[2]) - 1].z);
				vertices.push_back(uvs[std::stoi(tokens[3]) - 1].x);
				vertices.push_back(uvs[std::stoi(tokens[3]) - 1].y);

				vertices.push_back(verts[std::stoi(tokens[4]) - 1].x);
				vertices.push_back(verts[std::stoi(tokens[4]) - 1].y);
				vertices.push_back(verts[std::stoi(tokens[4]) - 1].z);
				vertices.push_back(uvs[std::stoi(tokens[5]) - 1].x);
				vertices.push_back(uvs[std::stoi(tokens[5]) - 1].y);
			}
		}
	}
	printf("Worked");
	return true;
}