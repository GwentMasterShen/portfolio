#ifndef _SCENE_
#define _SCENE_

#include "cloth.h"
#include "accel.h"


class Scene
{
public:
	Scene(Cloth cloth, std::vector<std::shared_ptr<Triangle>>& triangles, vec3 light_position, vec3 light_color);
	void render();
	void update();


	Cloth* cloth;
	KdTree* poly;
	std::vector<std::shared_ptr<Triangle>> triangles;
	vec3 light_position;
	vec3 light_color;
	vec3 poly_color = { 0.8,0.4,0.4 };
};

#endif // !_SCENE_

