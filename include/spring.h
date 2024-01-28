#ifndef _SPRING_
#define _SPRING_

#include <defines.h>
#include <mass.h>

struct Spring {
    Spring(Mass* a, Mass* b, float k)
        : m1(a), m2(b), k(k), rest_length(distance(a->position, b->position)) {}

    float k;
    float rest_length;

    Mass* m1;
    Mass* m2;
}; // struct Spring



#endif
