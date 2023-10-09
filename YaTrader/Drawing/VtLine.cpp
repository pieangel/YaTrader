#include "stdafx.h"
#include "VtLine.h"
#include "../Util/formatstdstring.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Util/SmUtil.h"
#include "../Chart/ChartViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VtLine::VtLine(int x, int y, int x1, int y1)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;
	_IsLine = true;
	_IsSelected = true;
	EndMoveRedim();
	_EleType = EditOption::Line;
}

VtLine::VtLine()
{
}


VtLine::VtLine(XYChart* chart, int x, int y, int x1, int y1)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;
	_IsLine = true;
	_IsSelected = true;
	EndMoveRedim(chart);
	_EleType = EditOption::Line;
}

VtLine::VtLine(SmPoint start, SmPoint end, VtColor penColor, int penWidth)
{
	_StartValue = start;
	_EndValue = end;
	_PenColor = penColor;
	_PenWidth = penWidth;
}

VtLine::~VtLine()
{
}

VtElement * VtLine::Copy()
{
	auto r = new VtLine(*this);
	return r;
}

void VtLine::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	//VtElement::ValueToPixel();
	int clrPen = VtElement::ColorToInt(_PenColor);
	int penWidth = _PenWidth;

	CString msg;
	msg.Format(_T("LineCoor :: x = %d, y = %d\n"), Location0.x, Location0.y);
	//TRACE(msg);

	DrawLine(Location0, Location1, d, chart, dx, dy, clrPen, penWidth);
}

void VtLine::Draw(DoubleArray& timeStamp, XYChart* chart)
{
	int clrPen = VtElement::ColorToInt(_PenColor);
	int penWidth = _PenWidth;
	VtPoint start, end;
	int xIndex = (int)floor(Chart::bSearch(timeStamp, _StartValue.x));
	start.x = chart->getXCoor(xIndex);
	start.y = chart->getYCoor(_StartValue.y);
	int xIndex2 = (int)floor(Chart::bSearch(timeStamp, _EndValue.x));
	end.x = chart->getXCoor(xIndex2);
	end.y = chart->getYCoor(_EndValue.y);


	CString msg;
	msg.Format("Draw xindex1 = %d, xindex2 = %d,  x1 = %d, y1 = %d, x2 = %d, y2 = %d\n", xIndex, xIndex2, _StartValue.x, _StartValue.y, _EndValue.x, _EndValue.y);
	TRACE(msg);

	DrawLine(start, end, chart, clrPen, penWidth);
}

void VtLine::MovePoint(int dx, int dy, VtPoint* linkedPoint)
{
	if (linkedPoint != nullptr)
	{
		if (linkedPoint == &Location0)
		{
			Location0.x = Start0.x - dx;
			Location0.y = Start0.y - dy;
		}
		else
		{
			Location1.x = Start1.x - dx;
			Location1.y = Start1.y - dy;
		}
	}
}

void VtLine::MovePoint(XYChart* chart, int dx, int dy, VtPoint* linkedPoint)
{
	if (linkedPoint != nullptr)
	{
		if (linkedPoint == &Location0)
		{
			Location0.x = Start0.x - dx;
			Location0.y = Start0.y - dy;
		}
		else
		{
			Location1.x = Start1.x - dx;
			Location1.y = Start1.y - dy;
		}
	}
}


void VtLine::DrawLine(VtPoint start, VtPoint end, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int penWidth)
{
	d->line(start.x + dx, start.y + dy, end.x + dx, end.y + dy, clrPen, penWidth);
	int extend = 100;
	d->line(start.x + dx - extend, start.y + dy, start.x + dx + extend, start.y + dy, clrPen, penWidth);
	d->line(end.x + dx - extend, end.y + dy, end.x + dx + extend, end.y + dy, clrPen, penWidth);

	double first = chart->getYValue(start.y + dy);
	double last = chart->getYValue(end.y + dy);
	double height = abs(last - first);

	
	std::string valueText = std::format(_T("%.2f"), first);
	std::string font = _T("±¼¸²");
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + HandleRadius, start.y + dy + HandleRadius, 0x000000);
	t->destroy();

	valueText = std::format(_T("%.2f"), last);
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + HandleRadius, end.y + dy + HandleRadius, 0x000000);
	t->destroy();

	valueText = std::format(_T("%.2f"), height);
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + HandleRadius, start.y + dy + HandleRadius*4, 0x0000ff);
	t->destroy();
}

void VtLine::DrawLine(VtPoint start, VtPoint end, XYChart* chart, DrawArea* d, int clrPen, int penWidth)
{
	d->line(start.x, start.y, end.x, end.y, clrPen, penWidth);
	int extend = 300;
	d->line(start.x, start.y, start.x + extend, start.y, clrPen, penWidth);
	d->line(end.x, end.y, end.x + extend, end.y, clrPen, penWidth);

	double first = chart->getYValue(start.y);
	double last = chart->getYValue(end.y);
	double height = abs(last - first);

	CString msg;
	msg.Format("x1 = %d, y1 = %d, x2 = %d, y2 = %d\n", start.x, start.y, end.x, end.y);
	//TRACE(msg);

	std::string valueText = std::format(_T("%.2f"), first);
	std::string font = _T("±¼¸²");
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + HandleRadius, start.y + HandleRadius, 0x000000);
	t->destroy();

	valueText = std::format(_T("%.2f"), last);
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + HandleRadius, end.y + HandleRadius, 0x000000);
	t->destroy();

	valueText = std::format(_T("%.2f"), height);
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + HandleRadius, start.y + HandleRadius * 4, 0x0000ff);
	t->destroy();
}

void VtLine::DrawLine(VtPoint start, VtPoint end, XYChart* chart, int clrPen, int penWidth)
{
	DrawArea* d = chart->makeChart();
	d->line(start.x, start.y, end.x, end.y, clrPen, penWidth);
	//int extend = 300;
	//d->line(start.x, start.y, start.x + extend, start.y, clrPen, penWidth);
	//d->line(end.x, end.y, end.x + extend, end.y, clrPen, penWidth);

	double first = chart->getYValue(start.y);
	double last = chart->getYValue(end.y);
	double height = abs(last - first);

	CString msg;
	msg.Format("x1 = %d, y1 = %d, x2 = %d, y2 = %d\n", start.x, start.y, end.x, end.y);
	//TRACE(msg);

	std::string valueText = std::format(_T("%.2f"), first);
	std::string font = _T("±¼¸²");
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + HandleRadius, start.y + HandleRadius, 0x000000);
	t->destroy();

	valueText = std::format(_T("%.2f"), last);
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + HandleRadius, end.y + HandleRadius, 0x000000);
	t->destroy();

	valueText = std::format(_T("%.2f"), height);
	t = d->text(valueText.c_str(), font.c_str(), 8);
	//t->draw(start.x + HandleRadius, start.y + HandleRadius * 4, 0x0000ff);
	t->destroy();
}

void VtLine::DrawLine(const VtPoint start, const VtPoint end, XYChart* chart, const int clrPen, const int penWidth, const std::string& symbol_code)
{
	DrawArea* d = chart->makeChart();
	d->line(start.x, start.y, end.x, end.y, clrPen, penWidth);
	

	std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (symbol) {
		double tick_value = symbol->TickValue();
		double val = tick_value * 5000;

		double first = chart->getYValue(start.y);
		double last = chart->getYValue(end.y);
		double height = abs(last - first);
		double tick_size = symbol->TickSize() * std::pow(10, symbol->decimal());
		double tick_count = height / tick_size;

		double total_value = tick_count * 5;

		CString msg;
		msg.Format("x1 = %d, y1 = %d, x2 = %d, y2 = %d\n", start.x, start.y, end.x, end.y);
		//TRACE(msg);

		std::string valueText = std::format(_T("%.2f"), first);
		std::string font = _T("±¼¸²");
		TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
		//t->draw(start.x + HandleRadius, start.y + HandleRadius, 0x000000);
		t->destroy();

// 		valueText = format(_T("%.2f"), total_value);
// 		t = d->text(valueText.c_str(), font.c_str(), 8);
// 		t->draw(end.x + HandleRadius, end.y + HandleRadius, 0x000000);
// 		t->destroy();
		valueText = std::format(_T("%.2f"), total_value);
		//valueText = format(_T("%.2f £Ü"), total_value);
		DarkHorse::SmUtil::to_thou_sep(valueText);
		t = d->text(TCHARtoUTF8(valueText.c_str()), font.c_str(), 8);
		t->draw(start.x + HandleRadius, start.y + HandleRadius * 4, 0xff00ff);
		t->destroy();
	}
}

void VtLine::DrawLine(const bool& show_text, const VtPoint& start, const VtPoint& end, XYChart* chart, const int& clrPen, const int& penWidth, const std::string& symbol_code)
{
	DrawArea* d = chart->makeChart();
	d->line(start.x, start.y, end.x, end.y, clrPen, penWidth);


	std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (symbol) {
		double tick_value = symbol->TickValue();
		double val = tick_value * 5000;

		double first = chart->getYValue(start.y);
		double last = chart->getYValue(end.y);
		double height = abs(last - first);

		double tick_size = symbol->TickSize() * std::pow(10, symbol->decimal());
		double tick_count = height / tick_size;

		double total_value = tick_count * 5;

		CString msg;
		msg.Format("x1 = %d, y1 = %d, x2 = %d, y2 = %d\n", start.x, start.y, end.x, end.y);
		//TRACE(msg);

		std::string valueText = std::format(_T("%.2f"), first);
		std::string font = _T("±¼¸²");
		TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
		//t->draw(start.x + HandleRadius, start.y + HandleRadius, 0x000000);
		t->destroy();

		if (show_text) {
			valueText = std::format(_T("%.2f"), total_value);
			//valueText = format(_T("%.2f £Ü"), total_value);
			DarkHorse::SmUtil::to_thou_sep(valueText);
			t = d->text(TCHARtoUTF8(valueText.c_str()), font.c_str(), 8);
			t->draw(start.x + HandleRadius, start.y + HandleRadius * 4, 0xff00ff);
			t->destroy();
		}
	}
}

void VtLine::DrawLine(const std::string& str_text, const VtPoint& start, const VtPoint& end, XYChart* chart, const int& clrPen, const int& penWidth, const std::string& symbol_code)
{
	DrawArea* d = chart->makeChart();
	d->line(start.x, start.y, end.x, end.y, clrPen, penWidth);


	std::string font = _T("±¼¸²");
	TTFText* t = d->text(str_text.c_str(), font.c_str(), 10);
	t->draw(start.x + HandleRadius, start.y + HandleRadius, 0xff00ff);
	t->destroy();
}
