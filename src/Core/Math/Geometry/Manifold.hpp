#ifndef MANIFOLD_HPP_INCLUDED
#define MANIFOLD_HPP_INCLUDED

#include "../Vector/Vector.h"
struct Manifold
{
	Vect3f32 normal = Vect3f32(0.0f, 0.0f, 0.0f);
	Vect3f32 contact = Vect3f32(0.0f, 0.0f, 0.0f);
	float penetration = 0.0f;
};

#endif // !MANIFOLD_HPP_INCLUDED