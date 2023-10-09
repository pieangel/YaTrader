// SmOrderWnd.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "AbAccountOrderCenterWindow.h"
#include "afxdialogex.h"
#include "../Grid/customcells.h"
#include "SmOrderGridConst.h"
#include "../Util/VtStringUtil.h"
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "BCGPGridCtrl.h"
#include <format>
#include "../Log/MyLogger.h"
#include "../SmGrid/SmGrid.h"
#include "../SmGrid/SmCell.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "AbAccountOrderWindow.h"
#include "../Symbol/SmSymbolTableDialog.h"
#include "../Task/SmTaskRequestMaker.h"
#include "../Client/ViStockClient.h"
#include "OrderWndConst.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmAccountPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Order/SmOrderRequest.h"
#include "../Client/ViStockClient.h"
#include "../Order/SmOrderRequestManager.h"
#include "SmOrderSetDialog.h"
#include "../Event/EventHub.h"
#include "../Event/EventHubArg.h"
#include "../Position/TotalPositionManager.h"
#include "../Position/AccountPositionManager.h"
#include "../Position/Position.h"
#include "../Order/OrderRequest/OrderRequestManager.h"
#include "../Order/OrderRequest/OrderRequest.h"
#include "../Task/SmTaskRequestManager.h"
#include "../Util/IdGenerator.h"
#include "../Position/GroupPositionManager.h"
#include "AbFundOrderWindow.h"
#include "../Fund/SmFund.h"

// SmOrderWnd dialog
#define BTN_ORDER_AMOUNT 0x00000001
#define BTN_ORDER_SELL 0x00000002
#define BTN_ORDER_BUY 0x00000003
#define BTN_ORDER_CENTER_QUOTE 0x00000004

using namespace DarkHorse;

int AbAccountOrderCenterWindow::DeltaOrderArea = 0;

using account_position_manager_p = std::shared_ptr<DarkHorse::AccountPositionManager>;

IMPLEMENT_DYNAMIC(AbAccountOrderCenterWindow, CBCGPDialog)

AbAccountOrderCenterWindow::AbAccountOrderCenterWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_CENTER, pParent)
{
	id_ = IdGenerator::get_id();
	symbol_order_view_.symbol_type(SymbolType::Abroad);
	symbol_position_view_.symbol_type(SymbolType::Abroad);
	symbol_order_view_.set_order_request_type(OrderRequestType::Abroad);
	symbol_order_view_.set_fill_condition(SmFilledCondition::Day);
	symbol_order_view_.set_center_window_id(id_);
	symbol_order_view_.set_order_window_id(order_window_id_);
	symbol_tick_view_.set_center_window_id(id_);
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();

	mainApp.event_hub()->add_parameter_event(symbol_order_view_.get_id(), std::bind(&AbAccountOrderCenterWindow::on_paramter_event, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	mainApp.event_hub()->add_ab_symbol_event(1, std::bind(&AbAccountOrderCenterWindow::on_symbol_event, this, std::placeholders::_1));
}

AbAccountOrderCenterWindow::~AbAccountOrderCenterWindow()
{
	//KillTimer(1);
}

void AbAccountOrderCenterWindow::Account(std::shared_ptr<DarkHorse::SmAccount> val)
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

void AbAccountOrderCenterWindow::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	fund_ = val;
	position_type_ = OrderType::Fund;
	symbol_order_view_.fund(val);
	symbol_order_view_.Refresh();
	symbol_position_view_.fund(val);
	symbol_position_view_.Refresh();
}

void AbAccountOrderCenterWindow::Selected(bool val)
{
	_Selected = val;
	symbol_order_view_.Selected(val);
}

void AbAccountOrderCenterWindow::DoDataExchange(CDataExchange* pDX)
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

	DDX_Control(pDX, IDC_BTN_SEARCH, _BtnSearch);
	DDX_Control(pDX, IDC_STATIC_GROUP2, _Group2);
	DDX_Control(pDX, IDC_STATIC_GROUP3, _Group3);
	DDX_Control(pDX, IDC_STATIC_GROUP4, _Group4);
	DDX_Control(pDX, IDC_STATIC_FILLED_REMAIN, _FilledRemainButton);
	DDX_Control(pDX, IDC_STATIC_REMAIN, _RemainButton);
}


void AbAccountOrderCenterWindow::on_paramter_event(const DarkHorse::OrderSetEvent& event, const std::string& event_message, const bool enable)
{
	//symbol_order_view_.on_paramter_event(event, event_message, enable);
// 	order_set_ = event;
// 	symbol_order_view_.set_stop_as_real_order(event.stop_as_real_order);
// 	symbol_order_view_.SetAllRowHeight(event.grid_height);
// 	RecalcOrderAreaHeight(this);
// 	symbol_order_view_.Invalidate();


	order_set_ = event;
	symbol_order_view_.set_stop_as_real_order(event.stop_as_real_order);
	symbol_order_view_.SetAllRowHeight(event.grid_height);
	symbol_order_view_.reset_col_widths(event);
	recal_window_size();
	trigger_resize_event();
}

void AbAccountOrderCenterWindow::on_symbol_event(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;
	const int index = add_to_symbol_combo(symbol);
	if (index >= 0) {
		current_combo_index_ = index;
		combo_symbol_.SetCurSel(current_combo_index_);
	}
	set_symbol_name(symbol);
	set_symbol_info(symbol);
	set_symbol(symbol);
}

BEGIN_MESSAGE_MAP(AbAccountOrderCenterWindow, CBCGPDialog)
	ON_WM_TIMER()
	ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &AbAccountOrderCenterWindow::OnCbnSelchangeComboSymbol)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_SHOW_REAL_QUOTE, &AbAccountOrderCenterWindow::OnBnClickedCheckShowRealQuote)
	ON_BN_CLICKED(IDC_BTN_REFRESH_ORDER, &AbAccountOrderCenterWindow::OnBnClickedBtnRefreshOrder)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_SYMBOL, &AbAccountOrderCenterWindow::OnBnClickedBtnSymbol)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &AbAccountOrderCenterWindow::OnUmSymbolSelected)
	ON_BN_CLICKED(IDC_BTN_AMOUNT1, &AbAccountOrderCenterWindow::OnBnClickedBtnAmount1)
	ON_BN_CLICKED(IDC_BTN_AMOUNT2, &AbAccountOrderCenterWindow::OnBnClickedBtnAmount2)

	ON_BN_CLICKED(IDC_BTN_AMOUNT5, &AbAccountOrderCenterWindow::OnBnClickedBtnAmount5)
	ON_BN_CLICKED(IDC_BTN_AMOUNT3, &AbAccountOrderCenterWindow::OnBnClickedBtnAmount3)
	ON_BN_CLICKED(IDC_BTN_AMOUNT4, &AbAccountOrderCenterWindow::OnBnClickedBtnAmount4)
	ON_EN_CHANGE(IDC_EDIT_AMOUNT, &AbAccountOrderCenterWindow::OnEnChangeEditAmount)
	ON_BN_CLICKED(IDC_BTN_LIQ_SYMBOL_POSITION, &AbAccountOrderCenterWindow::OnBnClickedBtnLiqSymbolPosition)
	ON_BN_CLICKED(IDC_CHECK_PROFIT, &AbAccountOrderCenterWindow::OnBnClickedCheckProfit)
	ON_BN_CLICKED(IDC_CHECK_LOSS, &AbAccountOrderCenterWindow::OnBnClickedCheckLoss)
	ON_BN_CLICKED(IDC_RADIO_MARKET, &AbAccountOrderCenterWindow::OnBnClickedRadioMarket)
	ON_BN_CLICKED(IDC_RADIO_PRICE, &AbAccountOrderCenterWindow::OnBnClickedRadioPrice)
	ON_EN_CHANGE(IDC_EDIT_PROFIT, &AbAccountOrderCenterWindow::OnEnChangeEditProfit)
	ON_EN_CHANGE(IDC_EDIT_LOSS, &AbAccountOrderCenterWindow::OnEnChangeEditLoss)
	ON_EN_CHANGE(IDC_EDIT_SLIP, &AbAccountOrderCenterWindow::OnEnChangeEditSlip)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &AbAccountOrderCenterWindow::OnBnClickedBtnSearch)
	ON_BN_CLICKED(IDC_BTN_SET, &AbAccountOrderCenterWindow::OnBnClickedBtnSet)
	ON_STN_CLICKED(IDC_STATIC_FILLED_REMAIN, &AbAccountOrderCenterWindow::OnStnClickedStaticFilledRemain)
	ON_BN_CLICKED(IDC_CHECK_FIX_HOGA, &AbAccountOrderCenterWindow::OnBnClickedCheckFixHoga)
END_MESSAGE_MAP()


// SmOrderWnd message handlers

#define WS_CAPTION          0x00C00000L

LRESULT AbAccountOrderCenterWindow::OnEnterSizeMove(WPARAM wparam, LPARAM lparam)
{
	_Resizing = true;
	// do stuff      
	return (LRESULT)0;
}
LRESULT AbAccountOrderCenterWindow::OnExitSizeMove(WPARAM wparam, LPARAM lparam)
{
	
	_Resizing = false;
	// do stuff      
	return (LRESULT)0;
}




void AbAccountOrderCenterWindow::SetMainDialog(AbAccountOrderWindow* main_dialog)
{
	if (!main_dialog) return;
}

void AbAccountOrderCenterWindow::SetFundDialog(AbFundOrderWindow* fund_dialog)
{
	if (!fund_dialog) return;
}

void AbAccountOrderCenterWindow::SetSelected(const bool& selected)
{
	_Selected = selected;
	symbol_order_view_.Selected(selected);
	symbol_order_view_.Invalidate();
}

void AbAccountOrderCenterWindow::OnSymbolClicked(const std::string& symbol_code)
{
	auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (symbol) set_symbol_info(symbol);
}

void AbAccountOrderCenterWindow::SetOrderAmount(const int& count)
{
	CString strValue;
	strValue.Format("%d", count);
	_EditAmount.SetWindowText(strValue);
}

int AbAccountOrderCenterWindow::GetPositionCount()
{
	if (!account_ || !symbol_) return 0;

	std::shared_ptr<SmPosition> position = mainApp.TotalPosiMgr()->FindAddPosition(account_->No(), symbol_->SymbolCode());
	
	return position->OpenQty;
}

void AbAccountOrderCenterWindow::ArrangeCenterValue()
{
	symbol_order_view_.ArrangeCenterValue();
}

void AbAccountOrderCenterWindow::CreateResource()
{
	
}

BOOL AbAccountOrderCenterWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	init_views();
	init_control();
	init_ab_symbol();
	_Init = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void AbAccountOrderCenterWindow::SetQuote(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	

	const int& close = symbol->Qoute.close;
	const int int_tick_size = static_cast<int>(symbol->TickSize() * std::pow(10, symbol->decimal()));
	const int start_value = close + (_CloseRow - _ValueStartRow) * int_tick_size;
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

void AbAccountOrderCenterWindow::SetHoga(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{

}

void AbAccountOrderCenterWindow::OnClickSymbol(const std::string& symbol_info)
{
	
}

void AbAccountOrderCenterWindow::OnTimer(UINT_PTR nIDEvent)
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


void AbAccountOrderCenterWindow::trigger_resize_event()
{
	CWnd* parent = GetParent();

	if (account_)
		((AbAccountOrderWindow*)parent)->RecalcChildren(show_symbol_tick_view_ ? CM_SHOW_TICK : CM_HIDE_TICK);
	if (fund_)
		((AbFundOrderWindow*)parent)->RecalcChildren(show_symbol_tick_view_ ? CM_SHOW_TICK : CM_HIDE_TICK);
}

void AbAccountOrderCenterWindow::recal_window_size()
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

void AbAccountOrderCenterWindow::request_symbol_quote(const std::string& symbol_code)
{
	DhTaskArg arg;
	arg.detail_task_description = symbol_code;
	arg.task_type = DhTaskType::AbSymbolQuote;
	arg.parameter_map["symbol_code"] = symbol_code;
	mainApp.TaskReqMgr()->AddTask(std::move(arg));
}

void AbAccountOrderCenterWindow::request_symbol_hoga(const std::string& symbol_code)
{
	DhTaskArg arg;
	arg.detail_task_description = symbol_code;
	arg.task_type = DhTaskType::AbSymbolHoga;
	arg.parameter_map["symbol_code"] = symbol_code;
	mainApp.TaskReqMgr()->AddTask(std::move(arg));
}

void AbAccountOrderCenterWindow::init_control()
{
	//SetTimer(1, 40, NULL);
	combo_symbol_.SetDroppedWidth(250);
	//_ComboAccount.SetDroppedWidth(200);


	CRect rect;
	//CWnd* pWnd = GetDlgItem(IDC_STATIC_SET);
	//pWnd->GetWindowRect(&rect);
	//ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	//_OrderSetGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, IDC_STATIC_SET);





	_CheckShowRealTick.SetCheck(BST_CHECKED);
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

	rcWnd.bottom = rcWnd.top + rcWnd.Height() + AbAccountOrderCenterWindow::DeltaOrderArea;

	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("1");

	((CEdit*)GetDlgItem(IDC_EDIT_PROFIT))->SetWindowText("2");
	((CEdit*)GetDlgItem(IDC_EDIT_LOSS))->SetWindowText("2");
	((CEdit*)GetDlgItem(IDC_EDIT_SLIP))->SetWindowText("2");


	_OrderAmountSpin.SetRange32(0, 100);
	_SpinLossCut.SetRange32(0, 100);
	_SpinProfitCut.SetRange32(0, 100);
	_SpinSlip.SetRange32(0, 100);

	((CButton*)GetDlgItem(IDC_RADIO_PRICE))->SetCheck(BST_CHECKED);

	_BtnSearch.SetImage(IDB_BITMAP_SEARCH, IDB_BITMAP_SEARCH, 0, IDB_BITMAP_SEARCH);
	_BtnSearch.m_bRighImage = FALSE;
	_BtnSearch.m_bTopImage = FALSE;
	//_BtnSearch.SizeToContent();
	_BtnSearch.RedrawWindow();

	//_ConfigGrid.AttachGrid(this, IDC_STATIC_CONFIG);

	_FilledRemainButton.SetUp();
	_FilledRemainButton.Text("체결된 잔고");
	_FilledRemainButton.ShowWindow(SW_HIDE);

	_RemainButton.SetUp();
	_RemainButton.Text("잔고");
	_RemainButton.OrderCenterWnd(this);
}

void AbAccountOrderCenterWindow::init_views()
{
	symbol_order_view_.SetUp();
	symbol_tick_view_.SetUp();
	symbol_position_view_.SetUp();
}

void AbAccountOrderCenterWindow::init_ab_symbol()
{
	const std::map<int, std::shared_ptr<SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	for (const auto& it : favorite_map) {
		add_to_symbol_combo(it.second);
	}
	set_current_symbol(2);
}

void AbAccountOrderCenterWindow::set_current_symbol(const int index)
{
	if (index_to_symbol_.empty()) return;
	current_combo_index_ = index;
	combo_symbol_.SetCurSel(current_combo_index_);

	const auto symbol = index_to_symbol_[current_combo_index_];
	set_symbol_name(symbol);
	set_symbol_info(symbol);
	set_symbol(symbol);
}

void AbAccountOrderCenterWindow::set_symbol(std::shared_ptr<DarkHorse::SmSymbol>symbol)
{
	symbol_ = symbol;
}

void AbAccountOrderCenterWindow::set_symbol_name(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	static_symbol_name_.SetWindowText(make_symbol_name(symbol).c_str());
}

std::string AbAccountOrderCenterWindow::make_symbol_name(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return "";
	std::string symbol_info(symbol->SymbolNameKr());
	symbol_info.append(" [");
	symbol_info.append(symbol->SymbolCode());
	symbol_info.append("]");
	return symbol_info;
}

int AbAccountOrderCenterWindow::add_to_symbol_combo(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return -1;

	auto found = symbol_to_index_.find(symbol->SymbolCode());
	if (found != symbol_to_index_.end()) return -1;
	const std::string symbol_name = make_symbol_name(symbol);
	if (symbol_name.empty()) return -1;
	const int index = combo_symbol_.AddString(symbol_name.c_str());
	index_to_symbol_[index] = symbol;
	symbol_to_index_[symbol->SymbolCode()] = index;
	return index;
}

void AbAccountOrderCenterWindow::set_symbol_info(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;
	symbol_position_view_.Clear();
	symbol_tick_view_.Clear();
	symbol_order_view_.Clear();
	symbol_position_view_.Symbol(symbol);
	symbol_order_view_.Symbol(symbol);
	symbol_tick_view_.Symbol(symbol);
	//if (!symbol->quote_requested()) {
	//	request_symbol_quote(symbol->SymbolCode());
	//	request_symbol_hoga(symbol->SymbolCode());
	//}
	mainApp.SymMgr()->RegisterSymbolToServer(symbol->SymbolCode(), true);
}

void AbAccountOrderCenterWindow::UpdateOrderSettings()
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

void AbAccountOrderCenterWindow::SetCutMode()
{
	_CutMode = 0;
	if (_CheckLoss.GetCheck() == BST_CHECKED) {
		_CutMode = 2;
		if (_CheckProfit.GetCheck() == BST_CHECKED) {
			_CutMode = 3;
		}
	}
	else if (_CheckProfit.GetCheck() == BST_CHECKED) {
		_CutMode = 1;
		if (_CheckLoss.GetCheck() == BST_CHECKED) {
			_CutMode = 3;
		}
	}
	symbol_order_view_.CutMode(_CutMode);
}

void AbAccountOrderCenterWindow::SetInfo(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	symbol_tick_view_.Clear();
	symbol_position_view_.Clear();
	symbol_order_view_.Clear();
	symbol_tick_view_.Symbol(symbol);
	//_OrderArea.SetCenterValues(symbol);
	symbol_order_view_.Symbol(symbol);
	//_OrderArea.SetQuote(symbol);
	//_OrderArea.SetHoga(symbol);
	symbol_position_view_.Symbol(symbol);
	//_OrderArea.UpdateOrder(_Symbol->SymbolCode());
	//_OrderArea.Refresh();
}

void AbAccountOrderCenterWindow::SetRowWide()
{
	symbol_order_view_.SetAllRowHeight(WideRowHeight);
	RecalcOrderAreaHeight(this);
	symbol_order_view_.Invalidate();
}

void AbAccountOrderCenterWindow::SetRowNarrow()
{
	symbol_order_view_.SetAllRowHeight(DefaultRowHeight);
	RecalcOrderAreaHeight(this);
	symbol_order_view_.Invalidate();
}

void AbAccountOrderCenterWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	//symbol_order_view_.OnOrderChanged(account_id, symbol_id);
	symbol_position_view_.OnOrderChanged(account_id, symbol_id);
}

int AbAccountOrderCenterWindow::RecalcOrderAreaHeight(CWnd* wnd, bool bottom_up)
{
	CRect rcTopMost;
	CRect rcOrderArea;
	CRect rcOrderWnd;
	symbol_order_view_.GetWindowRect(rcOrderArea);

	GetWindowRect(rcOrderWnd);

	wnd->GetWindowRect(rcTopMost);

	CRect rcSymbol;
	static_symbol_name_.GetWindowRect(rcSymbol);

	int y_del = 0, delta_height = 0;
	y_del = rcTopMost.bottom - rcOrderArea.top;
	y_del -= 8;

	delta_height = symbol_order_view_.RecalRowCount(y_del);

	if (delta_height > 0) {
		CRect rcParent;
		CRect rect;
		symbol_order_view_.GetWindowRect(rect);

		rect.right -= 2;
		rect.bottom -= delta_height;
		//_OrderArea.SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);

		GetWindowRect(rect);
		//ScreenToClient(rect);
		rect.bottom -= delta_height;
		//MoveWindow(rect);
		//SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);
	}
	

	return delta_height;
}

void AbAccountOrderCenterWindow::OnCbnSelchangeComboSymbol()
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


void AbAccountOrderCenterWindow::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);
	
	if (_Init && symbol_order_view_.GetSafeHwnd()) {
		CRect rcWnd;
		GetWindowRect(rcWnd);
		//_OrderArea.RecalRowCount(rcWnd.Height());
		symbol_order_view_.Invalidate();
	}
	
}


void AbAccountOrderCenterWindow::OnBnClickedCheckShowRealQuote()
{
	_ShowQuoteArea ? _ShowQuoteArea = false : _ShowQuoteArea = true;
	CWnd* parent = GetParent();
	CRect rcQuote;
	symbol_tick_view_.GetWindowRect(rcQuote);
	CRect rcWnd;
	GetWindowRect(rcWnd);
	parent->ScreenToClient(rcWnd);
	if (_ShowQuoteArea) {
		rcWnd.right = rcWnd.left + rcWnd.Width() + rcQuote.Width();
		symbol_tick_view_.ShowWindow(SW_SHOW);
	}
	else {
		rcWnd.right = rcWnd.left + rcWnd.Width() - rcQuote.Width();
		symbol_tick_view_.ShowWindow(SW_HIDE);
	}

	MoveWindow(rcWnd);

	((AbAccountOrderWindow*)parent)->RecalcChildren(_ShowQuoteArea ? CM_SHOW_TICK : CM_HIDE_TICK);
}


void AbAccountOrderCenterWindow::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
}


void AbAccountOrderCenterWindow::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
}


void AbAccountOrderCenterWindow::OnBnClickedBtnRefreshOrder()
{
	//_OrderArea.UpdateOrder(_Symbol->SymbolCode());
	//_OrderArea.Invalidate(FALSE);
	mainApp.Client()->GetFilledOrderList(account_->No(), account_->Pwd());
}


BOOL AbAccountOrderCenterWindow::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return FALSE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void AbAccountOrderCenterWindow::OnBnClickedBtnSymbol()
{
	symbol_table_dialog_ = std::make_shared<SmSymbolTableDialog>(this);
	symbol_table_dialog_->Create(IDD_SYMBOL_TABLE, this);
	symbol_table_dialog_->order_window_id(order_window_id_);
	symbol_table_dialog_->ShowWindow(SW_SHOW);
}

LRESULT AbAccountOrderCenterWindow::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = (int)wParam;
	std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbolById(symbol_id);
	if (symbol) set_symbol_info(symbol);
	return 1;
}


BOOL AbAccountOrderCenterWindow::PreTranslateMessage(MSG* pMsg)
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


void AbAccountOrderCenterWindow::OnBnClickedBtnAmount1()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("1");
}


void AbAccountOrderCenterWindow::OnBnClickedBtnAmount2()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("2");
}


void AbAccountOrderCenterWindow::OnBnClickedBtnAmount5()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("5");
}


void AbAccountOrderCenterWindow::OnBnClickedBtnAmount3()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("3");
}


void AbAccountOrderCenterWindow::OnBnClickedBtnAmount4()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("4");
}


void AbAccountOrderCenterWindow::OnEnChangeEditAmount()
{
	CEdit* amount = (CEdit*)GetDlgItem(IDC_EDIT_AMOUNT);
	if (!amount) return;

	CString strValue;
	amount->GetWindowText(strValue);
	symbol_order_view_.OrderAmount(_ttoi(strValue));
}





void AbAccountOrderCenterWindow::OnBnClickedBtnLiqSymbolPosition()
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


void AbAccountOrderCenterWindow::OnBnClickedCheckProfit()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnBnClickedCheckLoss()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnBnClickedRadioMarket()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnBnClickedRadioPrice()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnEnChangeEditProfit()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnEnChangeEditLoss()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnEnChangeEditSlip()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnBnClickedBtnSearch()
{
	symbol_table_dialog_ = std::make_shared<SmSymbolTableDialog>(this);
	symbol_table_dialog_->Create(IDD_SYMBOL_TABLE, this);
	symbol_table_dialog_->order_window_id(order_window_id_);
	symbol_table_dialog_->OrderWnd = this;
	symbol_table_dialog_->ShowWindow(SW_SHOW);
}


void AbAccountOrderCenterWindow::OnBnClickedBtnSet()
{
	order_set_dialog_ = std::make_shared<SmOrderSetDialog>(this, symbol_order_view_.get_id(), order_set_);
	order_set_dialog_->Create(IDD_ORDER_SET, this);
	//_OrderSetDlg->OrderWnd(this);
	order_set_dialog_->ShowWindow(SW_SHOW);
}


void AbAccountOrderCenterWindow::OnStnClickedStaticFilledRemain()
{
	// TODO: Add your control notification handler code here
}


void AbAccountOrderCenterWindow::OnBnClickedCheckFixHoga()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_FIX_HOGA))->GetCheck() == BST_CHECKED)
		symbol_order_view_.FixedMode(true);
	else
		symbol_order_view_.FixedMode(false);
}
