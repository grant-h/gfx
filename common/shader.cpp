#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "shader.hpp"

GLuint LoadShader(const char * path, std::string & code)
{
	// Read the Vertex Shader code from the file
	std::ifstream stream(path, std::ios::in);

	if(stream.is_open()){
		std::string line = "";
		while(getline(stream, line))
			code += "\n" + line;
		stream.close();
	} else{
		printf("Impossible to open shader '%s'.\n", path);
		return 0;
	}

	return code.size();
}

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path,
		const char * geometry_file_path) {

	// Create the shaders
	std::vector<GLuint> ids;
	std::string vert_code, frag_code, geo_code;
	const char * paths[] = {vertex_file_path, fragment_file_path, geometry_file_path};
	std::string codes[] = {vert_code, frag_code, geo_code};

	GLint Result = GL_FALSE;
	int InfoLogLength = 0;

	ids.push_back(glCreateShader(GL_VERTEX_SHADER));
	ids.push_back(glCreateShader(GL_FRAGMENT_SHADER));

	if(geometry_file_path != NULL)
		ids.push_back(glCreateShader(GL_GEOMETRY_SHADER));

	for(int i = 0; i < ids.size(); i++) {
		// Read shader
		if(!LoadShader(paths[i], codes[i])) {
			return 0;
		}

		// Compile Shader
		printf("Compiling shader : %s\n", paths[i]);

		char const * sourcePointer = codes[i].c_str();
		glShaderSource(ids[i], 1, &sourcePointer , NULL);
		glCompileShader(ids[i]);

		// Check Shader
		glGetShaderiv(ids[i], GL_COMPILE_STATUS, &Result);
		glGetShaderiv(ids[i], GL_INFO_LOG_LENGTH, &InfoLogLength);

		if ( InfoLogLength > 0 ){
			std::vector<char> shaderMessage(InfoLogLength+1);

			glGetShaderInfoLog(ids[i], InfoLogLength, NULL, &shaderMessage[0]);
			printf("%s\n", &shaderMessage[0]);

			return 0;
		}
	}

	// Link the program
	printf("Linking program\n");

	GLuint ProgramID = glCreateProgram();

	for(int i = 0; i < ids.size(); i++) {
		glAttachShader(ProgramID, ids[i]);
	}

	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);

		return 0;
	}

	
	// Free resources
	for(int i = 0; i < ids.size(); i++) {
		glDetachShader(ProgramID, ids[i]);
		glDeleteShader(ids[i]);
	}

	return ProgramID;
}


