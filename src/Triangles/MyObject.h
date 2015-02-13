#pragma once

#include <vector>
#include <memory>

#include "definitions.h"
#include "shaderhelper.h"
#include "LightSource.h"

//helper struct for Vertex
//contains position, normal and texture coordinates
struct VertexData
{
	 glm::vec3 pos;
	 glm::vec3 nor;
	 glm::vec2 tex;
};


//some object for drawing
class MyObject
{
protected:	
	uint vbo[2];//VertexBufferObject: 1 for MeshVertexData, 2 for Indexes
	uint vao;//one VertexArrayObject
    
    uint levels_remain;     //how many levels of children remains
    std::vector<std::unique_ptr<MyObject>> children;
    
    glm::vec3 start_coord;      //coordinates of object's start point
    float inclination_angle;
    float rotation_angle;
    float height;               //scale by Oy
    float width;                //scale by Ox and Oz
    
    //to be redefined:
    const float light_koef_ambient;
    const float light_koef_diffuse;
    const float light_koef_specular;
    const float light_koef_shininess;
    
    MyObject *parent;
    
    glm::mat4x4 get_ModelMatrix();

public:
	static const int NO_TEXTURE = -1;
    
    MyObject();
    
    MyObject(
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
    );
    
	virtual ~MyObject(void);
    
    float get_height();
    
    //function for initialization
	virtual void initGLBuffers(
        uint programId,
        const char *posName,
        const char *norName,
        const char *texName
    ) = 0;
	//function for drawing
	virtual void draw() = 0;
    
    virtual void display(
        glm::mat4x4 modelMatrix,
        const glm::mat4x4 &viewMatrix,
        const glm::mat4x4 &projectionMatrix,
        GLuint *texture_ids
    ) = 0;

	//generates two triangles
	virtual void initData() = 0;
    virtual void make_children();
};


//This workaround is going to hold static variables that needs 
//to be the same for each derived class, but not all derived classes:
template<typename T>
class DerivedMyObjectTemplate : public MyObject
{
protected:
    static VertexData *pData;	//pointer to object's internal data
	static uint dataCount;

	static uint *pIndices;	//pointer to indexes (list of vetrices) 
	static uint indicesCount;
    
    DerivedMyObjectTemplate(    //wrap of MyObject constructor
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
        MyObject *_parent = nullptr
    );
    
public:
    static int texture_num;
    
    //function for initialization
	void initGLBuffers(
        uint programId,
        const char *posName,
        const char *norName,
        const char *texName
    );
	//function for drawing
	void draw();
    
    void display(
        glm::mat4x4 modelMatrix,
        const glm::mat4x4 &viewMatrix,
        const glm::mat4x4 &projectionMatrix,
        GLuint *texture_ids
    );
    
    virtual void initData() = 0;
};

//DerivedMyObjectTemplate implementation and static fields initialization:
#include "MyObject.hpp"
