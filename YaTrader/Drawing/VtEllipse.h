#pragma once
#include "VtElement.h"
class XYChart;
class VtEllipse :
	public VtElement
{
public:
	VtEllipse(VtEllipse& e)
		: VtElement(e)
	{
		;
	}
	VtEllipse(SmPoint start, SmPoint end, VtColor penColor, int penWidth, VtColor fillColor, bool filled);
	VtEllipse(int x, int y, int x1, int y1);
	VtEllipse(XYChart* chart, int x, int y, int x1, int y1);
	VtEllipse();
	virtual ~VtEllipse();
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	virtual VtElement* Copy();
	static void DrawEllipse(VtPoint start, VtPoint end, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int clrFill);

	virtual void Draw(DoubleArray& timeStamp, XYChart* chart);
	static void DrawEllipse(VtPoint start, VtPoint end, XYChart* chart, int clrPen, int clrFill);
	static void DrawEllipse(VtPoint start, VtPoint end, XYChart* chart, DrawArea* d, int clrPen, int clrFill);
};

