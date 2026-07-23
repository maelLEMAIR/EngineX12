#ifndef MATH_UTILS_H_DEFINED
#define MATH_UTILS_H_DEFINED

#include "../Core/define.h"

class MathUtils
{
public:
    inline static const float PI           = 3.14159265358979323846f;
    inline static const float TWO_PI       = 2.0f * PI;
    inline static const float HALF_PI      = PI * 0.5f;
    inline static const float E            = 2.7182818284f;
    
    inline static const float DEG_TO_RAD   = PI / 180.0f;
    inline static const float RAD_TO_DEG   = 180.0f / PI;
    
    inline static const float LARGE_EPSILON = 1e-5f;
    inline static const float EPSILON       = 1.192092896e-07F;
    
    inline static const float FLOAT_MAX    = 3.402823466e+38F;
    inline static const float FLOAT_MIN    = 1.175494351e-38F;
    
    template <typename T>
    static T Min(T _a, T _b)
    {
        return _a < _b ? _a : _b;
    }
    
    template <typename T>
    static T Max(T _a, T _b)
    {
        return _a > _b ? _a : _b;
    }
    
    template <typename T>
    static T Clamp(T _v, T _lo, T _hi)
    {
        T result = _v;
        if (result < _lo)
            result = _lo;
        if (result > _hi)
            result = _hi;
        return result;
    }
    
    template <typename T>
    static T Abs(T _v)
    {
        return _v < 0 ? -_v : _v;
    }
    
    template <typename T>
    static int Sign(T _v)
    {
        return (_v < 0) ? -1 : (_v > 0) ? 1 : 0;
    }
    
    static int Floor(float _v)
    {
        int i = (int)_v;
        return (i > _v) ? i - 1 : i;
    }

    static int Ceil(float _v)
    {
        int i = (int)_v;
        return (i < _v) ? i + 1 : i;
    }

    static int Round(float _v)
    {
        return (int)(_v + (_v < 0.0f ? -0.5f : 0.5f));
    }
    
    static float Sqrt(float _v);
    static float ISqrt(float _v);
    
    static int Factorial(uint8 _x);
    static float Pow(float _x, uint8 _e);
    static float Exp(float _v);
    
    static float Log2(float _x);
    static float Log10(float _x);
    static float Ln(float _x);
    static float LogN(float _x, float _base);
    
    static float WrapAngle(float _x);
    static float ToRadians(float _x);
    static float ToDegrees(float _x);
    
    static float Sin(float _x);
    static float Cos(float _x);
    static float Tan(float _x);
    
    static float Asin(float _x);
    static float Acos(float _x);
    static float Atan(float _x);
    static float Atan2(float _y, float _x);
    
    static bool NearlyEqual(float _x, float _y);
    static bool NearlyEqual(float _x, float _y, float _epsilon);
    static bool IsPowerOfTwo(int _x);
    static int  NextPowerOfTwo(int _x);
    static int  PreviousPowerOfTwo(int _x);
    
    
private:
    inline static const float lbE = Log2(E);
    inline static const float lb10 = Log2(10);
};

#endif
