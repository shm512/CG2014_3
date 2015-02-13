#pragma once

#include "definitions.h"
#include "MyObject.h"

//branch of tree is flat object, symmetric around y axes
//example equation: y=+-sqrt(1 - x*x/100) (also is symmetric around x)
class Leaf : public DerivedMyObjectTemplate<Leaf>
{   
public:
	Leaf(
        glm::vec3 _start_coord,
        float _inclination_angle,
        float _rotation_angle,
        float _scale_size,
        MyObject *_parent
    );
	virtual ~Leaf(void);
	virtual void initData();

private:
	float equation(float x);
};

