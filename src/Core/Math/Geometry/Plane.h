#ifndef PLANE_H_DEFINED
#define PLANE_H_DEFINED

#include "../Vector/Vector.h"

class AABB;
class Sphere;
class OBB;
class Ray;

class Plane
{
public:
    float distance;
    Vect3f32 normal;

    Plane();
    Plane(Vect3f32 const& _normal, float _dist);
    Plane(Vect3f32 const& _normal, Vect3f32 const& _pos);
    Plane(Vect3f32 const& _p1, Vect3f32 const& _p2, Vect3f32 const& _p3);

    int ClassifyPoint(Vect3f32 const& _p)       const;
    float DistanceToPoint(Vect3f32 const& _p)   const;
    
    bool Contains(Vect3f32 const& _pos)     const;
    bool Intersects(Plane const& _o)        const;
    bool Intersects(Ray const& _r)          const;
    bool Intersects(AABB const& _a)         const;
    bool Intersects(Sphere const& _s)       const;
    bool Intersects(OBB const& _o)          const;
    
    static bool ThreeWayIntersect(Plane const& _o1, Plane const& _o2, Plane const& _o3, Vect3f32* _p);
};

#endif
