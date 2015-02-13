#pragma once

#include "definitions.h"
#include "MyObject.h"

//branch of tree is cylinder
class Branch : public DerivedMyObjectTemplate<Branch>
{
    const int children_c;
    const int children_c_diff; //every level children_c is reduced on this value
    
    bool end_too_low(
        glm::vec3 new_start_point,
        float new_inclination,
        float new_rotation,
        float new_height,
        float new_width,
        glm::vec3 new_end_point
    );

public:
	Branch(
        uint _levels_remain,
        glm::vec3 _start_coord,
        float _inclination_angle,
        float _rotation_angle,
        float _height,
        float _width,
        int _children_count,
        int _chilren_count_diff,
        MyObject *_parent = nullptr
    );
    
	virtual ~Branch(void);
    
	//redefinitions: 
	virtual void initData();
    virtual void make_children();
};

