#include "plyloader.hpp"

#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

unsigned int readUInt(string input)
{
	stringstream ss;
	unsigned int output = 0;

	ss << input;
	ss >> output;

	return output;
}

bool load_ply(const char * path, std::vector<Vertex> & data, std::vector<unsigned int> & mindices)
{
	string line;
	ifstream file(path, ios::in | ios::binary);
	unsigned int vertexCount = 0, faceCount = 0;

	bool format_be = false;
	int vertexProps = 0;
	string in_block = "";

	if(!file.is_open()) {
		printf("Could not open the requested path '%s'\n", path);
		return false;
	}

	getline(file, line);

	if(!file.good())
		goto eof;

	if(line != "ply") {
		printf("Bad file magic\n");
		return false;
	}

	while(true) {
		getline(file, line);

		if(!file.good())
			goto eof;

		if(line == "")
			continue;

		vector<string> subitems;

		string item;
		stringstream ss(line);

		// split input line in to tokens delimited by ' '
		while(getline(ss, item, ' '))
		{
			if(item == "")
				continue;

			subitems.push_back(item);
			item = "";
		}

		string cmd = subitems[0];
		int argc = subitems.size()-1;

		// XXX: we are assuming the format here...
		if(cmd == "end_header") {
			break;
		} else if(cmd == "format") {
			if(argc == 2) {
				format_be = subitems[1] != "binary_little_endian";
			} else {
				printf("Invalid format block\n");
				return false;
			}
		} else if(cmd == "element") {
			if(argc == 2) {
				if(subitems[1] == "vertex") { 
					vertexCount = readUInt(subitems[2]);
				} else if(subitems[1] == "face") {
					faceCount = readUInt(subitems[2]);
				}

				in_block = subitems[1];
			} else {
				printf("Invalid element block\n");
				return false;
			}
		} else if(cmd == "property") {
			if(in_block == "vertex") {
				vertexProps++;
				printf("vertex property %s\n", subitems[2].c_str());
			}
		}
	}

	// read in element data
	if(vertexCount == 0 || faceCount == 0)
	{
		printf("Unexpected vertex/face count\n");
		return false;
	}
	
	printf("Loading PLY model '%s' with %u vertices and %u faces...\n", path, vertexCount, faceCount);

#define BSWAP32(x) do { \
	char * __x = (char *)(x); \
	char __tmp = __x[0]; \
	__x[0] = __x[3]; \
	__x[3] = __tmp; \
	__tmp = __x[1]; \
	__x[1] = __x[2]; \
	__x[2] = __tmp; \
} while(0)

	vertexProps -= 3;

	for(size_t i = 0; i < vertexCount; i++) {
		Vertex v;

		file.read((char *)v.Position, sizeof(float)*3);

		if(!file.good())
			goto eof;

		if(format_be) {
			BSWAP32(&v.Position[0]);
			BSWAP32(&v.Position[1]);
			BSWAP32(&v.Position[2]);
		}

		v.Position[3] = 1.0;

		if(vertexProps != 3 && vertexProps != 4) {
			printf("Unexpected color property size\n");
			return false;
		}

		for(size_t j = 0; j < vertexProps; j++)
		{
			float color = file.get()/255.0;
			v.Color[j] = color;

			if(!file.good())
				goto eof;
		}

		v.Color[3] = 1.0;

		data.push_back(v);

		// we calculate normals in the face step
		v.Normal[0] = 0.0;
		v.Normal[1] = 0.0;
		v.Normal[2] = 0.0;
		v.Normal[3] = 0.0;
	}

	for(size_t i = 0; i < faceCount; i++)
	{
		int32_t indices[3];
		unsigned char listCount = 0;
		file.read((char *)&listCount, 1);

		if(!file.good())
			goto eof;

		if(listCount > 3) {
			printf("PLY loader only supports triangulated faces\n");
			return false;
		}

		for(size_t j = 0; j < listCount; j++) {
			int32_t index;
			file.read((char *)&index, sizeof(int32_t));

			if(format_be)
				BSWAP32(&index);

			indices[j] = index;

			if(index < 0 || index >= data.size())
			{
				printf("PLY loader encountered an invalid vertex index\n");
				return false;
			}
		}

		mindices.push_back(indices[0]);
		mindices.push_back(indices[1]);
		mindices.push_back(indices[2]);

		// process face
		glm::vec3 p1 = glm::vec3(data[indices[0]].Position[0],
			data[indices[0]].Position[1],
			data[indices[0]].Position[2]);
		glm::vec3 p2 = glm::vec3(data[indices[1]].Position[0],
			data[indices[1]].Position[1],
			data[indices[1]].Position[2]);
		glm::vec3 p3 = glm::vec3(data[indices[2]].Position[0],
			data[indices[2]].Position[1],
			data[indices[2]].Position[2]);
		glm::vec3 normal = glm::cross((p2-p1), (p3-p1));

		// Add face normals to each vertex. Normalize at the end
		for(size_t j = 0; j < listCount; j++) {
			data[indices[j]].Normal[0] += normal.x;
			data[indices[j]].Normal[1] += normal.y;
			data[indices[j]].Normal[2] += normal.z;
		}
	}

	// Calculate vector norms
	for(size_t i = 0; i < vertexCount; i++) {
		glm::vec3 normal = glm::vec3(data[i].Normal[0], data[i].Normal[1], data[i].Normal[2]);
		glm::vec3 norm = glm::normalize(normal);
		data[i].Normal[0] = norm.x;
		data[i].Normal[1] = norm.y;
		data[i].Normal[2] = norm.z;
	}

	file.close();
	return true;

eof:
	printf("Unexpected EOF reached\n");
	return false;
}
