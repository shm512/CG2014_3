#pragma once

#include "definitions.h"
#include "shader.h"

class ShaderProgram
{
	Shader vertex,fragment;
public:
	uint programObject;
	
    //uniform locations of related external stuff:
    int loc_ModelViewMatrix, loc_NormalMatrix, loc_ModelViewProjectionMatrix;
    int loc_TextureSampler, loc_UseTextureFlag;
    int loc_lightPos;
    int loc_lightPower_ambient, loc_lightPower_diffuse, loc_lightPower_specular;
    int loc_k_ambient, loc_k_diffuse, loc_k_specular, loc_shininess;
    
    
	ShaderProgram();
	~ShaderProgram();
	void init(const char* vName, const char* fName);
};
