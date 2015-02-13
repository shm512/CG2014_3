#include "Branch.h"
#include "Leaf.h"
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <cstdlib>
#include <climits>
#include <algorithm>


Branch::Branch(
    uint _levels_remain,
    glm::vec3 _start_coord,
    float _inclination_angle,
    float _rotation_angle,
    float _height,
    float _width,
    int _children_count,
    int _children_count_diff,
    MyObject *_parent
)
    : DerivedMyObjectTemplate<Branch>(
        _levels_remain,
        _start_coord,
        _inclination_angle,
        _rotation_angle,
        _height,
        _width,
        0.3f,           //light_koef_ambient
        0.4f,           //light_koef_diffuse
        0.4f,           //light_koef_specular
        1.0f,           //light_koef_shininess
        _parent
    ),
    children_c(_children_count),
    children_c_diff(_children_count_diff)
{
    if (!pData)
	{
		initData();
	}
    //initialize opengl buffers and variables inside of object:
    extern ShaderProgram shaderProgram;
    initGLBuffers(shaderProgram.programObject, "pos", "nor", "tex");
}


Branch::~Branch(void)
{
}

void
Branch::initData()
{
    if (pData != nullptr)
    {
        return;
    }

    uint radialStep = 10;
    uint heightStep = 10;
    float baseRadius = 1.0f;
    float topRadius = 0.12f;
    float cylHeight = 1.0f;
    float angleNorm = atan2(baseRadius - topRadius, cylHeight);
    float xzNorm = cos(angleNorm);
    float yNorm = sin(angleNorm);

    //number of points
    dataCount = (radialStep + 1) * heightStep + 2; 
    //number of triangles
    uint nTriangles = 
        2 * radialStep * (heightStep - 1) + 2 * radialStep;
    //number of indices
    indicesCount = 3 * nTriangles;

    pData = new VertexData[dataCount];
    pIndices = new uint[indicesCount];
    
    ///fill in pData array

    //generate elements on side
    for (uint j = 0; j < heightStep; j++)
    {
        float zPos = cylHeight * j / (heightStep  -  1);
        float curRadius = 
            baseRadius - (baseRadius - topRadius) * j / heightStep;
        for (uint i = 0; i < radialStep + 1; ++i)
        {
            uint pointId = j * (radialStep + 1) + i;

            float phi = 2 * M_PI * i / radialStep; //from 0 to 360 degrees
            float xPos = cos(phi);
            float yPos = sin(phi);
            
            pData[pointId].pos = 
                glm::vec3(curRadius * xPos, zPos, curRadius * yPos);
            pData[pointId].nor = glm::vec3(xPos * xzNorm, yNorm, yPos * xzNorm);
            pData[pointId].tex = 
                glm::vec2(phi / 2 / M_PI, zPos * 5);     
        }
    }
    //generate north pole
    {
        uint pointId = heightStep * (radialStep + 1);
        pData[pointId].pos = glm::vec3(0, cylHeight, 0);
        pData[pointId].nor = glm::vec3(0, 1, 0);
        pData[pointId].tex = glm::vec2(0.5f, 0.5f);     
    }
    //generate south pole
    {
        uint pointId = heightStep * (radialStep + 1) + 1;
        pData[pointId].pos = glm::vec3(0, 0 , 0);
        pData[pointId].nor = glm::vec3(0, -1, 0);
        pData[pointId].tex = glm::vec2(0.5f, 0.5f);     
    }
    
    ///fill in pIndices array

    //fill in side triangles (first 6 * radialStep * (heightStep - 1))
    for (uint j = 0; j < heightStep - 1; j++)
    {
        for (uint i = 0; i < radialStep; ++i)
        {
            uint pointId = j * (radialStep + 1) + i;
            uint indexId = j * radialStep + i;
            //pData configuration
            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
            // -  - .(i,j + 1) -  - .(i + 1,j + 1) -  - 
            // -  - .(i,    j) -  - .(i + 1,    j) -  - 
            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 

            //pData indices
            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
            // -  - pointId + radialStep + 1 -  - pointId + radialStep + 2 -  -  -  - 
            // -  - pointId -  -  -  -  -  -  -  -  -  -  -  -  -  -  - pointId + 1 -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
            
            //triangle 1            
            //   /|
            //  / |
            // /__|  
            pIndices[6 * indexId + 0] = pointId;
            pIndices[6 * indexId + 1] = pointId + 1;
            pIndices[6 * indexId + 2] = pointId + radialStep + 2;
            //triangle 2
            // ____
            // |  /
            // | /
            // |/  
            pIndices[6 * indexId + 3] = pointId;
            pIndices[6 * indexId + 4] = pointId + radialStep + 2;
            pIndices[6 * indexId + 5] = pointId + radialStep + 1;
        }
    }
    //fill in north pole triangles (next 3 * radialStep)
    {
        uint startIndex = 6 * radialStep * (heightStep - 1);
        uint northPoleId = heightStep * (radialStep + 1);
        for (uint i=0; i<radialStep; ++i)
        {
            //get last row
            uint pointId = (heightStep - 1) * (radialStep + 1) + i;
            pIndices[startIndex + 3 * i + 0] = pointId;
            pIndices[startIndex + 3 * i + 1] = pointId + 1;
            pIndices[startIndex + 3 * i + 2] = northPoleId;
        }
    }
    
    //fill in south pole triangles (last 3 * radialStep)
    {
        uint startIndex = 6 * radialStep * (heightStep - 1) + 3 * radialStep;
        uint southPoleId = heightStep * (radialStep + 1) + 1;

        for (uint i = 0; i < radialStep; ++i)
        {
            //get first row
            uint pointId = i;
            pIndices[startIndex + 3 * i + 0] = pointId;
            pIndices[startIndex + 3 * i + 1] = southPoleId;
            pIndices[startIndex + 3 * i + 2] = pointId + 1;
        }
    }
}

static inline float
frand_in(float a, float b)
{
    return a + (float(rand()) / float(RAND_MAX)) * (b - a); 
}

bool
Branch::end_too_low(
    glm::vec3 new_start_point,
    float new_inclination,
    float new_rotation,
    float new_height,
    float new_width,
    glm::vec3 new_end_point)
{
    float absolute_height_border = 0.5f;

    auto new_ModelMatrix =
        glm::translate(get_ModelMatrix(), height * new_start_point);
    static const glm::vec3 rotation_axis = glm::vec3(0, 1, 0);
    static const glm::vec3 inclination_axis = glm::vec3(0, 0, 1);
    new_ModelMatrix = glm::rotate(new_ModelMatrix, new_rotation, rotation_axis);
    new_ModelMatrix =
        glm::rotate(new_ModelMatrix, new_inclination, inclination_axis);
    new_ModelMatrix =
        glm::scale(new_ModelMatrix, glm::vec3(new_width, new_height, new_width));
    
    auto absolute_height_vec = new_ModelMatrix * glm::vec4(new_end_point, 1);
    return (absolute_height_vec[1] < absolute_height_border);
}

void
Branch::make_children()
{
    static const float x0 = 0.0f;
    static const float y0 = 0.0f;
    static const float z0 = 0.0f;
    static const float h = 1.0f;
    static float leaf_size;
    static auto end_point = glm::vec3(0, h, 0);
    
    if (!parent)
    {
        leaf_size = 0.8f * pow(0.3f, levels_remain - 1) * height;
    }
    //leaf to the end of the branch:
    auto pLeaf = new Leaf(
        glm::vec3(x0, h, z0),
        0.45f,                      //inclination angle
        frand_in(0.0f, 360.0f),     //rotation angle
        leaf_size,                  //size
        this                        //parent
    );
    children.push_back(std::unique_ptr<MyObject>(pLeaf));
    if (levels_remain > 1)
    {
        static const float part_len = h / float(children_c);
        static const float min_inclination = 40.0f;
        static const float max_inclination = 85.0f;
        static const int skip_parts_c_begin = 5;
        const int skip_parts_c_end = (parent) ? 0 : 1;
        
        for (int i = skip_parts_c_begin; i < children_c - skip_parts_c_end; ++i)
        {
            float y = y0 + frand_in(i * part_len, (i + 1) * part_len);
            float new_rotation = frand_in(0.0f, 360.0f);
            float new_inclination =
                frand_in(min_inclination, max_inclination);
            float new_height = 0.45f * height;
            float new_width = 0.25f * width;
            auto new_start = glm::vec3(x0, y, z0);

            if (end_too_low(new_start, new_inclination, new_rotation, new_height, new_width, end_point)
                || rand() % (5 + levels_remain) == 0)
            {
                //no branch 
                //if end is too low or with probability = 1/(5 + levels_remain)
                continue;
            }
            auto pBranch = new Branch(
                levels_remain - 1,
                new_start,                    //start coord
                new_inclination,              //inclination angle
                new_rotation,                 //rotation angle
                new_height,                   //heigth
                new_width,                    //width
                std::max(children_c - children_c_diff, 5), //children number
                children_c_diff,              
                this                          //parent
            );
            children.push_back(std::unique_ptr<MyObject>(pBranch));
            pBranch->make_children();
        }
    }
}
