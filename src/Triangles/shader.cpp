#include <stdio.h>
#include <cstring>
#include <iostream>

using namespace std;

#include "shader.h"
#include <GL/glew.h>


Shader::Shader(void):
	shaderObject(0),
    strName(0),
    strSource(0),
	iLength(0),
	shaderType(0)
{}

Shader::STATUS
Shader::read(const char *filename, uint type)
{
	if (strName)
	{
		delete[] strName;
		delete[] strSource;
		strName=0; strSource=0;
	}		
	strName = new char [strlen(filename) + 1];
    strcpy(strName,filename);
	strName[strlen(filename)] = 0;

    FILE *file = fopen(strName, "rb");
	if (file)
	{
		fseek (file, 0, SEEK_END);
		iLength = ftell(file);
		
		if (iLength == 0)
		{
			strSource = 0;
			iStatus = EMPTY_FILE;
		}
		else
		{
			strSource = new char[iLength + 1];
			strSource[iLength] = 0;
			rewind (file);
			size_t actualLength = fread(strSource, 1, iLength, file);

			iStatus = (actualLength == size_t(iLength)) ? SUCCESS : READ_ERROR;
		}
		fclose(file);
	}
	else
	{
		iStatus = FILE_NOT_FOUND;
	}
	

	shaderType = type;
	shaderObject = glCreateShader(shaderType);
	if (strSource)
    {
		glShaderSource(
            shaderObject,
            1,
            (const GLchar **) &strSource,
            (const GLint *) &iLength
        );
    }

	return iStatus;
}

int
Shader::compile()
{
	int success;
	glCompileShader(shaderObject);
	glGetShaderiv (shaderObject, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		cerr << "Shader" << shaderType << "compile error" << endl;
		GLint maxLength = 0;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &maxLength);
 
        char* errorLog = new char[maxLength];
        glGetShaderInfoLog(shaderObject, maxLength, &maxLength, &errorLog[0]);
		cout << errorLog << endl;
		delete[] errorLog;

		glDeleteShader(shaderObject);
	}
	else
		cout << "Shader" << " compilation succeed" << endl;

	
	return success;
}

int
Shader::readAndCompile(const char *filename, uint type)
{
	read(filename, type);
	if (iStatus != Shader::SUCCESS)
	{
		cerr << "Error while reading shader. Invalid name or empty file." << endl;
		return 0;
	}
	return compile();
	
}

void
Shader::Release()
{
	if (shaderObject > 0)
	{
		glDeleteShader(shaderObject);
		shaderObject = -1;
	}
}

Shader::~Shader()
{
	delete[] strName;
	delete[] strSource;
}
