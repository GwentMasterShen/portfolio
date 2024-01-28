
#include <scene.h>

const float steps_per_frame = 50;

std::vector<float> phong(vec3 pos1, vec3 pos2, vec3 pos3,vec3 normal, vec3 light_pos, vec3 light_color, vec3 cloth_color)
{
	float amS = 0.01;
	vec3 am = amS * light_color;
	vec3 view_pos = { 0,0,1 };
	//vec3 normal = normalize(cross(pos3 - pos1, pos2 - pos1));
	vec3 LD1 = normalize(light_pos - pos1);
	vec3 LD2 = normalize(light_pos - pos2);
	vec3 LD3 = normalize(light_pos - pos3);
	float proj1 = std::max(dot(normal, LD1), 0.f);
	float proj2 = std::max(dot(normal, LD2), 0.f);
	float proj3 = std::max(dot(normal, LD3), 0.f);
	vec3 diff1 = proj1 * light_color;
	vec3 diff2 = proj2 * light_color;
	vec3 diff3 = proj3 * light_color;
	float mirror = 1;
	vec3 VD1 = normalize(view_pos - pos1);
	vec3 VD2 = normalize(view_pos - pos2);
	vec3 VD3 = normalize(view_pos - pos3);
	vec3 RD1 = reflect(-LD1, normal);
	vec3 RD2 = reflect(-LD2, normal);
	vec3 RD3 = reflect(-LD3, normal);
	float power1 = pow(std::max(dot(VD1, RD1), 0.f), 10);
	float power2 = pow(std::max(dot(VD2, RD2), 0.f), 10);
	float power3 = pow(std::max(dot(VD3, RD3), 0.f), 10);
	vec3 mir1 = mirror * power1 * light_color;
	vec3 mir2 = mirror * power2 * light_color;
	vec3 mir3 = mirror * power3 * light_color;
	vec3 fin1 = (am + diff1 + mir1) * cloth_color;
	vec3 fin2 = (am + diff2 + mir2) * cloth_color;
	vec3 fin3 = (am + diff3 + mir3) * cloth_color;
	return { fin1.x,fin1.y,fin1.z,fin2.x,fin2.y,fin2.z,fin3.x,fin3.y,fin3.z };
}

Scene::Scene(Cloth cloth, std::vector<std::shared_ptr<Triangle>>& triangles, vec3 light_position, vec3 light_color)
{
	this->cloth = new Cloth(cloth);
	this->light_position = light_color;
	this->light_color = light_color;
	this->triangles = triangles;
	this->poly = new KdTree(triangles);
}

void Scene::render()
{
	glBegin(GL_TRIANGLES);
	for (auto& triangle : triangles)
	{
		vec3 v0 = triangle->v0;
		vec3 v1 = triangle->v1;
		vec3 v2 = triangle->v2;
		//std::cout << triangle->normal.x;
		std::vector<float> color = phong(v0, v1, v2, triangle->normal, light_position, light_color, poly_color);
		glColor3f(color[0], color[1], color[2]);
		glVertex3f(v0.x, v0.y, v0.z);
		glColor3f(color[3], color[4], color[5]);
		glVertex3f(v1.x, v1.y, v1.z);
		glColor3f(color[6], color[7], color[8]);
		glVertex3f(v2.x, v2.y, v2.z);
	}
	glEnd();
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < cloth->size.x - 1; i++)
	{
		for (int j = 0; j < cloth->size.y - 1; j++)
		{
			vec3 pos0 = cloth->particles[i * cloth->size.x + j]->position;
			vec3 pos1 = cloth->particles[i * cloth->size.x + j + 1]->position;
			vec3 pos2 = cloth->particles[(i + 1) * cloth->size.x + j]->position;
			vec3 pos3 = cloth->particles[(i + 1) * cloth->size.x + j + 1]->position;
			vec3 normal = normalize(cross(pos2 - pos0, pos1 - pos0));
			std::vector<float> color = phong(pos0, pos1, pos2, normal, light_position, light_color, cloth->color);
			glColor3f(color[0], color[1], color[2]);
			glVertex3f(pos0.x, pos0.y, pos0.z);
			glColor3f(color[3], color[4], color[5]);
			glVertex3f(pos1.x, pos1.y, pos1.z);
			glColor3f(color[6], color[7], color[8]);
			glVertex3f(pos2.x, pos2.y, pos2.z);
			normal = normalize(cross(pos1 - pos3, pos2 - pos3));
			color = phong(pos1, pos2, pos3, normal, light_position, light_color, cloth->color);
			glColor3f(color[0], color[1], color[2]);
			glVertex3f(pos1.x, pos1.y, pos1.z);
			glColor3f(color[3], color[4], color[5]);
			glVertex3f(pos2.x, pos2.y, pos2.z);
			glColor3f(color[6], color[7], color[8]);
			glVertex3f(pos3.x, pos3.y, pos3.z);
		}
	}
	glEnd();
}

void Scene::update()
{
	for (int i = 0; i < steps_per_frame; i++)
	{
		cloth->verlet(1 / steps_per_frame, { 0,-1,0 }, poly);
	}
	render();
}
