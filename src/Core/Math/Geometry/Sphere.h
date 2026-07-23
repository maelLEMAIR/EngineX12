#ifndef SPHERE_H_DEFINED
#define SPHERE_H_DEFINED

#include "../Vector/Vector.h"
#include "../Matrix/Matrix.h"

class Ray;
class Plane;
class AABB;
class OBB;

class Sphere
{
public:
    Vect3f32 center;
    float radius;
    
    Sphere();
    Sphere(Vect3f32 const& _center, float _radius);
    
    void Merge(Sphere const& _sphere);
    void Union(Sphere const& _sphere);
    void Expand(float _scalar);
    
    void   Transform(Mat4f32 const& _t);
    Sphere Transformed(Mat4f32 const& _t);
    
    bool Contains(Vect3f32 const& _p)               const;
    bool Contains(Sphere const& _o)                 const;
    bool Intersects(Ray const& _r, Vect3f32* _p)    const;
    bool Intersects(Plane const& _plane)            const;
    bool Intersects(AABB const& _a)                 const;
    bool Intersects(Sphere const& _o)               const;
    bool Intersects(OBB const& _o)                  const;
    
    static Sphere Merge(Sphere const& _sphere, Sphere const& _o);
    static Sphere Union(Sphere const& _sphere, Sphere const& _o);
    static Sphere Expand(Sphere const& _sphere, float _scalar);
    static Sphere Transform(Sphere const& _sphere, Mat4f32 const& _t);
};

#endif
