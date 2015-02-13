//opengl headers
#include <GL/glew.h>
#include <GL/freeglut.h>

//opengl mathematics
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <cmath>
#include <ctime>


//initialization of static fields:
template<typename T>
VertexData *DerivedMyObjectTemplate<T>::pData = nullptr;
template<typename T>
uint DerivedMyObjectTemplate<T>::dataCount = 0;
template<typename T>
uint *DerivedMyObjectTemplate<T>::pIndices = nullptr;
template<typename T>
uint DerivedMyObjectTemplate<T>::indicesCount = 0;
template<typename T>
int DerivedMyObjectTemplate<T>::texture_num = MyObject::NO_TEXTURE;

template<typename T>
DerivedMyObjectTemplate<T>::DerivedMyObjectTemplate(
    uint _levels_remain,
    glm::vec3 _start_coord,
    float _inclination_angle,
    float _rotation_angle,
    float _height,
    float _width,
    float _light_koef_ambient,
    float _light_koef_diffuse,
    float _light_koef_specular,
    float _light_koef_shininess,
    MyObject *_parent
)
    : MyObject(
        _levels_remain,
        _start_coord,
        _inclination_angle,
        _rotation_angle,
        _height,
        _width,
        _light_koef_ambient,
        _light_koef_diffuse,
        _light_koef_specular,
        _light_koef_shininess,
        _parent
    )
{
}

template<typename T>
void
DerivedMyObjectTemplate<T>::initGLBuffers(
    GLuint programId,
    const char *posName,
    const char *norName,
    const char *texName)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(2, &vbo[0]);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER,
        dataCount * sizeof(VertexData),
        pData,
        GL_STATIC_DRAW
    );
		
	glEnable(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indicesCount * sizeof(uint),
        pIndices,
        GL_STATIC_DRAW
    );
	
	int	loc = glGetAttribLocation(programId, posName);
	if (loc > -1)
	{
		glVertexAttribPointer(
            loc,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VertexData),
            (GLuint *)nullptr
        );
		glEnableVertexAttribArray(loc);
	}
	int loc2 = glGetAttribLocation(programId, norName);
	if (loc2 > -1)
	{
		glVertexAttribPointer(
            loc2,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VertexData),
            (GLuint *)(0 + sizeof(float) * 3)
        );
		glEnableVertexAttribArray(loc2);
	}
	int loc3 = glGetAttribLocation(programId, texName);
	if (loc3 > -1)
	{
		glVertexAttribPointer(
            loc3,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VertexData),
            (GLuint *)(0 + sizeof(float) * 6)
        );
		glEnableVertexAttribArray(loc3);
	}
	glBindVertexArray(0);
	
}

template<typename T>
void
DerivedMyObjectTemplate<T>::draw()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

template<typename T>
void
DerivedMyObjectTemplate<T>::display(
    glm::mat4x4 modelMatrix,
    const glm::mat4x4 &viewMatrix,
    const glm::mat4x4 &projectionMatrix,
    GLuint *texture_ids)
{    
    //1. Translate object start to the point with start_coord
    if (parent)
    {
        modelMatrix =
            glm::translate(modelMatrix, parent->get_height() * start_coord);
    }
    
    //2. Rotate
    static const glm::vec3 rotation_axis = glm::vec3(0, 1, 0);
    static const glm::vec3 inclination_axis = glm::vec3(0, 0, 1);
    float amplitude = 0.009f / sqrt(width);
    float phase = time(NULL) + rand();
    rotation_angle += amplitude * std::cos(phase);
    inclination_angle += amplitude * std::sin(phase);
    modelMatrix = glm::rotate(modelMatrix, rotation_angle, rotation_axis);
    modelMatrix = glm::rotate(modelMatrix, inclination_angle, inclination_axis);
    
    //children should be scaled independently!
    for (auto &chld : children)
    {
        chld->display(modelMatrix, viewMatrix, projectionMatrix, texture_ids);
    }
    
    //3. Scale
    modelMatrix = glm::scale(modelMatrix, glm::vec3(width, height, width));    
        
    //modelViewMatrix consists of viewMatrix and modelMatrix
    auto modelViewMatrix = viewMatrix * modelMatrix;
    //calculate normal matrix 
    auto normalMatrix = glm::inverseTranspose(modelViewMatrix);
    //finally calculate modelViewProjectionMatrix
    auto modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
    //light position in view coordinates
    extern LightSource lightSource;
    auto lightViewPos = glm::vec3(viewMatrix * glm::vec4(lightSource.pos, 1));
    
    extern ShaderProgram shaderProgram;
    glUniformMatrix4fv(shaderProgram.loc_ModelViewMatrix,
                       1, 0,
                       glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(shaderProgram.loc_NormalMatrix,
                       1, 0,
                       glm::value_ptr(normalMatrix));
    glUniformMatrix4fv(shaderProgram.loc_ModelViewProjectionMatrix,
                       1, 0,
                       glm::value_ptr(modelViewProjectionMatrix));
    glUniform3fv(shaderProgram.loc_lightPos, 1, glm::value_ptr(lightViewPos));
    glUniform1f(shaderProgram.loc_lightPower_ambient, lightSource.ambient);
    glUniform1f(shaderProgram.loc_lightPower_diffuse,lightSource.diffuse);
    glUniform1f(shaderProgram.loc_lightPower_specular, lightSource.specular);
    glUniform1f(shaderProgram.loc_k_ambient, light_koef_ambient);
    glUniform1f(shaderProgram.loc_k_diffuse, light_koef_diffuse);
    glUniform1f(shaderProgram.loc_k_specular, light_koef_specular);
    glUniform1f(shaderProgram.loc_shininess, light_koef_shininess);
    
    extern bool useTexture;
    if (texture_num != NO_TEXTURE && useTexture)
    {
        //bind texture
        glBindTexture(GL_TEXTURE_2D, texture_ids[texture_num]);
        
        //pass variables to the shaders
        glUniform1i(shaderProgram.loc_TextureSampler, 0);
        glUniform1i(shaderProgram.loc_UseTextureFlag, useTexture);
    }
    
    //draw object
    draw();
}
