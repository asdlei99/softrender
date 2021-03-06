#ifndef _SOFTRENDER_SRTYPES_HPP_
#define _SOFTRENDER_SRTYPES_HPP_

#include "base/header.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"
#include "softrender/clipper.hpp"

namespace sr
{

struct Projection
{
	int x = 0;
	int y = 0;
	float z = 0.f;
	float invW = 1.0f;

	static Projection CalculateViewProjection(const Vector4& position, uint32_t width, uint32_t height)
	{
		float w = position.w;
		assert(w > 0.f);
		float invW = 1.f / w;

		Projection point;
		point.x = Mathf::RoundToInt(((position.x * invW) + 1.f) / 2.f * width);
		point.y = Mathf::RoundToInt(((position.y * invW) + 1.f) / 2.f * height);
		point.z = position.z * invW;
		point.invW = invW;
		return point;
	}

	static int Orient2D(int x0, int y0, int x1, int y1, int x2, int y2)
	{
		return (x1 - x0) * (y2 - y1) - (y1 - y0) * (x2 - x1);
	}

	static int Orient2D(const Projection& p0, const Projection& p1, const Projection& p2)
	{
		return Orient2D(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
	}
};

template <typename Type>
struct Line
{
	Type v0, v1;

	Line() = default;
	Line(const Type& _v0, const Type& _v1)
		: v0(_v0), v1(_v1) {}

	const Type& operator[](int index) const
	{
		switch (index)
		{
		case 0: return v0;
		case 1: return v1;
		default:
			throw std::out_of_range("Line Out of Rnage");
		}
	}

	Type& operator[](int index)
	{
		switch (index)
		{
		case 0: return v0;
		case 1: return v1;
		default:
			throw std::out_of_range("Line Out of Rnage");
		}
	}
};

template <typename Type>
struct Triangle
{
	Type v0, v1, v2;

	Triangle() = default;
	Triangle(const Type& _v0, const Type& _v1, const Type& _v2)
		: v0(_v0), v1(_v1), v2(_v2) {}

	const Type& operator[](int index) const
	{
		switch (index)
		{
		case 0: return v0;
		case 1: return v1;
		case 2: return v2;
		default:
			throw std::out_of_range("Triangle Out of Rnage");
		}
	}

	Type& operator[](int index)
	{
		switch (index)
		{
		case 0: return v0;
		case 1: return v1;
		case 2: return v2;
		default:
			throw std::out_of_range("Triangle Out of Rnage");
		}
	}
};

template <typename Type>
struct Quad
{
	Type v0, v1, v2, v3;

	Quad() = default;
	Quad(const Type& _v0, const Type& _v1, const Type& _v2, const Type& _v3)
		: v0(_v0), v1(_v1), v2(_v2), v3(_v3) {}

	const Type& operator[](int index) const
	{
		switch (index)
		{
		case 0: return v0;
		case 1: return v1;
		case 2: return v2;
		case 3: return v3;
		default:
			throw std::out_of_range("Quad Out of Range");
		}
	}

	Type& operator[](int index)
	{
		switch (index)
		{
		case 0: return v0;
		case 1: return v1;
		case 2: return v2;
		case 3: return v3;
		default:
			throw std::out_of_range("Quad Out of Range");
		}
	}
};

}

#endif //! _SOFTRENDER_SRTYPES_HPP_
