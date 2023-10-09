#include "stdafx.h"
#include "ActiveOutSystemView.h"

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
#include "../OutSystem/SmOutSystem.h"
#include "../OutSystem/SmOutSystemManager.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Fund/SmFund.h"
#include "../Fund/SmFundManager.h"
#include <format>

using namespace DarkHorse;
IMPLEMENT_DYNAMIC(ActiveOutSystemView, CBCGPGridCtrl)

BEGIN_MESSAGE_MAP(ActiveOutSystemView, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()



ActiveOutSystemView::ActiveOutSystemView()
{

}

ActiveOutSystemView::~ActiveOutSystemView()
{

}

void ActiveOutSystemView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* pItem;
	HitTest(point, id, pItem);



	CString msg;
	//msg.Format("%d", nColumn);
	//AfxMessageBox(msg);

	auto found = row_to_out_system_.find(id.m_nRow);
	if (found == row_to_out_system_.end()) return;

	//auto symbol = mainApp.SymMgr()->FindSymbol(found->second->symbol_code);
	//if (!symbol) return;
	//mainApp.event_hub()->trigger_ab_symbol_event(1, symbol);
	Invalidate();

}



void ActiveOutSystemView::add_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system)
{
	if (!out_system) return;
	out_systems_.push_back(out_system);
	init_grid();
	Invalidate();
}

void ActiveOutSystemView::remove_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system)
{
	if (!out_system) return;

	auto it = row_to_out_system_.find(out_system->id());
	if (it == row_to_out_system_.end()) return;

	CString msg;
	msg.Format("id [%d], index [%d]\n", out_system->id(), it->second);
	TRACE(msg);

	std::erase_if(out_systems_, [&](const std::shared_ptr<SmOutSystem>& in_out_system) {
		return in_out_system->id() == out_system->id(); });
	init_grid();
	Invalidate();
}

void ActiveOutSystemView::on_update_quote()
{

}

void ActiveOutSystemView::on_update_position()
{

}

void ActiveOutSystemView::update_position(const position_p& position)
{

}

void ActiveOutSystemView::update_quute(const quote_p& quote)
{

}

int ActiveOutSystemView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	std::vector<std::string> title = { "계좌번호", "종목", "포지션", "평균단가", "현재가", "평가손익", "매도주문", "매수주문", "신호차트", "청산" };
	std::vector<int> col_width = { 100, 100, 80, 100, 100, 100, 100, 100, 124 - GetSystemMetrics(SM_CXVSCROLL), 80 };


	// Insert columns:
	for (size_t i = 0; i < title.size(); i++) {
		InsertColumn(i, title[i].c_str(), col_width[i]);
	}


	FreezeColumns(0);

	const int nColumns = GetColumnCount();

	for (int row = 0; row < active_out_system_row; row++) {
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
		AddRow(pRow, FALSE);

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

	init_grid();
	//SetTimer(1, 10, NULL);

	init_ = true;
	return 0;
}

void ActiveOutSystemView::OnDestroy()
{
	KillTimer(1);
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void ActiveOutSystemView::OnTimer(UINT_PTR nIDEvent)
{
	TRACE("ActiveOutSystemView::OnTimer\n");
}

void ActiveOutSystemView::init_grid()
{
	row_to_out_system_.clear();
	out_system_map_.clear();
	for (size_t i = 0; i < out_systems_.size(); i++)
	{
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		if (out_systems_[i]->order_type() == DarkHorse::OrderType::Fund)
			pRow->GetItem(0)->SetValue(out_systems_[i]->fund()->Name().c_str());
		else
			pRow->GetItem(0)->SetValue(out_systems_[i]->account()->No().c_str());
		pRow->GetItem(1)->SetValue(out_systems_[i]->symbol()->SymbolCode().c_str());

		row_to_out_system_[out_systems_[i]->id()] = i;
		out_system_map_[out_systems_[i]->id()] = out_system_map_[i];
	}
	clear_old_contents(out_systems_.size());
	max_index_ = out_systems_.size();
}

void ActiveOutSystemView::clear_old_contents(const int& last_index)
{
	if (last_index > max_index_) return;
	for (size_t row = last_index; row < (size_t)max_index_; row++)
	{
		CBCGPGridRow* pRow = GetRow(row);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->SetValue("");
		}
	}
}

void ActiveOutSystemView::remap_row_to_out_system()
{
	row_to_out_system_.clear();
	for (size_t i = 0; i < out_systems_.size(); i++)
	{
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		row_to_out_system_[out_systems_[i]->id()] = i;
	}
}

void ActiveOutSystemView::ClearGrid()
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
