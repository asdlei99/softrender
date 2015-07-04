#ifndef _MATH_VECTOR3_INLINE_
#define _MATH_VECTOR3_INLINE_

#include "mathf.h"

namespace rasterizer
{

float Vector3::Length() const { return Mathf::Sqrt(SqrLength()); }
float Vector3::SqrLength() const { return (x * x + y * y + z * z); }
const Vector3 Vector3::Normalize() const { return Multiply(Mathf::InvSqrt(SqrLength())); }
const Vector3 Vector3::Negate() const { return Vector3(-x, -y, -z); }
const Vector3 Vector3::Add(const Vector3 &v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
const Vector3 Vector3::Subtract(const Vector3 &v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
const Vector3 Vector3::Multiply(float f) const { return Vector3(x * f, y * f, z * f); }
const Vector3 Vector3::Multiply(const Vector3& v) const { return Vector3(x * v.x, y * v.y, z * v.z); }
const Vector3 Vector3::Divide(float f) const { return Multiply(1.f / f); }

const Vector3 Vector3::operator +() const { return *this; }
const Vector3 Vector3::operator -() const { return Vector3(-x, -y, -z); }
const Vector3 Vector3::operator +(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
const Vector3 Vector3::operator -(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
const Vector3 Vector3::operator *(float f) const { return Vector3(x * f, y * f, z * f); }
const Vector3 Vector3::operator *(const Vector3& v) const { return Vector3(x * v.x, y * v.y, z * v.z); }
const Vector3 Vector3::operator /(float f) const { return this->operator*(1.f / f); }
const Vector3 Vector3::operator += (const Vector3& v) { x += v.x; y += v.y, z += v.z; return *this; }
const Vector3 Vector3::operator -= (const Vector3& v) { x -= v.x; y -= v.y, z -= v.z; return *this; }
const Vector3 Vector3::operator *= (float f) { x *= f; y *= f, z *= f; return *this; }
const Vector3 Vector3::operator /= (float f) { return (*this) *= (1.f / f); }
    
float Vector3::Dot(const Vector3& v) const
{
    return x * v.x + y * v.y + z * v.z;
}
    
const Vector3 Vector3::Cross(const Vector3& v) const
{
    return Vector3(
                   y * v.z - z * v.y,
                   z * v.x - x * v.z,
                   x * v.y - y * v.x
                   );
}

const float Vector3::Dot(const Vector3& a, const Vector3& b) { return a.Dot(b); }
const Vector3 Vector3::Cross(const Vector3& a, const Vector3& b) { return a.Cross(b); }

const Vector3 Vector3::LinearInterp(const Vector3& a, const Vector3& b, float t)
{
	return Vector3(
		Mathf::Lerp(a.x, b.x, t),
		Mathf::Lerp(a.y, b.y, t),
		Mathf::Lerp(a.z, b.z, t));
}

const Vector3 Vector3::TriangleInterp(const Vector3& a, const Vector3& b, const Vector3& c, float t0, float t1, float t2)
{
	return Vector3(
		Mathf::Terp(a.x, b.x, c.x, t0, t1, t2),
		Mathf::Terp(a.y, b.y, c.y, t0, t1, t2),
		Mathf::Terp(a.z, b.z, c.z, t0, t1, t2));
}

}

#endif // !_MATH_VECTOR3_INLINE_