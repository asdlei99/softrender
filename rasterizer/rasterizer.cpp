#include "rasterizer.h"

namespace rasterizer
{

void TestColor(Canvas* canvas)
{
	int w = canvas->GetWidth();
	int h = canvas->GetHeight();

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			canvas->SetPixel(x, y,
				Color(1.f, (float)x / w, (float)y / h, 0.f));
		}
	}
}

void Rasterizer::Line(Canvas* canvas, const Color32& color, int x0, int x1, int y0, int y1)
{
	bool steep = Mathf::Abs(y1 - y0) > Mathf::Abs(x1 - x0);
	if (steep)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int deltaX = x1 - x0;
	int deltaY = Mathf::Abs(y1 - y0);
	int error = deltaX / 2;
	int yStep = -1;
	if (y0 < y1) yStep = 1;

	for (int x = x0, y = y0; x < x1; ++x)
	{
		Plot(canvas, x, y, color, steep);
		error = error - deltaY;
		if (error < 0)
		{
			y = y + yStep;
			error = error + deltaX;
		}
	}
}

void Rasterizer::SmoothLine(Canvas* canvas, const Color32& color, float x0, float x1, float y0, float y1)
{
	float deltaX = x1 - x0;
	float deltaY = y1 - y0;

	bool steep = Mathf::Abs(deltaX) < Mathf::Abs(deltaY);
	if (steep)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		std::swap(deltaX, deltaY);
	}
	if (x1 < x0)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	float gradient = (float)deltaY / deltaX;

	float xend, yend, xgap, intery;
	int xpxl1, xpxl2, ypxl1, ypxl2;

	float yfpart;

	xend = Mathf::Round(x0);
	yend = y0 + gradient * (xend - x0);
	xgap = 1.f - FloatPart(x0 + 0.5);
	xpxl1 = Mathf::RoundToInt(xend);
	ypxl1 = IntPart(yend);
	yfpart = FloatPart(yend);
	Plot(canvas, xpxl1, ypxl1, color, xgap * (1 - yfpart), steep);
	Plot(canvas, xpxl1, ypxl1 + 1, color, xgap * yfpart, steep);
	intery = yend + gradient;

	xend = Mathf::Round(x1);
	yend = y1 + gradient * (xend - x1);
	xgap = 1.f - FloatPart(x1 + 0.5);
	xpxl2 = Mathf::RoundToInt(xend);
	ypxl2 = IntPart(yend);
	yfpart = FloatPart(yend);
	Plot(canvas, xpxl2, ypxl2, color, xgap * (1 - yfpart), steep);
	Plot(canvas, xpxl2, ypxl2 + 1, color, xgap * yfpart, steep);

	for (int x = xpxl1 + 1; x <= xpxl2 - 1; ++x)
	{
		int ipartIntery = IntPart(intery);
		float fpartIntery = FloatPart(intery);
		Plot(canvas, x, ipartIntery, color, 1.0f - fpartIntery, steep);
		Plot(canvas, x, ipartIntery + 1, color, fpartIntery, steep);

		intery += gradient;
	}

}

float Rasterizer::FloatPart(float v)
{
	return v - Mathf::Floor(v);
}

int Rasterizer::IntPart(float v)
{
	return Mathf::FloorToInt(v);
}

void Rasterizer::Plot(Canvas* canvas, int x, int y, const Color32& color, float alpha, bool swapXY /*= false*/)
{
	assert(canvas != nullptr);

	Color32 drawColor = color;
	drawColor.a *= Mathf::Clamp01(alpha);
	Plot(canvas, x, y, drawColor, swapXY);
}

void Rasterizer::Plot(Canvas* canvas, int x, int y, const Color32& color, bool swapXY)
{
	assert(canvas != nullptr);

	if (swapXY) Plot(canvas, y, x, color);
	else Plot(canvas, x, y, color);
}

void Rasterizer::Plot(Canvas* canvas, int x, int y, const Color32& color)
{
	assert(canvas != nullptr);

	canvas->SetPixel(x, y, color);
}

void Rasterizer::DrawMeshPoint(Canvas* canvas, const Camera& camera, const Mesh& mesh, const Matrix4x4& transform, const Color32& color)
{
	assert(canvas != nullptr);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	const Matrix4x4* view = camera.GetViewMatrix();
	const Matrix4x4* projection = camera.GetProjectionMatrix();

	for (int i = 0; i < (int)mesh.vertices.size(); ++i)
	{
		Vector3 posW = transform.MultiplyPoint3x4(mesh.vertices[i]);
		Vector3 posC = view->MultiplyPoint(posW);
		Vector3 point = projection->MultiplyPoint(posC);
		float x = (point.x + 1) * width / 2;
		float y = (point.y + 1) * height / 2;
		Plot(canvas, Mathf::RoundToInt(x), Mathf::RoundToInt(y), color);
		//printf("%s => %s => %s => %s => (%.2f %.2f)\n",
		//	mesh.vertices[i].ToString().c_str(),
		//	posW.ToString().c_str(),
		//	posC.ToString().c_str(),
		//	point.ToString().c_str(),
		//	x, y);
	}
}

void Rasterizer::DrawMeshWireFrame(Canvas* canvas, const Camera& camera, const Mesh& mesh, const Matrix4x4& transform, const Color32& color)
{
	assert(canvas != nullptr);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	const Matrix4x4* view = camera.GetViewMatrix();
	const Matrix4x4* projection = camera.GetProjectionMatrix();

	std::vector<Vector2> points;
	for (int i = 0; i < (int)mesh.vertices.size(); ++i)
	{
		Vector3 posW = transform.MultiplyPoint3x4(mesh.vertices[i]);
		Vector3 posC = view->MultiplyPoint(posW);
		Vector3 point = projection->MultiplyPoint(posC);
		float x = (point.x + 1) * width / 2;
		float y = (point.y + 1) * height / 2;
		points.push_back(Vector2(x, y));
	}

	for (int i = 0; i + 2 < (int)mesh.indices.size(); i += 3)
	{
		int v0 = mesh.indices[i];
		int v1 = mesh.indices[i+1];
		int v2 = mesh.indices[i+2];

		int x0 = Mathf::RoundToInt(points[v0].x);
		int y0 = Mathf::RoundToInt(points[v0].y);
		int x1 = Mathf::RoundToInt(points[v1].x);
		int y1 = Mathf::RoundToInt(points[v1].y);
		int x2 = Mathf::RoundToInt(points[v2].x);
		int y2 = Mathf::RoundToInt(points[v2].y);

		Line(canvas, color, x0, x1, y0, y1);
		Line(canvas, color, x0, x2, y0, y2);
		Line(canvas, color, x1, x2, y1, y2);
	}
}

}