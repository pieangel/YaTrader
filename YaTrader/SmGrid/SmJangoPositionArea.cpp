#include "stdafx.h"
#include "SmJangoPositionArea.h"

#include "SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "SmCell.h"
#include "../GridDefine.h"
#include "../Global/SmTotalManager.h"

#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmAccountPositionManager.h"
#include "../Position/SmPosition.h"

#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Fund/SmFund.h"
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(SmJangoPositionArea, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(GVN_ENDLABELEDIT, SM_INPLACE_CONTROL, OnEndInPlaceEdit)
	ON_WM_TIMER()
END_MESSAGE_MAP()

SmJangoPositionArea::SmJangoPositionArea()
{

}

SmJangoPositionArea::~SmJangoPositionArea()
{
	KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmJangoPositionArea::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 100, 6);
	int colWidth[5] = { 60, 35, 40, 70, 70 };
	int width_sum = 0;
	for (int i = 0; i < 5; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	width_sum -= colWidth[4];
	_Grid->SetColWidth(4, rect.Width() - width_sum);

	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();
	{
		_HeaderTitles.push_back("종목");
		_HeaderTitles.push_back("구분");
		_HeaderTitles.push_back("잔고");
		_HeaderTitles.push_back("평균가");
		_HeaderTitles.push_back("평가손익");

		_Grid->SetColHeaderTitles(_HeaderTitles);
	}

	mainApp.CallbackMgr()->SubscribeQuoteCallback((long)this, std::bind(&SmJangoPositionArea::OnQuoteEvent, this, _1));
	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SmJangoPositionArea::OnOrderEvent, this, _1, _2));
	SetTimer(1, 40, NULL);
}

void SmJangoPositionArea::OnPaint()
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

void SmJangoPositionArea::Clear()
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

void SmJangoPositionArea::UpdateSymbolInfo()
{

}

void SmJangoPositionArea::ClearOldCotents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		for (int i = 1; i < _Grid->ColCount(); i++) {
			auto cell = _Grid->FindCell(*it, i);
			if (cell) cell->Text("");
		}
	}
}

void SmJangoPositionArea::UpdateAccountPositionInfo()
{
	if (!_Account) return;

	ClearOldCotents();
	auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(_Account->No());
	const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();
	int row = 1;
	for (auto it = account_pos_map.begin(); it != account_pos_map.end(); ++it) {
		const auto position = it->second;

		if (position->OpenQty == 0) continue;

		auto cell = _Grid->FindCell(row, 0);
		if (cell) cell->Text(position->SymbolCode);
		cell = _Grid->FindCell(row, 1);
		if (cell) {
			if (position->Position == SmPositionType::Buy) {
				cell->Text("매수");
				cell->CellType(SmCellType::CT_REMAIN_BUY);
			}
			else if (position->Position == SmPositionType::Sell) {
				cell->Text("매도");
				cell->CellType(SmCellType::CT_REMAIN_SELL);
			}
			else {
				cell->Text("");
				cell->CellType(SmCellType::CT_NORMAL);
			}
		}

		const int avg_price = static_cast<int>(position->AvgPrice);

		cell = _Grid->FindCell(row, 2);
		if (cell) cell->Text(std::to_string(position->OpenQty));
		cell = _Grid->FindCell(row, 3);
		std::string value_string = std::format("{0}", avg_price);

		std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(position->SymbolCode);
		if (symbol) {
			if (symbol->decimal() > 0 && value_string.length() > (size_t)symbol->decimal())
				value_string.insert(value_string.length() - symbol->decimal(), 1, '.');
			if (cell) cell->Text(value_string);
		}
		


		cell = _Grid->FindCell(row, 4);
		if (cell) {
			const std::string open_pl = std::format("{0:.2f}", position->OpenPL);
			cell->Text(open_pl);
			if (position->OpenPL > 0) {

				cell->CellType(SmCellType::CT_REMAIN_BUY);
			}
			else if (position->OpenPL < 0) {

				cell->CellType(SmCellType::CT_REMAIN_SELL);
			}
			else {
				cell->Text("");
				cell->CellType(SmCellType::CT_NORMAL);
			}
		}
		_OldContentRowSet.insert(row);
		row++;
	}
	//Invalidate();
}

void SmJangoPositionArea::UpdateFundPositionInfo()
{
	if (!_Fund) return;

	ClearOldCotents();

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	int row = 1, total_pos_count = 0;
	for (size_t i = 0; i < account_vec.size(); ++i) {
		auto account = account_vec[i];

		auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(account->No());
		const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();
		for (auto it = account_pos_map.begin(); it != account_pos_map.end(); ++it) {
			const auto position = it->second;

			if (position->OpenQty == 0) continue;

			auto cell = _Grid->FindCell(row, 0);
			if (cell) cell->Text(position->SymbolCode);
			cell = _Grid->FindCell(row, 1);
			if (cell) {
				if (position->Position == SmPositionType::Buy) {
					cell->Text("매수");
					cell->CellType(SmCellType::CT_REMAIN_BUY);
				}
				else if (position->Position == SmPositionType::Sell) {
					cell->Text("매도");
					cell->CellType(SmCellType::CT_REMAIN_SELL);
				}
				else {
					cell->Text("");
					cell->CellType(SmCellType::CT_NORMAL);
				}
			}

			const int avg_price = static_cast<int>(position->AvgPrice);

			cell = _Grid->FindCell(row, 2);
			if (cell) cell->Text(std::to_string(position->OpenQty));
			cell = _Grid->FindCell(row, 3);
			std::string value_string = std::format("{0}", avg_price);

			std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(position->SymbolCode);
			if (symbol) {
				if (symbol->decimal() > 0 && value_string.length() > (size_t)symbol->decimal())
					value_string.insert(value_string.length() - symbol->decimal(), 1, '.');
				if (cell) cell->Text(value_string);
			}



			cell = _Grid->FindCell(row, 4);
			if (cell) {
				const std::string open_pl = std::format("{0:.2f}", position->OpenPL);
				cell->Text(open_pl);
				if (position->OpenPL > 0) {

					cell->CellType(SmCellType::CT_REMAIN_BUY);
				}
				else if (position->OpenPL < 0) {

					cell->CellType(SmCellType::CT_REMAIN_SELL);
				}
				else {
					cell->Text("");
					cell->CellType(SmCellType::CT_NORMAL);
				}
			}
			_OldContentRowSet.insert(row);
			row++;
		}
	}
}

void SmJangoPositionArea::OnEndEditCell(int nRow, int nCol, CString str)
{
	auto cell = _Grid->FindCell(nRow, nCol);
	if (cell) {
		_Grid->SetCellText(nRow, nCol, std::string(str));
	}
	_Grid->OnEndEdit();
	_Editing = false;
}

void SmJangoPositionArea::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

void SmJangoPositionArea::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void SmJangoPositionArea::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SmJangoPositionArea::CreateResource()
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

void SmJangoPositionArea::InitHeader()
{
	_HeaderTitles.push_back("종목");
	_HeaderTitles.push_back("구분");
	_HeaderTitles.push_back("잔고");
	_HeaderTitles.push_back("평균가");
	_HeaderTitles.push_back("현재가");
	_HeaderTitles.push_back("평가손익");
}


void SmJangoPositionArea::OnLButtonDblClk(UINT nFlags, CPoint point)
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


void SmJangoPositionArea::OnLButtonDown(UINT nFlags, CPoint point)
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

void SmJangoPositionArea::OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult)
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


void SmJangoPositionArea::OnTimer(UINT_PTR nIDEvent)
{
	//if (!_Symbol) return;
	bool needDraw = false;
	if (_EnableQuoteShow) {
		_Mode == 0 ? UpdateAccountPositionInfo() : UpdateFundPositionInfo();
		_EnableQuoteShow = false;
		needDraw = true;
	}
	if (_EnableOrderShow) {
		_Mode == 0 ? UpdateAccountPositionInfo() : UpdateFundPositionInfo();
		_EnableOrderShow = false;
		needDraw = true;
	}

	if (needDraw) Invalidate();

	CBCGPStatic::OnTimer(nIDEvent);
}


void SmJangoPositionArea::PreSubclassWindow()
{
	/*
	UINT uiHHeight = GetSystemMetrics(SM_CYHSCROLL);
	UINT uiVWidth = GetSystemMetrics(SM_CXVSCROLL);
	CRect rectClient, rectH, rectV;
	GetClientRect(rectClient);
	rectH = rectClient;
	rectH.top = rectH.bottom - uiHHeight;
	rectH.right -= uiVWidth;

	rectV = rectClient;
	rectV.left = rectV.right - uiVWidth;
	rectV.bottom -= uiHHeight;

	m_HScroll.Create(SBS_HORZ | SBS_BOTTOMALIGN | WS_CHILD | WS_VISIBLE, rectH, this, 1100);
	m_VScroll.Create(SBS_VERT | SBS_RIGHTALIGN | WS_CHILD | WS_VISIBLE, rectV, this, 1101);

	SCROLLINFO sci;
	memset(&sci, 0, sizeof(sci));

	sci.cbSize = sizeof(SCROLLINFO);
	sci.fMask = SIF_ALL;
	sci.nMax = 1000;
	sci.nPage = 200;

	m_HScroll.SetScrollInfo(&sci, TRUE);
	m_VScroll.SetScrollInfo(&sci, TRUE);
	*/

	CBCGPStatic::PreSubclassWindow();
}
