#include "stdafx.h"
#include "AccountFundView.h"

#include <vector>
#include "../Global/SmTotalManager.h"
#include "../Symbol/MarketDefine.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmProductYearMonth.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Symbol/SmProduct.h"
#include "../Util/SmUtil.h"
#include "../Account/SmAccount.h"
#include "../Fund/SmFund.h"
#include "../Account/SmAccountManager.h"
#include "../Fund/SmFundManager.h"
#include "../Event/EventHub.h"
#include <format>

using namespace DarkHorse;
IMPLEMENT_DYNAMIC(AccountFundView, CBCGPGridCtrl)

BEGIN_MESSAGE_MAP(AccountFundView, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()



AccountFundView::AccountFundView()
{

}

AccountFundView::~AccountFundView()
{

}

void AccountFundView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* pItem;
	HitTest(point, id, pItem);



	CString msg;
	//msg.Format("%d", nColumn);
	//AfxMessageBox(msg);
	SetFocus();

	if (mode_ == 0) {
		auto found = row_to_account_map_.find(id.m_nRow);
		if (found == row_to_account_map_.end()) return;
		auto account = found->second;
		mainApp.event_hub()->process_account_event(source_id_, account);
	}
	else {
		auto found = row_to_fund_map_.find(id.m_nRow);
		if (found == row_to_fund_map_.end()) return;
		auto fund = found->second;
		mainApp.event_hub()->process_fund_event(source_id_, fund);
	}

	Invalidate();
	CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}



void AccountFundView::init_grid()
{
	ClearGrid();
	int row = 0;
	if (mode_ == 0) {
		SetColumnName(0, _T("°èÁÂ¸í"));
		SetColumnName(1, _T("°èÁÂ¹øÈ£"));
		row_to_account_map_.clear();
		std::vector<std::shared_ptr<SmAccount>> main_account_vector;

		std::string account_type;
		if (mainApp.mode == 0) {
			account_type = "9";
		}
		else {
			account_type = "1";
		}

		mainApp.AcntMgr()->get_main_account_vector(account_type, main_account_vector);
		if (main_account_vector.empty()) return;
		for (auto ita = main_account_vector.begin(); ita != main_account_vector.end(); ++ita) {
			auto main_acnt = *ita;
			row_to_account_map_[row] = main_acnt;

			CBCGPGridRow* pRow = GetRow(row);
			if (!pRow) continue;
			pRow->GetItem(0)->SetValue(main_acnt->Name().c_str());
			pRow->GetItem(1)->SetValue(main_acnt->No().c_str());
			row++;

			const std::vector<std::shared_ptr<SmAccount>>& sub_account_vector = main_acnt->get_sub_accounts();
			for (auto it = sub_account_vector.begin(); it != sub_account_vector.end(); it++) {
				auto account = *it;
				row_to_account_map_[row] = account;

				pRow = GetRow(row);
				if (!pRow) continue;
				pRow->GetItem(0)->SetValue(account->Name().c_str());
				pRow->GetItem(1)->SetValue(account->No().c_str());
				row++;
			}
		}
		if (!row_to_account_map_.empty()) {
			account_ = row_to_account_map_.begin()->second;
		}
	}
	else {
		SetColumnName(0, _T("ÆÝµåÀÌ¸§"));
		SetColumnName(1, _T("¼³¸í"));
		row_to_fund_map_.clear();
		auto fund_map = mainApp.FundMgr()->GetFundMap();
		for (auto it = fund_map.begin(); it != fund_map.end(); ++it) {
			auto fund = it->second;
			CString str;
			str.Format(_T("%s"), fund->Name().c_str());
			row_to_fund_map_[row] = fund;

			CBCGPGridRow* pRow = GetRow(row);
			if (!pRow) continue;
			pRow->GetItem(0)->SetValue(fund->Name().c_str());
			pRow->GetItem(1)->SetValue("");
			row++;
		}

		if (!row_to_fund_map_.empty()) {
			fund_ = row_to_fund_map_.begin()->second;
		}
	}
	Invalidate();
}

int AccountFundView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetGraphicsManager()->SetDefaultGraphicsManagerType(CBCGPGraphicsManager::BCGP_GRAPHICS_MANAGER_D2D);

	EnableHeader(TRUE, ~BCGP_GRID_HEADER_SORT);
	EnableRowHeader(TRUE);
	EnableMarkSortedColumn(FALSE);
	SetClearInplaceEditOnEnter(FALSE);

	SetWholeRowSel(FALSE);
	EnableAlternateRows(FALSE);


	SetRowHeaderWidth(1);
	SetVisualManagerColorTheme(TRUE);

	// Insert columns:
	InsertColumn(0, _T("°èÁÂ¸í"), 80);
	InsertColumn(1, _T("°èÁÂ¹øÈ£"), 100);


	FreezeColumns(0);

	const int nColumns = GetColumnCount();


	for (int row = 0; row < grid_row_count5; row++) {
		// Create new row:
		CBCGPGridRow* pRow = CreateRow(nColumns);
		// Set each column data:
		for (int nColumn = 0; nColumn < nColumns; nColumn++)
		{
			long lValue = (nColumn + 1) * (row + 1);
			//pRow->GetItem(nColumn)->SetValue(lValue);


			//pRow->GetItem(nColumn)->SetValue(lValue);

			pRow->GetItem(nColumn)->AllowEdit(FALSE);
			_DefaultBackColor = pRow->GetItem(0)->GetBackgroundColor();
		}

		// Add row to grid:
		AddRow(pRow, FALSE /* Don't recal. layout */);

		row++;
	}


	SetColumnAlign(0, 2);
	SetColumnAlign(1, 2);
	SetColumnAlign(2, 2);
	SetHeaderAlign(0, 2);
	SetHeaderAlign(1, 2);
	SetHeaderAlign(2, 2);



	AdjustLayout();

	SetHighlightActiveItem(FALSE);
	SetReadOnly(TRUE);


	//start_timer();

	init_ = true;
	return 0;
}

void AccountFundView::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void AccountFundView::ClearGrid()
{
	if (!init_) return;

	for (int row = 0; row < GetRowCount(); row++) {
		CBCGPGridRow* pRow = GetRow(row);
		if (!pRow) continue;
		for (int col = 0; col < GetColumnCount(); col++) {
			pRow->GetItem(col)->SetValue("");
		}
	}
}