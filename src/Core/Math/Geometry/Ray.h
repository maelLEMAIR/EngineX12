#ifndef RAY_H_DEFINED
#define RAY_H_DEFINED

#include "../Vector/Vector.h"

class Plane;
class AABB;
class OBB;
class Sphere;

class Ray
{
public:
    Vect3f32 origin;
    Vect3f32 direction;
    float length;

    Ray(Vect3f32 _origin, Vect3f32 _direction, float _length);
    Ray(Vect3f32 _origin, Vect3f32 _direction);

    bool Contains(Vect3f32 const& _p) const;
    
    bool Intersects(Ray const& _ray, Vect3f32* _p = nullptr)          const;
    bool Intersects(Plane const& _plane, Vect3f32* _p = nullptr)      const;
    bool Intersects(AABB const& _aabb, Vect3f32* _p = nullptr)        const;
    bool Intersects(Sphere const& _sphere, Vect3f32* _p = nullptr)    const;
    bool Intersects(OBB const& _obb, Vect3f32* _p = nullptr)          const;
};

#endif
