#include "shaderprogram.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

using namespace std;

ShaderProgram::ShaderProgram()
    : programObject(0)
{
}


ShaderProgram::~ShaderProgram()
{
	programObject = 0;
}

void
ShaderProgram::init(const char *vName, const char *fName)
{
	GLint success; //local variable to check status
	//load and compile vertex shader
	success = vertex.readAndCompile(vName, GL_VERTEX_SHADER);
	if (!success)
	{
		throw "Vertex Compilation Error";
	}
	//load and compile fragment shader
	success = fragment.readAndCompile(fName, GL_FRAGMENT_SHADER);
	if (!success)
	{
		throw "Fragment Compilation Error";
	}
	//create programObject
	programObject = glCreateProgram();
	//attach shaders
	glAttachShader(programObject, vertex.shaderObject);
	glAttachShader(programObject, fragment.shaderObject);

	glBindFragDataLocation(programObject, GL_NONE,"fragColor");
	//link shaders in program
	glLinkProgram(programObject);
	glGetProgramiv (programObject, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLint maxLength = 0;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &maxLength);

		char *errorLog = new char[maxLength];
		glGetProgramInfoLog(programObject, maxLength, &maxLength, &errorLog[0]);
		cout << errorLog << endl;
		delete[] errorLog;
		
		glDetachShader(programObject, vertex.shaderObject);
		glDetachShader(programObject, fragment.shaderObject);
		vertex.Release();
		fragment.Release();
		
		throw "Link Error";
	}
    
    //binding related external stuff uniform locations:
    loc_ModelViewMatrix =
        glGetUniformLocation(programObject, "modelViewMatrix");
    loc_NormalMatrix =
        glGetUniformLocation(programObject, "normalMatrix");
    loc_ModelViewProjectionMatrix =
        glGetUniformLocation(programObject, "modelViewProjectionMatrix");
    loc_TextureSampler =
        glGetUniformLocation(programObject, "textureSampler");
    loc_UseTextureFlag =
        glGetUniformLocation(programObject, "useTexture");
    loc_lightPos = 
        glGetUniformLocation(programObject, "lightPos");
    loc_lightPower_ambient =
        glGetUniformLocation(programObject, "lightPower_ambient");
    loc_lightPower_diffuse =
        glGetUniformLocation(programObject, "lightPower_diffuse");
    loc_lightPower_specular =
        glGetUniformLocation(programObject, "lightPower_specular");
    loc_k_ambient =
        glGetUniformLocation(programObject, "k_ambient");
    loc_k_diffuse =
        glGetUniformLocation(programObject, "k_diffuse");
    loc_k_specular =
        glGetUniformLocation(programObject, "k_specular");
    loc_shininess =
        glGetUniformLocation(programObject, "shininess");
    //if there is some problem
    if (loc_ModelViewMatrix < 0 || loc_NormalMatrix < 0
        || loc_ModelViewProjectionMatrix < 0 || loc_TextureSampler < 0
        || loc_UseTextureFlag < 0 || loc_lightPos < 0
        || loc_lightPower_ambient < 0 || loc_lightPower_diffuse < 0
        || loc_lightPower_specular < 0 || loc_k_ambient < 0 || loc_k_diffuse < 0
        || loc_k_specular < 0 || loc_shininess < 0)
    {
        //not all uniforms were allocated - show blue screen.
        glClearColor(0,0,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        //end frame visualization
        glutSwapBuffers();
        return;
    }
}
