#include "stdafx.h"
#include "SmAccountGrid.h"



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
#include "../Yuanta/YaStockClient.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Event/SmCallbackManager.h"
#include "SmFundEditDialog.h"
#include "../Account/SmAccountManager.h"
#include "SmFundAccountGrid.h"

#include "SmFund.h"
#include "SmFundManager.h"
#include <format>

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

IMPLEMENT_DYNAMIC(SmAccountGrid, CBCGPGridCtrl)

SmAccountGrid::SmAccountGrid()
{
	m_bExtendedPadding = FALSE;
}

void SmAccountGrid::MoveSelectedAccounts()
{
	for (auto it = _RowToAccountMap.begin(); it != _RowToAccountMap.end(); it++) {
		CBCGPGridRow* pRow = GetRow(it->first);
		if (pRow->IsSelected()) {
			//if (!it->second->is_subaccount()) continue;
			_Fund->AddAccount(it->second);
			
		}
	}
	FundAccountGrid->InitFund();
	InitUnregAccount(account_type_);
}

void SmAccountGrid::MoveFirstAccount()
{
	if (!_Fund) return;
	if (_RowToAccountMap.empty()) return;

	auto found = _RowToAccountMap.begin();
	//if (!found->second->is_subaccount()) return;
	_Fund->AddAccount(found->second);
	FundAccountGrid->InitFund();
	InitUnregAccount(account_type_);
}

void SmAccountGrid::MoveAllAccounts()
{
	if (!_Fund) return;
	int old_size = _RowToAccountMap.size() - 1;
	for (auto it = _RowToAccountMap.begin(); it != _RowToAccountMap.end(); ++it) {
		//if (!it->second->is_subaccount()) continue;
		_Fund->AddAccount(it->second);
	}

	FundAccountGrid->InitFund();
	
	InitUnregAccount(account_type_);
	
	ClearAll();
}

SmAccountGrid::~SmAccountGrid()
{
	//KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
}

void SmAccountGrid::OnHeaderCheckBoxClick(int nColumn)
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

void SmAccountGrid::OnRowCheckBoxClick(CBCGPGridRow* pRow)
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

void SmAccountGrid::UpdateAcceptedOrder()
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

void SmAccountGrid::CancelSelOrders()
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

void SmAccountGrid::CancelAll()
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

BEGIN_MESSAGE_MAP(SmAccountGrid, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl message handlers

int SmAccountGrid::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	InsertColumn(0, _T("계좌번호"), 95);
	InsertColumn(1, _T("계좌이름"), 145);

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
	EnableCheckBoxes(TRUE);
	SetHeaderAlign(0, 2);
	SetHeaderAlign(1, 2);
	SetHeaderAlign(2, 2);
	SetHeaderAlign(3, 2);


	CheckAll(FALSE);
	UpdateHeaderCheckbox();
	AdjustLayout();

	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SmAccountGrid::OnOrderEvent, this, _1, _2));





	return 0;
}

void SmAccountGrid::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void SmAccountGrid::ClearOldContents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(*it);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->EmptyValue();

			pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
		}
	}
}

void SmAccountGrid::ClearOldContents(const int& last_index)
{
	for (int row = last_index; row <= _OldMaxRow; row++) {
		CBCGPGridRow* pRow = GetRow(row);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->EmptyValue();
			pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
			pRow->GetItem(i)->Redraw();
		}
	}
}

void SmAccountGrid::InitUnregAccount(const std::string& account_type)
{
	_RowToAccountMap.clear();

	std::vector<std::shared_ptr<DarkHorse::SmAccount>> unused_acnt_vector;
	mainApp.AcntMgr()->GetUnusedAccountForFund(account_type_, unused_acnt_vector);
	int row = 0;
	for (auto it = unused_acnt_vector.begin(); it != unused_acnt_vector.end(); ++it) {
		auto account = *it;
		
		CString str;
		str.Format(_T("%d"), row);

		CBCGPGridRow* pRow = GetRow(row);
		if (!pRow) continue;
		pRow->GetItem(0)->SetValue(account->No().c_str(), TRUE);

		pRow->GetItem(1)->SetValue(account->Name().c_str(), TRUE);
		if (!account->is_subaccount()) {
			pRow->GetItem(0)->SetBackgroundColor(RGB(2, 115, 202), TRUE);
			pRow->GetItem(1)->SetBackgroundColor(RGB(2, 115, 202), TRUE);
		}
		else {
			pRow->GetItem(0)->SetBackgroundColor(RGB(255, 255, 255), TRUE);
			pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255), TRUE);
		}


		_RowToAccountMap[row] = account;



		row++;
	}
	ClearAccountGrid(row);
}

void SmAccountGrid::ToggleExtendedPadding()
{
	m_bExtendedPadding = !m_bExtendedPadding;

	EnableRowHeader(TRUE);	// Need to recalculate the width
	AdjustLayout();
}



void SmAccountGrid::ClearAll()
{
	for (int i = 0; i <= _MaxAccountRow; i++) {
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		pRow->GetItem(0)->EmptyValue(TRUE);

		pRow->GetItem(1)->EmptyValue(TRUE);

	}
}

void SmAccountGrid::ClearAccountGrid(const int& row)
{
	for (int i = row; i <= _MaxAccountRow; i++) {
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		pRow->GetItem(0)->EmptyValue(TRUE);

		pRow->GetItem(1)->EmptyValue(TRUE);

	}

	_MaxAccountRow = row - 1;
}

void SmAccountGrid::StartTimer()
{
	SetTimer(1, 40, NULL);
}

void SmAccountGrid::Update()
{
	if (_EnableOrderShow) {
		UpdateAcceptedOrder();
		//needDraw = true;
		_EnableOrderShow = false;
	}
}

void SmAccountGrid::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SmAccountGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}


void SmAccountGrid::OnTimer(UINT_PTR nIDEvent)
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



void SmAccountGrid::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//CBCGPGridRow* HitTest(CPoint pt, CBCGPGridItemID & id, CBCGPGridItem * &pItem, CBCGPGridRow::ClickArea * pnArea = NULL, BOOL bItemsOnly = FALSE)
	CBCGPGridItemID id;
	CBCGPGridItem* item = nullptr;
	CBCGPGridRow*  pRow = HitTest(point, id, item);
	auto found = _RowToAccountMap.find(id.m_nRow);
	if (found == _RowToAccountMap.end()) return;
	//if (_FundEditDialog) {
	//	_FundEditDialog->UnregAccountDoubleClickEvent(found->second);
	//}
	if (!_Fund) return;
	//if (!found->second->is_subaccount()) return;
	_Fund->AddAccount(found->second);
	FundAccountGrid->InitFund();
	InitUnregAccount(account_type_);

	CBCGPGridCtrl::OnLButtonDblClk(nFlags, point);
}
