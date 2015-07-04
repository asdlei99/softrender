#ifndef _BASE_VECTOR3_H_
#define _BASE_VECTOR3_H_

#include "base/header.h"

namespace rasterizer
{

struct Vector3
{
	union
	{
		float f[3];
		struct { float x, y, z; };
	};

	Vector3() = default;
    Vector3(float _x, float _y, float _z): x(_x), y(_y), z(_z) {}

    inline float Length() const;
    inline float SqrLength() const;
    inline const Vector3 Normalize() const;
    inline const Vector3 Negate() const;
    inline const Vector3 Add(const Vector3& v) const;
    inline const Vector3 Subtract(const Vector3& v) const;
    inline const Vector3 Multiply(float f) const;
    inline const Vector3 Multiply(const Vector3& v) const;
    inline const Vector3 Divide(float f) const;
    inline float Dot(const Vector3& v) const;
    inline const Vector3 Cross(const Vector3& v) const;

    inline const Vector3 operator +() const;
    inline const Vector3 operator -() const;
    inline const Vector3 operator +(const Vector3& v) const;
    inline const Vector3 operator -(const Vector3& v) const;
    inline const Vector3 operator *(float f) const;
    inline const Vector3 operator *(const Vector3& v) const;
    inline const Vector3 operator /(float f) const;
    inline const Vector3 operator += (const Vector3& v);
    inline const Vector3 operator -= (const Vector3& v);
    inline const Vector3 operator *= (float f);
    inline const Vector3 operator /= (float f);

	static inline const float Dot(const Vector3& a, const Vector3& b);
	static inline const Vector3 Cross(const Vector3& a, const Vector3& b);

	static inline const Vector3 LinearInterp(const Vector3& a, const Vector3& b, float t);
	static inline const Vector3 TriangleInterp(const Vector3& a, const Vector3& b, const Vector3& c, float t0, float t1, float t2);

	static const Vector3 zero;
	static const Vector3 one;
	static const Vector3 front;
	static const Vector3 up;
	static const Vector3 right;
	static const Vector3 back;
	static const Vector3 down;
	static const Vector3 left;
};

} // namespace rasterizer

#include "vector3.inl"

#endif //!_BASE_VECTOR3_H_