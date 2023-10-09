#include "stdafx.h"
#include "SmAcceptedGrid.h"
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
#include "../Fund/SmFundManager.h"
#include "../Fund/SmFund.h"
#include "../Account/SmAccountManager.h"
#include "AbAccountOrderWindow.h"
#include "../Fund/SmFundOrderDialog.h"
#include <functional>
#include "../CompOrder/SmOrderCompMainDialog.h"
#include "../CompOrder/SmFundCompMainDialog.h"

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

IMPLEMENT_DYNAMIC(SmAcceptedGrid, CBCGPGridCtrl)

SmAcceptedGrid::SmAcceptedGrid()
{
	m_bExtendedPadding = FALSE;
}

void SmAcceptedGrid::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	_Fund = val;
	UpdateFundAcceptedOrders();
}

SmAcceptedGrid::~SmAcceptedGrid()
{
	KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
}

void SmAcceptedGrid::OnHeaderCheckBoxClick(int nColumn)
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

void SmAcceptedGrid::OnRowCheckBoxClick(CBCGPGridRow* pRow)
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

void SmAcceptedGrid::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	_Account = val;
	UpdateAccountAcceptedOrders();
}

void SmAcceptedGrid::UpdateAcceptedOrder()
{
	_Mode == 0 ? UpdateAccountAcceptedOrders() : UpdateFundAcceptedOrders();
}

void SmAcceptedGrid::CancelSelOrders()
{
	_Mode == 0 ? CancelSelAccountOrders() : CancelSelFundOrders();
	ClearCheck();
}

void SmAcceptedGrid::CancelAll()
{
	_Mode == 0 ? CancelAllAccountOrders() : CancelAllFundOrders();
	ClearCheck();
}

BEGIN_MESSAGE_MAP(SmAcceptedGrid, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl message handlers

int SmAcceptedGrid::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	InsertColumn(0, _T("종목"), 75);
	InsertColumn(1, _T("구분"), 40);
	InsertColumn(2, _T("미체결"), 48);
	InsertColumn(3, _T("가격"), 68);

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
			_DefaultBackColor = pRow->GetItem(0)->GetBackgroundColor();
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

	SetHighlightActiveItem(FALSE);
	SetReadOnly(TRUE);

	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SmAcceptedGrid::OnOrderEvent, this, _1, _2));

	



	return 0;
}

void SmAcceptedGrid::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void SmAcceptedGrid::ClearCheck()
{
	_HeaderCheck = false; CheckAll(FALSE);
	UpdateHeaderCheckbox();	Invalidate();
}

void SmAcceptedGrid::ClearOldContents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(*it);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->EmptyValue(FALSE);
			pRow->GetItem(i)->SetBackgroundColor(_DefaultBackColor);
			pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
		}
	}
}

void SmAcceptedGrid::ClearOldContents(const int& last_index)
{
	for (int row = last_index; row < _OldMaxRow; row++) {
		CBCGPGridRow* pRow = GetRow(row);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->EmptyValue(FALSE);
			pRow->GetItem(i)->SetBackgroundColor(_DefaultBackColor);
			pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
		}
	}
}

void SmAcceptedGrid::UpdateAccountAcceptedOrders()
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
				pRow->GetItem(0)->SetBackgroundColor(RGB(255, 0, 0));
				pRow->GetItem(1)->SetBackgroundColor(RGB(255, 0, 0));
				pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
				pRow->GetItem(3)->SetBackgroundColor(RGB(255, 0, 0));

				pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
				pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
				pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
				pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
				pRow->GetItem(1)->SetValue("매수");

			}
			else {
				pRow->GetItem(1)->SetValue("매도");
				pRow->GetItem(0)->SetBackgroundColor(RGB(0, 0, 255));
				pRow->GetItem(1)->SetBackgroundColor(RGB(0, 0, 255));
				pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
				pRow->GetItem(3)->SetBackgroundColor(RGB(0, 0, 255));

				pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
				pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
				pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
				pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
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

}

void SmAcceptedGrid::UpdateFundAcceptedOrders()
{
	if (!_Fund) return;
	

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	int row = 0;
	for (size_t i = 0; i < account_vec.size(); ++i) {
		auto account = account_vec[i];
		auto account_order_mgr = mainApp.TotalOrderMgr()->FindAccountOrderManager(account->No());
		if (!account_order_mgr) return;

		const std::map<std::string, std::shared_ptr<SmSymbolOrderManager>>& symbol_order_mgr_map = account_order_mgr->GetSymbolOrderMgrMap();
		
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
					pRow->GetItem(0)->SetBackgroundColor(RGB(255, 0, 0));
					pRow->GetItem(1)->SetBackgroundColor(RGB(255, 0, 0));
					pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
					pRow->GetItem(3)->SetBackgroundColor(RGB(255, 0, 0));

					pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
					pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
					pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
					pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
					pRow->GetItem(1)->SetValue("매수");

				}
				else {
					pRow->GetItem(1)->SetValue("매도");
					pRow->GetItem(0)->SetBackgroundColor(RGB(0, 0, 255));
					pRow->GetItem(1)->SetBackgroundColor(RGB(0, 0, 255));
					pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
					pRow->GetItem(3)->SetBackgroundColor(RGB(0, 0, 255));

					pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
					pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
					pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
					pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
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
	}
	ClearOldContents(row);
	_OldMaxRow = row;

}

void SmAcceptedGrid::CancelSelAccountOrders()
{
	if (!_Account) return;

	for (auto it = _RowToOrderMap.begin(); it != _RowToOrderMap.end(); ++it) {
		

		CBCGPGridRow* pRow = GetRow(it->first);
		if (pRow->GetCheck()) {
			auto account = mainApp.AcntMgr()->FindAccount(it->second->AccountNo);
			if (!account) continue;

			auto order_req = SmOrderRequestManager::MakeCancelOrderRequest(
				account->No(),
				account->Pwd(),
				it->second->SymbolCode,
				it->second->OrderNo,
				it->second->OrderPrice,
				it->second->PositionType,
				it->second->OrderAmount);
			mainApp.Client()->CancelOrder(order_req);
		}
	}
}

void SmAcceptedGrid::CancelSelFundOrders()
{
	if (!_Fund) return;

	for (auto it = _RowToOrderMap.begin(); it != _RowToOrderMap.end(); ++it) {
		

		CBCGPGridRow* pRow = GetRow(it->first);
		if (pRow->GetCheck()) {
			auto account = mainApp.AcntMgr()->FindAccount(it->second->AccountNo);
			if (!account) continue;

			auto order_req = SmOrderRequestManager::MakeCancelOrderRequest(
				account->No(),
				account->Pwd(),
				it->second->SymbolCode,
				it->second->OrderNo,
				it->second->OrderPrice,
				it->second->PositionType,
				it->second->OrderAmount);
			mainApp.Client()->CancelOrder(order_req);
		}
	}
}

void SmAcceptedGrid::CancelAllAccountOrders()
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
		mainApp.Client()->CancelOrder(order_req);
	}
}

void SmAcceptedGrid::CancelAllFundOrders()
{
	if (!_Fund) return;

	for (auto it = _RowToOrderMap.begin(); it != _RowToOrderMap.end(); ++it) {
		auto account = mainApp.AcntMgr()->FindAccount(it->second->AccountNo);
		if (!account) continue;
		CBCGPGridRow* pRow = GetRow(it->first);
		auto order_req = SmOrderRequestManager::MakeCancelOrderRequest(
			account->No(),
			account->Pwd(),
			it->second->SymbolCode,
			it->second->OrderNo,
			it->second->OrderPrice,
			it->second->PositionType,
			it->second->OrderAmount);
		mainApp.Client()->CancelOrder(order_req);
	}
}

void SmAcceptedGrid::ToggleExtendedPadding()
{
	m_bExtendedPadding = !m_bExtendedPadding;

	EnableRowHeader(TRUE);	// Need to recalculate the width
	AdjustLayout();
}



void SmAcceptedGrid::StartTimer()
{
	SetTimer(1, 40, NULL);
}

void SmAcceptedGrid::Update()
{
	if (_EnableOrderShow) {
		UpdateAcceptedOrder();
		//needDraw = true;
		_EnableOrderShow = false;
	}
}

void SmAcceptedGrid::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SmAcceptedGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* pItem;
	HitTest(point, id, pItem);
	
	


	if (id.m_nColumn == -1) {
		if (id.m_nRow == -1) {
			if (_HeaderCheck) { _HeaderCheck = false; CheckAll(FALSE); }
			else { _HeaderCheck = true; CheckAll(TRUE); }
			UpdateHeaderCheckbox();
		}
		else {
			CBCGPGridRow* pRow = GetRow(id.m_nRow);
			if (pRow->GetCheck())
				pRow->SetCheck(FALSE);
			else
				pRow->SetCheck(TRUE);
		}
	}
	else {
		auto found = _RowToOrderMap.find(id.m_nRow);
		if (found == _RowToOrderMap.end()) return;

		if (_OrderWnd) _OrderWnd->OnSymbolClicked(found->second->SymbolCode);
		if (_FundOrderWnd) _FundOrderWnd->OnSymbolClicked(found->second->SymbolCode);
		if (_CompOrderWnd) _CompOrderWnd->OnSymbolClicked(found->second->SymbolCode);
		if (_CompFundWnd) _CompFundWnd->OnSymbolClicked(found->second->SymbolCode);
	}
	Invalidate();

	
	
	//Invalidate();
	//CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}


void SmAcceptedGrid::OnTimer(UINT_PTR nIDEvent)
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
