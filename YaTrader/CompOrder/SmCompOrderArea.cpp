#include "stdafx.h"
#include "SmCompOrderArea.h"


#include "../SmGrid/SmGrid.h"
#include "../SmGrid/SmCell.h"
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

#include "SmCompOrderDialog.h"
#include "SmOrderCompMainDialog.h"
#include "SmFundCompMainDialog.h"
#include "../Yuanta/YaStockClient.h"
#include "../Log/MyLogger.h"
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;


BEGIN_MESSAGE_MAP(SmCompOrderArea, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

SmCompOrderArea::SmCompOrderArea()
{
	
}

SmCompOrderArea::~SmCompOrderArea()
{
	mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);
	mainApp.CallbackMgr()->UnsubscribeHogaCallback((long)this);
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmCompOrderArea::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	m_pGM = CBCGPGraphicsManager::CreateInstance();

	//rect.right -= 1;
	rect.bottom -= 1;

	CreateResource();


	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 60, 9);

	//ResetHeaderWidth(rect.Width());

	int colWidth[5] = { 60, 60, 80, 60, 60 };

	int width_sum = 0;
	for (int i = 0; i < 5; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	width_sum -= colWidth[2];
	_Grid->SetColWidth(2, rect.Width() - width_sum);


	_Grid->HeaderMode(SmHeaderMode::HeaderColOnly);
	//_Grid->SetOrderHeaderTitles();
	_Grid->MakeRowHeightMap();
	_Grid->RecalRowCount(rect.Height(), true);
	// _MergedCellMap[std::make_pair(1, 2)] = std::make_pair(1, 2);
	//_MergedCellMap[std::make_pair(1, 5)] = std::make_pair(1, 2);
	_Grid->AddMergeCell(1, 3, 5, 2);
	_Grid->AddMergeCell(6, 0, 5, 2);
	_Grid->MakeColWidthMap();

	_Grid->CreateGrids();

	{

		_HeaderTitles.push_back("건수");
		_HeaderTitles.push_back("수량");
		_HeaderTitles.push_back("호가");
		_HeaderTitles.push_back("수량");
		_HeaderTitles.push_back("건수");

		

		_Grid->SetColHeaderTitles(_HeaderTitles);
	}

	//_Grid->RegisterOrderButtons(_ButtonMap);

	
	mainApp.CallbackMgr()->SubscribeQuoteCallback((long)this, std::bind(&SmCompOrderArea::OnQuoteEvent, this, _1));
	mainApp.CallbackMgr()->SubscribeHogaCallback((long)this, std::bind(&SmCompOrderArea::OnHogaEvent, this, _1));
	//mainApp.CallbackMgr()->SubscribeMasterCallback((long)this, std::bind(&SmOrderArea::OnSymbolMasterEvent, this, _1));
	//mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SmOrderArea::OnOrderEvent, this, _1, _2));

	//SetTimer(1, 10, NULL);

	return;
}

void SmCompOrderArea::OnPaint()
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

	try {

		m_pGM->BindDC(pDC, rect);



		if (!m_pGM->BeginDraw())
		{
			return;
		}

		m_pGM->Clear();

		m_pGM->FillRectangle(rect, _Resource.GridNormalBrush);
		rect.right -= 1;
		rect.bottom -= 1;
		_Grid->SetColHeaderTitles(_OrderTableHeader);
		_Grid->DrawGrid(m_pGM, rect);
		_Grid->DrawCells(m_pGM, rect);
		_Grid->DrawBorder(m_pGM, rect, _Selected);
		//DrawFixedSelectedCell();

		//if (_Hover) DrawMovingRect();

		//DrawMovingOrder();

		//DrawStopOrder();

		DrawHogaLine(rect);


		m_pGM->EndDraw();
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SmCompOrderArea::SetMainOrderWnd(SmOrderCompMainDialog* wnd)
{
	_OrderMainWnd = wnd;
}

void SmCompOrderArea::SetFundCompOrderWnd(SmFundCompMainDialog* wnd)
{
	_FundCompOrderWnd = wnd;
}

void SmCompOrderArea::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	_Fund = val;
	UpdatePositionInfo();
}

size_t SmCompOrderArea::GetRowCount()
{
	if (!_Grid) return 0;
	else return _Grid->RowCount();
}

void SmCompOrderArea::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	_Account = val;
	UpdatePositionInfo();
}

void SmCompOrderArea::Symbol(std::shared_ptr<DarkHorse::SmSymbol> val)
{
	_Symbol = val;
	
	SetHoga(val);
	SetQuote(val);

	_EnableHogaShow = true;
	_EnableQuoteShow = true;
}

void SmCompOrderArea::UpdateSymbolInfo()
{
	if (!_Symbol) return;

	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	if (cell) cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));

	Invalidate();
}

void SmCompOrderArea::UpdatePositionInfo()
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

void SmCompOrderArea::LiqSelPositions()
{
	_Mode == 0 ? LiqSelPositionsForAccount() : LiqSelPositionsForFund();
}

void SmCompOrderArea::LiqAll()
{
	_Mode == 0 ? LiqAllForAccount() : LiqAllForFund();
}

void SmCompOrderArea::Update()
{
	if (!_Symbol) return;

	bool needToDraw = false;
	if (_EnableHogaShow) {
		SetHoga(_Symbol);
		_EnableHogaShow = false;
		needToDraw = true;
	}

	if (_EnableQuoteShow) {
		SetQuote(_Symbol);
		_EnableQuoteShow = false;
		needToDraw = true;
	}

	if (needToDraw) Invalidate();
}

void SmCompOrderArea::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SmCompOrderArea::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void SmCompOrderArea::OnHogaEvent(const std::string& symbol_code)
{
	_EnableHogaShow = true;
}

void SmCompOrderArea::CreateResource()
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

void SmCompOrderArea::InitHeader()
{
	_HeaderTitles.push_back("");
	_HeaderTitles.push_back("종목");
	_HeaderTitles.push_back("구분");
	_HeaderTitles.push_back("평가손익");
}

void SmCompOrderArea::ClearOldContents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		for (int i = 0; i < _Grid->ColCount(); i++) {
			auto cell = _Grid->FindCell(*it, i);
			cell->Text("");
			cell->CellType(SmCellType::CT_NORMAL);
		}
	}
}

void SmCompOrderArea::ClearOldContents(const int& last_index)
{
	for (int row = last_index; row <= _OldMaxRow; row++) {

		for (int i = 0; i < _Grid->ColCount(); i++) {
			auto cell = _Grid->FindCell(row, i);
			cell->Text("");
			cell->CellType(SmCellType::CT_NORMAL);
		}
	}
}

void SmCompOrderArea::SetHoga(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	try {
		std::shared_ptr<SmCell> pCell = nullptr;

		for (int i = 0; i < 5; i++) {

			pCell = _Grid->FindCell(5 - i, 0);
			pCell->Text(std::to_string(symbol->Hoga.Ary[i].SellCnt));
			pCell->CellType(SmCellType::CT_HOGA_SELL);

			pCell = _Grid->FindCell(5 - i, 1);
			pCell->CellType(SmCellType::CT_HOGA_SELL);
			pCell->Text(std::to_string(symbol->Hoga.Ary[i].SellQty));

			pCell = _Grid->FindCell(5 - i, 2);
			pCell->CellType(SmCellType::CT_HOGA_SELL);


			std::string value_string;
		
			value_string = std::format("{0}", symbol->Hoga.Ary[i].SellPrice);
			if (symbol->decimal() > 0)
				value_string.insert(value_string.length() - symbol->decimal(), 1, '.');

			pCell->Text(value_string);

			pCell = _Grid->FindCell(6 + i, 2);
			pCell->CellType(SmCellType::CT_HOGA_BUY);


			value_string = std::format("{0}", symbol->Hoga.Ary[i].BuyPrice);
			if (symbol->decimal() > 0)
				value_string.insert(value_string.length() - symbol->decimal(), 1, '.');

			pCell->Text(value_string);

			pCell = _Grid->FindCell(6 + i, 3);
			pCell->CellType(SmCellType::CT_HOGA_BUY);
			pCell->Text(std::to_string(symbol->Hoga.Ary[i].BuyQty));

			pCell = _Grid->FindCell(6 + i, 4);
			pCell->CellType(SmCellType::CT_HOGA_BUY);
			pCell->Text(std::to_string(symbol->Hoga.Ary[i].BuyCnt));
		}
		
	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SmCompOrderArea::SetQuote(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	auto cell = _Grid->FindCell(6, 0);
	std::string value_text;

	std::string  close_text = std::format("시가 : {0}", symbol->Qoute.open);
	if (symbol->decimal() > 0)
		close_text.insert(close_text.length() - symbol->decimal(), 1, '.');

	value_text.append(close_text);
	value_text.append("\n");
	value_text.append("\n");


	close_text = std::format("고가 : {0}", symbol->Qoute.high);
	if (symbol->decimal() > 0)
		close_text.insert(close_text.length() - symbol->decimal(), 1, '.');

	value_text.append(close_text);
	value_text.append("\n");
	value_text.append("\n");


	close_text = std::format("저가 : {0}", symbol->Qoute.low);
	if (symbol->decimal() > 0)
		close_text.insert(close_text.length() - symbol->decimal(), 1, '.');

	value_text.append(close_text);
	value_text.append("\n");
	value_text.append("\n");


	close_text = std::format("종가 : {0}", symbol->Qoute.close);
	if (symbol->decimal() > 0)
		close_text.insert(close_text.length() - symbol->decimal(), 1, '.');

	value_text.append(close_text);
	//value_text.append("\n");



	cell->Text(value_text);
}

void SmCompOrderArea::DrawHogaLine(const CRect& rect)
{
	auto pCell = _Grid->FindCell(6, 0);
	m_pGM->DrawLine(0, pCell->Y(), rect.Width(), pCell->Y(), _Resource.SelectedBrush);
}

void SmCompOrderArea::LiqSelPositionsForAccount()
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

void SmCompOrderArea::LiqSelPositionsForFund()
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

void SmCompOrderArea::LiqAllForAccount()
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

void SmCompOrderArea::LiqAllForFund()
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

void SmCompOrderArea::UpdateAccountPositionInfo()
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
	ClearOldContents(row);
	_OldMaxRow = row;
}

void SmCompOrderArea::UpdateFundPositionInfo()
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

void SmCompOrderArea::ProcessCheck(const CPoint& point)
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

void SmCompOrderArea::ClearOldCotents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		for (int i = 1; i < _Grid->ColCount(); i++) {
			auto cell = _Grid->FindCell(*it, i);
			if (cell) cell->Text("");
		}
	}
}

void SmCompOrderArea::Refresh()
{
	Update();
}

void SmCompOrderArea::OnLButtonDown(UINT nFlags, CPoint point)
{
	
	if (_OrderMainWnd) {

		if (_Symbol) _OrderMainWnd->ChangedSymbol(_Symbol);

		SmCompOrderDialog* wnd = (SmCompOrderDialog*)GetParent();
		_OrderMainWnd->ChangedCenterWindow(wnd->ID());
	}

	if (_FundCompOrderWnd) {

		if (_Symbol) _FundCompOrderWnd->ChangedSymbol(_Symbol);

		SmCompOrderDialog* wnd = (SmCompOrderDialog*)GetParent();
		_FundCompOrderWnd->ChangedCenterWindow(wnd->ID());
	}

	CBCGPStatic::OnLButtonDown(nFlags, point);
}


void SmCompOrderArea::PreSubclassWindow()
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

void SmCompOrderArea::OnTimer(UINT_PTR nIDEvent)
{
	Update();
}
