#include "stdafx.h"
#include "SmAcceptedArea.h"

#include "SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "SmCell.h"
#include "SmCellType.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Order/SmAccountOrderManager.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccount.h"
#include "../Event/SmCallbackManager.h"
#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(SmAcceptedArea, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

SmAcceptedArea::SmAcceptedArea()
{

}

SmAcceptedArea::~SmAcceptedArea()
{
	
}

void SmAcceptedArea::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();

	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 200, 9);
	
	int colWidth[9] = { 80, 65, 65, 40, 60, 50, 60, 50, 60 };

	int width_sum = 0;
	for (int i = 0; i < 9; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	width_sum -= colWidth[8];
	_Grid->SetColWidth(8, rect.Width() - width_sum);

	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();
	{
		_HeaderTitles.push_back("주문번호");
		_HeaderTitles.push_back("종목");
		_HeaderTitles.push_back("계좌번호");
		_HeaderTitles.push_back("구분");
		_HeaderTitles.push_back("주문가격");
		_HeaderTitles.push_back("주문량");
		_HeaderTitles.push_back("체결가격");
		_HeaderTitles.push_back("체결량");
		_HeaderTitles.push_back("체결시각");

		_Grid->SetColHeaderTitles(_HeaderTitles);
	}

	//_Grid->SetColCellType(0, SmCellType::CT_CHECK);
	//_Grid->SetCellType(0, 0, SmCellType::CT_CHECK_HEADER);

	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SmAcceptedArea::OnOrderEvent, this, _1, _2));

	_Init = true;
}

void SmAcceptedArea::OnPaint()
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
	
	_Grid->DrawBorder(m_pGM, rect);

	m_pGM->EndDraw();
}

void SmAcceptedArea::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	_Account = val;
	Clear();
	UpdateAccountFilledOrders();
}

void SmAcceptedArea::UpdateSymbolInfo()
{
	if (!_Symbol) return;

	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	if (cell) cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));

	Invalidate();
}

void SmAcceptedArea::UpdateAcceptedOrder()
{
	if (!_Account) return;
	ClearOldCotents();
	auto account_order_mgr = mainApp.TotalOrderMgr()->FindAccountOrderManager(_Account->No());
	if (!account_order_mgr) return;

	const std::map<std::string, std::shared_ptr<SmSymbolOrderManager>>& symbol_order_mgr_map = account_order_mgr->GetSymbolOrderMgrMap();
	int row = 1;
	for (auto it = symbol_order_mgr_map.begin(); it != symbol_order_mgr_map.end(); ++it) {
		auto symbol_order_mgr = it->second;
		const std::map<std::string, std::shared_ptr<SmOrder>>& accepted_map = symbol_order_mgr->GetAcceptedOrders();
		for (auto it2 = accepted_map.begin(); it2 != accepted_map.end(); it2++) {
			auto order = it2->second;
			auto cell = _Grid->FindCell(row, 1);
			if (cell) cell->Text(order->SymbolCode);
			cell = _Grid->FindCell(row, 2);
			if (cell) {
				if (order->PositionType == SmPositionType::Buy)
					cell->Text("Buy");
				else
					cell->Text("Sell");
			}
			cell = _Grid->FindCell(row, 3);
			if (cell) cell->Text(std::to_string(order->OrderAmount));
			_OldContentRowSet.insert(row);
			row++;
		}
	}
	Invalidate();
}

size_t SmAcceptedArea::GetRowCount()
{
	if (!_Grid) return 0;
	else return _Grid->RowCount();
}

void SmAcceptedArea::UpdateAccountFilledOrders()
{
	if (!_Account) return;
	//ClearOldCotents();
	auto account_order_mgr = mainApp.TotalOrderMgr()->FindAccountOrderManager(_Account->No());
	if (!account_order_mgr) return;

	int row = 1;
	const std::list<DarkHorse::SmFilledInfo>& filled_map = account_order_mgr->GetFilledInfo();

	for (auto it2 = filled_map.begin(); it2 != filled_map.end(); it2++) {
		const DarkHorse::SmFilledInfo& order = (*it2);
		//CBCGPGridRow* pRow = GetRow(row);
		//if (!pRow) continue;
		auto cell = _Grid->FindCell(row, 0);
		if (cell) cell->Text(order.Order->OrderNo);
		cell = _Grid->FindCell(row, 1);
		if (cell) cell->Text(order.Order->SymbolCode);
		cell = _Grid->FindCell(row, 2);
		if (cell) cell->Text(order.Order->AccountNo);
		cell = _Grid->FindCell(row, 3);
		//pRow->GetItem(3)->SetTextColor(RGB(255, 0, 0));
		
		if (order.Order->PositionType == SmPositionType::Buy) {
			
			if (cell) {
				cell->Text("매수");
				for (int i = 0; i < 9; i++) { 
					cell = _Grid->FindCell(row, i);
					if (cell) cell->CellType(SmCellType::CT_HOGA_BUY); 
				}
			}

		}
		else {
			if (cell) {
				cell->Text("매도");
				for (int i = 0; i < 9; i++) {
					cell = _Grid->FindCell(row, i);
					if (cell) cell->CellType(SmCellType::CT_HOGA_SELL);
				}
			}
			
		}
		cell = _Grid->FindCell(row, 4);
		if (cell) cell->Text(std::to_string(order.Order->OrderPrice));
		cell = _Grid->FindCell(row, 5);
		if (cell) cell->Text(std::to_string(order.Order->OrderAmount));
		
		cell = _Grid->FindCell(row, 6);
		if (cell) cell->Text(std::to_string(order.FilledPrice));
		cell = _Grid->FindCell(row, 7);
		if (cell) cell->Text(std::to_string(order.FilledAmount));
		cell = _Grid->FindCell(row, 8);
		if (cell) cell->Text(order.FilledTime);


		_OldContentRowSet.insert(row);
		
		
		row++;
	}
	Invalidate();
	//ClearOldContents(row);
	//_OldMaxRow = row;
}

void SmAcceptedArea::UpdateFundFilledOrders()
{

}

void SmAcceptedArea::Update()
{
	if (_EnableOrderShow) {
		UpdateAccountFilledOrders();
		//needDraw = true;
		_EnableOrderShow = false;
	}
}

void SmAcceptedArea::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SmAcceptedArea::RecalSize()
{
	if (!_Init) return;

	CRect rcWnd;
	GetWindowRect(rcWnd);
	_Grid->RecalRowCount(rcWnd.Height(), false);
	_Grid->CreateGrids();
	_Grid->SetColHeaderTitles(_HeaderTitles);
	Invalidate();
}

void SmAcceptedArea::Clear()
{
	_Grid->Clear();
}

void SmAcceptedArea::CreateResource()
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

void SmAcceptedArea::InitHeader()
{
	_HeaderTitles.push_back("");
	_HeaderTitles.push_back("종목");
	_HeaderTitles.push_back("구분");
	_HeaderTitles.push_back("미체결");
}



void SmAcceptedArea::ClearOldCotents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		for (int i = 1; i < _Grid->ColCount(); i++) {
			auto cell = _Grid->FindCell(*it, i);
			if (cell) cell->Text("");
		}
	}
}

void SmAcceptedArea::ProcessCheck(const CPoint& point)
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

void SmAcceptedArea::OnLButtonDown(UINT nFlags, CPoint point)
{
	//ProcessCheck(point);

	CBCGPStatic::OnLButtonDown(nFlags, point);
}


void SmAcceptedArea::OnDestroy()
{
	CBCGPStatic::OnDestroy();

	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}
