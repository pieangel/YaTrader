#include "stdafx.h"
#include "VtRect.h"
#include "../Util/formatstdstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VtRect::VtRect(int x, int y, int x1, int y1)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;
	_IsSelected = true;
	EndMoveRedim();
	_EleType = EditOption::Rect;
}

VtRect::VtRect()
{
}


VtRect::VtRect(XYChart* chart, int x, int y, int x1, int y1)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;
	_IsSelected = true;
	EndMoveRedim(chart);
	_EleType = EditOption::Rect;
}

VtRect::VtRect(SmPoint start, SmPoint end, VtColor penColor, int penWidth, VtColor fillColor, bool filled)
{
	_StartValue = start;
	_EndValue = end;
	_PenColor = penColor;
	_PenWidth = penWidth;
	_FillColor = fillColor;
	_IsFlled = filled;
}

VtRect::~VtRect()
{
}

VtElement * VtRect::Copy()
{
	auto r = new VtRect(*this);
	return r;
}

void VtRect::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	int clrPen = VtElement::ColorToInt(_PenColor);
	int clrFill = VtElement::ColorToInt(_FillColor);
	DrawRect(Location0, Location1, d, chart, dx, dy, clrPen, clrFill);

	std::string font = _T("±¼¸²");
	std::string valueText = std::format(_T("Value X = %.2f, Value Y = %.2f"), Value1.x, Value1.y);
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(Location0.x + dx + HandleRadius, Location0.y + dy + HandleRadius * 4 , 0x000000);
	t->destroy();
}

void VtRect::Draw(DoubleArray& timeStamp, XYChart* chart)
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

	DrawRect(start, end, chart, clrPen, clrFill);
}

void VtRect::DrawRect(VtPoint start, VtPoint end, DrawArea * d, XYChart * chart, int dx, int dy, int clrPen, int clrFill)
{
	d->rect(start.x + dx, start.y + dy, end.x + dx, end.y + dy, clrPen, clrFill);

	std::string font = _T("±¼¸²");
	std::string valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(start.x + dx), chart->getYValue(start.y + dy));
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + dx + HandleRadius, start.y + dy + HandleRadius, 0x000000);
	t->destroy();

	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(end.x + dx), chart->getYValue(end.y + dy));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + dx  + HandleRadius, end.y + dy + HandleRadius, 0x000000);
	t->destroy();
}

void VtRect::DrawRect(VtPoint start, VtPoint end, VtValue value1, VtValue value2, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int clrFill)
{
	d->rect(start.x + dx, start.y + dy, end.x + dx, end.y + dy, clrPen, clrFill);

	std::string font = _T("±¼¸²");
	std::string valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(start.x + dx), chart->getYValue(start.y + dy));
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + dx + HandleRadius, start.y + dy + HandleRadius, 0x000000);
	t->destroy();

	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(end.x + dx), chart->getYValue(end.y + dy));
	//valueText = format(_T("x = %.2f, y = %.2f"), chart->getXValue(end.x + dx), chart->getYValue(end.y + dy));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + dx + HandleRadius, end.y + dy + HandleRadius, 0x000000);
	t->destroy();
}

void VtRect::DrawRect(VtPoint start, VtPoint end, XYChart* chart, int clrPen, int clrFill)
{
	DrawArea* d = chart->makeChart();
	d->rect(start.x, start.y, end.x, end.y, clrPen, clrFill);

	std::string font = _T("±¼¸²");
	std::string valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(start.x), chart->getYValue(start.y));
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + HandleRadius, start.y + HandleRadius, 0x000000);
	t->destroy();

	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(end.x), chart->getYValue(end.y));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + HandleRadius, end.y + HandleRadius, 0x000000);
	t->destroy();
}

void VtRect::DrawRect(VtPoint start, VtPoint end, XYChart* chart, DrawArea* d, int clrPen, int clrFill)
{
	d->rect(start.x, start.y, end.x, end.y, clrPen, clrFill);

	std::string font = _T("±¼¸²");
	std::string valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(start.x), chart->getYValue(start.y));
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + HandleRadius, start.y + HandleRadius, 0x000000);
	t->destroy();

	valueText = std::format(_T("x = %.2f, y = %.2f"), chart->getXValue(end.x), chart->getYValue(end.y));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + HandleRadius, end.y + HandleRadius, 0x000000);
	t->destroy();
}
