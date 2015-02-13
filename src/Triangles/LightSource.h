#pragma once

#include "definitions.h"


struct LightSource
{
    glm::vec3 pos;          //position
    float ambient;          //ambient intensity
    float diffuse;          //diffuse intensity
    float specular;         //specular intensity
};
