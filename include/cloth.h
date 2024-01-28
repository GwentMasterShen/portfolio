#ifndef _CLOTH_
#define _CLOTH_

#include "defines.h"
#include "mass.h"
#include "spring.h"
#include "accel.h"


//single mass-spring system
class Cloth
{
public:
	std::vector<Mass *> particles;  //所有质点
	std::vector<Spring *> springs;  //所有弹簧
	vec2 size;
	vec3 color = { 0.8,0.5,0.5 };
	std::vector<vec3> indices;

public:
	Cloth(vec3 upper_left, vec3 upper_right, vec3 lower_left, vec2 num_nodes, float node_mass, float k, std::vector<int> pinned_nodes);
	void verlet(float delta, vec3 gravity, KdTree* tree);
};


#endif
