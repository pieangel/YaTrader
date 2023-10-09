#include "stdafx.h"
#include "SmFundPositionArea.h"
#include "../SmGrid/SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../SmGrid/SmCell.h"
#include "../GridDefine.h"
#include "../Global/SmTotalManager.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "SmFund.h"
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(SmFundPositionArea, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(GVN_ENDLABELEDIT, SM_INPLACE_CONTROL, OnEndInPlaceEdit)
	ON_WM_TIMER()
END_MESSAGE_MAP()

SmFundPositionArea::SmFundPositionArea()
{

}

SmFundPositionArea::~SmFundPositionArea()
{
	KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmFundPositionArea::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 2, 6);
	int colWidth[6] = { 60, 35, 40, 70, 90, 90 };
	int width_sum = 0;
	for (int i = 0; i < 6; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	width_sum -= colWidth[5];
	_Grid->SetColWidth(5, rect.Width() - width_sum);

	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();
	{
		_HeaderTitles.push_back("종목");
		_HeaderTitles.push_back("구분");
		_HeaderTitles.push_back("잔고");
		_HeaderTitles.push_back("평균가");
		_HeaderTitles.push_back("현재가");
		_HeaderTitles.push_back("평가손익");

		_Grid->SetColHeaderTitles(_HeaderTitles);
	}

	mainApp.CallbackMgr()->SubscribeQuoteCallback((long)this, std::bind(&SmFundPositionArea::OnQuoteEvent, this, _1));
	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SmFundPositionArea::OnOrderEvent, this, _1, _2));
	SetTimer(1, 40, NULL);
}

void SmFundPositionArea::OnPaint()
{
	try {
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
		_Grid->DrawCells(m_pGM, rect);
		_Grid->DrawBorder(m_pGM, rect);

		m_pGM->EndDraw();
	}
	catch (const std::exception& e) {
		const std::string error = e.what();

	}
}

void SmFundPositionArea::Clear()
{
	for (int i = 0; i < _Grid->ColCount(); i++) {
		auto cell = _Grid->FindCell(1, i);
		if (cell) {
			cell->CellType(SmCellType::CT_NORMAL);
			cell->Text("");
		}
	}
	Invalidate();
}

void SmFundPositionArea::UpdateSymbolInfo()
{

}

void SmFundPositionArea::UpdatePositionInfo()
{
	if (!_Fund || !_Symbol) return Clear();

	int fund_avg_price = 0, fund_open_qty = 0;
	double fund_open_pl = 0.0;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	if (account_vec.empty()) return Clear();

	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);
		std::shared_ptr<SmPosition> position = mainApp.TotalPosiMgr()->FindAddPosition(account->No(), _Symbol->SymbolCode());
		fund_avg_price += static_cast<int>(position->AvgPrice);
		fund_open_qty += position->OpenQty;
		fund_open_pl += position->OpenPL;
	}

	//fund_avg_price = static_cast<int>((double)fund_avg_price / (double)account_vec.size());

	fund_avg_price = static_cast<int>(fund_avg_price / account_vec.size());
	

	if (fund_open_qty == 0) return Clear();
	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	//fund_open_qty > 0 ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);
	cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	std::string value_string = std::format("{0}", _Symbol->Qoute.close);
	if (_Symbol->decimal() > 0 && value_string.length() > (size_t)_Symbol->decimal())
		value_string.insert(value_string.length() - _Symbol->decimal(), 1, '.');
	cell->Text(value_string);
	//fund_open_qty > 0 ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);


	//const int avg_price = static_cast<int>(position->AvgPrice);
	cell = _Grid->FindCell(1, 1);
	fund_open_qty > 0 ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);
	if (fund_open_qty > 0)
		cell->Text("매수");
	else if (fund_open_qty < 0)
		cell->Text("매도");
	cell = _Grid->FindCell(1, 2);
	cell->Text(std::to_string(fund_open_qty));
	//fund_open_qty > 0 ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);
	cell = _Grid->FindCell(1, 3);
	//fund_open_qty > 0 ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);
	value_string = std::format("{0}", fund_avg_price);
	if (_Symbol->decimal() > 0 && value_string.length() > (size_t)_Symbol->decimal())
		value_string.insert(value_string.length() - _Symbol->decimal(), 1, '.');
	cell->Text(value_string);
	cell = _Grid->FindCell(1, 5);
	//fund_open_qty > 0 ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);
	if (fund_open_pl != 0)
		fund_open_pl > 0 ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);
	else
		cell->CellType(SmCellType::CT_NORMAL);
	const std::string open_pl = std::format("{0:.2f}", fund_open_pl);
	cell->Text(open_pl);

	//Invalidate();
}

void SmFundPositionArea::OnEndEditCell(int nRow, int nCol, CString str)
{
	auto cell = _Grid->FindCell(nRow, nCol);
	if (cell) {
		_Grid->SetCellText(nRow, nCol, std::string(str));
	}
	_Grid->OnEndEdit();
	_Editing = false;
}

void SmFundPositionArea::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

void SmFundPositionArea::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void SmFundPositionArea::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SmFundPositionArea::Refresh()
{
	_EnableOrderShow = true;
	_EnableQuoteShow = true;
	Invalidate();
}

void SmFundPositionArea::CreateResource()
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
}

void SmFundPositionArea::InitHeader()
{
	_HeaderTitles.push_back("종목");
	_HeaderTitles.push_back("구분");
	_HeaderTitles.push_back("잔고");
	_HeaderTitles.push_back("평균가");
	_HeaderTitles.push_back("현재가");
	_HeaderTitles.push_back("평가손익");
}


void SmFundPositionArea::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!_Grid) return;

	// 	auto cell = _Grid->FindCellByPos(point.x, point.y);
	// 	if (cell) {
	// 		CRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	// 		_Grid->Edit(this, cell->Row(), cell->Col(), rect, SM_INPLACE_CONTROL, VK_LBUTTON);
	// 		_Editing = true;
	// 	}

	CBCGPStatic::OnLButtonDblClk(nFlags, point);
}


void SmFundPositionArea::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!_Grid) return;

	// 	auto cell = _Grid->FindCellByPos(point.x, point.y);
	// 	if (cell) {
	// 		CRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	// 		_Grid->Edit(this, cell->Row(), cell->Col(), rect, SM_INPLACE_CONTROL, VK_LBUTTON);
	// 		_Editing = true;
	// 	}

	CBCGPStatic::OnLButtonDown(nFlags, point);
}

void SmFundPositionArea::OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	GV_DISPINFO* pgvDispInfo = (GV_DISPINFO*)pNMHDR;
	GV_ITEM* pgvItem = &pgvDispInfo->item;

	// In case OnEndInPlaceEdit called as window is being destroyed
	if (!IsWindow(GetSafeHwnd()))
		return;

	OnEndEditCell(pgvItem->row, pgvItem->col, pgvItem->strText);
	//InvalidateCellRect(CCellID(pgvItem->row, pgvItem->col));

	switch (pgvItem->lParam)
	{
	case VK_TAB:
	case VK_DOWN:
	case VK_UP:
	case VK_RIGHT:
	case VK_LEFT:
	case VK_NEXT:
	case VK_PRIOR:
	case VK_HOME:
	case VK_END:
		OnKeyDown((UINT)pgvItem->lParam, 0, 0);
		//OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint(-1, -1), (UINT)pgvItem->lParam);
	}

	*pResult = 0;
}


void SmFundPositionArea::OnTimer(UINT_PTR nIDEvent)
{
	if (!_Symbol) return;
	bool needDraw = false;
	if (_EnableQuoteShow) {
		UpdatePositionInfo();
		_EnableQuoteShow = false;
		needDraw = true;
	}
	if (_EnableOrderShow) {
		UpdatePositionInfo();
		_EnableOrderShow = false;
		needDraw = true;
	}

	if (needDraw) Invalidate();

	CBCGPStatic::OnTimer(nIDEvent);
}
