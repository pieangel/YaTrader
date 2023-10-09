#include "stdafx.h"
#include "FavoriteSymbolView.h"
#include <map>
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Order/SmAccountOrderManager.h"
#include "../Order/SmSymbolOrderManager.h"

#include "../Account/SmAccount.h"
#include "../resource.h"
#include "../Symbol/SmSymbolTableDialog.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Order/AbAccountOrderWindow.h"
#include "../Order/AbFundOrderWindow.h"
#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "../CompOrder/SmOrderCompMainDialog.h"
#include "../CompOrder/SmFundCompMainDialog.h"
#include "../Controller/QuoteControl.h"
#include "../ViewModel/VmQuote.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Util/SmUtil.h"
#include "../Event/EventHub.h"
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

IMPLEMENT_DYNAMIC(FavoriteSymbolView, CBCGPGridCtrl)

FavoriteSymbolView::FavoriteSymbolView()
{
	m_bExtendedPadding = FALSE;
	quote_control_ = std::make_shared<DarkHorse::QuoteControl>();
	quote_control_->set_event_handler(std::bind(&FavoriteSymbolView::on_update_quote, this));
	mainApp.SymMgr()->RegisterFavoriteSymbols();
}

FavoriteSymbolView::~FavoriteSymbolView()
{
	mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);
}

void FavoriteSymbolView::on_update_quote()
{
	_EnableQuoteShow = true;
}
void FavoriteSymbolView::update_quote()
{
	if (!quote_control_) return;
	const VmQuote quote = quote_control_->get_quote();
	auto found = symbol_to_row_.find(quote.symbol_id);
	if (found == symbol_to_row_.end()) return;
	int row = found->second;
	auto found_symbol = _RowToSymbolMap.find(row);
	if (found_symbol == _RowToSymbolMap.end()) return;

	CBCGPGridRow* pRow = GetRow(row);
	std::string value_string;
	value_string = std::format("{0}", quote.close);
	SmUtil::insert_decimal(value_string, found_symbol->second->decimal());

	pRow->GetItem(1)->SetValue(value_string.c_str(), TRUE);
}

void FavoriteSymbolView::OnHeaderCheckBoxClick(int nColumn)
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

void FavoriteSymbolView::OnRowCheckBoxClick(CBCGPGridRow* pRow)
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

void FavoriteSymbolView::UpdateAcceptedOrder()
{
	
}

void FavoriteSymbolView::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

BEGIN_MESSAGE_MAP(FavoriteSymbolView, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_MENU_REMOVE, &FavoriteSymbolView::OnMenuRemove)
	ON_COMMAND(ID_MENU_ADD, &FavoriteSymbolView::OnMenuAdd)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &FavoriteSymbolView::OnUmSymbolSelected)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl message handlers

int FavoriteSymbolView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	InsertColumn(1, _T("현재가"), 100);



	FreezeColumns(0);

	const int nColumns = GetColumnCount();


	for (int row = 0; row < 100; row++) {
		// Create new row:
		CBCGPGridRow* pRow = CreateRow(nColumns);
		// Set each column data:
		for (int nColumn = 0; nColumn < nColumns; nColumn++)
		{
			//long lValue = (nColumn + 1) * (row + 1);
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

	EnableCheckBoxes(TRUE);
	SetHeaderAlign(0, 2);
	SetHeaderAlign(1, 2);



	CheckAll(FALSE);
	UpdateHeaderCheckbox();
	AdjustLayout();

	SetTimer(1, 40, NULL);

	return 0;
}

void FavoriteSymbolView::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void FavoriteSymbolView::ClearOldCotents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(*it);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->SetValue("");
			pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
		}
	}
}

void FavoriteSymbolView::ClearOldContents(const int& last_index)
{
	for (int row = last_index; row < _OldMaxRow; row++) {
		CBCGPGridRow* pRow = GetRow(row);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->SetValue("");
		}
	}
}

void FavoriteSymbolView::ToggleExtendedPadding()
{
	m_bExtendedPadding = !m_bExtendedPadding;

	EnableRowHeader(TRUE);	// Need to recalculate the width
	AdjustLayout();
}

void FavoriteSymbolView::Clear()
{
	for (int row = 1; row < (int)_OldContentRowSet.size(); row++) {
		CBCGPGridRow* pRow = GetRow(row);
		for (int col = 0; col < GetColumnCount(); col++) {
			pRow->GetItem(col)->SetValue("");
		}
	}
}

void FavoriteSymbolView::SetFavorite()
{
	Clear();
	// 반드시 실시간 등록을 해줄것
	const std::map<int, std::shared_ptr<SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	_RowToSymbolMap.clear();
	symbol_to_row_.clear();
	int row = 0;
	for (auto it = favorite_map.begin(); it != favorite_map.end(); ++it) {
		_RowToSymbolMap[row] = it->second;
		symbol_to_row_[it->second->Id()] = row;
		_OldContentRowSet.insert(row);
		CBCGPGridRow* pRow = GetRow(row);
		pRow->GetItem(0)->SetValue(it->second->SymbolCode().c_str(), TRUE);

		std::string value_string;
		auto quote = mainApp.QuoteMgr()->get_quote(it->second->SymbolCode());
		value_string = std::format("{0}", quote->close);
		if (it->second->decimal() > 0 && value_string.length() > (size_t)it->second->decimal())
			value_string.insert(value_string.length() - it->second->decimal(), 1, '.');

		pRow->GetItem(1)->SetValue(value_string.c_str(), TRUE);
		row++;
	}
	Invalidate();
}

void FavoriteSymbolView::Update()
{
	if (_EnableQuoteShow) {
		update_quote();
		_EnableQuoteShow = false;
	}
}

void FavoriteSymbolView::UpdateQuote()
{
	const std::map<int, std::shared_ptr<SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	int row = 0;
	for (auto it = favorite_map.begin(); it != favorite_map.end(); ++it) {
		_RowToSymbolMap[row] = it->second;
		_OldContentRowSet.insert(row);
		CBCGPGridRow* pRow = GetRow(row);


		std::string value_string;

		value_string = std::format("{0}", it->second->Qoute.close);
		if (it->second->decimal() > 0 && value_string.length() > (size_t)it->second->decimal())
			value_string.insert(value_string.length() - it->second->decimal(), 1, '.');

		pRow->GetItem(1)->SetValue(value_string.c_str(), TRUE);
		row++;
	}
}

void FavoriteSymbolView::AddSymbol(const int& symbol_id)
{
	auto symbol = mainApp.SymMgr()->FindSymbolById(symbol_id);
	if (!symbol) return;

	mainApp.SymMgr()->AddFavorite(symbol_id);

	SetFavorite();
}

void FavoriteSymbolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu;

	// 팝업 메뉴를 생성한다.
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_MENU_REMOVE, _T("Remove"));
	menu.AppendMenu(MF_STRING, ID_MENU_ADD, _T("Add"));

	CPoint pt = point;
	ClientToScreen(&pt);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

	menu.DestroyMenu();

	CBCGPGridCtrl::OnRButtonDown(nFlags, point);
}


void FavoriteSymbolView::OnLButtonDown(UINT nFlags, CPoint point)
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

		auto found = _RowToSymbolMap.find(id.m_nRow);
		if (found == _RowToSymbolMap.end()) return;

		//if (_OrderWnd) _OrderWnd->OnSymbolClicked(found->second);
		//if (_FundOrderWnd) _FundOrderWnd->OnSymbolClicked(found->second);
		//if (_CompOrderWnd) _CompOrderWnd->OnSymbolClicked(found->second);
		//if (_CompFundWnd) _CompFundWnd->OnSymbolClicked(found->second);
		mainApp.event_hub()->process_symbol_event(order_window_id_, found->second);
	}
	Invalidate();

	//CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}

LRESULT FavoriteSymbolView::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	AddSymbol(static_cast<int>(wParam));
	return 1;
}

void FavoriteSymbolView::OnMenuRemove()
{

}

void FavoriteSymbolView::OnMenuAdd()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	_SymbolTableDlg->favorite_symbol_view_ = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

void FavoriteSymbolView::OnTimer(UINT_PTR nIDEvent)
{
	bool needDraw = false;
	if (_EnableQuoteShow) {
		update_quote();
		_EnableQuoteShow = false;
		needDraw = true;
	}

	if (needDraw) Invalidate();

	CBCGPGridCtrl::OnTimer(nIDEvent);
}

