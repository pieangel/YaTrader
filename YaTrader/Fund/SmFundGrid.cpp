#include "stdafx.h"
#include "SmFundGrid.h"

#include <map>
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Order/SmAccountOrderManager.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccount.h"
#include "../Order/SmOrderRequest.h"
#include "../Client/ViStockClient.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Event/SmCallbackManager.h"
#include "SmFundDialog.h"

#include "SmFundAccountGrid.h"

#include "SmFundManager.h"
#include "SmFund.h"

#include <CommCtrl.h>
#include <vector>
#include "../resource.h"
#include "../Account/SmAccountManager.h"

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl

IMPLEMENT_DYNAMIC(SmFundGrid, CBCGPGridCtrl)

SmFundGrid::SmFundGrid()
{
	m_bExtendedPadding = FALSE;
}

SmFundGrid::~SmFundGrid()
{
	//KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
}

void SmFundGrid::OnHeaderCheckBoxClick(int nColumn)
{
	CString msg;
	msg.Format("%d", nColumn);
	//AfxMessageBox(msg);
	if (_HeaderCheck) {
		_HeaderCheck = false;

		CheckAll(FALSE);

	}
	else {
		_HeaderCheck = true;

		CheckAll(TRUE);
	}
	UpdateHeaderCheckbox();
	Invalidate();
}

void SmFundGrid::OnRowCheckBoxClick(CBCGPGridRow* pRow)
{
	CString msg;
	msg.Format("%d", pRow->GetRowId());
	//AfxMessageBox(msg);
	if (pRow->GetCheck() == 1)
		pRow->SetCheck(0);
	else if (pRow->GetCheck() == 0)
		pRow->SetCheck(1);
	Invalidate();
}

void SmFundGrid::UpdateAcceptedOrder()
{
	if (!_Account) return;
	//ClearOldCotents();
	auto account_order_mgr = mainApp.TotalOrderMgr()->FindAccountOrderManager(_Account->No());
	if (!account_order_mgr) return;

	const std::map<std::string, std::shared_ptr<SmSymbolOrderManager>>& symbol_order_mgr_map = account_order_mgr->GetSymbolOrderMgrMap();
	int row = 0;
	for (auto it = symbol_order_mgr_map.begin(); it != symbol_order_mgr_map.end(); ++it) {
		auto symbol_order_mgr = it->second;
		const std::map<std::string, std::shared_ptr<SmOrder>>& accepted_map = symbol_order_mgr->GetAcceptedOrders();

		for (auto it2 = accepted_map.begin(); it2 != accepted_map.end(); it2++) {
			auto order = it2->second;
			CBCGPGridRow* pRow = GetRow(row);
			if (!pRow) continue;
			pRow->GetItem(0)->SetValue(order->SymbolCode.c_str());
			pRow->GetItem(0)->SetTextColor(RGB(255, 0, 0));
			if (order->PositionType == SmPositionType::Buy) {
				pRow->GetItem(0)->SetTextColor(RGB(255, 128, 255));
				pRow->GetItem(1)->SetTextColor(RGB(255, 128, 255));
				pRow->GetItem(2)->SetTextColor(RGB(255, 128, 255));
				pRow->GetItem(3)->SetTextColor(RGB(255, 128, 255));
				pRow->GetItem(1)->SetValue("매수");
			}
			else {
				pRow->GetItem(1)->SetValue("매도");
				pRow->GetItem(0)->SetTextColor(RGB(128, 255, 255));
				pRow->GetItem(1)->SetTextColor(RGB(128, 255, 255));
				pRow->GetItem(2)->SetTextColor(RGB(128, 255, 255));
				pRow->GetItem(3)->SetTextColor(RGB(128, 255, 255));
			}
			pRow->GetItem(2)->SetValue(std::to_string(order->OrderAmount).c_str());
			pRow->GetItem(3)->SetValue(std::to_string(order->OrderPrice).c_str());
			_OldContentRowSet.insert(row);
			_RowToOrderMap[row] = order;
			pRow->GetItem(0)->Redraw();
			pRow->GetItem(1)->Redraw();
			pRow->GetItem(2)->Redraw();
			pRow->GetItem(3)->Redraw();
			row++;
		}
	}
	ClearOldContents(row);
	_OldMaxRow = row;

	//Invalidate();
}

void SmFundGrid::CancelSelOrders()
{
	if (!_Account) return;

	for (auto it = _RowToOrderMap.begin(); it != _RowToOrderMap.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(it->first);
		if (pRow->GetCheck()) {
			auto order_req = SmOrderRequestManager::MakeCancelOrderRequest(_Account->No(),
				_Account->Pwd(),
				it->second->SymbolCode,
				it->second->OrderNo,
				it->second->OrderPrice,
				it->second->PositionType,
				it->second->OrderAmount);
			mainApp.Client()->NewOrder(order_req);
		}
	}
}

void SmFundGrid::CancelAll()
{
	if (!_Account) return;

	for (auto it = _RowToOrderMap.begin(); it != _RowToOrderMap.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(it->first);
		auto order_req = SmOrderRequestManager::MakeCancelOrderRequest(_Account->No(),
			_Account->Pwd(),
			it->second->SymbolCode,
			it->second->OrderNo,
			it->second->OrderPrice,
			it->second->PositionType,
			it->second->OrderAmount);
		mainApp.Client()->NewOrder(order_req);
	}
}

void SmFundGrid::AddFund(std::shared_ptr<DarkHorse::SmFund> fund)
{
	if (!fund) return;

	size_t row = _RowToFundMap.size();

	CBCGPGridRow* pRow = GetRow(row);
	if (!pRow) return;
	pRow->GetItem(0)->SetValue(fund->Name().c_str(), TRUE);
	pRow->GetItem(1)->SetValue(std::to_string(fund->GetAccountCount()).c_str(), TRUE);

	_RowToFundMap[row] = fund;

	SelectRow(row);

	_MaxFundRow = row - 1;
}

BEGIN_MESSAGE_MAP(SmFundGrid, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl message handlers

int SmFundGrid::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableHeader(TRUE, ~BCGP_GRID_HEADER_SORT);
	EnableRowHeader(TRUE);

	SetClearInplaceEditOnEnter(FALSE);

	SetWholeRowSel(FALSE);
	EnableAlternateRows(FALSE);

	SetRowHeaderWidth(1);
	SetVisualManagerColorTheme(TRUE);

	// Insert columns:
	InsertColumn(0, _T("펀드이름"), 120);
	InsertColumn(1, _T("계좌수"), 50);
	
	FreezeColumns(0);

	const int nColumns = GetColumnCount();


	for (int row = 0; row < 200; row++) {
		// Create new row:
		CBCGPGridRow* pRow = CreateRow(nColumns);
		// Set each column data:
		for (int nColumn = 0; nColumn < nColumns; nColumn++)
		{
			long lValue = (nColumn + 1) * (row + 1);
			//pRow->GetItem(nColumn)->SetValue(lValue);


			//pRow->GetItem(nColumn)->SetValue(lValue);

			pRow->GetItem(nColumn)->AllowEdit(FALSE);

		}

		// Add row to grid:
		AddRow(pRow, FALSE /* Don't recal. layout */);

		row++;
	}

	SetColumnAlign(0, 2);
	SetColumnAlign(1, 2);
	SetColumnAlign(2, 2);
	SetColumnAlign(3, 2);
	EnableCheckBoxes(FALSE);
	SetHeaderAlign(0, 2);
	SetHeaderAlign(1, 2);
	SetHeaderAlign(2, 2);
	SetHeaderAlign(3, 2);
	//SetSortColumn();

	RemoveSortColumn(0);
	RemoveSortColumn(1);
	RemoveSortColumn(2);
	RemoveSortColumn(3);

	
	


	//CheckAll(FALSE);
	//UpdateHeaderCheckbox();
	AdjustLayout();

	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SmFundGrid::OnOrderEvent, this, _1, _2));

	return 0;
}

void SmFundGrid::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void SmFundGrid::ClearOldContents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(*it);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->EmptyValue();

			pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
		}
	}
}

void SmFundGrid::ClearOldContents(const int& last_index)
{
	for (int row = last_index; row < _OldMaxRow; row++) {
		CBCGPGridRow* pRow = GetRow(row);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->EmptyValue();
			pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
			pRow->GetItem(i)->Redraw();
		}
	}
}

void SmFundGrid::ClearAll()
{
	for (int i = 0; i <= _MaxFundRow; i++) {
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		pRow->GetItem(0)->EmptyValue(TRUE);

		pRow->GetItem(1)->EmptyValue(TRUE);
	}

}

void SmFundGrid::ToggleExtendedPadding()
{
	m_bExtendedPadding = !m_bExtendedPadding;

	EnableRowHeader(TRUE);	// Need to recalculate the width
	AdjustLayout();
}



void SmFundGrid::UpdateFundInfo(std::shared_ptr<DarkHorse::SmFund> fund)
{
	if (!fund) return;

	for (auto it = _RowToFundMap.begin(); it != _RowToFundMap.end(); it++) {
		if (it->second->Id() == fund->Id()) {
			CBCGPGridRow* pRow = GetRow(it->first);
			if (!pRow) continue;
			pRow->GetItem(0)->SetValue(fund->Name().c_str(), TRUE);

			pRow->GetItem(1)->SetValue(std::to_string(fund->GetAccountCount()).c_str(), TRUE);

			break;
		}
	}
}

std::shared_ptr<DarkHorse::SmFund> SmFundGrid::GetFirstFund()
{
	if (_RowToFundMap.empty()) return nullptr;

	return _RowToFundMap.begin()->second;
}

std::shared_ptr<DarkHorse::SmFund> SmFundGrid::GetLastFund()
{
	if (_RowToFundMap.empty()) return nullptr;

	return std::prev(_RowToFundMap.end())->second;
}

void SmFundGrid::ClearFundGrid(const int& row)
{
	for (int i = row; i <= _MaxFundRow; i++) {
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		pRow->GetItem(0)->EmptyValue(TRUE);

		pRow->GetItem(1)->EmptyValue(TRUE);

	}

	_MaxFundRow = row - 1;
}

void SmFundGrid::SetFundList()
{
	_RowToFundMap.clear();
	const std::map<std::string, std::shared_ptr<DarkHorse::SmFund>>& fund_map = mainApp.FundMgr()->GetFundMap();
	int row = 0;
	for (auto it = fund_map.begin(); it != fund_map.end(); it++) {
		const std::string fund_name = it->second->Name();
		CBCGPGridRow* pRow = GetRow(row);
		if (!pRow) continue;
		pRow->GetItem(0)->SetValue(fund_name.c_str(), TRUE);

		pRow->GetItem(1)->SetValue(std::to_string(it->second->GetAccountCount()).c_str(), TRUE);


		_RowToFundMap[row] = it->second;
		row++;
	}

	ClearFundGrid(row);
}

void SmFundGrid::StartTimer()
{
	SetTimer(1, 40, NULL);
}

void SmFundGrid::Update()
{
	if (_EnableOrderShow) {
		UpdateAcceptedOrder();
		//needDraw = true;
		_EnableOrderShow = false;
	}
}

void SmFundGrid::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SmFundGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* item = nullptr;
	CBCGPGridRow* pRow = HitTest(point, id, item);

	auto found = _RowToFundMap.find(id.m_nRow);
	if (found == _RowToFundMap.end()) return;
	if (FundAccountGrid) FundAccountGrid->InitFund(found->second);
	if (_FundDialog) _FundDialog->CurFund(found->second);

	CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}


void SmFundGrid::OnTimer(UINT_PTR nIDEvent)
{
	//bool needDraw = false;

	if (_EnableOrderShow) {
		UpdateAcceptedOrder();
		//needDraw = true;
		_EnableOrderShow = false;
	}

	//if (needDraw) Invalidate(FALSE);

	CBCGPGridCtrl::OnTimer(nIDEvent);
}

