#include "stdafx.h"
#include "SmFilledArea.h"
#include "SmGrid.h"
#include "SmCell.h"
#include <map>
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Order/SmAccountOrderManager.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccount.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmAccountPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Order/SmOrderRequest.h"
#include "../Client/ViStockClient.h"
#include "../Yuanta/YaStockClient.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Fund/SmFund.h"

#include "../Event/SmCallbackManager.h"
#include "../Order/AbAccountOrderWindow.h"
#include "../Fund/SmFundOrderDialog.h"
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(SmFilledArea, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

SmFilledArea::SmFilledArea()
{

}

SmFilledArea::~SmFilledArea()
{
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmFilledArea::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 10, 4);

	// int colWidth[4] = { 21, 46, 32, 58 };
	int colWidth[4] = { 21, 58, 32, 46 };

	int width_sum = 0;
	for (int i = 0; i < 4; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	width_sum -= colWidth[3];
	_Grid->SetColWidth(3, rect.Width() - width_sum);

	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();
	{
		_HeaderTitles.push_back("");
		_HeaderTitles.push_back("종목");
		_HeaderTitles.push_back("구분");
		_HeaderTitles.push_back("평가손익");

		_Grid->SetColHeaderTitles(_HeaderTitles);
	}

	_Grid->SetColCellType(0, SmCellType::CT_CHECK);
	_Grid->SetCellType(0, 0, SmCellType::CT_CHECK_HEADER);
}

void SmFilledArea::OnPaint()
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

void SmFilledArea::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	_Fund = val;
	UpdatePositionInfo();
}

size_t SmFilledArea::GetRowCount()
{
	if (!_Grid) return 0;
	else return _Grid->RowCount();
}

void SmFilledArea::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	_Account = val;
	UpdatePositionInfo();
}

void SmFilledArea::UpdateSymbolInfo()
{
	if (!_Symbol) return;

	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	if (cell) cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));

	Invalidate();
}

void SmFilledArea::UpdatePositionInfo()
{
	if (!_Account) return;

	ClearOldCotents();
	auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(_Account->No());
	const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();
	int row = 1;
	for (auto it = account_pos_map.begin(); it != account_pos_map.end(); ++it) {
		const auto position = it->second;
		
		if (position->OpenQty == 0) continue;

		auto cell = _Grid->FindCell(row, 1);
		if (cell) cell->Text(position->SymbolCode);
		cell = _Grid->FindCell(row, 2);
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
		cell = _Grid->FindCell(row, 3);
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
	Invalidate();
}

void SmFilledArea::LiqSelPositions()
{
	_Mode == 0 ? LiqSelPositionsForAccount() : LiqSelPositionsForFund();
}

void SmFilledArea::LiqAll()
{
	_Mode == 0 ? LiqAllForAccount() : LiqAllForFund();
}

void SmFilledArea::Update()
{
	if (_EnableOrderShow) {
		UpdatePositionInfo();
		//needDraw = true;
		_EnableOrderShow = false;
	}

	if (_EnableQuoteShow) {
		UpdatePositionInfo();
		_EnableQuoteShow = false;
	}
}

void SmFilledArea::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SmFilledArea::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void SmFilledArea::CreateResource()
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

	CBCGPTextFormat fmt2(_T("굴림"), globalUtils.ScaleByDPI(30.0f));

	fmt2.SetFontSize(12);
	fmt2.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt2.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt2.SetClipText();
	fmt2.SetWordWrap();
	//fmt2.SetFontWeight(FW_BOLD);

	_Resource.QuoteNumberFormat = fmt2;
}

void SmFilledArea::InitHeader()
{
	_HeaderTitles.push_back("");
	_HeaderTitles.push_back("종목");
	_HeaderTitles.push_back("구분");
	_HeaderTitles.push_back("평가손익");
}

void SmFilledArea::ClearOldContents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		for (int i = 0; i < _Grid->ColCount(); i++) {
			auto cell = _Grid->FindCell(*it, i);
			cell->Text("");
			cell->CellType(SmCellType::CT_NORMAL);
		}
	}
}

void SmFilledArea::ClearOldContents(const int& last_index)
{
	for (int row = last_index; row <= _OldMaxRow; row++) {
		
		for (int i = 0; i < _Grid->ColCount(); i++) {
			auto cell = _Grid->FindCell(row, i);
			cell->Text("");
			cell->CellType(SmCellType::CT_NORMAL);
		}
	}
}

void SmFilledArea::LiqSelPositionsForAccount()
{
	if (!_Account) return;

	for (auto it = _RowToPositionMap.begin(); it != _RowToPositionMap.end(); ++it) {
		auto cell = _Grid->FindCell(it->first, 0);
		//if (cell && cell->GetCheck()) {
			std::shared_ptr<SmOrderRequest> order_req = nullptr;
			if (it->second->Position == SmPositionType::Buy)
				order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(_Account->No(), _Account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
			else
				order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(_Account->No(), _Account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
			mainApp.Client()->NewOrder(order_req);
		//}
	}
}

void SmFilledArea::LiqSelPositionsForFund()
{
	if (!_Fund) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it2 = account_vec.begin(); it2 != account_vec.end(); it2++) {
		auto account = *it2;
		for (auto it = _RowToPositionMap.begin(); it != _RowToPositionMap.end(); ++it) {
			auto cell = _Grid->FindCell(it->first, 0);
			//if (cell && cell->GetCheck()) {
				std::shared_ptr<SmOrderRequest> order_req = nullptr;
				if (it->second->Position == SmPositionType::Buy)
					order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
				else
					order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
				mainApp.Client()->NewOrder(order_req);
			//}
		}
	}
}

void SmFilledArea::LiqAllForAccount()
{
	if (!_Account) return;

	for (auto it = _RowToPositionMap.begin(); it != _RowToPositionMap.end(); ++it) {
		std::shared_ptr<SmOrderRequest> order_req = nullptr;
		if (it->second->Position == SmPositionType::Buy)
			order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(_Account->No(), _Account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
		else
			order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(_Account->No(), _Account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
		mainApp.Client()->NewOrder(order_req);
	}
}

void SmFilledArea::LiqAllForFund()
{
	if (!_Fund) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it2 = account_vec.begin(); it2 != account_vec.end(); it2++) {
		auto account = *it2;
		for (auto it = _RowToPositionMap.begin(); it != _RowToPositionMap.end(); ++it) {
			std::shared_ptr<SmOrderRequest> order_req = nullptr;
			if (it->second->Position == SmPositionType::Buy)
				order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
			else
				order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
			mainApp.Client()->NewOrder(order_req);
		}
	}
}

void SmFilledArea::UpdateAccountPositionInfo()
{
	if (!_Account) return;

	//ClearOldContents();
	_RowToPositionMap.clear();
	auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(_Account->No());
	const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();
	int row = 0;
	for (auto it = account_pos_map.begin(); it != account_pos_map.end(); ++it) {
		const auto position = it->second;
		
		if (position->OpenQty == 0) {
			continue;
		}
		auto cell = _Grid->FindCell(row, 0);
		if (cell) cell->Text(position->SymbolCode);
		
		cell = _Grid->FindCell(row, 1);
		if (position->Position == SmPositionType::Buy) {
			

			if (cell ) cell->Text("매수");
		}
		else {
			if (cell) cell->Text("매도");
		}

		
		const std::string open_pl = std::format("{0:.2f}", position->OpenPL);
		
		cell = _Grid->FindCell(row, 2);
		if (cell) cell->Text(open_pl);

		cell = _Grid->FindCell(row, 3);
		cell->Text(std::to_string(position->OpenQty));

		_OldContentRowSet.insert(row);
		_RowToPositionMap[row] = position;
		row++;
	}
	ClearOldContents(row);
	_OldMaxRow = row;
}

void SmFilledArea::UpdateFundPositionInfo()
{
	if (!_Fund) return;

	//ClearOldContents();
	_RowToPositionMap.clear();

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	int row = 0, total_pos_count = 0;
	for (size_t i = 0; i < account_vec.size(); ++i) {
		auto account = account_vec[i];

		auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(account->No());
		const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();

		for (auto it = account_pos_map.begin(); it != account_pos_map.end(); ++it) {
			const auto position = it->second;
			total_pos_count += position->OpenQty;
			if (position->OpenQty == 0) {
				continue;
			}
			auto cell = _Grid->FindCell(row, 0);
			if (cell) cell->Text(position->SymbolCode);

			cell = _Grid->FindCell(row, 1);
			if (position->Position == SmPositionType::Buy) {


				if (cell) cell->Text("매수");
			}
			else {
				if (cell) cell->Text("매도");
			}


			const std::string open_pl = std::format("{0:.2f}", position->OpenPL);

			cell = _Grid->FindCell(row, 2);
			if (cell) cell->Text(open_pl);

			cell = _Grid->FindCell(row, 3);
			cell->Text(std::to_string(position->OpenQty));

			_OldContentRowSet.insert(row);
			_RowToPositionMap[row] = position;
			row++;
		}
	}
	if (total_pos_count == 0) row = 0;
	ClearOldContents(row);
	_OldMaxRow = row;
}

void SmFilledArea::ProcessCheck(const CPoint& point)
{
	auto cell = _Grid->FindCellByPos(point.x, point.y);
	if (cell->Row() == 0) {
		SmCellType cell_type = cell->CellType();
		if (cell_type == CT_CHECK_HEADER) {
			for (int i = 0; i < _Grid->RowCount(); i++) {
				auto cell_each = _Grid->FindCell(i, 0);
				cell_each->CellType(SmCellType::CT_NORMAL);
			}
			cell->CellType(SmCellType::CT_HEADER);
		}
		else {
			for (int i = 0; i < _Grid->RowCount(); i++) {
				auto cell_each = _Grid->FindCell(i, 0);
				cell_each->CellType(SmCellType::CT_CHECK);
			}
			cell->CellType(SmCellType::CT_CHECK_HEADER);
		}
	}
	else {
		SmCellType cell_type = cell->CellType();
		if (cell_type == CT_CHECK) {
			cell->CellType(SmCellType::CT_NORMAL);
		}
		else {
			cell->CellType(SmCellType::CT_CHECK);
		}
	}
	Invalidate();
}

void SmFilledArea::ClearOldCotents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		for (int i = 1; i < _Grid->ColCount(); i++) {
			auto cell = _Grid->FindCell(*it, i);
			if (cell) cell->Text("");
		}
	}
}

void SmFilledArea::OnLButtonDown(UINT nFlags, CPoint point)
{
	ProcessCheck(point);

	CBCGPStatic::OnLButtonDown(nFlags, point);
}


void SmFilledArea::PreSubclassWindow()
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

void SmFilledArea::OnTimer(UINT_PTR nIDEvent)
{
	Update();
}
