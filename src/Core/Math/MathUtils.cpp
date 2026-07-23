#include "MathUtils.hpp"

float MathUtils::Sqrt(float _v)
{
    assert(_v >= 0.0f && "Can't compute the root of a negative number");

    float result = _v;
    float lastResult = 0.0f;
        
    while (result != lastResult)
    {
        lastResult = result;
        result = (result + _v / result) * 0.5f;
    }
    return result;
}

float MathUtils::ISqrt(float _v)
{
    return 1.0f / Sqrt(_v);
}

int MathUtils::Factorial(uint8 _x)
{
    if (_x == 0) return 1;
    
    int result = 1;
    
    for (uint8 i = 1; i <= _x; i++)
        result *= i;
    
    return result;
}

float MathUtils::Pow(float _x, uint8 _e)
{
    if (_e == 0) return 1.0f;
    if (_e == 1) return _x;
    
    float result = 1.0f;
    for (int i = 0; i < _e; i++)
    {
        result *= _x;
    }
    return result;
}

float MathUtils::Exp(float _x)
{
    float result = 1.0f;
    float term = 1.0f;

    for (int i = 1; i < 50; ++i)
    {
        term *= _x / i;   // next Taylor term
        result += term;

        if (std::abs(term) < 1e-15f)
            break;
    }

    return result;
}

float MathUtils::Log2(float _x)
{
    assert(_x > 0.0f && "Log isn't defined for x <= 0");
        
    int e = 0;
    while (_x >= 2.0f)
    {
        _x *= 0.5f; 
        e++;
    }
    while (_x < 1.0f)
    {
        _x *= 2.0f;
        e--;
    }
        
    float result = 0.0f;
    float bit = 0.5f;
        
    for (int i = 0; i < 23; i++)
    {
        _x *= _x;
        if (_x >= 2.0f)
        {
            result += bit;
            _x *= 0.5f;
        }
        bit *= 0.5f;
    }
    return float(e) + result;
}

float MathUtils::Log10(float _x)
{
    return Log2(_x) / lb10;
}

float MathUtils::LogN(float _x, float _base)
{
    return Log2(_x) / Log2(_base);
}

float MathUtils::Ln(float _x)
{
    return Log2(_x) / lbE;
}

float MathUtils::WrapAngle(float _x)
{
    while (_x > PI) _x -= TWO_PI;
    while (_x <= -PI) _x += TWO_PI;
    return _x;
}

float MathUtils::ToRadians(float _x)
{
    return _x * DEG_TO_RAD;
}

float MathUtils::ToDegrees(float _x)
{
    return _x * RAD_TO_DEG;
}

float MathUtils::Sin(float _x) 
{
    _x = WrapAngle(_x);

    float term   = _x;
    float result = _x;
    for (int n = 1; n <= 11; n++) 
    {
        term   *= -(_x * _x) / ((2*n) * (2*n + 1));
        result += term;
    }
    return result;
}

float MathUtils::Cos(float _x) 
{
    _x = WrapAngle(_x);

    float term   = 1.0f;
    float result = 1.0f;
    for (int n = 1; n <= 11; n++) 
    {
        term   *= -(_x * _x) / ((2*n - 1) * (2*n));
        result += term;
    }
    return result;
}

float MathUtils::Tan(float _x) 
{
    float c = Cos(_x);
    if (c == 0.0f) return NAN;
    return Sin(_x) / c;
}

float MathUtils::Asin(float _x)
{
    assert(_x <= 1.0f && _x >= -1.0f && "Acos isn't defined for x < -1 & x > 1");
    if (_x ==  1.0f) return  HALF_PI;
    if (_x == -1.0f) return -HALF_PI;
    return Atan(_x / Sqrt(1.0f - _x * _x));
}

float MathUtils::Acos(float _x)
{
    assert(_x <= 1.0f && _x >= -1.0f && "Acos isn't defined for x < -1 & x > 1");
    return HALF_PI - Asin(_x);
}

float MathUtils::Atan(float _x)
{
    if (Abs(_x) > 0.5f)
    {
        float t = _x / (1.0f + Sqrt(1.0f + _x * _x));
        return 2.0f * Atan(t);
    }

    float result = _x;
    float power = _x;

    for (int n = 1; n < 20; ++n)
    {
        power *= _x * _x;

        float term = ((n & 1) ? -1.0f : 1.0f) *
                     power / (2 * n + 1);

        result += term;
    }

    return result;
}

float MathUtils::Atan2(float _y, float _x)
{
    if (_x == 0.0f && _y == 0.0f) return NAN;
    if (_x >  0.0f) return Atan(_y / _x);
    if (_x <  0.0f) return Atan(_y / _x) + (_y >= 0.0f ?  PI : -PI);
    
    return _y > 0.0f ? HALF_PI : -HALF_PI;
}

bool MathUtils::NearlyEqual(float _x, float _y)
{
    return Abs(_x - _y) <= EPSILON;
}

bool MathUtils::NearlyEqual(float _x, float _y, float _epsilon)
{
    return Abs(_x - _y) <= _epsilon;
}

bool MathUtils::IsPowerOfTwo(int _x)
{
    return !(_x == 0) && !(_x & (_x - 1));
}

int MathUtils::NextPowerOfTwo(int _x)
{
    int power = 1;
    int result = 2;
    while (result <= _x)
    {
        result *= 2;
        power++;
    }
    return Pow(2, power);
}

int MathUtils::PreviousPowerOfTwo(int _x)
{
    int power = 1;
    int result = 2;
    while (result < _x)
    {
        result *= 2;
        power++;
    }
    return Pow(2, power - 1);
}







