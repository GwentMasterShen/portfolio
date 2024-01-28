#ifndef _ACCEL_
#define _ACCEL_

#include <defines.h>

struct AABB
{
	vec3 lower_bound;
	vec3 upper_bound;
	bool intersect(vec3 start, vec3 end);
	void merge(vec3 a);
};

struct Triangle
{
	vec3 v0;
	vec3 v1;
	vec3 v2;
	vec3 normal;
	Triangle(vec3 p0, vec3 p1, vec3 p2);
	Triangle(vec3 p0, vec3 p1, vec3 p2, vec3 normal);
	bool intersect(vec3 start, vec3 end, vec3& interscetion);
};

class KdTreeNode
{
public:
	KdTreeNode(std::vector<std::shared_ptr<Triangle>> triangles, AABB box, int depth);
	bool intersect(vec3 start, vec3 end, vec3& interscetion);
	bool isLeaf() { return !left_child && !right_child; };
private:
	KdTreeNode* left_child;
	KdTreeNode* right_child;
	AABB box;
	std::vector<std::shared_ptr<Triangle>> triangles;
};

class KdTree
{
public:
	KdTree(std::vector<std::shared_ptr<Triangle>>& triangles);
	bool intersect(vec3 start, vec3 end, vec3& interscetion);
private:
	KdTreeNode* root;
};

#endif
