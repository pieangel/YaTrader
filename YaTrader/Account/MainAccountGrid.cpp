#include "stdafx.h"
#include "MainAccountGrid.h"


#include <map>
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Order/SmAccountOrderManager.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Global/SmTotalManager.h"
#include "SmAccount.h"
#include "../Order/SmOrderRequest.h"
#include "../Client/ViStockClient.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Event/SmCallbackManager.h"
#include "SubAccountEditor.h"

#include "MainAccountGrid.h"

#include "SmAccountManager.h"

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

IMPLEMENT_DYNAMIC(MainAccountGrid, CBCGPGridCtrl)

MainAccountGrid::MainAccountGrid()
{
	m_bExtendedPadding = FALSE;
}

MainAccountGrid::~MainAccountGrid()
{
	//KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
}

void MainAccountGrid::OnHeaderCheckBoxClick(int nColumn)
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

void MainAccountGrid::OnRowCheckBoxClick(CBCGPGridRow* pRow)
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



void MainAccountGrid::AddAccount(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;

	size_t row = _RowToAccountMap.size();

	CBCGPGridRow* pRow = GetRow(row);
	if (!pRow) return;
	pRow->GetItem(0)->SetValue(account->Name().c_str(), TRUE);
	pRow->GetItem(1)->SetValue(std::to_string(account->get_sub_account_count()).c_str(), TRUE);

	_RowToAccountMap[row] = account;

	SelectRow(row);

	_MaxRow = row - 1;
}

BEGIN_MESSAGE_MAP(MainAccountGrid, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl message handlers

int MainAccountGrid::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	InsertColumn(0, _T("°èÁÂÀÌ¸§"), 120);
	InsertColumn(1, _T("°èÁÂ¹øÈ£"), 120);
	InsertColumn(2, _T("°èÁÂ¼ö"), 50);

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

	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&MainAccountGrid::OnOrderEvent, this, _1, _2));

	return 0;
}

void MainAccountGrid::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void MainAccountGrid::ClearOldContents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(*it);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->EmptyValue();

			pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
		}
	}
}

void MainAccountGrid::ClearOldContents(const int& last_index)
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

void MainAccountGrid::ClearAll()
{
	for (int i = 0; i <= _MaxRow; i++) {
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		pRow->GetItem(0)->EmptyValue(TRUE);

		pRow->GetItem(1)->EmptyValue(TRUE);
	}

}

void MainAccountGrid::ToggleExtendedPadding()
{
	m_bExtendedPadding = !m_bExtendedPadding;

	EnableRowHeader(TRUE);	// Need to recalculate the width
	AdjustLayout();
}



void MainAccountGrid::UpdateAccountInfo(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;

	for (auto it = _RowToAccountMap.begin(); it != _RowToAccountMap.end(); it++) {
		if (it->second->id() == account->id()) {
			CBCGPGridRow* pRow = GetRow(it->first);
			if (!pRow) continue;
			pRow->GetItem(0)->SetValue(account->Name().c_str(), TRUE);

			pRow->GetItem(1)->SetValue(std::to_string(account->get_sub_account_count()).c_str(), TRUE);

			break;
		}
	}
}

std::shared_ptr<DarkHorse::SmAccount> MainAccountGrid::GetFirstAccount()
{
	if (_RowToAccountMap.empty()) return nullptr;

	return _RowToAccountMap.begin()->second;
}

std::shared_ptr<DarkHorse::SmAccount> MainAccountGrid::GetLastAccount()
{
	if (_RowToAccountMap.empty()) return nullptr;

	return std::prev(_RowToAccountMap.end())->second;
}

void MainAccountGrid::ClearAccountGrid(const int& row)
{
	for (int i = row; i <= _MaxRow; i++) {
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		pRow->GetItem(0)->EmptyValue(TRUE);

		pRow->GetItem(1)->EmptyValue(TRUE);

	}

	_MaxRow = row - 1;
}

void MainAccountGrid::SetAccountList()
{
	_RowToAccountMap.clear();
	std::vector<std::shared_ptr<SmAccount>> account_vector;
	mainApp.AcntMgr()->get_main_account_vector(account_vector);
	int row = 0;
	for (auto it = account_vector.begin(); it != account_vector.end(); it++) {
		std::string account_name = (*it)->Name();
		const std::string account_no = (*it)->No();
		CBCGPGridRow* pRow = GetRow(row);
		if (!pRow) continue;
		pRow->GetItem(0)->SetValue(account_name.c_str(), TRUE);
		pRow->GetItem(1)->SetValue(account_no.c_str(), TRUE);

		pRow->GetItem(2)->SetValue(std::to_string((*it)->get_sub_account_count()).c_str(), TRUE);


		_RowToAccountMap[row] = (*it);
		row++;
	}

	ClearAccountGrid(row);
}

void MainAccountGrid::SetAccountList(const std::vector<std::shared_ptr<DarkHorse::SmAccount>>& account_vector)
{
	_RowToAccountMap.clear();
	int row = 0;
	for (auto it = account_vector.begin(); it != account_vector.end(); it++) {
		std::string account_name = (*it)->Name();
		const std::string account_no = (*it)->No();
		CBCGPGridRow* pRow = GetRow(row);
		if (!pRow) continue;
		pRow->GetItem(0)->SetValue(account_name.c_str(), TRUE);
		pRow->GetItem(1)->SetValue(account_no.c_str(), TRUE);

		pRow->GetItem(2)->SetValue(std::to_string((*it)->get_sub_account_count()).c_str(), TRUE);


		_RowToAccountMap[row] = (*it);
		row++;
	}

	ClearAccountGrid(row);
}

void MainAccountGrid::StartTimer()
{
	SetTimer(1, 40, NULL);
}

void MainAccountGrid::Update()
{
	if (_EnableOrderShow) {
		//needDraw = true;
		_EnableOrderShow = false;
	}
}

void MainAccountGrid::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void MainAccountGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* item = nullptr;
	CBCGPGridRow* pRow = HitTest(point, id, item);

	if (_SubAccountEditor)_SubAccountEditor->_BtnModify.EnableWindow(FALSE);

	auto found = _RowToAccountMap.find(id.m_nRow);
	if (found == _RowToAccountMap.end()) return;
	if (_SubAccountEditor) _SubAccountEditor->set_account(found->second);

	CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}


void MainAccountGrid::OnTimer(UINT_PTR nIDEvent)
{
	//bool needDraw = false;

	if (_EnableOrderShow) {
		//needDraw = true;
		_EnableOrderShow = false;
	}

	//if (needDraw) Invalidate(FALSE);

	CBCGPGridCtrl::OnTimer(nIDEvent);
}

