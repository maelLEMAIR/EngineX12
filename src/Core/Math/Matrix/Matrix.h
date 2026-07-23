#ifndef MATRIX_H_DEFINED
#define MATRIX_H_DEFINED

#include "Matrix3.h"
#include "Matrix4.h"

using Mat3i8   = Matrix3<int8>;
using Mat3i16  = Matrix3<int16>;
using Mat3i32  = Matrix3<int32>;
using Mat3i64  = Matrix3<int64>;

using Mat3ui8  = Matrix3<uint8>;
using Mat3ui16 = Matrix3<uint16>;
using Mat3ui32 = Matrix3<uint32>;
using Mat3ui64 = Matrix3<uint64>;

using Mat3f32  = Matrix3<float32>;
using Mat3f64  = Matrix3<float64>;

using Mat4i8   = Matrix4<int8>;
using Mat4i16  = Matrix4<int16>;
using Mat4i32  = Matrix4<int32>;
using Mat4i64  = Matrix4<int64>;

using Mat4ui8  = Matrix4<uint8>;
using Mat4ui16 = Matrix4<uint16>;
using Mat4ui32 = Matrix4<uint32>;
using Mat4ui64 = Matrix4<uint64>;

using Mat4f32  = Matrix4<float32>;
using Mat4f64  = Matrix4<float64>;

#endif