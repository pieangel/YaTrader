#include "stdafx.h"
#include "AccountPositionView.h"
#include <vector>
#include <map>
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Order/SmAccountOrderManager.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccount.h"
#include "../Position/TotalPositionManager.h"
#include "../Position/AccountPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Order/SmOrderRequest.h"
#include "../Client/ViStockClient.h"
#include "../Yuanta/YaStockClient.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Fund/SmFund.h"

#include "../Event/SmCallbackManager.h"
#include "../Order/OrderUi/DmAccountOrderWindow.h"
#include "../Fund/SmFundOrderDialog.h"
#include "../CompOrder/SmOrderCompMainDialog.h"
#include "../CompOrder/SmFundCompMainDialog.h"
#include "../Controller/AccountPositionControl.h"
#include "../Position/Position.h"
#include "../Util/VtStringUtil.h"
#include "../Log/MyLogger.h"
#include "../Event/EventHub.h"
#include "../Util/IdGenerator.h"
#include "../Symbol/SmSymbolManager.h"

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

void AccountPositionView::start_timer()
{
	SetTimer(1, 40, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl

IMPLEMENT_DYNAMIC(AccountPositionView, CBCGPGridCtrl)

AccountPositionView::AccountPositionView()
	: id_(IdGenerator::get_id())
{
	m_bExtendedPadding = FALSE;
	account_position_control_ = std::make_shared<DarkHorse::AccountPositionControl>();
	account_position_control_->set_single_position_event_handler(std::bind(&AccountPositionView::on_update_single_position, this, _1));
	account_position_control_->set_event_handler(std::bind(&AccountPositionView::on_update_whole_position, this, _1));
	ab_column_widths_vector_.push_back(70);
	ab_column_widths_vector_.push_back(35);
	ab_column_widths_vector_.push_back(70);
	ab_column_widths_vector_.push_back(40);

	dm_column_widths_vector_.push_back(65);
	dm_column_widths_vector_.push_back(35);
	dm_column_widths_vector_.push_back(50);
	dm_column_widths_vector_.push_back(65);
}

void AccountPositionView::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	fund_ = val;

	if (!account_position_control_) return;
	account_position_control_->set_fund(fund_);
	enable_position_show_ = true;
	// 계좌 유형에 따라 표시 내용과 표시 간격을 바꾼다.
	//SetColumnName(3, "평균단가");
	//SetColumnWidth(0, column_widths_vector_[0]);
	set_column_widths(fund_->fund_type());
	set_column_names(fund_->fund_type());
	update_account_position();
}

AccountPositionView::~AccountPositionView()
{
	//KillTimer(1);
	//mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);
	//mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
}

void AccountPositionView::OnHeaderCheckBoxClick(int nColumn)
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

void AccountPositionView::OnRowCheckBoxClick(CBCGPGridRow* pRow)
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

void AccountPositionView::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	account_ = val;
	if (!account_position_control_) return;
	account_position_control_->set_account(account_);
	enable_position_show_ = true;
	// 계좌 유형에 따라 표시 내용과 표시 간격을 바꾼다.
	//SetColumnName(3, "평균단가");
	//SetColumnWidth(0, column_widths_vector_[0]);
	set_column_widths(account_->Type());
	set_column_names(account_->Type());
	update_account_position();
}



void AccountPositionView::UpdatePositionInfo()
{
	
}

void AccountPositionView::LiqSelPositions()
{
	_Mode == 0 ? LiqSelPositionsForAccount() : LiqSelPositionsForFund();
	ClearCheck();
}

void AccountPositionView::LiqAll()
{
	_Mode == 0 ? LiqAllForAccount() : LiqAllForFund();
	ClearCheck();
}

void AccountPositionView::on_update_single_position(const int position_id)
{
	/*
	auto found = position_to_row_.find(position_id);
	if (found == position_to_row_.end()) return;
	std::string format_type;
	if (account_) format_type = account_->Type();
	else if (fund_) format_type = fund_->fund_type();
	else return;

	CBCGPGridRow* pRow = GetRow(found->second);
	if (!pRow) return;
	auto found2 = row_to_position_.find(found->second);
	if (found2 == row_to_position_.end()) return;
	auto position = found2->second;
	if (format_type == "1")
		update_ab_account_position(pRow, position, format_type);
	else
		update_dm_account_position(pRow, position, format_type);
	Invalidate();
	*/
	enable_position_show_ = true;
}

void AccountPositionView::on_update_whole_position(const int result)
{
// 	if (result == 0) {
// 		ClearOldContents(0);
// 	}
	enable_position_show_ = true;
}

BEGIN_MESSAGE_MAP(AccountPositionView, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl message handlers

int AccountPositionView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	InsertColumn(0, _T("종목"), ab_column_widths_vector_[0]);
	InsertColumn(1, _T("구분"), ab_column_widths_vector_[1]);
	InsertColumn(2, _T("평가손익"), ab_column_widths_vector_[2]);
	InsertColumn(3, _T("수량"), ab_column_widths_vector_[3]);

	

	FreezeColumns(0);

	const int nColumns = GetColumnCount();


	for (int row = 0; row < 100; row++) {
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


	//start_timer();
	

	return 0;
}

void AccountPositionView::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void AccountPositionView::on_timer()
{
	bool needDraw = false;
	if (enable_position_show_) {
		update_account_position();
		needDraw = true;
		enable_position_show_ = false;
	}
	if (needDraw) Invalidate();
}

void AccountPositionView::ClearCheck()
{
	_HeaderCheck = false; CheckAll(FALSE);
	UpdateHeaderCheckbox();	Invalidate();
}

void AccountPositionView::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	enable_position_show_ = true;
}

void AccountPositionView::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void AccountPositionView::set_column_widths(std::vector<int> column_width_vector)
{
	ab_column_widths_vector_ = column_width_vector;
	SetColumnWidth(0, ab_column_widths_vector_[0]);
	SetColumnWidth(1, ab_column_widths_vector_[1]);
	SetColumnWidth(2, ab_column_widths_vector_[2]);
	SetColumnWidth(3, ab_column_widths_vector_[3]);

	EnableCheckBoxes(FALSE);

	AdjustLayout();
}

void AccountPositionView::set_column_widths(const std::string& type)
{
	if (type == "1") {
		SetColumnWidth(0, ab_column_widths_vector_[0]);
		SetColumnWidth(1, ab_column_widths_vector_[1]);
		SetColumnWidth(2, ab_column_widths_vector_[2]);
		SetColumnWidth(3, ab_column_widths_vector_[3]);
		EnableCheckBoxes(FALSE);
	}
	else {
		SetColumnWidth(0, dm_column_widths_vector_[0]);
		SetColumnWidth(1, dm_column_widths_vector_[1]);
		SetColumnWidth(2, dm_column_widths_vector_[2]);
		SetColumnWidth(3, dm_column_widths_vector_[3]);
		EnableCheckBoxes(FALSE);
	}
	AdjustLayout();
}

void AccountPositionView::set_column_names(const std::string& type)
{
	if (type == "1") { // abroad
		SetColumnName(0, _T("종목"));
		SetColumnName(1, _T("구분"));
		SetColumnName(2, _T("평가손익"));
		SetColumnName(3, _T("수량"));
	}
	else { // domestic
		SetColumnName(0, _T("종목"));
		SetColumnName(1, _T("구분"));
		SetColumnName(2, _T("평균가"));
		SetColumnName(3, _T("평가손익"));
	}
}

void AccountPositionView::LiqSelPositionsForAccount()
{
	if (!account_) return;

	for (auto it = row_to_position_.begin(); it != row_to_position_.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(it->first);
		if (pRow->GetCheck()) {
			std::shared_ptr<SmOrderRequest> order_req = nullptr;
			if (it->second->open_quantity  > 0)
				order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account_->No(), account_->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
			else if (it->second->open_quantity < 0)
				order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account_->No(), account_->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
			mainApp.Client()->NewOrder(order_req);
		}
	}
}

void AccountPositionView::LiqSelPositionsForFund()
{
	if (!fund_) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = fund_->GetAccountVector();
	for (auto it2 = account_vec.begin(); it2 != account_vec.end(); it2++) {
		auto account = *it2;
		for (auto it = row_to_position_.begin(); it != row_to_position_.end(); ++it) {
			CBCGPGridRow* pRow = GetRow(it->first);
			if (pRow->GetCheck()) {
				std::shared_ptr<SmOrderRequest> order_req = nullptr;
				if (it->second->open_quantity > 0)
					order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
				else if (it->second->open_quantity < 0)
					order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
				mainApp.Client()->NewOrder(order_req);
			}
		}
	}
}

void AccountPositionView::LiqAllForAccount()
{
	if (!account_) return;

	for (auto it = row_to_position_.begin(); it != row_to_position_.end(); ++it) {
		std::shared_ptr<SmOrderRequest> order_req = nullptr;
		if (it->second->open_quantity > 0)
			order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account_->No(), account_->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
		else if (it->second->open_quantity < 0)
			order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account_->No(), account_->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
		mainApp.Client()->NewOrder(order_req);
	}
}

void AccountPositionView::LiqAllForFund()
{
	if (!fund_) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = fund_->GetAccountVector();
	for (auto it2 = account_vec.begin(); it2 != account_vec.end(); it2++) {
		auto account = *it2;
		for (auto it = row_to_position_.begin(); it != row_to_position_.end(); ++it) {
			std::shared_ptr<SmOrderRequest> order_req = nullptr;
			if (it->second->open_quantity > 0)
				order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
			else if (it->second->open_quantity < 0)
				order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
			mainApp.Client()->NewOrder(order_req);
		}
	}
}

void AccountPositionView::update_account_position()
{
	if (!account_position_control_) return;

	if (updating_) return;
	updating_ = true;

	row_to_position_.clear();
	position_to_row_.clear();
	
	std::string format_type;
	if (account_) format_type = account_->Type();
	else if (fund_) format_type = fund_->fund_type();
	else return;

	const std::map<std::string, position_p>& active_positions = account_position_control_->get_active_position_map();
	int row = 0;
	for (auto it = active_positions.begin(); it != active_positions.end(); ++it) {
		const auto& position = it->second;
		CBCGPGridRow* pRow = GetRow(row);
		if (position->open_quantity == 0 || !pRow) continue;
		if (format_type == "1")
			update_ab_account_position(pRow, position, format_type);
		else
			update_dm_account_position(pRow, position, format_type);
			
		_OldContentRowSet.insert(row);
		row_to_position_[row] = position;
		position_to_row_[position->id] = row;
		row++;
	}
	ClearOldContents(row);
	_OldMaxRow = row;
	updating_ = false;
	enable_position_show_ = true;
}

void AccountPositionView::update_dm_account_position(CBCGPGridRow* pRow, position_p position, const std::string& format_type)
{
	pRow->GetItem(0)->SetValue(position->symbol_code.c_str(), TRUE);
	pRow->GetItem(0)->SetBackgroundColor(RGB(255, 255, 255));
	pRow->GetItem(0)->SetTextColor(RGB(0, 0, 0));
	if (position->open_quantity > 0) {
		//pRow->GetItem(0)->SetBackgroundColor(RGB(255, 0, 0));
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255));
		//pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
		//pRow->GetItem(3)->SetBackgroundColor(RGB(255, 0, 0));

		//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(255, 0, 0));
		//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
		
	}
	else if (position->open_quantity < 0) {
		//pRow->GetItem(0)->SetBackgroundColor(RGB(0, 0, 255));
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255));
		//pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
		//pRow->GetItem(3)->SetBackgroundColor(RGB(0, 0, 255));

		//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(0, 0, 255));
		//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
	}
	else {
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(0, 0, 0));
	}

	std::string open_quantity = VtStringUtil::get_format_value(position->open_quantity, 0, true);
	pRow->GetItem(1)->SetValue(open_quantity.c_str(), TRUE);
	pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255));
	pRow->GetItem(1)->SetTextColor(RGB(0, 0, 0));
	std::string value_string = VtStringUtil::get_format_value(position->average_price / pow(10, 2), 2, true);
	pRow->GetItem(2)->SetValue(value_string.c_str(), TRUE);
	pRow->GetItem(2)->SetBackgroundColor(RGB(255, 255, 255));
	pRow->GetItem(2)->SetTextColor(RGB(0, 0, 0));
	//const std::string open_pl = std::format("{0:.2f}", position->open_profit_loss);
	const int decimal = format_type == "1" ? 2 : 0;
	std::string open_pl = VtStringUtil::get_format_value(position->open_profit_loss, decimal, true);

	CBCGPGridItem* pItem = pRow->GetItem(3);
	if (pItem) {
		pItem->SetValue(open_pl.c_str(), TRUE);
		if (position->open_profit_loss > 0) {
			pItem->SetBackgroundColor(RGB(255, 255, 255));
			pItem->SetTextColor(RGB(255, 0, 0));
		}
		else if (position->open_profit_loss < 0) {
			pItem->SetBackgroundColor(RGB(255, 255, 255));
			pItem->SetTextColor(RGB(0, 0, 255));
		}
		else {
			pItem->SetBackgroundColor(RGB(255, 255, 255));
			pItem->SetTextColor(RGB(0, 0, 0));
		}
	}
	
}

void AccountPositionView::update_ab_account_position(CBCGPGridRow* pRow, position_p position, const std::string& format_type)
{
	pRow->GetItem(0)->SetValue(position->symbol_code.c_str(), TRUE);
	pRow->GetItem(0)->SetBackgroundColor(RGB(255, 255, 255));
	pRow->GetItem(0)->SetTextColor(RGB(0, 0, 0));
	if (position->open_quantity > 0) {
		pRow->GetItem(0)->SetBackgroundColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255));
		//pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
		//pRow->GetItem(3)->SetBackgroundColor(RGB(255, 0, 0));

		//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(255, 0, 0));
		//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));

		pRow->GetItem(1)->SetValue("매수", TRUE);
	}
	else if (position->open_quantity < 0) {
		pRow->GetItem(1)->SetValue("매도", TRUE);
		pRow->GetItem(0)->SetBackgroundColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255));
		//pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
		//pRow->GetItem(3)->SetBackgroundColor(RGB(0, 0, 255));

		//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(0, 0, 255));
		//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
	}
	std::string open_quantity = VtStringUtil::get_format_value(position->open_quantity, 0, true);
	pRow->GetItem(3)->SetValue(open_quantity.c_str(), TRUE);
	pRow->GetItem(3)->SetBackgroundColor(RGB(255, 255, 255));
	pRow->GetItem(3)->SetTextColor(RGB(0, 0, 0));
	//const std::string open_pl = std::format("{0:.2f}", position->open_profit_loss);
	const int decimal = format_type == "1" ? 2 : 0;
	std::string open_pl = VtStringUtil::get_format_value(position->open_profit_loss, decimal, true);
	if (position->open_profit_loss > 0) {
		pRow->GetItem(2)->SetBackgroundColor(RGB(255, 255, 255));
		pRow->GetItem(2)->SetTextColor(RGB(255, 0, 0));
	}
	else if (position->open_profit_loss < 0) {
		pRow->GetItem(2)->SetBackgroundColor(RGB(255, 255, 255));
		pRow->GetItem(2)->SetTextColor(RGB(0, 0, 255));
	}
	else {
		pRow->GetItem(2)->SetBackgroundColor(_DefaultBackColor);
		pRow->GetItem(2)->SetTextColor(RGB(0, 0, 0));
	}
	pRow->GetItem(2)->SetValue(open_pl.c_str(), TRUE);
}

void AccountPositionView::ClearOldContents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(*it);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->SetValue("");
			pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
		}
	}
}

void AccountPositionView::ClearOldContents(const int& last_index)
{
	try {
		for (int row = last_index; row <= _OldMaxRow; row++) {
			CBCGPGridRow* pRow = GetRow(row);
			for (int i = 0; i < GetColumnCount(); i++) {
				pRow->GetItem(i)->SetValue("");
				pRow->GetItem(i)->EmptyValue(FALSE);
				pRow->GetItem(i)->SetBackgroundColor(_DefaultBackColor);
				pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
			}
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
	}
}

void AccountPositionView::ToggleExtendedPadding()
{
	m_bExtendedPadding = !m_bExtendedPadding;

	EnableRowHeader(TRUE);	// Need to recalculate the width
	AdjustLayout();
}




void AccountPositionView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* pItem;
	HitTest(point, id, pItem);



	CString msg;
	//msg.Format("%d", nColumn);
	//AfxMessageBox(msg);

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

		auto found = row_to_position_.find(id.m_nRow);
		if (found == row_to_position_.end()) return;

		auto symbol = mainApp.SymMgr()->FindSymbol(found->second->symbol_code);
		if (!symbol) return;
		mainApp.event_hub()->trigger_ab_symbol_event(1, symbol);
	}
	Invalidate();






	//CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}


void AccountPositionView::OnTimer(UINT_PTR nIDEvent)
{
	if (enable_position_show_) {
		update_account_position();
		//needDraw = true;
		enable_position_show_ = false;
	}

	if (_EnableQuoteShow) {
		UpdatePositionInfo();
		_EnableQuoteShow = false;
	}

	CBCGPGridCtrl::OnTimer(nIDEvent);
}
