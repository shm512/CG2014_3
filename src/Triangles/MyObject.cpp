#include "MyObject.h"

MyObject::MyObject(
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
) :
    levels_remain(_levels_remain),
    start_coord(_start_coord),
    inclination_angle(_inclination_angle),
    rotation_angle(_rotation_angle),
    height(_height),
    width(_width),
    light_koef_ambient(_light_koef_ambient),
    light_koef_diffuse(_light_koef_diffuse),
    light_koef_specular(_light_koef_specular),
    light_koef_shininess(_light_koef_shininess),
    parent(_parent)
{
}

MyObject::~MyObject(void)
{
	glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, &vao);
}

void
MyObject::make_children()
{
}

float
MyObject::get_height()
{
    return height;
}

glm::mat4x4
MyObject::get_ModelMatrix()
{
    auto modelMatrix = glm::mat4x4();
    if (parent)
    {
        modelMatrix = parent->get_ModelMatrix();
        //1. Translate object start to the point with start_coord
        modelMatrix =
            glm::translate(modelMatrix, parent->get_height() * start_coord);
    }
    
    //2. Rotate
    static const glm::vec3 rotation_axis = glm::vec3(0, 1, 0);
    static const glm::vec3 inclination_axis = glm::vec3(0, 0, 1);
    modelMatrix = glm::rotate(modelMatrix, rotation_angle, rotation_axis);
    modelMatrix = glm::rotate(modelMatrix, inclination_angle, inclination_axis);
    
    return modelMatrix;
}
