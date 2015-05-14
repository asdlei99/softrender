#ifndef _BASE_TEXTURE_SAMPLER_HPP_
#define _BASE_TEXTURE_SAMPLER_HPP_

#include "base/header.h"
#include "math/vector3.h"
#include "math/mathf.h"
#include "rasterizer/texture.h"

namespace rasterizer
{

struct WarpAddresser
{
	static float CalcAddress(float coord, u32 length)
	{
		return (coord - Mathf::Floor(coord)) * (int)length - 0.5f;
	}
	static int FixAddress(int coord, u32 length)
	{
		if (coord < 0) return length - 1;
		if (coord >= (int)length) return 0;
		return coord;
	}
};

struct ClampAddresser
{
	static float CalcAddress(float coord, u32 length)
	{
		return Mathf::Clamp(coord * length, 0.5f, length - 0.5f) - 0.5f;
	}
	static int FixAddress(int coord, u32 length)
	{
		if (coord < 0) return 0;
		if (coord >= (int)length) return length - 1;
		return coord;
	}
};

struct MirrorAddresser
{
	static float CalcAddress(float coord, u32 length)
	{
		int round = Mathf::FloorToInt(coord);
		float tmpCoord = (round & 1) ? (1 + round - coord) : (coord - round);
		return tmpCoord * length - 0.5f;
	}
	static int FixAddress(int coord, u32 length)
	{
		if (coord < 0) return 0;
		if (coord >= (int)length) return length - 1;
		return coord;
	}
};

struct PointSampler
{
	template<typename XAddresserType, typename YAddresserType>
	static Color Sample(const BitmapPtr bitmap, float u, float v)
	{
		u32 width = bitmap->GetWidth();
		u32 height = bitmap->GetHeight();

		float fx = XAddresserType::CalcAddress(u, width);
		int x = XAddresserType::FixAddress(Mathf::RoundToInt(fx), width);
		float fy = YAddresserType::CalcAddress(v, height);
		int y = YAddresserType::FixAddress(Mathf::RoundToInt(fy), height);

		return bitmap->GetColor(x, y);
	}
};

struct LinearSampler
{
	template<typename XAddresserType, typename YAddresserType>
	static Color Sample(const BitmapPtr bitmap, float u, float v)
	{
		u32 width = bitmap->GetWidth();
		u32 height = bitmap->GetHeight();

		float fx = XAddresserType::CalcAddress(u, width);
		int x0 = Mathf::FloorToInt(fx);
		float fy = YAddresserType::CalcAddress(v, height);
		int y0 = Mathf::FloorToInt(fy);
		float xFrac = fx - x0;
		float yFrac = fy - y0;
		x0 = XAddresserType::FixAddress(x0, width);
		y0 = YAddresserType::FixAddress(y0, height);
		int x1 = XAddresserType::FixAddress(x0 + 1, width);
		int y1 = YAddresserType::FixAddress(y0 + 1, height);

		Color c0 = bitmap->GetColor(x0, y0);
		Color c1 = bitmap->GetColor(x1, y0);
		Color c2 = bitmap->GetColor(x0, y1);
		Color c3 = bitmap->GetColor(x1, y1);

		return Color::Lerp(c0, c1, c2, c3, xFrac, yFrac);
	}
};

}

#endif //! _BASE_TEXTURE_SAMPLER_HPP_