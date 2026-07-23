#ifndef AABB_H_DEFINED
#define AABB_H_DEFINED

#include "../Vector/Vector.h"
#include "../Matrix/Matrix.h"

class Ray;
class Plane;
class Sphere;
class OBB;

class AABB
{
public:
    Vect3f32 min;
    Vect3f32 max;
    
    AABB();
    AABB(Vect3f32 const& _min, Vect3f32 const& _max);
    
    static AABB FromMinMax(Vect3f32 const& _min, Vect3f32 const& _max);
    static AABB FromCenterExtent(Vect3f32 const& _center, Vect3f32 const& _extent);
    
    Vect3f32 Center() const;
    Vect3f32 Extent() const;
    float Volume() const;
    
    void SetCenter(Vect3f32 const& _center);
    void SetExtent(Vect3f32 const& _extent);
    
    void Merge(AABB const& _o);
    void Union(AABB const& _o);
    void Expand(float _scalar);
    
    void Transform(Mat4f32 const& _t);
    AABB Transformed(Mat4f32 const& _t) const;
    
    bool Contains(Vect3f32 const& _p)               const;
    bool Contains(AABB const& _o)                   const;
    bool Intersects(Ray const& _r, Vect3f32* _p)    const;
    bool Intersects(Plane const& _plane)            const;
    bool Intersects(AABB const& _o)                 const;
    bool Intersects(Sphere const& _o)               const;
    bool Intersects(OBB const& _o)                  const;
    
    static AABB Merge(AABB const& _a, AABB const& _b);
    static AABB Union(AABB const& _a, AABB const& _b);
    static AABB Expand(AABB const& _o, float _scalar);
    static AABB Transform(AABB const& _o, Mat4f32 const& _t);
};

#endif
