#include "stdafx.h"
#include "VtEllipse.h"
#include "chartdir.h"
#include "../Util/formatstdstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VtEllipse::VtEllipse(int x, int y, int x1, int y1)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;
	_IsSelected = true;
	EndMoveRedim();
	_EleType = EditOption::Ellipse;
}

VtEllipse::VtEllipse()
{
}


VtEllipse::VtEllipse(XYChart* chart, int x, int y, int x1, int y1)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;
	_IsSelected = true;
	EndMoveRedim(chart);
	_EleType = EditOption::Ellipse;
}

VtEllipse::VtEllipse(SmPoint start, SmPoint end, VtColor penColor, int penWidth, VtColor fillColor, bool filled)
{
	_StartValue = start;
	_EndValue = end;
	_PenColor = penColor;
	_PenWidth = penWidth;
	_FillColor = fillColor;
	_IsFlled = filled;
}

VtEllipse::~VtEllipse()
{
}

void VtEllipse::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	int clrPen = VtElement::ColorToInt(_PenColor);
	int clrFill = VtElement::ColorToInt(_FillColor);
	//ValueToPixel(idStart, zoomStartIndex, chart);
	
	DrawEllipse(Location0, Location1, d, chart, dx, dy, clrPen, clrFill);
}

void VtEllipse::Draw(DoubleArray& timeStamp, XYChart* chart)
{
	int clrPen = VtElement::ColorToInt(_PenColor);
	int clrFill = VtElement::ColorToInt(_FillColor);

	VtPoint start, end;
	int xIndex = (int)floor(Chart::bSearch(timeStamp, _StartValue.x));
	start.x = chart->getXCoor(xIndex);
	start.y = chart->getYCoor(_StartValue.y);
	int xIndex2 = (int)floor(Chart::bSearch(timeStamp, _EndValue.x));
	end.x = chart->getXCoor(xIndex2);
	end.y = chart->getYCoor(_EndValue.y);

	DrawEllipse(start, end, chart, clrPen, clrFill);
}

VtElement * VtEllipse::Copy()
{
	auto r = new VtEllipse(*this);
	return r;
}

void VtEllipse::DrawEllipse(VtPoint start, VtPoint end, DrawArea * d, XYChart * chart, int dx, int dy, int clrPen, int clrFill)
{
	d->circle(
		start.x + dx + (int)((end.x - start.x) / 2.0),
		start.y + dy + (int)((end.y - start.y) / 2.0),
		(int)((end.x - start.x) / 2.0),
		(int)((end.y - start.y) / 2.0), clrPen, clrFill);

	int halfX = (int)((end.x - start.x) / 2.0);
	int halfY = (int)((end.y - start.y) / 2.0);

	std::string font = _T("±¼¸²");
	std::string valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(halfX + dx), chart->getYValue(start.y + dy));
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + dx + halfX + HandleRadius, start.y + dy + HandleRadius, 0x000000);
	t->destroy();
	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(halfX + dx), chart->getYValue(end.y + dy));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + dx + halfX + HandleRadius, end.y + dy + HandleRadius, 0x000000);
	t->destroy();
	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(start.x + dx), chart->getYValue(halfY + dy));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + dx + HandleRadius, start.y + halfY + dy + HandleRadius, 0x000000);
	t->destroy();
	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(end.x + dx), chart->getYValue(halfY + dy));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + dx + HandleRadius, start.y + halfY + dy + HandleRadius, 0x000000);
	t->destroy();
}

void VtEllipse::DrawEllipse(VtPoint start, VtPoint end, XYChart* chart, int clrPen, int clrFill)
{
	DrawArea* d = chart->makeChart();
	d->circle(
		start.x + (int)((end.x - start.x) / 2.0),
		start.y + (int)((end.y - start.y) / 2.0),
		abs((int)((end.x - start.x) / 2.0)),
		abs((int)((end.y - start.y) / 2.0)), clrPen, clrFill);

	int halfX = (int)((end.x - start.x) / 2.0);
	int halfY = (int)((end.y - start.y) / 2.0);

	std::string font = _T("±¼¸²");
	std::string valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(halfX), chart->getYValue(start.y));
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + halfX + HandleRadius, start.y + HandleRadius, 0x000000);
	t->destroy();
	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(halfX), chart->getYValue(end.y));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + halfX + HandleRadius, end.y + HandleRadius, 0x000000);
	t->destroy();
	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(start.x), chart->getYValue(halfY));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + HandleRadius, start.y + halfY + HandleRadius, 0x000000);
	t->destroy();
	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(end.x), chart->getYValue(halfY));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + HandleRadius, start.y + halfY + HandleRadius, 0x000000);
	t->destroy();
}

void VtEllipse::DrawEllipse(VtPoint start, VtPoint end, XYChart* chart, DrawArea* d, int clrPen, int clrFill)
{
	d->circle(
		start.x + (int)((end.x - start.x) / 2.0),
		start.y + (int)((end.y - start.y) / 2.0),
		abs((int)((end.x - start.x) / 2.0)),
		abs((int)((end.y - start.y) / 2.0)), clrPen, clrFill);

	int halfX = (int)((end.x - start.x) / 2.0);
	int halfY = (int)((end.y - start.y) / 2.0);

	std::string font = _T("±¼¸²");
	std::string valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(halfX), chart->getYValue(start.y));
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + halfX + HandleRadius, start.y + HandleRadius, 0x000000);
	t->destroy();
	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(halfX), chart->getYValue(end.y));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + halfX + HandleRadius, end.y + HandleRadius, 0x000000);
	t->destroy();
	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(start.x), chart->getYValue(halfY));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + HandleRadius, start.y + halfY + HandleRadius, 0x000000);
	t->destroy();
	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(end.x), chart->getYValue(halfY));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + HandleRadius, start.y + halfY + HandleRadius, 0x000000);
	t->destroy();
}

