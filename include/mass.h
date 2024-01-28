#ifndef _MASS_
#define _MASS_

#include <defines.h>
#include <vector>

struct Mass {
    Mass(vec3 position, float mass, bool pinned)
        : start_position(position), position(position), last_position(position),
        mass(mass), pinned(pinned) {}

    float mass;
    bool pinned;

    vec3 start_position;
    vec3 position;
    vec3 last_position;
    vec3 forces;
};





#endif
