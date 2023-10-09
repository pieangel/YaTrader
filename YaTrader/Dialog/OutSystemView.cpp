#include "stdafx.h"
#include "OutSystemView.h"

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
#include "SmAddOutSigDefDlg.h"
#include "../resource.h"
#include "../Account/SmAccount.h"
#include "../Fund/SmFund.h"
#include "HdSymbolSelecter.h"
#include <format>
#include "../Grid/customcells.h"
#include "../Util/IdGenerator.h"
#include "../Event/EventHub.h"

#include "../OutSystem/SmOutSystem.h"
#include "../OutSystem/SmOutSystemManager.h"
#include "../OutSystem/SmOutSignalDef.h"
#include "SmAccountFundSelector.h"
#include "SmAutoSignalManagerDialog.h"

#include <functional>

#define PROP_HAS_LIST	0x0001
#define PROP_HAS_BUTTON	0x0002
#define PROP_HAS_SPIN	0x0004
#define PROP_AUTOGROUP	0x0100
const int max_out_system_seung_su = 100;

using namespace DarkHorse;
IMPLEMENT_DYNAMIC(OutSystemView, CBCGPGridCtrl)

BEGIN_MESSAGE_MAP(OutSystemView, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()



OutSystemView::OutSystemView()
{

}

OutSystemView::~OutSystemView()
{

}

void OutSystemView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* pItem;
	HitTest(point, id, pItem);



	CString msg;
	//msg.Format("%d", nColumn);
	//AfxMessageBox(msg);
	selected_row_ = id.m_nRow;
	auto found = row_to_out_system_.find(id.m_nRow);
	if (found == row_to_out_system_.end()) { Invalidate(); return; }
	/*
	CBCGPGridRow* pRow = GetRow(id.m_nRow);
	CBCGPGridCheckItem* checkItem = (CBCGPGridCheckItem*)pRow->GetItem(0);
	const int mode = checkItem->GetState();
	if (mode == 1) {
		found->second->enable(true);
	}
	else {
		found->second->enable(false);
	}
	*/
	Invalidate();
	CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}



void OutSystemView::OnItemChanged(CBCGPGridItem* pItem, int nRow, int nColumn)
{
	if (!pItem) return;
	TRACE("OutSystemView::OnItemChanged nRow = %d\n", nRow);
	const int col_id = pItem->GetColumnId();
	if (col_id == 0) {
		auto found = row_to_out_system_.find(nRow);
		if (found == row_to_out_system_.end()) { Invalidate(); return; }
		CBCGPGridCheckItem* checkItem = (CBCGPGridCheckItem*)pItem;
		int mode = checkItem->GetState();
		if (mode == 1 && !found->second->enable()) {
			found->second->enable(true);
			parent_dlg_->add_active_out_system(found->second);
		}
		else {
			TRACE("parent_dlg_->remove_active_out_system nRow = %d\n", nRow);
			found->second->enable(false);
			parent_dlg_->remove_active_out_system(found->second);
		}
	}
	Invalidate();
}

void OutSystemView::add_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system)
{
	if (!out_system) return;
	if (!out_system->symbol()) return;
	if (out_system->order_type() == DarkHorse::OrderType::MainAccount && !out_system->account()) return;
	if (out_system->order_type() == DarkHorse::OrderType::SubAccount && !out_system->account()) return;
	if (out_system->order_type() == DarkHorse::OrderType::Fund && !out_system->fund()) return;

	size_t out_system_count = mainApp.out_system_manager()->get_out_system_count();
	if (out_system_count == 0) return;

	init_grid();
}

void OutSystemView::remove_out_system()
{
	if (selected_row_ < 0) return;
	auto found = row_to_out_system_.find(selected_row_);
	if (found == row_to_out_system_.end()) return; 
	mainApp.out_system_manager()->remove_out_system(found->second);
	init_grid();
}

std::shared_ptr<DarkHorse::SmOutSystem> OutSystemView::get_out_system(const int row)
{
	if (row < 0) return nullptr;
	auto found = row_to_out_system_.find(row);
	if (found == row_to_out_system_.end()) return nullptr;
	return found->second;
}

std::shared_ptr<DarkHorse::SmOutSystem> OutSystemView::get_selected_out_system()
{
	if (selected_row_ < 0) return nullptr;
	auto found = row_to_out_system_.find(selected_row_);
	if (found == row_to_out_system_.end()) return nullptr;
	return found->second;
}

int OutSystemView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetGraphicsManager()->SetDefaultGraphicsManagerType(CBCGPGraphicsManager::BCGP_GRAPHICS_MANAGER_D2D);

	EnableHeader(TRUE, ~BCGP_GRID_HEADER_SORT);
	EnableRowHeader(TRUE);
	EnableMarkSortedColumn(FALSE);
	SetClearInplaceEditOnEnter(FALSE);

	SetWholeRowSel(FALSE);
	EnableAlternateRows(TRUE);

	EnableInvertSelOnCtrl();
	EnableCustomToolTips();
	SetSingleSel();

	SetRowHeaderWidth(1);
	SetVisualManagerColorTheme(TRUE);
	std::vector<std::string> title = { "실행", "계좌번호", "종목", "신호차트", "승수", "평가손익", "청산손익", "총손익", "로그" };
	std::vector<int> col_width = { 40, 98, 90, 80, 58, 100, 100, 100, 85 };

	// Insert columns:
	for(size_t i = 0; i < title.size(); i++) {
		InsertColumn(i, title[i].c_str(), col_width[i]);
	}


	FreezeColumns(0);
	const int nColumns = GetColumnCount();

	
	for (int row = 0; row < out_system_row_count; row++) {
		// Create new row:
		CBCGPGridRow* pRow = CreateRow(nColumns);
		// Set each column data:
		for (int nColumn = 0; nColumn < nColumns; nColumn++)
		{
			//long lValue = (nColumn + 1) * (row + 1);
			//pRow->GetItem(nColumn)->SetValue(lValue);


			//pRow->GetItem(nColumn)->SetValue(lValue);
			if (nColumn > 4)
				pRow->GetItem(nColumn)->AllowEdit(FALSE);
			_DefaultBackColor = pRow->GetItem(0)->GetBackgroundColor();
		}

		// Add row to grid:
		AddRow(pRow, FALSE );

		row++;
	}
	

	for(int i = 0; i < GetColumnCount(); i++)
	{
		SetColumnAlign(i, 2);
		SetHeaderAlign(i, 2);
	}

	AdjustLayout();

	SetHighlightActiveItem(FALSE);


	//start_timer();

	init_grid();
	return 0;
}

void OutSystemView::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void OutSystemView::init_grid()
{
	auto out_system_vector = mainApp.out_system_manager()->get_out_system_vector();

	row_to_out_system_.clear();
	int row = 0;
	for (auto out_system : out_system_vector) {
		CBCGPGridRow* pRow = create_or_get_row(row);
		if (!pRow) return;
		create_out_system_cells(pRow, out_system);
		row_to_out_system_[row] = out_system;
		if (out_system->enable()) {
			CBCGPGridRow* pRow = GetRow(row);
			CBCGPGridCheckItem* checkItem = (CBCGPGridCheckItem*)pRow->GetItem(0);
			checkItem->SetState(1);
		}

		row++;
	}
	clear_old_contents(row);
	max_index_ = row;
}

void OutSystemView::clear_old_contents(const int& last_index)
{
	if (last_index > max_index_) return;
	for (size_t row = last_index; row < (size_t)max_index_; row++)
	{
		CBCGPGridRow* pRow = GetRow(row);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->ReplaceItem(i, new CBCGPGridItem(""));
		}
	}
}

void OutSystemView::remap_row_to_out_system()
{
	auto out_system_vector = mainApp.out_system_manager()->get_out_system_vector();
	if (out_system_vector.empty()) { Invalidate(); return; }

	row_to_out_system_.clear();
	int row = 0;
	for (auto out_system : out_system_vector) {
		row_to_out_system_[row] = out_system;
		row++;
	}
}

void OutSystemView::ClearGrid()
{
	for (int row = 0; row < GetRowCount(); row++) {
		CBCGPGridRow* pRow = GetRow(row);
		if (!pRow) continue;
		for (int col = 0; col < GetColumnCount(); col++) {
			pRow->GetItem(col)->SetValue("");
		}
	}
}



void OutSystemView::create_out_system_cells(CBCGPGridRow* pRow, std::shared_ptr<DarkHorse::SmOutSystem> out_system)
{
	if (!pRow || !out_system) return;

	pRow->ReplaceItem(0, new CBCGPGridCheckItem(FALSE));
	pRow->GetItem(0);
	std::string target;
	if (out_system->order_type() == DarkHorse::OrderType::Fund) {
		target = out_system->fund()->Name();
	}
	else {
		target = out_system->account()->No();
	}
	pRow->ReplaceItem(1, new CAccountItem(target.c_str(), *this));

	pRow->ReplaceItem(2, new CSymbolItem(out_system->symbol()->SymbolCode().c_str(), *this));

	COutDefItem* pItem = new COutDefItem(out_system->name().c_str(), *this);

	auto signal_def_vector = mainApp.out_system_manager()->get_out_system_signal_map();
	int selIndex = -1;
	for (auto it = signal_def_vector.begin(); it != signal_def_vector.end(); ++it) {
		pItem->AddOption((*it)->name.c_str(), 1);
	}

	pRow->ReplaceItem(3, pItem);

	CBCGPGridItem* pSpinItem = new CBCGPGridItem(out_system->seung_su());
	pSpinItem->EnableSpinControl(TRUE, 0, max_out_system_seung_su);

	pRow->ReplaceItem(4, pSpinItem);
}

CBCGPGridRow* OutSystemView::create_or_get_row(const int row_index)
{
	CBCGPGridRow* pRow = nullptr;
	if (row_index < 0) return pRow;
	int nColumns = GetColumnCount();
	if (row_index > GetRowCount()) {
		pRow = CreateRow(nColumns);

		// Set each column data:
		for (int nColumn = 0; nColumn < nColumns; nColumn++) {
			if (nColumn > 4)
				pRow->GetItem(nColumn)->AllowEdit(FALSE);
		}

		// Add row to grid:
		AddRow(pRow, FALSE);
		out_system_row_count = GetRowCount();
	}
	else {
		pRow = GetRow(row_index);
	}

	return pRow;
}

/////////////////////////////////////////////////////////////////////////////
// CFileItem Class

CSymbolItem::CSymbolItem(const CString& strValue, OutSystemView& pOutSystemVeiw) :
	CBCGPGridItem(_variant_t((LPCTSTR)strValue)), pOutSystemVeiw_(pOutSystemVeiw)
{
	m_dwFlags = PROP_HAS_BUTTON;
	id_ = IdGenerator::get_id();

	mainApp.event_hub()->subscribe_symbol_event_handler(id_, std::bind(&CSymbolItem::set_symbol_from_out, this, std::placeholders::_1, std::placeholders::_2));
}
//****************************************************************************************
void CSymbolItem::OnClickButton(CPoint point)
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList();

	pGridCtrl->EndEditItem(TRUE);

	m_bButtonIsDown = TRUE;
	Redraw();

	HdSymbolSelecter dlg;
	dlg.set_source_window_id(id_);
	dlg.DoModal();

	m_bButtonIsDown = FALSE;

	
	pGridCtrl->SetFocus();
	SetItemChanged();
	Redraw();
}

void CSymbolItem::set_symbol_from_out(const int window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (window_id != id_ || !symbol) return;
	m_varValue = symbol->SymbolCode().c_str();
	SetItemChanged();
	Redraw();
}



/////////////////////////////////////////////////////////////////////////////
// CFileItem Class

CAccountItem::CAccountItem(const CString& strValue, OutSystemView& pOutSystemVeiw) :
	CBCGPGridItem(_variant_t((LPCTSTR)strValue)), pOutSystemVeiw_(pOutSystemVeiw)
{
	m_dwFlags = PROP_HAS_BUTTON;
	id_ = IdGenerator::get_id();

	mainApp.event_hub()->subscribe_account_event_handler(id_, std::bind(&CAccountItem::set_account_from_out, this, std::placeholders::_1, std::placeholders::_2));
	mainApp.event_hub()->subscribe_fund_event_handler(id_, std::bind(&CAccountItem::set_fund_from_out, this, std::placeholders::_1, std::placeholders::_2));
}
//****************************************************************************************
void CAccountItem::OnClickButton(CPoint /*point*/)
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList();

	pGridCtrl->EndEditItem(TRUE);

	m_bButtonIsDown = TRUE;
	Redraw();
	//SmAccountFundSelector dlg;
	//dlg.set_source_id(id_);
	//dlg.DoModal();

	m_bButtonIsDown = FALSE;

// 	if (m_pWndInPlace != NULL)
// 	{
// 		m_pWndInPlace->SetWindowText(dlg.m_SelectedCode);
// 		m_pWndInPlace->SetFocus();
// 	}
// 	else
// 	{
// 		pGridCtrl->SetFocus();
// 	}

	SetItemChanged();
	Redraw();
}

void CAccountItem::set_account_from_out(const int window_id, std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (window_id != id_ || !account) return;
	m_varValue = account->No().c_str();
	CBCGPGridRow* pRow = GetParentRow();
	const int row_index = pRow->GetRowId();
	auto out_system = pOutSystemVeiw_.get_out_system(row_index);
	if (!out_system) return;
	out_system->account(account);

	SetItemChanged();
	Redraw();
}

void CAccountItem::set_fund_from_out(const int window_id, std::shared_ptr<DarkHorse::SmFund> fund)
{
	if (window_id != id_ || !fund) return;
	m_varValue = fund->Name().c_str();
	CBCGPGridRow* pRow = GetParentRow();
	const int row_index = pRow->GetRowId();
	auto out_system = pOutSystemVeiw_.get_out_system(row_index);
	if (!out_system) return;
	out_system->fund(fund);

	SetItemChanged();
	Redraw();
}

COutDefItem::COutDefItem(const CString& strValue, OutSystemView& pOutSystemVeiw)
	:CBCGPGridItem(_variant_t((LPCTSTR)strValue)), pOutSystemVeiw_(pOutSystemVeiw)
{
	id_ = IdGenerator::get_id();
}

void COutDefItem::SetItemChanged()
{
	CString value = m_varValue;
 	CBCGPGridRow* pRow = GetParentRow();
 	const int row_index = pRow->GetRowId();

	auto out_system = pOutSystemVeiw_.get_out_system(row_index);
	if (!out_system) return;
	out_system->name(std::string(value));

	if (m_pWndCombo) {
		int index = m_pWndCombo->GetCurSel();
		if (index >= 0) {
			CString text;
			m_pWndCombo->GetLBText(index, text);
		}
	}

	CBCGPGridItem::SetItemChanged();
}
