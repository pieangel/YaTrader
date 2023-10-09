//#include "stdafx.h"
//#include "AccountOrderView.h"

#include "stdafx.h"
#include "AccountOrderView.h"
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
#include "../Fund/SmFundManager.h"
#include "../Fund/SmFund.h"
#include "../Account/SmAccountManager.h"
#include "../Order/OrderUi/DmAccountOrderWindow.h"
#include "../Order/OrderUi/DmFundOrderWindow.h"
#include <functional>
#include "../CompOrder/SmOrderCompMainDialog.h"
#include "../CompOrder/SmFundCompMainDialog.h"
#include "../Controller/AccountOrderControl.h"
#include "../Order/Order.h"
#include "../Util/VtStringUtil.h"
#include "../Util/IdGenerator.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Event/EventHub.h"
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

IMPLEMENT_DYNAMIC(AccountOrderView, CBCGPGridCtrl)

AccountOrderView::AccountOrderView()
	: id_(IdGenerator::get_id())
{
	m_bExtendedPadding = FALSE;
	account_order_control_ = std::make_shared<DarkHorse::AccountOrderControl>();
	account_order_control_->set_event_handler(std::bind(&AccountOrderView::on_update_order, this));
}

void AccountOrderView::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	_Fund = val;
	account_order_control_->set_fund(val);
}

void AccountOrderView::on_update_order()
{
	enable_order_show_ = true;
}

AccountOrderView::~AccountOrderView()
{
	//KillTimer(1);
}

void AccountOrderView::OnHeaderCheckBoxClick(int nColumn)
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

void AccountOrderView::OnRowCheckBoxClick(CBCGPGridRow* pRow)
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

void AccountOrderView::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	_Account = val;
	account_order_control_->set_account(_Account);
}

void AccountOrderView::UpdateAcceptedOrder()
{
	_Mode == 0 ? UpdateAccountAcceptedOrders() : UpdateFundAcceptedOrders();
}

void AccountOrderView::CancelSelOrders()
{
	_Mode == 0 ? CancelSelAccountOrders() : CancelSelFundOrders();
	ClearCheck();
}

void AccountOrderView::CancelAll()
{
	_Mode == 0 ? CancelAllAccountOrders() : CancelAllFundOrders();
	ClearCheck();
}

BEGIN_MESSAGE_MAP(AccountOrderView, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl message handlers

int AccountOrderView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	return 0;
}

void AccountOrderView::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void AccountOrderView::update_order()
{
	if (!account_order_control_ ) return;
	std::vector<order_p> order_vec;
	account_order_control_->get_accepted_order_vector(order_vec);
	if (order_vec.empty()) {
		ClearOldContents(0);
		return;
	}
	int row = 0;
	for (auto it2 = order_vec.begin(); it2 != order_vec.end(); it2++) {
		auto order = *it2;
		CBCGPGridRow* pRow = GetRow(row);
		if (!pRow) continue;
		pRow->GetItem(0)->SetValue(order->symbol_code.c_str());
		pRow->GetItem(0)->SetTextColor(RGB(255, 0, 0));
		if (order->position == SmPositionType::Buy) {
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
		pRow->GetItem(2)->SetValue(std::to_string(order->order_amount).c_str());

		//const int decimal = _Account->Type() == "1" ? 2 : 0;
		//const std::string open_p1 = VtStringUtil::get_format_value(order->order_price, decimal, true);

		pRow->GetItem(3)->SetValue(std::to_string(order->order_price).c_str());
		_OldContentRowSet.insert(row);
		row_to_order_[row] = order;
		pRow->GetItem(0)->Redraw();
		pRow->GetItem(1)->Redraw();
		pRow->GetItem(2)->Redraw();
		pRow->GetItem(3)->Redraw();
		row++;
	}
	ClearOldContents(row);
	_OldMaxRow = row;
	enable_order_show_ = true;
}

void AccountOrderView::ClearCheck()
{
	_HeaderCheck = false; CheckAll(FALSE);
	UpdateHeaderCheckbox();	Invalidate();
}

void AccountOrderView::ClearOldContents()
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

void AccountOrderView::ClearOldContents(const int& last_index)
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

void AccountOrderView::UpdateAccountAcceptedOrders()
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

void AccountOrderView::UpdateFundAcceptedOrders()
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

void AccountOrderView::CancelSelAccountOrders()
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

void AccountOrderView::CancelSelFundOrders()
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

void AccountOrderView::CancelAllAccountOrders()
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

void AccountOrderView::CancelAllFundOrders()
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

void AccountOrderView::ToggleExtendedPadding()
{
	m_bExtendedPadding = !m_bExtendedPadding;

	EnableRowHeader(TRUE);	// Need to recalculate the width
	AdjustLayout();
}



void AccountOrderView::on_timer()
{
	bool needDraw = false;
	if (enable_order_show_) {
		update_order();
		needDraw = true;
		enable_order_show_ = false;
	}
	if (needDraw) Invalidate();
}

void AccountOrderView::StartTimer()
{
	SetTimer(1, 40, NULL);
}

void AccountOrderView::Update()
{
	if (enable_order_show_) {
		UpdateAcceptedOrder();
		//needDraw = true;
		enable_order_show_ = false;
	}
}

void AccountOrderView::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	enable_order_show_ = true;
}

void AccountOrderView::OnLButtonDown(UINT nFlags, CPoint point)
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
		auto found = row_to_order_.find(id.m_nRow);
		if (found == row_to_order_.end()) return;

		auto symbol = mainApp.SymMgr()->FindSymbol(found->second->symbol_code);
		if (!symbol) return;
		mainApp.event_hub()->trigger_ab_symbol_event(1, symbol);
	}
	Invalidate();



	//Invalidate();
	//CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}


void AccountOrderView::OnTimer(UINT_PTR nIDEvent)
{
	//bool needDraw = false;

	if (enable_order_show_) {
		UpdateAcceptedOrder();
		//needDraw = true;
		enable_order_show_ = false;
	}

	//if (needDraw) Invalidate(FALSE);

	CBCGPGridCtrl::OnTimer(nIDEvent);
}
