#ifndef FRUSTUM_H_DEFINED
#define FRUSTUM_H_DEFINED

#include "Plane.h"
#include "../Matrix/Matrix.h"

class Sphere;
class AABB;
class OBB;

class Frustum
{
public:
    union 
    {
        Plane planes[6];

        struct
        {
            Plane left;
            Plane right;
            Plane bottom;
            Plane top;
            Plane nearZ;
            Plane farZ;
        };
        
    };
    
    Frustum(bool _isGl = false);
    Frustum(Mat4f32 const& _view, Mat4f32 const& _proj, bool _isGl = false);
    Frustum(Mat4f32 const& _viewProj, bool _isGl = false);

    bool Contains(Vect3f32 const& _p) const;
    bool Intersects(AABB const& _o) const;
    bool Intersects(Sphere const& _s) const;
    bool Intersects(OBB const& _o) const;
};

#endif
