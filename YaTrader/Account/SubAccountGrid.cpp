#include "stdafx.h"
#include "SubAccountGrid.h"



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
#include "SubAccountEditor.h"


#include "../Account/SmAccountManager.h"
#include "../Fund/SmEditItem.h"

#include "../Fund/SmFund.h"
#include "../Fund/SmFundManager.h"
#include "SmAccountManager.h"
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

IMPLEMENT_DYNAMIC(SubAccountGrid, CBCGPGridCtrl)

SubAccountGrid::SubAccountGrid()
{
	m_bExtendedPadding = FALSE;
}

SubAccountGrid::~SubAccountGrid()
{
	//KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
}

void SubAccountGrid::OnHeaderCheckBoxClick(int nColumn)
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

void SubAccountGrid::OnRowCheckBoxClick(CBCGPGridRow* pRow)
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

BEGIN_MESSAGE_MAP(SubAccountGrid, CBCGPGridCtrl)
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

int SubAccountGrid::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	InsertColumn(0, _T("°èÁÂ"), 140);
	InsertColumn(1, _T("°èÁÂÀÌ¸§"), 160);
	InsertColumn(2, _T("½Â¼ö"), 60);
	InsertColumn(3, _T("ºñÀ²"), 50);

	FreezeColumns(0);

	const int nColumns = GetColumnCount();


	for (int row = 0; row < 800; row++) {
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

	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SubAccountGrid::OnOrderEvent, this, _1, _2));

	return 0;
}

void SubAccountGrid::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void SubAccountGrid::ClearOldContents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(*it);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->EmptyValue();

			pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
		}
	}
}

void SubAccountGrid::ClearOldContents(const int& last_index)
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

void SubAccountGrid::ToggleExtendedPadding()
{
	m_bExtendedPadding = !m_bExtendedPadding;

	EnableRowHeader(TRUE);	// Need to recalculate the width
	AdjustLayout();
}



std::shared_ptr<DarkHorse::SmAccount> SubAccountGrid::get_selected_account()
{
	if (_SelectedIndex == -1) return nullptr;
	if (_RowToSubAccountMap.empty()) return nullptr;

	auto found = _RowToSubAccountMap.find(_SelectedIndex);
	if (found == _RowToSubAccountMap.end()) return nullptr;
	return found->second;
}

void SubAccountGrid::MoveSelectedAccounts()
{
	for (auto it = _RowToSubAccountMap.begin(); it != _RowToSubAccountMap.end(); it++) {
		CBCGPGridRow* pRow = GetRow(it->first);
		if (pRow->GetCheck()) {
			//_Fund->RemoveAccount(it->second->No());

		}
	}
	InitAccount();

	//AccountGrid->InitUnregAccount();
}

void SubAccountGrid::MoveUp()
{
	if (!_Account) return;
	if (_SelectedIndex == -1) return;

	//_Account->MoveUp(_SelectedIndex);
}

void SubAccountGrid::MoveDown()
{
	if (!_Account) return;
	if (_SelectedIndex == -1) return;

	//_Account->MoveDown(_SelectedIndex);
}

void SubAccountGrid::MoveFirstAccount()
{
	if (!_Account) return;
	if (_RowToSubAccountMap.empty()) return;

	auto found = _RowToSubAccountMap.begin();

	//_Account->RemoveAccount(found->second->No());

	InitAccount();

	//AccountGrid->InitUnregAccount();
}

void SubAccountGrid::MoveAllAccounts()
{
	if (!_Account) return;

	int old_size = _RowToSubAccountMap.size() - 1;
// 	for (auto it = _RowToSubAccountMap.begin(); it != _RowToSubAccountMap.end(); ++it) {
// 		_Account->RemoveAccount(it->second->No());
// 	}


	InitAccount();

	//AccountGrid->InitUnregAccount();

	ClearAll();
}

void SubAccountGrid::ClearAll()
{
	for (int i = 0; i <= _MaxAccountRow; i++) {
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		pRow->GetItem(0)->EmptyValue(TRUE);

		pRow->GetItem(1)->EmptyValue(TRUE);
		pRow->GetItem(2)->EmptyValue(TRUE);
		pRow->GetItem(3)->EmptyValue(TRUE);
	}
}

void SubAccountGrid::ClearSubAccountGrid(const int& row)
{
	for (int i = row; i <= _MaxAccountRow; i++) {
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;

		pRow->GetItem(2)->AllowEdit(FALSE);
		pRow->GetItem(0)->EmptyValue(TRUE);

		pRow->GetItem(1)->EmptyValue(TRUE);
		pRow->GetItem(2)->EmptyValue(TRUE);
		pRow->GetItem(3)->EmptyValue(TRUE);



	}

	_MaxAccountRow = row - 1;
}

void SubAccountGrid::InitAccount(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;
	_Account = account;
	InitAccount();
}


void SubAccountGrid::InitAccount()
{
	if (!_Account) return;

	_RowToSubAccountMap.clear();

	const std::vector<std::shared_ptr<DarkHorse::SmAccount>>& acnt_vector = _Account->get_sub_accounts();
	int row = 0;
	for (auto it = acnt_vector.begin(); it != acnt_vector.end(); it++) {
		auto account = *it;
		CString str;
		str.Format(_T("%d"), row);
		CBCGPGridRow* pRow = GetRow(row);
		if (!pRow) continue;
		pRow->GetItem(0)->SetValue(account->No().c_str(), TRUE);

		pRow->GetItem(1)->SetValue(account->Name().c_str(), TRUE);
		std::string seung_su;
		seung_su = std::to_string(account->SeungSu());
		((SmEditItem*)pRow->GetItem(2))->Account(account);
		pRow->GetItem(2)->EmptyValue(FALSE);
		pRow->GetItem(2)->SetValue(account->SeungSu(), TRUE);
		pRow->GetItem(2)->AllowEdit(TRUE);



		CString strPercent;
		strPercent.Format("%d%%", 0);
		pRow->GetItem(3)->SetValue((const char*)strPercent, TRUE);

		_RowToSubAccountMap[row] = account;

		row++;
	}

	ClearSubAccountGrid(row);
}

void SubAccountGrid::set_default_account()
{
	std::vector<std::shared_ptr<SmAccount>> account_vector;
	mainApp.AcntMgr()->get_main_account_vector(account_vector);
	if (account_vector.empty()) return;
	InitAccount(*account_vector.begin());
}

void SubAccountGrid::StartTimer()
{
	SetTimer(1, 40, NULL);
}

void SubAccountGrid::Update()
{
	
}

void SubAccountGrid::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SubAccountGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* item = nullptr;
	CBCGPGridRow* pRow = HitTest(point, id, item);

	_SelectedIndex = id.m_nRow;

	auto found = _RowToSubAccountMap.find(id.m_nRow);	
	if (found == _RowToSubAccountMap.end()) return;
	_Account = found->second;
	if (_SubAccountEditor) _SubAccountEditor->set_sub_account(found->second);
	if (_SubAccountEditor) _SubAccountEditor->_BtnModify.EnableWindow(TRUE);

	CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}

void SubAccountGrid::update_sub_account(std::shared_ptr<DarkHorse::SmAccount> sub_account)
{
	if (!sub_account || _SelectedIndex < 0) return;

	CBCGPGridRow* pRow = GetRow(_SelectedIndex);
	if (!pRow) return;
	pRow->GetItem(0)->SetValue(sub_account->No().c_str(), TRUE);
	pRow->GetItem(1)->SetValue(sub_account->Name().c_str(), TRUE);
}


void SubAccountGrid::OnTimer(UINT_PTR nIDEvent)
{
	//bool needDraw = false;

	

	//if (needDraw) Invalidate(FALSE);

	CBCGPGridCtrl::OnTimer(nIDEvent);
}



void SubAccountGrid::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* item = nullptr;
	CBCGPGridRow* pRow = HitTest(point, id, item);

	auto found = _RowToSubAccountMap.find(id.m_nRow);
	if (found == _RowToSubAccountMap.end()) return;

	if (_SubAccountEditor) _SubAccountEditor->_BtnModify.EnableWindow(TRUE);
	
	//_Account->RemoveAccount(found->second->No());

	//InitAccount(_Account);


	CBCGPGridCtrl::OnLButtonDblClk(nFlags, point);
}
