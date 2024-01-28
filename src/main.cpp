#include <iostream>
#include <vector>
#include <utils.h>
#include <time.h>

#include "scene.h"

const int W = 800;
const int H = 800;

const float m = 0.1;

vec3 cameraPos = { 0,0,1 };
vec3 cameraUp = { 0,1,0 };
vec3 cameraLook = { 0,0,0 };

vec3 lightpos = { 1,0,1 };
vec3 lightcolor = { 1,1,1 };

GLFWwindow* window;

vec3 horizontalRotate3Degree(vec3 a, vec3 b, int j)
{
    float cos3 = 0.998629;
    float sin3 = 0.052336;
    vec3 result;
    result[0] = b[0] + (a[0] - b[0]) * cos3 - j * (a[2] - b[2]) * sin3;
    result[2] = b[2] + (a[2] - b[2]) * cos3 + j * (a[0] - b[0]) * sin3;
    result[1] = a[1];
    return result;
}

void processInput(GLFWwindow* window)
{
    float cameraSpeed = 0.005f; /* adjust accordingly */
    vec3 wcameraFront = { 0,0,1 };
    vec3 scameraFront = { 0,0,-1 };
    vec3 dcameraFront = { 1,0,0 };
    vec3 acameraFront = { -1,0,0 };
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * wcameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos += cameraSpeed * scameraFront;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos = horizontalRotate3Degree(cameraPos, cameraLook, 1);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos = horizontalRotate3Degree(cameraPos, cameraLook, -1);
}

vec3 midarc(vec3 a, vec3 b, float r)
{
    vec3 c = { a.x + b.x,a.y + b.y,a.z + b.z };
    float mod = r / sqrt(c.x * c.x + c.y * c.y + c.z * c.z);
    return c * mod;
}

std::vector<std::shared_ptr<Triangle>> build_sphere(vec3 center, float r, float resolution)
{
    std::vector<std::shared_ptr<Triangle>> sphere;
    vec3 v0 = { r,0,0 };   v0 += center;
    vec3 v1 = { 0,r,0 };   v1 += center;
    vec3 v2 = { 0,0,r };   v2 += center;
    vec3 v3 = { -r,0,0 };  v3 += center;
    vec3 v4 = { 0,-r,0 };  v4 += center;
    vec3 v5 = { 0,0,-r };  v5 += center;
    sphere.push_back(std::make_shared<Triangle>(v0, v1, v2));
    sphere.push_back(std::make_shared<Triangle>(v0, v1, v5));
    sphere.push_back(std::make_shared<Triangle>(v0, v4, v2));
    sphere.push_back(std::make_shared<Triangle>(v0, v4, v5));
    sphere.push_back(std::make_shared<Triangle>(v3, v1, v2));
    sphere.push_back(std::make_shared<Triangle>(v3, v1, v5));
    sphere.push_back(std::make_shared<Triangle>(v3, v4, v2));
    sphere.push_back(std::make_shared<Triangle>(v3, v4, v5));
    while (1)
    {
        std::shared_ptr<Triangle> temp = sphere[0];
        if (distance(temp->v0, temp->v1) < resolution && distance(temp->v1, temp->v2) < resolution && distance(temp->v0, temp->v2) < resolution) break;
        vec3 m0 = midarc(temp->v1, temp->v2, r);
        vec3 m1 = midarc(temp->v0, temp->v2, r);
        vec3 m2 = midarc(temp->v1, temp->v0, r);
        sphere.push_back(std::make_shared<Triangle>(temp->v0, m1, m2));
        sphere.push_back(std::make_shared<Triangle>(temp->v1, m0, m2));
        sphere.push_back(std::make_shared<Triangle>(temp->v2, m0, m1));
        sphere.push_back(std::make_shared<Triangle>(m0, m1, m2));
        sphere.erase(sphere.begin());
    }
    for (auto& triangle : sphere)
    {
        vec3 norm = normalize(cross(triangle->v1 - triangle->v0, triangle->v2 - triangle->v0));
        if (dot(norm, triangle->v0 - center) > 0) triangle->normal = norm;
        else triangle->normal = -norm;
    }
    return sphere;
}

int main()
{
	WindowGuard windowGuard(window, W, H, "Shiyuan Shen Portfolio");
    
    Cloth cloth({ -0.6,0.6,0 }, { 0.6,0.6,0 }, { -0.6,0.6,-1 }, { 10,10 }, m, 100, { 0,9 });
    Scene scene(cloth, build_sphere({0,0,0},0.5,0.1),lightpos,lightcolor);
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window);
        glLoadIdentity();
        gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2], cameraLook[0], cameraLook[1],
            cameraLook[2], cameraUp[0], cameraUp[1], cameraUp[2]);
        //cloth.Update();
        scene.update();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    return 0;
}

