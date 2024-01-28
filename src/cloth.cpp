
#include <cloth.h>

const float damping = 0.001;


using namespace std;


Cloth::Cloth(vec3 upper_left, vec3 upper_right, vec3 lower_left, vec2 num_nodes, float node_mass, float k, vector<int> pinned_nodes)
{
	size = num_nodes;
	vec3 step_x = (upper_right - upper_left) / (num_nodes.x - 1);
	vec3 step_y = (lower_left - upper_left) / (num_nodes.y - 1);
	for (float i = 0; i < num_nodes.y; i++)
	{
		for (float j = 0; j < num_nodes.x; j++)
		{
			Mass* mass = new Mass(upper_left + step_x * j + step_y * i, node_mass, false);
			mass->forces = { 0,0,0 };
			if (j > 0)
			{
				Spring* spring = new Spring(particles.back(), mass, k);
				springs.push_back(spring);
				if (j > 1)
				{
					Spring* spring = new Spring(particles[particles.size() - 2], mass, 0.1 * k);
					springs.push_back(spring);
				}
			}
			if (i > 0)
			{
				Spring* spring = new Spring(particles[particles.size() - num_nodes.x], mass, k);
				springs.push_back(spring);
				if (i > 2)
				{
					Spring* spring = new Spring(particles[particles.size() - 2 * num_nodes.x], mass, 0.1 * k);
					springs.push_back(spring);
				}
				if (j > 0)
				{
					Spring* spring = new Spring(particles[particles.size() - num_nodes.x - 1], mass, 0.1 * k);
					springs.push_back(spring);
				}
				if (j < num_nodes.x - 1)
				{
					Spring* spring = new Spring(particles[particles.size() - num_nodes.x + 1], mass, 0.1 * k);
					springs.push_back(spring);
				}
			}
			particles.push_back(mass);
		}
	}
	for (auto& i : pinned_nodes) particles[i]->pinned = true;
}


void Cloth::verlet(float delta, vec3 gravity, KdTree* tree)
{
	for (auto& s : springs)
	{
		vec3 dir = (s->m2)->position - (s->m1)->position;
		vec3 force = s->k * normalize(dir) * (length(dir) - s->rest_length);
		s->m1->forces += force;
		s->m2->forces -= force;
	}
	for (auto& m : particles)
	{
		if (!m->pinned)
		{
			vec3 a = m->forces / m->mass + gravity;
			vec3 temp_last_position = m->position;
			m->position += (1 - damping) * (m->position - m->last_position + a * delta * delta);
			m->last_position = temp_last_position;
			vec3 adjust;
			if (tree->intersect(m->last_position, m->position, adjust))
			{
				m->position = adjust;
			}
		}
		m->forces = { 0,0,0 };
	}
}



