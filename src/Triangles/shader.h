#pragma once

#include "definitions.h"

class Shader
{
public:
	enum STATUS {SUCCESS, FILE_NOT_FOUND, EMPTY_FILE, READ_ERROR};
	uint shaderObject;
private:
	STATUS iStatus;
	char *strName;
	char *strSource;
	unsigned long iLength;
	uint shaderType;
public:
	Shader();
	~Shader();
	STATUS read(const char *filename, uint type);
	int compile();
	int readAndCompile(const char *filename, uint type);
	void Release();
};

