#include "stdafx.h"
#include "OutSystemDefView.h"


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
#include "../Global/SmTotalManager.h"
#include "../OutSystem/SmOutSystem.h"
#include "../OutSystem/SmOutSystemManager.h"
#include "../OutSystem/SmOutSignalDef.h"

#include <format>

using namespace DarkHorse;
IMPLEMENT_DYNAMIC(OutSystemDefView, CBCGPGridCtrl)

BEGIN_MESSAGE_MAP(OutSystemDefView, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()



OutSystemDefView::OutSystemDefView()
{

}

OutSystemDefView::~OutSystemDefView()
{

}

void OutSystemDefView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* pItem;
	HitTest(point, id, pItem);



	CString msg;
	//msg.Format("%d", nColumn);
	//AfxMessageBox(msg);

	auto found = row_to_symbol_.find(id.m_nRow);
	if (found == row_to_symbol_.end()) return;

	//auto symbol = mainApp.SymMgr()->FindSymbol(found->second->symbol_code);
	//if (!symbol) return;
	//mainApp.event_hub()->trigger_ab_symbol_event(1, symbol);
	Invalidate();

}



int OutSystemDefView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	std::vector<std::string> title = { "신호차트", "설명" };
	std::vector<int> col_width = { 100, 71 - GetSystemMetrics(SM_CXVSCROLL) };

	// Insert columns:
	for (size_t i = 0; i < title.size(); i++) {
		InsertColumn(i, title[i].c_str(), col_width[i]);
	}


	FreezeColumns(0);

	const int nColumns = GetColumnCount();


	for (int row = 0; row < grid_row_count2; row++) {
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

	for (int i = 0; i < GetColumnCount(); i++)
	{
		SetColumnAlign(i, 2);
		SetHeaderAlign(i, 2);
	}

	AdjustLayout();

	SetHighlightActiveItem(FALSE);
	SetReadOnly(TRUE);


	//start_timer();

	init_ = true;
	return 0;
}

void OutSystemDefView::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void OutSystemDefView::init_grid()
{
	auto signal_def_vector = mainApp.out_system_manager()->get_out_system_signal_map();
	int selIndex = -1;
	for (size_t i = 0; i < signal_def_vector.size(); i++) {
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		pRow->GetItem(0)->SetValue(signal_def_vector[i]->name.c_str());
		pRow->GetItem(1)->SetValue(signal_def_vector[i]->desc.c_str());
	}
}

void OutSystemDefView::ClearGrid()
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
