#include "accel.h"

using namespace std;

const float INF = std::numeric_limits<float>::infinity();

bool AABB::intersect(vec3 start, vec3 end)
{
    vec3 dir = end - start;
    float dirX = (dir[0] == 0) ? INF : float(1) / dir[0];
    float dirY = (dir[1] == 0) ? INF : float(1) / dir[1];
    float dirZ = (dir[2] == 0) ? INF : float(1) / dir[2];

    float tx1 = (lower_bound[0] - start[0]) * dirX;
    float tx2 = (upper_bound[0] - start[0]) * dirX;
    float ty1 = (lower_bound[1] - start[1]) * dirY;
    float ty2 = (upper_bound[1] - start[1]) * dirY;
    float tz1 = (lower_bound[2] - start[2]) * dirZ;
    float tz2 = (upper_bound[2] - start[2]) * dirZ;

    if (dir[0] == 0)
    {
        if (lower_bound[0] - start[0] == 0 || upper_bound[0] - start[0] == 0)
        {
            tx1 = 0;
            tx2 = INF;
        }
    }
    if (dir[1] == 0)
    {
        if (lower_bound[1] - start[1] == 0 || upper_bound[1] - start[1] == 0)
        {
            ty1 = 0;
            ty2 = INF;
        }
    }
    if (dir[2] == 0)
    {
        if (lower_bound[2] - start[2] == 0 || upper_bound[2] - start[2] == 0)
        {
            tz1 = 0;
            tz2 = INF;
        }
    }

    float tIn = max(max(min(tx1, tx2), min(ty1, ty2)),min(tz1, tz2));
    float tOut = min(min(max(tx1, tx2), max(ty1, ty2)),max(tz1, tz2));

    return tOut >= tIn && tOut <= 1 && tOut >= 0;
}

void AABB::merge(vec3 a)
{
    lower_bound = { min(lower_bound.x, a.x), min(lower_bound.y, a.y),min(lower_bound.z, a.z) };
    upper_bound = { max(upper_bound.x, a.x), max(upper_bound.y, a.y),max(upper_bound.z, a.z) };
}

Triangle::Triangle(vec3 p0, vec3 p1, vec3 p2)
{
    this->v0 = p0;
    this->v1 = p1;
    this->v2 = p2;
    this->normal = { 0,0,0 };
}

Triangle::Triangle(vec3 p0, vec3 p1, vec3 p2, vec3 normal)
{
    this->v0 = p0;
    this->v1 = p1;
    this->v2 = p2;
    this->normal = normal;
}

bool Triangle::intersect(vec3 start, vec3 end, vec3& intersection)
{
    bool isIn = false;
    vec3 dir = end - start;

    vec3 E1 = v1 - v0;
    vec3 E2 = v2 - v0;
    vec3 S = start - v0;
    vec3 S1 = cross(dir, E2);
    vec3 S2 = cross(S, E1);
    float coeff = 1.0 / dot(S1, E1);
    float t = coeff * dot(S2, E2);
    float b1 = coeff * dot(S1, S);
    float b2 = coeff * dot(S2, dir);
    if (t >= 0 && t <= 1 && b1 >= 0 && b2 >= 0 && (1 - b1 - b2) >= 0)
    {
        isIn = true;
        intersection = start + t * dir;
    }
    return isIn;
}

bool TriangleIntersectsAABB(std::shared_ptr<Triangle> triangle, AABB box)
{
    vec3 center = (box.lower_bound + box.upper_bound) * 0.5f;
    vec3 extents = box.upper_bound - center;

    vec3 v0 = triangle->v0 - center;
    vec3 v1 = triangle->v1 - center;
    vec3 v2 = triangle->v2 - center;

    vec3 f0 = v1 - v0; // B - A
    vec3 f1 = v2 - v1; // C - B
    vec3 f2 = v0 - v2; // A - C

    vec3 u0(1.0f, 0.0f, 0.0f);
    vec3 u1(0.0f, 1.0f, 0.0f);
    vec3 u2(0.0f, 0.0f, 1.0f);

    vector<vec3> axis;
    axis.push_back(normalize(cross( u0, f0)));
    axis.push_back(normalize(cross( u0, f1)));
    axis.push_back(normalize(cross( u0, f2)));
    axis.push_back(normalize(cross( u1, f0)));
    axis.push_back(normalize(cross( u1, f1)));
    axis.push_back(normalize(cross( u1, f2)));
    axis.push_back(normalize(cross( u2, f0)));
    axis.push_back(normalize(cross( u2, f1)));
    axis.push_back(normalize(cross( u2, f2)));

    axis.push_back({1, 0, 0});
    axis.push_back({0, 1, 0});
    axis.push_back({0, 0, 1});

    axis.push_back(normalize(cross(f0, f1)));

    for (int i = 0; i < 13; i++)
    {
        float p0 = dot(v0, axis[i]);
        float p1 = dot(v1, axis[i]);
        float p2 = dot(v2, axis[i]);

        float r = extents.x * abs(dot(u0, axis[i])) +
            extents.y * abs(dot(u1, axis[i])) +
            extents.z * abs(dot(u2, axis[i]));

        if (max(-max(max(p0, p1), p2), min(min(p0, p1), p2)) > r) 
        {
            return false;
        }
    }
    return true;
}

KdTreeNode::KdTreeNode(std::vector<std::shared_ptr<Triangle>> triangles, AABB box, int depth)
{
    this->box = box;

    if (triangles.size() < 8 || depth > 40)
    {
        this->triangles = triangles;
        left_child = nullptr;
        right_child = nullptr;
        return;
    }

    int a = 0;
    a = depth % 3;
    vec3 newu = box.upper_bound;
    vec3 newl = box.lower_bound;
    newu[a] = (box.upper_bound[a] + box.lower_bound[a]) / 2;
    newl[a] = newu[a];
    AABB left_space;
    AABB right_space;
    left_space.lower_bound = box.lower_bound;
    left_space.upper_bound = newu;
    right_space.lower_bound = newl;
    right_space.upper_bound = box.upper_bound;

    std::vector<std::shared_ptr<Triangle>> left_triangles;
    std::vector<std::shared_ptr<Triangle>> right_triangles;
    for (int i = 0; i < triangles.size(); i++)
    {
        if (TriangleIntersectsAABB(triangles[i], left_space)) left_triangles.push_back(triangles[i]);
        if (TriangleIntersectsAABB(triangles[i], right_space)) right_triangles.push_back(triangles[i]);
    }
    left_child = new KdTreeNode(left_triangles, left_space, depth + 1);
    right_child = new KdTreeNode(right_triangles, right_space, depth + 1);
    return;
}

bool KdTreeNode::intersect(vec3 start, vec3 end, vec3& interscetion)
{
    if (!box.intersect(start, end)) return false;
    if (isLeaf())
    {
        bool is_intersected = false;
        for (int i = 0; i < triangles.size(); i++)
        {
            if (triangles[i]->intersect(start, end, interscetion))
            {
                is_intersected = true;
            }
        }
        return is_intersected;
    }
    bool a = left_child->intersect(start, end, interscetion);
    bool b = right_child->intersect(start, end, interscetion);
    return a || b;
}

KdTree::KdTree(std::vector<std::shared_ptr<Triangle>>& triangles)
{
    AABB box;
    box.lower_bound = { 0,0,0 };
    box.upper_bound = { 0,0,0 };
    for (auto& triangle : triangles)
    {
        box.merge(triangle->v0);
        box.merge(triangle->v1);
        box.merge(triangle->v2);
    }
    root = new KdTreeNode(triangles, box, 0);
}

bool KdTree::intersect(vec3 start, vec3 end, vec3& interscetion)
{
    if (root) return root->intersect(start, end, interscetion);
    return false;
}
