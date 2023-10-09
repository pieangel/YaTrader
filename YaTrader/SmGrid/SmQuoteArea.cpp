#include "stdafx.h"
#include "SmQuoteArea.h"
#include "SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "SmCell.h"
#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(SmQuoteArea, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

SmQuoteArea::SmQuoteArea()
{

}

SmQuoteArea::~SmQuoteArea()
{
	KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmQuoteArea::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 26, 3);
	int colWidth[3] = { 60, 55, 41 };
	int width_sum = 0;
	for (int i = 0; i < 3; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	width_sum -= colWidth[2];
	_Grid->SetColWidth(2, rect.Width() - width_sum);

	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();

	{
		_HeaderTitles.push_back("시각");
		_HeaderTitles.push_back("체결가");
		_HeaderTitles.push_back("체결");
		_Grid->SetColHeaderTitles(_HeaderTitles);
	}

	mainApp.CallbackMgr()->SubscribeQuoteCallback((long)this, std::bind(&SmQuoteArea::OnQuoteEvent, this, _1));
	SetTimer(1, 40, NULL);
}

void SmQuoteArea::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CBCGPMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rect;
	GetClientRect(rect);

	if (m_pGM == NULL)
	{
		return;
	}

	m_pGM->BindDC(pDC, rect);

	if (!m_pGM->BeginDraw())
	{
		return;
	}



	m_pGM->FillRectangle(rect, _Resource.GridNormalBrush);
	rect.right -= 1;
	rect.bottom -= 1;

	_Grid->DrawGrid(m_pGM, rect);
	_Grid->DrawCells(m_pGM, rect, true, false);
	//_Grid->DrawHorizontalHeader(m_pGM, _HeaderTitles, 0);
	_Grid->DrawBorder(m_pGM, rect);

	m_pGM->EndDraw();
}

void SmQuoteArea::Clear()
{
	for(int row = 1; row < _Grid->RowCount(); row++)
		for (int col = 0; col < _Grid->ColCount(); col++) {
			auto cell = _Grid->FindCell(row, col);
			if (cell) cell->Text("");
		}
	Invalidate();
}

void SmQuoteArea::UpdateSymbolInfo()
{
	if (!_Symbol) return;

	
	for (size_t i = 1; i < _Symbol->TickVec.size(); i++) {
		std::shared_ptr<SmCell> cell = _Grid->FindCell(i, 0);


		std::string tick_time = _Symbol->TickVec[i].time;
		const int up_down = _Symbol->TickVec[i].updown;
		if (tick_time.length() > 0) {
			tick_time.insert(tick_time.length() - 2, ":");
			tick_time.insert(tick_time.length() - 5, ":");
		}
		else continue;

		if (cell) { cell->Text(tick_time); up_down == 1 ? cell->CellType(SmCellType::CT_TICK_BUY) : cell->CellType(SmCellType::CT_TICK_SELL); }
		

		cell = _Grid->FindCell(i, 1);
		std::string	value_string = std::format("{0}", _Symbol->TickVec[i].close);

		if (value_string.length() > (size_t)_Symbol->decimal()) {
			if (_Symbol->decimal() > 0)
				value_string.insert(value_string.length() - _Symbol->decimal(), 1, '.');
			if (cell) { cell->Text(value_string); up_down == 1 ? cell->CellType(SmCellType::CT_TICK_BUY) : cell->CellType(SmCellType::CT_TICK_SELL); }
		}
		cell = _Grid->FindCell(i, 2);
		if (cell) { cell->Text(std::to_string(_Symbol->TickVec[i].qty)); up_down == 1 ? cell->CellType(SmCellType::CT_TICK_BUY) : cell->CellType(SmCellType::CT_TICK_SELL); }
	}
	
	//Invalidate();
}

void SmQuoteArea::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void SmQuoteArea::CreateResource()
{
	_Resource.OrderStroke.SetStartCap(CBCGPStrokeStyle::BCGP_CAP_STYLE::BCGP_CAP_STYLE_ROUND);
	_Resource.OrderStroke.SetEndCap(CBCGPStrokeStyle::BCGP_CAP_STYLE::BCGP_CAP_STYLE_TRIANGLE);
	CBCGPTextFormat fmt3(_T("굴림"), globalUtils.ScaleByDPI(30.0f));

	fmt3.SetFontSize(12);
	fmt3.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt3.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt3.SetWordWrap();
	fmt3.SetClipText();

	_Resource.TextFormat = fmt3;

	CBCGPTextFormat fmt(_T("Tahoma"), globalUtils.ScaleByDPI(30.0f));

	fmt.SetFontSize(14);
	fmt.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt.SetFontWeight(FW_BOLD);
	fmt.SetWordWrap();
	fmt.SetClipText();

	_Resource.QuoteTextFormat = fmt;
}

void SmQuoteArea::InitHeader()
{
	_HeaderTitles.push_back("시각");
	_HeaderTitles.push_back("체결가");
	_HeaderTitles.push_back("체결");
}


void SmQuoteArea::OnTimer(UINT_PTR nIDEvent)
{
	bool needDraw = false;
	if (_EnableQuoteShow) {
		UpdateSymbolInfo();
		_EnableQuoteShow = false;
		needDraw = true;
	}

	if (needDraw) Invalidate();

	CBCGPStatic::OnTimer(nIDEvent);
}
