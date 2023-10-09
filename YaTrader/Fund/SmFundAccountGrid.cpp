#include "stdafx.h"
#include "SmFundAccountGrid.h"



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
#include "SmFundEditDialog.h"
#include "SmAccountGrid.h"
#include "SmFundGrid.h"


#include "../Account/SmAccountManager.h"
#include "SmEditItem.h"

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

IMPLEMENT_DYNAMIC(SmFundAccountGrid, CBCGPGridCtrl)

SmFundAccountGrid::SmFundAccountGrid()
{
	m_bExtendedPadding = FALSE;
}

SmFundAccountGrid::~SmFundAccountGrid()
{
	//KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
}

void SmFundAccountGrid::OnHeaderCheckBoxClick(int nColumn)
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

void SmFundAccountGrid::OnRowCheckBoxClick(CBCGPGridRow* pRow)
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

void SmFundAccountGrid::UpdateAcceptedOrder()
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
				pRow->GetItem(1)->SetValue("¸Å¼ö");
			}
			else {
				pRow->GetItem(1)->SetValue("¸Åµµ");
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

void SmFundAccountGrid::CancelSelOrders()
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

void SmFundAccountGrid::CancelAll()
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



BEGIN_MESSAGE_MAP(SmFundAccountGrid, CBCGPGridCtrl)
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

int SmFundAccountGrid::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	InsertColumn(0, _T("°èÁÂ"), 100);
	InsertColumn(1, _T("°èÁÂÀÌ¸§"), 90);
	InsertColumn(2, _T("½Â¼ö"), 20);
	InsertColumn(3, _T("ºñÀ²"), 40);

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
			if (nColumn != 2)
				pRow->GetItem(nColumn)->AllowEdit(FALSE);
			else {
				
				SmEditItem* pSpinItem = new SmEditItem(0);
				pSpinItem->EnableSpinControl(TRUE, 0, 1000);

				pRow->ReplaceItem(nColumn, pSpinItem);

				pSpinItem->EmptyValue(TRUE);
				pRow->GetItem(nColumn)->AllowEdit(FALSE);
			}

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

	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SmFundAccountGrid::OnOrderEvent, this, _1, _2));





	return 0;
}

void SmFundAccountGrid::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void SmFundAccountGrid::ClearOldContents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(*it);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->EmptyValue();

			pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
		}
	}
}

void SmFundAccountGrid::ClearOldContents(const int& last_index)
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

void SmFundAccountGrid::ToggleExtendedPadding()
{
	m_bExtendedPadding = !m_bExtendedPadding;

	EnableRowHeader(TRUE);	// Need to recalculate the width
	AdjustLayout();
}



void SmFundAccountGrid::MoveSelectedAccounts()
{
	for (auto it = _RowToFundAccountMap.begin(); it != _RowToFundAccountMap.end(); it++) {
		CBCGPGridRow* pRow = GetRow(it->first);
		if (pRow->IsSelected()) {
			_Fund->RemoveAccount(it->second->No());

		}
	}
	InitFund();

	AccountGrid->InitUnregAccount(account_type_);
}

void SmFundAccountGrid::MoveUp()
{
	if (!_Fund) return;
	if (_SelectedIndex == -1) return;

	_Fund->MoveUp(_SelectedIndex);

	_SelectedIndex--;

	if (_SelectedIndex < 0) {
		_SelectedIndex = 0;
	}

	CBCGPGridRow* pRow = GetRow(_SelectedIndex);
	if (pRow) {
		pRow->Select();
		//pRow->GetItem(0)->Select(TRUE);
	}
}

void SmFundAccountGrid::MoveDown()
{
	if (!_Fund) return;
	if (_SelectedIndex == -1) return;

	_Fund->MoveDown(_SelectedIndex);
	size_t size = _Fund->GetAccountCount();
	_SelectedIndex++;
	if (_SelectedIndex == size - 1) {
		_SelectedIndex = size - 1;
	}
	CBCGPGridRow* pRow = GetRow(_SelectedIndex);
	if (pRow) {
		pRow->Select();
		//pRow->GetItem(0)->Select(TRUE);
	}
}

void SmFundAccountGrid::MoveFirstAccount()
{
	if (!_Fund) return;
	if (_RowToFundAccountMap.empty()) return;

	auto found = _RowToFundAccountMap.begin();

	_Fund->RemoveAccount(found->second->No());

	InitFund();
	
	AccountGrid->InitUnregAccount(account_type_);
}

void SmFundAccountGrid::MoveAllAccounts()
{
	if (!_Fund) return;

	int old_size = _RowToFundAccountMap.size() - 1;
	for (auto it = _RowToFundAccountMap.begin(); it != _RowToFundAccountMap.end(); ++it) {
		_Fund->RemoveAccount(it->second->No());
	}

	
	InitFund();
	
	AccountGrid->InitUnregAccount(account_type_);
	
	ClearAll();
}

void SmFundAccountGrid::ClearAll()
{
	for (int i = 0; i <= _MaxFundAccountRow; i++) {
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		pRow->GetItem(0)->EmptyValue(TRUE);

		pRow->GetItem(1)->EmptyValue(TRUE);
		pRow->GetItem(2)->EmptyValue(TRUE);
		pRow->GetItem(3)->EmptyValue(TRUE);
	}
}

void SmFundAccountGrid::ClearFundAccountGrid(const int& row)
{
	for (int i = row; i <= _MaxFundAccountRow; i++) {
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;

		pRow->GetItem(2)->AllowEdit(FALSE);
		pRow->GetItem(0)->EmptyValue(TRUE);

		pRow->GetItem(1)->EmptyValue(TRUE);
		pRow->GetItem(2)->EmptyValue(TRUE);
		pRow->GetItem(3)->EmptyValue(TRUE);

		pRow->GetItem(0)->SetBackgroundColor(RGB(255, 255, 255), TRUE);
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255), TRUE);

	}

	_MaxFundAccountRow = row - 1;
}

void SmFundAccountGrid::InitFund(std::shared_ptr<DarkHorse::SmFund> fund)
{
	if (!fund) return;
	_Fund = fund;
	InitFund();
}


void SmFundAccountGrid::InitFund()
{
	if (!_Fund) return;

	_RowToFundAccountMap.clear();

	const std::vector<std::shared_ptr<DarkHorse::SmAccount>>& acnt_map = _Fund->GetAccountVector();
	int row = 0;
	for (auto it = acnt_map.begin(); it != acnt_map.end(); it++) {
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
		std::string seung_su;
		seung_su = std::to_string(account->SeungSu());
		((SmEditItem*)pRow->GetItem(2))->Account(account);
		((SmEditItem*)pRow->GetItem(2))->FundAccountGrid(this);
		pRow->GetItem(2)->EmptyValue(FALSE);
		pRow->GetItem(2)->SetValue(account->SeungSu(), TRUE);
		pRow->GetItem(2)->AllowEdit(TRUE);


		int seungsu_percent = _Fund->GetRatioPercent(account->No());

		CString strPercent;
		strPercent.Format("%d%%", seungsu_percent);
		pRow->GetItem(3)->SetValue((const char*)strPercent, TRUE);



		_RowToFundAccountMap[row] = account;



		row++;
	}

	ClearFundAccountGrid(row);
}

void SmFundAccountGrid::StartTimer()
{
	SetTimer(1, 40, NULL);
}

void SmFundAccountGrid::Update()
{
	if (_EnableOrderShow) {
		UpdateAcceptedOrder();
		//needDraw = true;
		_EnableOrderShow = false;
	}
}

void SmFundAccountGrid::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SmFundAccountGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* item = nullptr;
	CBCGPGridRow* pRow = HitTest(point, id, item);
	

	_SelectedIndex = id.m_nRow;
	

	CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}


void SmFundAccountGrid::OnTimer(UINT_PTR nIDEvent)
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



void SmFundAccountGrid::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* item = nullptr;
	CBCGPGridRow* pRow = HitTest(point, id, item);

	auto found = _RowToFundAccountMap.find(id.m_nRow);
	if (found == _RowToFundAccountMap.end()) return;

	//if (_FundEditDialog) {
	//	_FundEditDialog->FundAccountDoubleClickEvent(found->second);
	//}

	

	_Fund->RemoveAccount(found->second->No());



	//InitFund(_CurFund);
	//RefreshUnregAccounts();

	//InitFund(_CurFund);
	//_EditFundName.SetWindowText(_CurFund->Name().c_str());
	InitFund();
	//RefreshUnregAccounts();
	if (AccountGrid)
		AccountGrid->InitUnregAccount(account_type_);
	if (FundGrid)
		FundGrid->SetFundList();

	CBCGPGridCtrl::OnLButtonDblClk(nFlags, point);
}
