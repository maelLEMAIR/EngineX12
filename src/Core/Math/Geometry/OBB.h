#ifndef OBB_H_DEFINED
#define OBB_H_DEFINED

#include "../Vector/Vector.h"
#include "../Matrix/Matrix.h"
#include "Manifold.hpp"

class Ray;
class Plane;
class AABB;
class Sphere;

class OBB
{
public:
    Vect3f32 position;
    Vect3f32 extent;
    
    Mat3f32 orientation;
    
    OBB();
    OBB(Vect3f32 const& _pos, Vect3f32 const& _extent);
    OBB(Vect3f32 const& _pos, Vect3f32 const& _extent, Mat3f32 const& _orientation);

    Plane       GetPlane(int _index)                            const;
    Vect3f32    GetVertex(int _planeIndex, int _vertexIndex)    const;
    
    void Expand(float _scalar);
    
    void Transform(Mat4f32 const& _t);
    OBB Transformed(Mat4f32 const& _t) const;
    
    bool Contains(Vect3f32 const& _pos)             const;
    bool Intersects(Ray const& _r, Vect3f32* _p)                    const;
    bool Intersects(Plane const& _plane)                            const;
    bool Intersects(AABB const& _a, Manifold* _manifold = nullptr)  const;
    bool Intersects(Sphere const& _s, Manifold* _manifold = nullptr) const;
    bool Intersects(OBB const& _o, Manifold* _manifold = nullptr)   const;
    
    static OBB Expand(OBB const& _o, float _scalar);
    static OBB Transform(OBB const& _o, Mat4f32 const& _t);
};

#endif
