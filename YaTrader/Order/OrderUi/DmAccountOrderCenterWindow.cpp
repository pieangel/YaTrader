//#include "stdafx.h"
//#include "DmAccountOrderCenterWindow.h"

#include "stdafx.h"
#include "../../DarkHorse.h"
#include "DmAccountOrderCenterWindow.h"
#include "afxdialogex.h"
#include "../../Grid/customcells.h"
#include "../SmOrderGridConst.h"
#include "../../Util/VtStringUtil.h"
#include "../../Global/SmTotalManager.h"
#include "../../Symbol/SmSymbolManager.h"
#include "../../Symbol/SmSymbol.h"
#include "../../Symbol/SmProduct.h"
#include "../../Symbol/SmProductYearMonth.h"
#include "../../Symbol/MarketDefine.h"
#include "BCGPGridCtrl.h"
#include <format>
#include "../../Log/MyLogger.h"
#include "../../SmGrid/SmGrid.h"
#include "../../SmGrid/SmCell.h"
#include "../../Account/SmAccount.h"
#include "../../Account/SmAccountManager.h"
#include "DmAccountOrderWindow.h"
#include "../../Symbol/SmSymbolTableDialog.h"
#include "../../Task/SmTaskRequestMaker.h"
#include "../../Client/ViStockClient.h"
#include "../OrderWndConst.h"
#include "../../Order/SmSymbolOrderManager.h"
#include "../../Order/SmTotalOrderManager.h"
#include "../../Position/SmTotalPositionManager.h"
#include "../../Position/SmAccountPositionManager.h"
#include "../../Position/SmPosition.h"
#include "../../Order/SmOrderRequest.h"
#include "../../Client/ViStockClient.h"
#include "../../Order/SmOrderRequestManager.h"
#include "../SmOrderSetDialog.h"
#include "../../Global/SmTotalManager.h"
#include "../../Task/SmTaskArg.h"
#include "../../Task/SmTaskRequestManager.h"
#include "../../Util/IdGenerator.h"
#include "../../Event/EventHub.h"
#include "../../Position/TotalPositionManager.h"
#include "../../Position/AccountPositionManager.h"
#include "../../Position/Position.h"
#include "../../Order/OrderRequest/OrderRequestManager.h"
#include "../../Order/OrderRequest/OrderRequest.h"
#include "../../Symbol/SymbolConst.h"
#include "../../Dialog/HdSymbolSelecter.h"
#include "../../Position/GroupPositionManager.h"
#include "../../Fund/SmFund.h"
#include "DmFundOrderWindow.h"
#include <functional>
using namespace std::placeholders;
// SmOrderWnd dialog
#define BTN_ORDER_AMOUNT 0x00000001
#define BTN_ORDER_SELL 0x00000002
#define BTN_ORDER_BUY 0x00000003
#define BTN_ORDER_CENTER_QUOTE 0x00000004

using namespace DarkHorse;

int DmAccountOrderCenterWindow::DeltaOrderArea = 0;

using account_position_manager_p = std::shared_ptr<DarkHorse::AccountPositionManager>;

IMPLEMENT_DYNAMIC(DmAccountOrderCenterWindow, CBCGPDialog)

DmAccountOrderCenterWindow::DmAccountOrderCenterWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_CENTER, pParent), layout_manager_(this)
{
	id_ = IdGenerator::get_id();
	symbol_order_view_.symbol_type(SymbolType::Domestic);
	symbol_position_view_.symbol_type(SymbolType::Domestic);
	symbol_order_view_.set_order_request_type(OrderRequestType::Domestic);
	symbol_order_view_.set_fill_condition(SmFilledCondition::Fas);
	mainApp.event_hub()->subscribe_symbol_event_handler(id_, std::bind(&DmAccountOrderCenterWindow::set_symbol_from_out, this, std::placeholders::_1, std::placeholders::_2));
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	symbol_order_view_.set_parent(this);
	symbol_order_view_.set_center_window_id(id_);
	symbol_order_view_.set_order_window_id(order_window_id_);
	symbol_tick_view_.set_parent(this);
	mainApp.event_hub()->add_window_resize_event(symbol_order_view_.get_id(), std::bind(&DmAccountOrderCenterWindow::on_resize_event_from_order_view, this));
	mainApp.event_hub()->add_parameter_event(symbol_order_view_.get_id(), std::bind(&DmAccountOrderCenterWindow::on_paramter_event, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

DmAccountOrderCenterWindow::DmAccountOrderCenterWindow(CWnd* pParent, std::string symbol_code, DarkHorse::OrderSetEvent order_set)
	: CBCGPDialog(IDD_ORDER_CENTER, pParent), symbol_code_(symbol_code), order_set_(order_set), layout_manager_(this)
{
		id_ = IdGenerator::get_id();
		symbol_order_view_.order_set(order_set);
		if (order_set_.show_symbol_tick)
			show_symbol_tick_view_ = true;
		else
			show_symbol_tick_view_ = false;
		symbol_order_view_.symbol_type(SymbolType::Domestic);
		symbol_position_view_.symbol_type(SymbolType::Domestic);
		symbol_order_view_.set_order_request_type(OrderRequestType::Domestic);
		symbol_order_view_.set_fill_condition(SmFilledCondition::Fas);
		mainApp.event_hub()->subscribe_symbol_event_handler(id_, std::bind(&DmAccountOrderCenterWindow::set_symbol_from_out, this, std::placeholders::_1, std::placeholders::_2));
		EnableVisualManagerStyle(TRUE, TRUE);
		EnableLayout();
		symbol_order_view_.set_parent(this);
		symbol_order_view_.set_center_window_id(id_);
		symbol_order_view_.set_order_window_id(order_window_id_);
		symbol_tick_view_.set_parent(this);
		mainApp.event_hub()->add_window_resize_event(symbol_order_view_.get_id(), std::bind(&DmAccountOrderCenterWindow::on_resize_event_from_order_view, this));
		mainApp.event_hub()->add_parameter_event(symbol_order_view_.get_id(), std::bind(&DmAccountOrderCenterWindow::on_paramter_event, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

DmAccountOrderCenterWindow::~DmAccountOrderCenterWindow()
{
	//KillTimer(1);
	int i = 0;
	i = i + 0;
	mainApp.event_hub()->unsubscribe_symbol_event_handler(id_);
}

void DmAccountOrderCenterWindow::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	account_ = val;
	if (account_->is_subaccount()) {
		position_type_ = OrderType::SubAccount;
	}
	else {
		position_type_ = OrderType::MainAccount;
	}
	symbol_order_view_.Account(val);
	symbol_order_view_.Refresh();
	symbol_position_view_.Account(val);
	symbol_position_view_.Refresh();
}

void DmAccountOrderCenterWindow::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	fund_ = val;
	position_type_ = OrderType::Fund;
	symbol_order_view_.fund(val);
	symbol_order_view_.Refresh();
	symbol_position_view_.fund(val);
	symbol_position_view_.Refresh();
}

void DmAccountOrderCenterWindow::Selected(bool val)
{
	selected_ = val;
	symbol_order_view_.Selected(val);
}

void DmAccountOrderCenterWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, combo_symbol_);
	DDX_Control(pDX, IDC_STATIC_ORDER, symbol_order_view_);
	DDX_Control(pDX, IDC_STATIC_POSITION, symbol_position_view_);
	DDX_Control(pDX, IDC_STATIC_QUOTE, symbol_tick_view_);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_NAME_KR, static_symbol_name_);
	DDX_Control(pDX, IDC_CHECK_SHOW_REAL_QUOTE, _CheckShowRealTick);
	DDX_Control(pDX, IDC_SPIN_ORDER_AMOUNT, _OrderAmountSpin);
	DDX_Control(pDX, IDC_STATIC_FILLED, _StaticFilledCount);
	DDX_Control(pDX, IDC_CHECK_LOSS, _CheckLoss);
	DDX_Control(pDX, IDC_CHECK_PROFIT, _CheckProfit);
	DDX_Control(pDX, IDC_EDIT_LOSS, _EditLoss);
	DDX_Control(pDX, IDC_EDIT_PROFIT, _EditProfit);
	DDX_Control(pDX, IDC_EDIT_SLIP, _EditSlip);
	DDX_Control(pDX, IDC_EDIT_AMOUNT, _EditAmount);
	DDX_Control(pDX, IDC_SPIN_LCUT, _SpinLossCut);
	DDX_Control(pDX, IDC_SPIN_PCUT, _SpinProfitCut);
	DDX_Control(pDX, IDC_SPIN_SLIP, _SpinSlip);
	DDX_Control(pDX, IDC_RADIO_MARKET, _RadioMarket);
	DDX_Control(pDX, IDC_RADIO_PRICE, _RadioPrice);

	//DDX_Control(pDX, IDC_STATIC_CONFIG, _ConfigGrid);

	DDX_Control(pDX, IDC_BTN_SEARCH, btn_grid_config_);
	DDX_Control(pDX, IDC_STATIC_GROUP2, _Group2);
	DDX_Control(pDX, IDC_STATIC_GROUP3, _Group3);
	DDX_Control(pDX, IDC_STATIC_GROUP4, _Group4);
	DDX_Control(pDX, IDC_STATIC_FILLED_REMAIN, filled_remain_button_);
	DDX_Control(pDX, IDC_STATIC_REMAIN, remain_button_);
}


BEGIN_MESSAGE_MAP(DmAccountOrderCenterWindow, CBCGPDialog)
	ON_WM_TIMER()
	ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &DmAccountOrderCenterWindow::OnCbnSelchangeComboSymbol)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_SHOW_REAL_QUOTE, &DmAccountOrderCenterWindow::OnBnClickedCheckShowRealQuote)
	ON_BN_CLICKED(IDC_BTN_REFRESH_ORDER, &DmAccountOrderCenterWindow::OnBnClickedBtnRefreshOrder)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_SYMBOL, &DmAccountOrderCenterWindow::OnBnClickedBtnSymbol)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &DmAccountOrderCenterWindow::OnUmSymbolSelected)
	ON_BN_CLICKED(IDC_BTN_AMOUNT1, &DmAccountOrderCenterWindow::OnBnClickedBtnAmount1)
	ON_BN_CLICKED(IDC_BTN_AMOUNT2, &DmAccountOrderCenterWindow::OnBnClickedBtnAmount2)

	ON_BN_CLICKED(IDC_BTN_AMOUNT5, &DmAccountOrderCenterWindow::OnBnClickedBtnAmount5)
	ON_BN_CLICKED(IDC_BTN_AMOUNT3, &DmAccountOrderCenterWindow::OnBnClickedBtnAmount3)
	ON_BN_CLICKED(IDC_BTN_AMOUNT4, &DmAccountOrderCenterWindow::OnBnClickedBtnAmount4)
	ON_EN_CHANGE(IDC_EDIT_AMOUNT, &DmAccountOrderCenterWindow::OnEnChangeEditAmount)
	ON_BN_CLICKED(IDC_BTN_LIQ_SYMBOL_POSITION, &DmAccountOrderCenterWindow::OnBnClickedBtnLiqSymbolPosition)
	ON_BN_CLICKED(IDC_CHECK_PROFIT, &DmAccountOrderCenterWindow::OnBnClickedCheckProfit)
	ON_BN_CLICKED(IDC_CHECK_LOSS, &DmAccountOrderCenterWindow::OnBnClickedCheckLoss)
	ON_BN_CLICKED(IDC_RADIO_MARKET, &DmAccountOrderCenterWindow::OnBnClickedRadioMarket)
	ON_BN_CLICKED(IDC_RADIO_PRICE, &DmAccountOrderCenterWindow::OnBnClickedRadioPrice)
	ON_EN_CHANGE(IDC_EDIT_PROFIT, &DmAccountOrderCenterWindow::OnEnChangeEditProfit)
	ON_EN_CHANGE(IDC_EDIT_LOSS, &DmAccountOrderCenterWindow::OnEnChangeEditLoss)
	ON_EN_CHANGE(IDC_EDIT_SLIP, &DmAccountOrderCenterWindow::OnEnChangeEditSlip)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &DmAccountOrderCenterWindow::OnBnClickedBtnSearch)
	ON_BN_CLICKED(IDC_BTN_SET, &DmAccountOrderCenterWindow::OnBnClickedBtnSet)
	ON_STN_CLICKED(IDC_STATIC_FILLED_REMAIN, &DmAccountOrderCenterWindow::OnStnClickedStaticFilledRemain)
	ON_BN_CLICKED(IDC_CHECK_FIX_HOGA, &DmAccountOrderCenterWindow::OnBnClickedCheckFixHoga)
END_MESSAGE_MAP()


// DmAccountOrderCenterWindow message handlers

#define WS_CAPTION          0x00C00000L

LRESULT DmAccountOrderCenterWindow::OnEnterSizeMove(WPARAM wparam, LPARAM lparam)
{
	resizing_ = true;
	// do stuff      
	return (LRESULT)0;
}
LRESULT DmAccountOrderCenterWindow::OnExitSizeMove(WPARAM wparam, LPARAM lparam)
{

	resizing_ = false;
	// do stuff      
	return (LRESULT)0;
}

void DmAccountOrderCenterWindow::arrange_children()
{
	if (!init_dialog_) return;
	CRect rc_order_view, rc_tick_view, rc_window;
	symbol_order_view_.GetWindowRect(&rc_order_view);

	rcGrid.right = rcGrid.left + symbol_order_view_.get_entire_width();
	rcGrid.bottom = rc_order_view.top + rc_order_view.Height();
	const int tick_width = rcTick.Width();
	rcTick.left = rcGrid.right;
	rcTick.right = rcTick.left + tick_width;
	symbol_order_view_.MoveWindow(rcGrid, TRUE);
	if (show_symbol_tick_view_) {
		symbol_tick_view_.ShowWindow(SW_SHOW);
		window_width = symbol_order_view_.get_entire_width() + rcTick.Width();
		if (symbol_tick_view_.GetSafeHwnd())
			symbol_tick_view_.MoveWindow(rcTick);
	}
	else {
		symbol_tick_view_.ShowWindow(SW_HIDE);
		window_width = symbol_order_view_.get_entire_width();
	}
	symbol_order_view_.Invalidate();
}


void DmAccountOrderCenterWindow::SetMainDialog(DmAccountOrderWindow* main_dialog)
{
	symbol_order_view_.SetMainDialog(main_dialog);
}



void DmAccountOrderCenterWindow::SetFundDialog(DmFundOrderWindow* main_dialog)
{
	symbol_order_view_.SetFundDialog(main_dialog);
}

void DmAccountOrderCenterWindow::SetSelected(const bool& selected)
{
	selected_ = selected;
	symbol_order_view_.Selected(selected);
	symbol_order_view_.Invalidate();
}

void DmAccountOrderCenterWindow::OnSymbolClicked(const std::string& symbol_code)
{
	auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (symbol) set_symbol_from_out(order_window_id_, symbol);
}

void DmAccountOrderCenterWindow::SetOrderAmount(const int& count)
{
	CString strValue;
	strValue.Format("%d", count);
	_EditAmount.SetWindowText(strValue);
}

int DmAccountOrderCenterWindow::GetPositionCount()
{
	if (!account_ || !symbol_) return 0;

	std::shared_ptr<SmPosition> position = mainApp.TotalPosiMgr()->FindAddPosition(account_->No(), symbol_->SymbolCode());

	return position->OpenQty;
}

std::string DmAccountOrderCenterWindow::get_symbol_code()
{
	if (!symbol_) return "";
	return symbol_->SymbolCode();
}

void DmAccountOrderCenterWindow::ArrangeCenterValue()
{
	symbol_order_view_.ArrangeCenterValue();
}

void DmAccountOrderCenterWindow::CreateResource()
{

}

void DmAccountOrderCenterWindow::trigger_resize_event()
{
	CWnd* parent = GetParent();

	if (account_)
		((DmAccountOrderWindow*)parent)->RecalcChildren(show_symbol_tick_view_ ? CM_SHOW_TICK : CM_HIDE_TICK);
	if (fund_)
		((DmFundOrderWindow*)parent)->RecalcChildren(show_symbol_tick_view_ ? CM_SHOW_TICK : CM_HIDE_TICK);
}

void DmAccountOrderCenterWindow::recal_window_size()
{
	CRect rc_order_view, rc_tick_view, rc_window;
	symbol_order_view_.GetWindowRect(&rc_order_view);


	// 주문그리드 위치를 가져온다.
	//CRect& rcGrid = layout_manager_.GetRect(IDC_STATIC_ORDER);
	// 주문설정 보기 옵션에 따라 주문 그리드 하단을 설정한다.
	//int orderGridBottom = _ShowRemainConfig ? availableHeight - rcGrid.top - ConfigHeight : availableHeight - rcGrid.top;
	//CRect& rcTick = layout_manager_.GetRect(IDC_STATIC_QUOTE);
	// 주문 그리드 위치 및 크기 설정
	//rcGrid.left = 0;
	rcGrid.right = rcGrid.left + symbol_order_view_.get_entire_width();
	rcGrid.bottom = rc_order_view.Height() - extra_height_;
	const int tick_width = rcTick.Width();
	rcTick.left = rcGrid.right;
	rcTick.right = rcTick.left + tick_width;

	if (show_symbol_tick_view_) {
		symbol_tick_view_.ShowWindow(SW_SHOW);
		window_width = symbol_order_view_.get_entire_width() + rcTick.Width();
	}
	else {
		symbol_tick_view_.ShowWindow(SW_HIDE);
		window_width = symbol_order_view_.get_entire_width();
	}
}

CRect DmAccountOrderCenterWindow::GetClientArea(int resourceID)
{
	CWnd* wnd = (CWnd*)GetDlgItem(resourceID);
	CRect rcWnd;
	wnd->GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);

	return rcWnd;
}

void DmAccountOrderCenterWindow::save_control_size()
{
	layout_manager_.ParentWnd(this);
	//layout_manager_.AddWindow(_T("주문패널"), IDC_STATIC_ORDER, GetClientArea(IDC_STATIC_ORDER));
	//layout_manager_.AddWindow(_T("틱패널"), IDC_STATIC_QUOTE, GetClientArea(IDC_STATIC_QUOTE));
	rcGrid = GetClientArea(IDC_STATIC_ORDER);
	rcTick = GetClientArea(IDC_STATIC_QUOTE);
}

void DmAccountOrderCenterWindow::on_resize_event_from_order_view()
{
	if (!init_dialog_) return;
	CRect rc_order_view, rc_tick_view, rc_window;
	symbol_order_view_.GetWindowRect(&rc_order_view);


	// 주문그리드 위치를 가져온다.
	//CRect& rcGrid = layout_manager_.GetRect(IDC_STATIC_ORDER);
	// 주문설정 보기 옵션에 따라 주문 그리드 하단을 설정한다.
	//int orderGridBottom = _ShowRemainConfig ? availableHeight - rcGrid.top - ConfigHeight : availableHeight - rcGrid.top;
	//CRect& rcTick = layout_manager_.GetRect(IDC_STATIC_QUOTE);
	// 주문 그리드 위치 및 크기 설정
	//rcGrid.left = 0;
	rcGrid.right = rcGrid.left + symbol_order_view_.get_entire_width();
	rcGrid.bottom = rc_order_view.top + rc_order_view.Height();
	const int tick_width = rcTick.Width();
	rcTick.left = rcGrid.right;
	rcTick.right = rcTick.left + tick_width;
	symbol_order_view_.Invalidate(TRUE);
	symbol_order_view_.MoveWindow(rcGrid, TRUE);
	if (show_symbol_tick_view_) {
		symbol_tick_view_.ShowWindow(SW_SHOW);
		window_width = symbol_order_view_.get_entire_width() + rcTick.Width();
		if (symbol_tick_view_.GetSafeHwnd())
			symbol_tick_view_.MoveWindow(rcTick);
	}
	else {
		symbol_tick_view_.ShowWindow(SW_HIDE);
		window_width = symbol_order_view_.get_entire_width();
	}

	//symbol_order_view_.MoveWindow(rcGrid, TRUE);
	//symbol_order_view_.MoveWindow(rcGrid, TRUE);

	//symbol_order_view_.Invalidate();
}

void DmAccountOrderCenterWindow::on_resize_event_from_tick_view()
{

}

void DmAccountOrderCenterWindow::set_symbol(std::shared_ptr<DarkHorse::SmSymbol>symbol)
{
	symbol_ = symbol;
}

void DmAccountOrderCenterWindow::set_symbol_name(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	static_symbol_name_.SetWindowText(make_symbol_name(symbol).c_str());
}

std::string DmAccountOrderCenterWindow::make_symbol_name(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return "";
	std::string symbol_info(symbol->SymbolNameKr());
	symbol_info.append(" [");
	symbol_info.append(symbol->SymbolCode());
	symbol_info.append("]");
	return symbol_info;
}

int DmAccountOrderCenterWindow::add_to_symbol_combo(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return -1;
	if (symbol->symbol_type() != DarkHorse::SymbolType::Domestic) return -1;
	
	auto found = symbol_to_index_.find(symbol->SymbolCode());
	if (found != symbol_to_index_.end()) return -1;
	const std::string symbol_name = make_symbol_name(symbol);
	if (symbol_name.empty()) return -1;
	const int index = combo_symbol_.AddString(symbol_name.c_str());
	index_to_symbol_[index] = symbol;
	symbol_to_index_[symbol->SymbolCode()] = index;
	return index;
}

void DmAccountOrderCenterWindow::init_control()
{
	combo_symbol_.SetDroppedWidth(250);

	_CheckShowRealTick.SetCheck(BST_UNCHECKED);
	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
	if (pLayout != NULL)
	{
		pLayout->AddAnchor(IDC_STATIC_POSITION, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeNone);
		pLayout->AddAnchor(IDC_STATIC_ORDER, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeVert);
		pLayout->AddAnchor(IDC_STATIC_QUOTE, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeNone);
	}

	_StaticAccountName.m_clrText = RGB(255, 255, 255);
	static_symbol_name_.m_clrText = RGB(255, 255, 255);

	CRect rcWnd;
	symbol_order_view_.GetWindowRect(&rcWnd);
	ScreenToClient(rcWnd);

	rcWnd.bottom = rcWnd.top + rcWnd.Height() + DmAccountOrderCenterWindow::DeltaOrderArea;

	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("1");

	((CEdit*)GetDlgItem(IDC_EDIT_PROFIT))->SetWindowText("2");
	((CEdit*)GetDlgItem(IDC_EDIT_LOSS))->SetWindowText("2");
	((CEdit*)GetDlgItem(IDC_EDIT_SLIP))->SetWindowText("2");

	_OrderAmountSpin.SetRange32(0, 100);
	_SpinLossCut.SetRange32(0, 100);
	_SpinProfitCut.SetRange32(0, 100);
	_SpinSlip.SetRange32(0, 100);

	((CButton*)GetDlgItem(IDC_RADIO_PRICE))->SetCheck(BST_CHECKED);

	btn_grid_config_.SetImage(IDB_GRID_CONFIG, IDB_GRID_CONFIG, 0, IDB_GRID_CONFIG);
	btn_grid_config_.m_bRighImage = FALSE;
	btn_grid_config_.m_bTopImage = FALSE;
	//btn_grid_config_.SizeToContent();
	btn_grid_config_.RedrawWindow();

	//_ConfigGrid.AttachGrid(this, IDC_STATIC_CONFIG);

	filled_remain_button_.SetUp();
	filled_remain_button_.Text("체결된 잔고");
	filled_remain_button_.ShowWindow(SW_HIDE);

	remain_button_.SetUp();
	remain_button_.Text("잔고");
	//_RemainButton.OrderCenterWnd(this);
}

void DmAccountOrderCenterWindow::init_views()
{
	symbol_order_view_.SetUp();
	symbol_position_view_.SetUp();
	symbol_tick_view_.SetUp();
}

void DmAccountOrderCenterWindow::init_dm_symbol()
{
	symbol_to_index_.clear();
	index_to_symbol_.clear();
	const std::vector<DarkHorse::DmFuture>& future_vec = mainApp.SymMgr()->get_dm_future_vec();
	for (size_t i = 0; i < future_vec.size(); i++) {
		const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map = future_vec[i].product->get_yearmonth_map();
		if (year_month_map.size() == 0) continue;
		std::shared_ptr<DarkHorse::SmSymbol> symbol = year_month_map.begin()->second->get_first_symbol();
		if (!symbol) continue;
		add_to_symbol_combo(symbol);
	}

	auto restored_symbol = mainApp.SymMgr()->FindSymbol(symbol_code_);
	if (restored_symbol) {
		add_to_symbol_combo(restored_symbol);
	}
	set_default_symbol();
}

void DmAccountOrderCenterWindow::set_default_symbol()
{
	if (index_to_symbol_.empty()) return;
	if (symbol_code_.empty())
		current_combo_index_ = 0;
	else {
		auto it = symbol_to_index_.find(symbol_code_);
		if (it != symbol_to_index_.end())
			current_combo_index_ = it->second;
		else
			current_combo_index_ = 0;	
	}
	const auto symbol = index_to_symbol_[current_combo_index_];
	set_symbol_name(symbol);
	combo_symbol_.SetCurSel(current_combo_index_);
	set_symbol_info(symbol);
	set_symbol(symbol);
}

BOOL DmAccountOrderCenterWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	save_control_size();

	init_views();
	init_control();
	init_dm_symbol();
	recal_window_size();

	init_dialog_ = true;
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void DmAccountOrderCenterWindow::SetQuote(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;



	const int& close = symbol->Qoute.close;
	const int int_tick_size = static_cast<int>(symbol->TickSize() * std::pow(10, symbol->decimal()));
	const int start_value = close + (close_row_ - value_start_row_) * int_tick_size;
	try {
		std::shared_ptr<SmGrid> grid = symbol_order_view_.Grid();
		int value = start_value;
		for (int i = 1; i < grid->RowCount(); i++) {
			grid->SetCellText(i, DarkHorse::OrderHeader::QUOTE, std::to_string(value));
			value += int_tick_size;
		}
	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void DmAccountOrderCenterWindow::SetHoga(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{

}

void DmAccountOrderCenterWindow::OnClickSymbol(const std::string& symbol_info)
{

}

void DmAccountOrderCenterWindow::OnTimer(UINT_PTR nIDEvent)
{
	//if (!_Resizing) _OrderArea.Refresh();

	//_PositionArea.UpdatePositionInfo();
	//_PositionArea.UpdateSymbolInfo();
	//_QuoteArea.UpdateSymbolInfo();

	CTime t1;
	t1 = CTime::GetCurrentTime();
	//m_bar.SetPaneText(1, t1.Format("%H:%M:%S"));
	if (account_ && symbol_) {
		auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account_->No(), symbol_->SymbolCode());
		int filled_count = symbol_order_mgr->GetUnsettledCount();
		CString strCount;
		strCount.Format("%d", filled_count);
		_StaticFilledCount.SetWindowText(strCount);
		//_StaticFilledCount.Invalidate();
	}
}


void DmAccountOrderCenterWindow::set_symbol_from_out(const int order_window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (order_window_id != order_window_id_) return;
	if (!selected_  || symbol->symbol_type() != DarkHorse::SymbolType::Domestic) return;

	add_to_symbol_combo(symbol);
	set_symbol_info(symbol);
	set_symbol_name(symbol);
	set_symbol(symbol);
}

void DmAccountOrderCenterWindow::UpdateOrderSettings()
{
	if (!_EditLoss.GetSafeHwnd()) return;
	if (!_EditProfit.GetSafeHwnd()) return;
	if (!_EditSlip.GetSafeHwnd()) return;

	DarkHorse::SmOrderSettings settings;
	if (_CheckProfit.GetCheck() == BST_CHECKED)
		settings.ProfitCut = true;
	else
		settings.ProfitCut = false;

	if (_CheckLoss.GetCheck() == BST_CHECKED)
		settings.LossCut = true;
	else
		settings.LossCut = false;

	if (_RadioMarket.GetCheck() == BST_CHECKED)
		settings.PriceType = SmPriceType::Market;
	else
		settings.PriceType = SmPriceType::Price;

	CString strValue;
	_EditProfit.GetWindowText(strValue);
	settings.ProfitCutTick = _ttoi(strValue);
	_EditLoss.GetWindowText(strValue);
	settings.LossCutTick = _ttoi(strValue);
	_EditSlip.GetWindowText(strValue);
	settings.SlipTick = _ttoi(strValue);

	symbol_order_view_.UpdateOrderSettings(settings);
}

void DmAccountOrderCenterWindow::SetCutMode()
{
	profit_loss_cut_mode_ = 0;
	if (_CheckLoss.GetCheck() == BST_CHECKED) {
		profit_loss_cut_mode_ = 2;
		if (_CheckProfit.GetCheck() == BST_CHECKED) {
			profit_loss_cut_mode_ = 3;
		}
	}
	else if (_CheckProfit.GetCheck() == BST_CHECKED) {
		profit_loss_cut_mode_ = 1;
		if (_CheckLoss.GetCheck() == BST_CHECKED) {
			profit_loss_cut_mode_ = 3;
		}
	}
	symbol_order_view_.CutMode(profit_loss_cut_mode_);
}

void DmAccountOrderCenterWindow::set_symbol_info(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;
	if (symbol->symbol_type() != DarkHorse::SymbolType::Domestic) return;
	symbol_position_view_.Clear();
	symbol_tick_view_.Clear();
	symbol_order_view_.Clear();
	symbol_position_view_.Symbol(symbol);
	symbol_order_view_.Symbol(symbol);
	symbol_tick_view_.Symbol(symbol);
	//if (!symbol->Master_requested())
	//	request_dm_symbol_master(symbol->SymbolCode());
	mainApp.SymMgr()->RegisterSymbolToServer(symbol->SymbolCode(), true);
}

void DmAccountOrderCenterWindow::request_dm_symbol_master(const std::string symbol_code)
{
	DhTaskArg arg;
	arg.detail_task_description = symbol_code;
	arg.task_type = DhTaskType::DmSymbolMaster;
	arg.parameter_map["symbol_code"] = symbol_code;
	mainApp.TaskReqMgr()->AddTask(std::move(arg));
}

void DmAccountOrderCenterWindow::reset_order_set()
{
	const std::vector<DarkHorse::OrderGridHeaderInfo>& grid_header_vector = symbol_order_view_.grid_header_vector();
	order_set_.stop_width = grid_header_vector[0].width;
	order_set_.order_width = grid_header_vector[1].width;
	order_set_.count_width = grid_header_vector[2].width;
	order_set_.qty_width = grid_header_vector[3].width;
	order_set_.quote_width = grid_header_vector[4].width;
	order_set_.qty_width = grid_header_vector[5].width;
	order_set_.count_width = grid_header_vector[6].width;
	order_set_.order_width = grid_header_vector[7].width;
	order_set_.stop_width = grid_header_vector[8].width;
}

void DmAccountOrderCenterWindow::refresh_tick_view()
{
	recal_window_size();
	trigger_resize_event();
}

void DmAccountOrderCenterWindow::set_order_view(const DarkHorse::OrderSetEvent& event)
{
	order_set_ = event;
	symbol_order_view_.set_stop_as_real_order(event.stop_as_real_order);
	symbol_order_view_.SetAllRowHeight(event.grid_height);
	symbol_order_view_.reset_col_widths(event);
}

void DmAccountOrderCenterWindow::set_order_set(const DarkHorse::OrderSetEvent& event)
{
	set_order_view(event);
	recal_window_size();
	trigger_resize_event();
}

void DmAccountOrderCenterWindow::on_paramter_event(const DarkHorse::OrderSetEvent& event, const std::string& event_message, const bool enable)
{
	order_set_ = event;
	symbol_order_view_.set_stop_as_real_order(event.stop_as_real_order);
	symbol_order_view_.SetAllRowHeight(event.grid_height);
	symbol_order_view_.reset_col_widths(event);
	recal_window_size();
	trigger_resize_event();
}

void DmAccountOrderCenterWindow::on_order_set_event(const DarkHorse::OrderSetEvent& event, const bool flag)
{
	int i = 0;
	i = i + 0;
}

void DmAccountOrderCenterWindow::set_symbol_order_view_height_and_width(std::vector<int> value_vector)
{

}

void DmAccountOrderCenterWindow::SetRowWide()
{
	symbol_order_view_.SetAllRowHeight(WideRowHeight);
	RecalcOrderAreaHeight(this);
	symbol_order_view_.Invalidate();
}

void DmAccountOrderCenterWindow::SetRowNarrow()
{
	symbol_order_view_.SetAllRowHeight(DefaultRowHeight);
	RecalcOrderAreaHeight(this);
	symbol_order_view_.Invalidate();
}

void DmAccountOrderCenterWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	//symbol_order_view_.OnOrderChanged(account_id, symbol_id);
	symbol_position_view_.OnOrderChanged(account_id, symbol_id);
}

int DmAccountOrderCenterWindow::RecalcOrderAreaHeight(CWnd* wnd, bool bottom_up)
{
	CRect rcTopMost;
	CRect rcOrderArea;
	CRect rcOrderWnd;
	symbol_order_view_.GetWindowRect(rcOrderArea);

	GetWindowRect(rcOrderWnd);

	wnd->GetWindowRect(rcTopMost);

	CRect rcSymbol;
	static_symbol_name_.GetWindowRect(rcSymbol);

	int y_del = 0, extra_height = 0;
	y_del = rcTopMost.bottom - rcOrderArea.top;
	y_del -= 8;

	extra_height = symbol_order_view_.RecalRowCount(y_del);

	if (extra_height > 0) {
		CRect rcParent;
		CRect rect;
		symbol_order_view_.GetWindowRect(rect);

		rect.right -= 2;
		rect.bottom -= extra_height;
		//_OrderArea.SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);

		GetWindowRect(rect);
		//ScreenToClient(rect);
		rect.bottom -= extra_height;
		//MoveWindow(rect);
		//SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);
	}

	extra_height_ = extra_height;
	return extra_height;
}

void DmAccountOrderCenterWindow::layout_order_window(const bool show_tick)
{
	CWnd* parent = GetParent();
	CRect rc_tick_view;
	symbol_tick_view_.GetWindowRect(rc_tick_view);
	CRect rc_window;
	GetWindowRect(rc_window);
	parent->ScreenToClient(rc_window);
	if (show_tick) {
		rc_window.right = rc_window.left + rc_window.Width() + rc_tick_view.Width();
		symbol_tick_view_.ShowWindow(SW_SHOW);
	}
	else {
		rc_window.right = rc_window.left + rc_window.Width() - rc_tick_view.Width();
		symbol_tick_view_.ShowWindow(SW_HIDE);
	}

	MoveWindow(rc_window);

	((DmAccountOrderWindow*)parent)->RecalcChildren(show_tick ? CM_SHOW_TICK : CM_HIDE_TICK);
}

void DmAccountOrderCenterWindow::OnCbnSelchangeComboSymbol()
{
	const int cur_sel = combo_symbol_.GetCurSel();
	if (cur_sel < 0) return;

	auto found = index_to_symbol_.find(cur_sel);

	if (found == index_to_symbol_.end()) return;

	current_combo_index_ = cur_sel;

	symbol_ = index_to_symbol_[current_combo_index_];

	set_symbol_info(symbol_);
	set_symbol_name(symbol_);
}


void DmAccountOrderCenterWindow::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if (init_dialog_ && symbol_order_view_.GetSafeHwnd()) {
		CRect rcWnd;
		GetWindowRect(rcWnd);
		//_OrderArea.RecalRowCount(rcWnd.Height());
		symbol_order_view_.Invalidate();
	}

}


void DmAccountOrderCenterWindow::OnBnClickedCheckShowRealQuote()
{
	show_symbol_tick_view_ ? show_symbol_tick_view_ = false : show_symbol_tick_view_ = true;
	order_set_.show_symbol_tick = show_symbol_tick_view_;
	recal_window_size();
	trigger_resize_event();
}


void DmAccountOrderCenterWindow::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
}


void DmAccountOrderCenterWindow::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
}


void DmAccountOrderCenterWindow::OnBnClickedBtnRefreshOrder()
{
	//_OrderArea.UpdateOrder(_Symbol->SymbolCode());
	//_OrderArea.Invalidate(FALSE);
	mainApp.Client()->GetFilledOrderList(account_->No(), account_->Pwd());
}


BOOL DmAccountOrderCenterWindow::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return FALSE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void DmAccountOrderCenterWindow::OnBnClickedBtnSymbol()
{
// 	symbol_table_dialog_ = std::make_shared<SmSymbolTableDialog>(this);
// 	symbol_table_dialog_->Create(IDD_SYMBOL_TABLE, this);
// 	symbol_table_dialog_->order_window_id(order_window_id_);
// 	symbol_table_dialog_->ShowWindow(SW_SHOW);
	//HdSymbolSelecter symbol_selecter;
	//symbol_selecter.DoModal();

	_SymbolSelecter = std::make_shared<HdSymbolSelecter>();
	_SymbolSelecter->set_source_window_id(id_);
	_SymbolSelecter->Create(IDD_SYMBOL_SELECTER_HD, this);
	_SymbolSelecter->ShowWindow(SW_SHOW);
}

LRESULT DmAccountOrderCenterWindow::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = (int)wParam;
	std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbolById(symbol_id);
	if (symbol) set_symbol_from_out(order_window_id_, symbol);
	return 1;
}


BOOL DmAccountOrderCenterWindow::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
			//OnEntered();
			return TRUE;
		}
		else if (pMsg->wParam == VK_SPACE) {
			symbol_order_view_.PutOrderBySpaceBar();
			return TRUE;
		}
		else if (pMsg->wParam == VK_DOWN) {
			symbol_order_view_.ChangeOrderByKey(-1);
		}
		else if (pMsg->wParam == VK_UP) {
			symbol_order_view_.ChangeOrderByKey(1);
		}
	}

	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void DmAccountOrderCenterWindow::OnBnClickedBtnAmount1()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("1");
}


void DmAccountOrderCenterWindow::OnBnClickedBtnAmount2()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("2");
}


void DmAccountOrderCenterWindow::OnBnClickedBtnAmount5()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("5");
}


void DmAccountOrderCenterWindow::OnBnClickedBtnAmount3()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("3");
}


void DmAccountOrderCenterWindow::OnBnClickedBtnAmount4()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("4");
}


void DmAccountOrderCenterWindow::OnEnChangeEditAmount()
{
	CEdit* amount = (CEdit*)GetDlgItem(IDC_EDIT_AMOUNT);
	if (!amount) return;

	CString strValue;
	amount->GetWindowText(strValue);
	symbol_order_view_.OrderAmount(_ttoi(strValue));
}





void DmAccountOrderCenterWindow::OnBnClickedBtnLiqSymbolPosition()
{
	if (!symbol_) return;
	std::map<std::string, std::shared_ptr<Position>> active_position_vector_;
	if (position_type_ == OrderType::SubAccount) {
		if (!account_) return;
		auto position_manager = mainApp.total_position_manager()->find_position_manager(account_->No());
		if (!position_manager) return;
		active_position_vector_.clear();
		position_manager->get_active_positions(active_position_vector_);
	}
	else if (position_type_ == OrderType::Fund) {
		if (!fund_) return;
		auto position_manager = mainApp.total_position_manager()->find_fund_group_position_manager(fund_->Name());
		if (!position_manager) return;
		active_position_vector_.clear();
		position_manager->get_active_positions(active_position_vector_);
	}
	else {
		if (!account_) return;
		auto position_manager = mainApp.total_position_manager()->find_account_group_position_manager(account_->No());
		if (!position_manager) return;
		active_position_vector_.clear();
		position_manager->get_active_positions(active_position_vector_);
	}

	for (auto it = active_position_vector_.begin(); it != active_position_vector_.end(); it++) {
		auto position = it->second;
		if (position->open_quantity > 0) {
			symbol_order_view_.put_order(account_, position->symbol_code, DarkHorse::SmPositionType::Sell, 0, abs(position->open_quantity), SmPriceType::Market);
		}
		else if (position->open_quantity < 0) {
			symbol_order_view_.put_order(account_, position->symbol_code, DarkHorse::SmPositionType::Buy, 0, abs(position->open_quantity), SmPriceType::Market);
		}
	}
}

void DmAccountOrderCenterWindow::OnBnClickedCheckProfit()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnBnClickedCheckLoss()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnBnClickedRadioMarket()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnBnClickedRadioPrice()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnEnChangeEditProfit()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnEnChangeEditLoss()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnEnChangeEditSlip()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnBnClickedBtnSearch()
{
	//symbol_table_dialog_ = std::make_shared<SmSymbolTableDialog>(this);
	//symbol_table_dialog_->Create(IDD_SYMBOL_TABLE, this);
	//_SymbolTableDlg->OrderWnd = this;
	//symbol_table_dialog_->ShowWindow(SW_SHOW);
	//reset_order_set();
	order_set_dialog_ = std::make_shared<SmOrderSetDialog>(this, symbol_order_view_.get_id(), order_set_);
	order_set_dialog_->Create(IDD_ORDER_SET, this);
	order_set_dialog_->SetDmAccountWnd(this);
	order_set_dialog_->ShowWindow(SW_SHOW);
}


void DmAccountOrderCenterWindow::OnBnClickedBtnSet()
{
	//reset_order_set();
	order_set_dialog_ = std::make_shared<SmOrderSetDialog>(this, symbol_order_view_.get_id(), order_set_);
	order_set_dialog_->SetDmAccountWnd(this);
	order_set_dialog_->Create(IDD_ORDER_SET, this);
	order_set_dialog_->ShowWindow(SW_SHOW);
}


void DmAccountOrderCenterWindow::OnStnClickedStaticFilledRemain()
{
	// TODO: Add your control notification handler code here
}


void DmAccountOrderCenterWindow::OnBnClickedCheckFixHoga()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_FIX_HOGA))->GetCheck() == BST_CHECKED)
		symbol_order_view_.FixedMode(true);
	else
		symbol_order_view_.FixedMode(false);
}

void DmAccountOrderCenterWindow::saveToJson(json& j) const {
	if (!symbol_) return;
	j = {
		{"symbol_code", symbol_->SymbolCode()},
		{"message", order_set_.message},
		{"grid_height", order_set_.grid_height},
		{"stop_width", order_set_.stop_width},
		{"order_width", order_set_.order_width},
		{"count_width", order_set_.count_width},
		{"qty_width", order_set_.qty_width},
		{"quote_width", order_set_.quote_width},
		{"stop_as_real_order", order_set_.stop_as_real_order}
	};
}

void DmAccountOrderCenterWindow::loadFromJson(const json& j) {
	symbol_code_ = j["symbol_code"].get<std::string>();
	order_set_.message = j["message"].get<std::string>();
	order_set_.grid_height = j["grid_height"].get<int>();
	order_set_.stop_width = j["stop_width"].get<int>();
	order_set_.order_width = j["order_width"].get<int>();
	order_set_.count_width = j["count_width"].get<int>();
	order_set_.qty_width = j["qty_width"].get<int>();
	order_set_.quote_width = j["quote_width"].get<int>();
	order_set_.stop_as_real_order = j["stop_as_real_order"].get<bool>();
}