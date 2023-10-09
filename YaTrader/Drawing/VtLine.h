#pragma once
#include "VtElement.h"
#include <string>
class VtLine :
	public VtElement
{
public:
	VtLine(SmPoint start, SmPoint end, VtColor penColor, int penWidth);
	VtLine(VtLine& src)
		: VtElement(src)
	{

	}
	VtLine(int x, int y, int x1, int y1);
	VtLine(XYChart* chart, int x, int y, int x1, int y1);
	VtLine();
	virtual ~VtLine();
	virtual VtElement* Copy();
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	virtual void MovePoint(int dx, int dy, VtPoint* linkedPoint);
	virtual void MovePoint(XYChart* chart, int dx, int dy, VtPoint* linkedPoint);

	virtual void Draw(DoubleArray& timeStamp, XYChart* chart);

	static void DrawLine(VtPoint start, VtPoint end, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int penWidth);
	static void DrawLine(VtPoint start, VtPoint end, XYChart* chart, DrawArea* d, int clrPen, int penWidth);
	static void DrawLine(VtPoint start, VtPoint end, XYChart* chart, int clrPen, int penWidth);
	static void DrawLine(const VtPoint start, const VtPoint end, XYChart* chart, const int clrPen, const int penWidth, const std::string& symbol_code);
	static void DrawLine(const bool& show_text, const VtPoint& start, const VtPoint& end, XYChart* chart, const int& clrPen, const int& penWidth, const std::string& symbol_code);
	static void DrawLine(const std::string& text, const VtPoint& start, const VtPoint& end, XYChart* chart, const int& clrPen, const int& penWidth, const std::string& symbol_code);
	double Slope() const { return _Slope; }
	void Slope(double val) { _Slope = val; }
	double Intercept() const { return _Intercept; }
	void Intercept(double val) { _Intercept = val; }
private:
	double _Slope = 0.0;
	double _Intercept = 0.0;
};

