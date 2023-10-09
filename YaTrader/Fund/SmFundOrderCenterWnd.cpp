// SmFundOrderCenterWnd.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmFundOrderCenterWnd.h"
#include "afxdialogex.h"
#include "../Grid/customcells.h"
#include "../Order/SmOrderGridConst.h"
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
#include "SmFundOrderDialog.h"
#include "../Symbol/SmSymbolTableDialog.h"
#include "../Task/SmTaskRequestMaker.h"
#include "../Client/ViStockClient.h"
#include "../Order/OrderWndConst.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmAccountPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Order/SmOrderRequest.h"
#include "../Client/ViStockClient.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Order/SmOrderSetDialog.h"
#include "SmFund.h"
#include "SmFundManager.h"

// SmOrderWnd dialog
#define BTN_ORDER_AMOUNT 0x00000001
#define BTN_ORDER_SELL 0x00000002
#define BTN_ORDER_BUY 0x00000003
#define BTN_ORDER_CENTER_QUOTE 0x00000004

using namespace DarkHorse;

int SmFundOrderCenterWnd::DeltaOrderArea = 0;

IMPLEMENT_DYNAMIC(SmFundOrderCenterWnd, CBCGPDialog)

SmFundOrderCenterWnd::SmFundOrderCenterWnd(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_CENTER, pParent)
{

	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmFundOrderCenterWnd::~SmFundOrderCenterWnd()
{
	//KillTimer(1);
}

void SmFundOrderCenterWnd::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	_Fund = val;
	_OrderArea.Fund(val);
	_OrderArea.Refresh();
	_PositionArea.Fund(val);
	_PositionArea.Refresh();
}

void SmFundOrderCenterWnd::Selected(bool val)
{
	_Selected = val;
	_OrderArea.Selected(val);
}

void SmFundOrderCenterWnd::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	/*
	DDX_Control(pDX, IDC_COMBO_SYMBOL, _ComboSymbol);
	DDX_Control(pDX, IDC_STATIC_ORDER, _OrderArea);
	DDX_Control(pDX, IDC_STATIC_POSITION, _PositionArea);
	DDX_Control(pDX, IDC_STATIC_QUOTE, _QuoteArea);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_NAME_KR, _StaticSymbolName);
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
	*/

	DDX_Control(pDX, IDC_COMBO_SYMBOL, _ComboSymbol);
	DDX_Control(pDX, IDC_STATIC_ORDER, _OrderArea);
	DDX_Control(pDX, IDC_STATIC_POSITION, _PositionArea);
	DDX_Control(pDX, IDC_STATIC_QUOTE, _QuoteArea);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_NAME_KR, _StaticSymbolName);
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
	DDX_Control(pDX, IDC_SPIN_BUY_AVAIL, _BuyAvail);
	DDX_Control(pDX, IDC_SPIN_SELL_AVAIL, _SellAvail);
	DDX_Control(pDX, IDC_STATIC_FILLED_REMAIN, _FilledRemainButton);
	DDX_Control(pDX, IDC_STATIC_REMAIN, _RemainButton);

}


BEGIN_MESSAGE_MAP(SmFundOrderCenterWnd, CBCGPDialog)
	ON_WM_TIMER()
	ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &SmFundOrderCenterWnd::OnCbnSelchangeComboSymbol)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_SHOW_REAL_QUOTE, &SmFundOrderCenterWnd::OnBnClickedCheckShowRealQuote)
	ON_BN_CLICKED(IDC_BTN_REFRESH_ORDER, &SmFundOrderCenterWnd::OnBnClickedBtnRefreshOrder)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_SYMBOL, &SmFundOrderCenterWnd::OnBnClickedBtnSymbol)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &SmFundOrderCenterWnd::OnUmSymbolSelected)
	ON_BN_CLICKED(IDC_BTN_AMOUNT1, &SmFundOrderCenterWnd::OnBnClickedBtnAmount1)
	ON_BN_CLICKED(IDC_BTN_AMOUNT2, &SmFundOrderCenterWnd::OnBnClickedBtnAmount2)

	ON_BN_CLICKED(IDC_BTN_AMOUNT5, &SmFundOrderCenterWnd::OnBnClickedBtnAmount5)
	ON_BN_CLICKED(IDC_BTN_AMOUNT3, &SmFundOrderCenterWnd::OnBnClickedBtnAmount3)
	ON_BN_CLICKED(IDC_BTN_AMOUNT4, &SmFundOrderCenterWnd::OnBnClickedBtnAmount4)
	ON_EN_CHANGE(IDC_EDIT_AMOUNT, &SmFundOrderCenterWnd::OnEnChangeEditAmount)
	ON_BN_CLICKED(IDC_BTN_LIQ_SYMBOL_POSITION, &SmFundOrderCenterWnd::OnBnClickedBtnLiqSymbolPosition)
	ON_BN_CLICKED(IDC_CHECK_PROFIT, &SmFundOrderCenterWnd::OnBnClickedCheckProfit)
	ON_BN_CLICKED(IDC_CHECK_LOSS, &SmFundOrderCenterWnd::OnBnClickedCheckLoss)
	ON_BN_CLICKED(IDC_RADIO_MARKET, &SmFundOrderCenterWnd::OnBnClickedRadioMarket)
	ON_BN_CLICKED(IDC_RADIO_PRICE, &SmFundOrderCenterWnd::OnBnClickedRadioPrice)
	ON_EN_CHANGE(IDC_EDIT_PROFIT, &SmFundOrderCenterWnd::OnEnChangeEditProfit)
	ON_EN_CHANGE(IDC_EDIT_LOSS, &SmFundOrderCenterWnd::OnEnChangeEditLoss)
	ON_EN_CHANGE(IDC_EDIT_SLIP, &SmFundOrderCenterWnd::OnEnChangeEditSlip)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &SmFundOrderCenterWnd::OnBnClickedBtnSearch)
	ON_BN_CLICKED(IDC_CHECK_FIX_HOGA, &SmFundOrderCenterWnd::OnBnClickedCheckFixHoga)
	ON_BN_CLICKED(IDC_BTN_SET, &SmFundOrderCenterWnd::OnBnClickedBtnSet)
END_MESSAGE_MAP()


// SmFundOrderCenterWnd message handlers

#define WS_CAPTION          0x00C00000L

LRESULT SmFundOrderCenterWnd::OnEnterSizeMove(WPARAM wparam, LPARAM lparam)
{
	_Resizing = true;
	// do stuff      
	return (LRESULT)0;
}
LRESULT SmFundOrderCenterWnd::OnExitSizeMove(WPARAM wparam, LPARAM lparam)
{

	_Resizing = false;
	// do stuff      
	return (LRESULT)0;
}




void SmFundOrderCenterWnd::SetFundDialog(SmFundOrderDialog* fund_dialog)
{
	_OrderArea.SetFundDialog(fund_dialog);
}

void SmFundOrderCenterWnd::SetSelected(const bool& selected)
{
	_Selected = selected;
	_OrderArea.Selected(selected);
	_OrderArea.Invalidate();
}

void SmFundOrderCenterWnd::OnSymbolClicked(const std::string& symbol_code)
{
	auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	
	if (symbol) SetSymbolInfo(symbol);
}

int SmFundOrderCenterWnd::GetFundPositionCount()
{
	if (!_Fund || !_Symbol) return 0;

	return _Fund->GetPositionCount(_Symbol->SymbolCode());
}

int SmFundOrderCenterWnd::GetFilledOrderCount()
{
	if (!_Fund || !_Symbol) return 0;

	return _Fund->GetFilledOrderCount(_Symbol->SymbolCode());
}

void SmFundOrderCenterWnd::SetOrderAmount(const int& count)
{
	CString strValue;
	strValue.Format("%d", count);
	_EditAmount.SetWindowText(strValue);
}

void SmFundOrderCenterWnd::SetRowWide()
{
	_OrderArea.SetAllRowHeight(WideRowHeight);
	RecalcOrderAreaHeight(this);
	_OrderArea.Invalidate();
}

void SmFundOrderCenterWnd::SetRowNarrow()
{
	_OrderArea.SetAllRowHeight(DefaultRowHeight);
	RecalcOrderAreaHeight(this);
	_OrderArea.Invalidate();
}

void SmFundOrderCenterWnd::CreateResource()
{

}

BOOL SmFundOrderCenterWnd::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();


	//SetTimer(1, 40, NULL);
	_ComboSymbol.SetDroppedWidth(250);
	//_ComboAccount.SetDroppedWidth(200);


	CRect rect;
	//CWnd* pWnd = GetDlgItem(IDC_STATIC_SET);
	//pWnd->GetWindowRect(&rect);
	//ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	//_OrderSetGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, IDC_STATIC_SET);



	_OrderArea.SetUp();
	_PositionArea.SetUp();
	_QuoteArea.SetUp();

	_CheckShowRealTick.SetCheck(BST_CHECKED);
	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
	if (pLayout != NULL)
	{
		pLayout->AddAnchor(IDC_STATIC_POSITION, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeNone);
		pLayout->AddAnchor(IDC_STATIC_ORDER, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeVert);
		pLayout->AddAnchor(IDC_STATIC_QUOTE, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeNone);
	}

	//SetAccount();

	

	_StaticAccountName.m_clrText = RGB(255, 255, 255);
	_StaticSymbolName.m_clrText = RGB(255, 255, 255);

	CRect rcWnd;
	_OrderArea.GetWindowRect(&rcWnd);
	ScreenToClient(rcWnd);

	rcWnd.bottom = rcWnd.top + rcWnd.Height() + SmFundOrderCenterWnd::DeltaOrderArea;

	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("1");

	((CEdit*)GetDlgItem(IDC_EDIT_PROFIT))->SetWindowText("2");
	((CEdit*)GetDlgItem(IDC_EDIT_LOSS))->SetWindowText("2");
	((CEdit*)GetDlgItem(IDC_EDIT_SLIP))->SetWindowText("2");

	_Init = true;

	_ID = SmFundOrderDialog::GetId();

	_OrderAmountSpin.SetRange32(0, 100);
	_SpinLossCut.SetRange32(0, 100);
	_SpinProfitCut.SetRange32(0, 100);
	_SpinSlip.SetRange32(0, 100);

	_BuyAvail.SetRange32(0, 100);
	_SellAvail.SetRange32(0, 100);

	((CButton*)GetDlgItem(IDC_RADIO_PRICE))->SetCheck(BST_CHECKED);


	_BtnSearch.SetImage(IDB_BITMAP_SEARCH, IDB_BITMAP_SEARCH, 0, IDB_BITMAP_SEARCH);
	_BtnSearch.m_bRighImage = FALSE;
	_BtnSearch.m_bTopImage = FALSE;
	//_BtnSearch.SizeToContent();
	_BtnSearch.RedrawWindow();

	_FilledRemainButton.SetUp();
	_FilledRemainButton.Text("√º∞·µ» ¿‹∞Ì");
	_FilledRemainButton.FundOrderCenterWnd(this);
	_FilledRemainButton.SetFundOrderArea(&_OrderArea);

	_RemainButton.SetUp();
	_RemainButton.Text("¿‹∞Ì");
	//_RemainButton.FundOrderCenterWnd(this);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SmFundOrderCenterWnd::SetQuote(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;



	const int& close = symbol->Qoute.close;
	const int int_tick_size = static_cast<int>(symbol->TickSize() * std::pow(10, symbol->decimal()));
	const int start_value = close + (_CloseRow - _ValueStartRow) * int_tick_size;
	try {
		std::shared_ptr<SmGrid> grid = _OrderArea.Grid();
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

void SmFundOrderCenterWnd::SetHoga(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{

}

void SmFundOrderCenterWnd::OnClickSymbol(const std::string& symbol_info)
{
	
}

void SmFundOrderCenterWnd::OnTimer(UINT_PTR nIDEvent)
{
	//if (!_Resizing) _OrderArea.Refresh();

	//_PositionArea.UpdatePositionInfo();
	//_PositionArea.UpdateSymbolInfo();
	//_QuoteArea.UpdateSymbolInfo();

	CTime t1;
	t1 = CTime::GetCurrentTime();
	//m_bar.SetPaneText(1, t1.Format("%H:%M:%S"));
	if (_Fund && _Symbol) {
		int filled_count = 0;
		const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
		for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
			auto account = (*it);
			auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account->No(), _Symbol->SymbolCode());
			filled_count = symbol_order_mgr->GetUnsettledCount();
		}

		


		CString strCount;
		strCount.Format("%d", filled_count);
		_StaticFilledCount.SetWindowText(strCount);
		//_StaticFilledCount.Invalidate();
	}
}


void SmFundOrderCenterWnd::SetSymbolInfo(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	_Symbol = symbol;

	std::string symbol_info(symbol->SymbolNameKr());
	symbol_info.append(" [");
	symbol_info.append(symbol->SymbolCode());
	symbol_info.append("]");

	int result = _ComboSymbol.SelectString(-1, symbol_info.c_str());
	if (result == CB_ERR) {
		_CurrentIndex = _ComboSymbol.AddString(symbol_info.c_str());
		_IndexToSymbolMap[_CurrentIndex] = _Symbol;
		_ComboSymbol.SetCurSel(_CurrentIndex);
		mainApp.SymMgr()->RegisterSymbolToServer(_Symbol->SymbolCode(), true);
	}
	else {
		_CurrentIndex = result;

		auto found = _IndexToSymbolMap.find(result);

		if (found == _IndexToSymbolMap.end()) return;

		_Symbol = _IndexToSymbolMap[_CurrentIndex];
	}

	_StaticSymbolName.SetWindowText(symbol_info.c_str());

	SetInfo(_Symbol);
}

void SmFundOrderCenterWnd::UpdateOrderSettings()
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

	_OrderArea.UpdateOrderSettings(settings);
}

void SmFundOrderCenterWnd::SetCutMode()
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
	_OrderArea.CutMode(_CutMode);
}

void SmFundOrderCenterWnd::SetInfo(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	_PositionArea.Clear();
	_QuoteArea.Clear();
	_OrderArea.Clear();
	_PositionArea.Symbol(symbol);
	_OrderArea.Symbol(symbol);
	//_OrderArea.SetCenterValues(symbol);
	//_OrderArea.SetQuote(symbol);
	//_OrderArea.SetHoga(symbol);
	_QuoteArea.Symbol(symbol);
	//_OrderArea.UpdateOrder(_Symbol->SymbolCode());
	_OrderArea.Refresh();
	_RemainButton.Symbol(symbol);
	_FilledRemainButton.Symbol(symbol);
}

void SmFundOrderCenterWnd::ArrangeCenterValue()
{
	_OrderArea.ArrangeCenterValue();
}

void SmFundOrderCenterWnd::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	_OrderArea.OnOrderChanged(account_id, symbol_id);
	_PositionArea.OnOrderChanged(account_id, symbol_id);
}

int SmFundOrderCenterWnd::RecalcOrderAreaHeight(CWnd* wnd, bool bottom_up)
{
	CRect rcTopMost;
	CRect rcOrderArea;
	CRect rcOrderWnd;
	_OrderArea.GetWindowRect(rcOrderArea);

	GetWindowRect(rcOrderWnd);

	wnd->GetWindowRect(rcTopMost);

	CRect rcSymbol;
	_StaticSymbolName.GetWindowRect(rcSymbol);

	int y_del = 0, delta_height = 0;
	y_del = rcTopMost.bottom - rcOrderArea.top;
	y_del -= 8;

	delta_height = _OrderArea.RecalRowCount(y_del);

	if (delta_height > 0) {
		CRect rcParent;
		CRect rect;
		_OrderArea.GetWindowRect(rect);

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

void SmFundOrderCenterWnd::OnCbnSelchangeComboSymbol()
{
	const int cur_sel = _ComboSymbol.GetCurSel();
	if (cur_sel < 0) return;

	auto found = _IndexToSymbolMap.find(cur_sel);

	if (found == _IndexToSymbolMap.end()) return;

	_CurrentIndex = cur_sel;

	_Symbol = _IndexToSymbolMap[_CurrentIndex];


	std::string symbol_info(_Symbol->SymbolNameKr());
	symbol_info.append(" [");
	symbol_info.append(_Symbol->SymbolCode());
	symbol_info.append("]");

	_StaticSymbolName.SetWindowText(symbol_info.c_str());

	SetInfo(_Symbol);
}


void SmFundOrderCenterWnd::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if (_Init && _OrderArea.GetSafeHwnd()) {
		CRect rcWnd;
		GetWindowRect(rcWnd);
		//_OrderArea.RecalRowCount(rcWnd.Height());
		_OrderArea.Invalidate();
	}

}


void SmFundOrderCenterWnd::OnBnClickedCheckShowRealQuote()
{
	_ShowQuoteArea ? _ShowQuoteArea = false : _ShowQuoteArea = true;
	CWnd* parent = GetParent();
	CRect rcQuote;
	_QuoteArea.GetWindowRect(rcQuote);
	CRect rcWnd;
	GetWindowRect(rcWnd);
	parent->ScreenToClient(rcWnd);
	if (_ShowQuoteArea) {
		rcWnd.right = rcWnd.left + rcWnd.Width() + rcQuote.Width();
		_QuoteArea.ShowWindow(SW_SHOW);
	}
	else {
		rcWnd.right = rcWnd.left + rcWnd.Width() - rcQuote.Width();
		_QuoteArea.ShowWindow(SW_HIDE);
	}

	MoveWindow(rcWnd);

	((SmFundOrderDialog*)parent)->RecalcChildren(_ShowQuoteArea ? CM_SHOW_TICK : CM_HIDE_TICK);
}


void SmFundOrderCenterWnd::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
}


void SmFundOrderCenterWnd::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
}


void SmFundOrderCenterWnd::OnBnClickedBtnRefreshOrder()
{
	_OrderArea.UpdateOrder(_Symbol->SymbolCode());
	_OrderArea.Invalidate(FALSE);
}


BOOL SmFundOrderCenterWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return FALSE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void SmFundOrderCenterWnd::OnBnClickedBtnSymbol()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	_SymbolTableDlg->FundOrderWnd = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

LRESULT SmFundOrderCenterWnd::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = (int)wParam;
	std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbolById(symbol_id);
	if (symbol) SetSymbolInfo(symbol);
	return 1;
}


BOOL SmFundOrderCenterWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
			//OnEntered();
			return TRUE;
		}
		else if (pMsg->wParam == VK_SPACE) {
			_OrderArea.PutOrderBySpaceBar();
			return TRUE;
		}
		else if (pMsg->wParam == VK_DOWN) {
			_OrderArea.ChangeOrderByKey(-1);
		}
		else if (pMsg->wParam == VK_UP) {
			_OrderArea.ChangeOrderByKey(1);
		}
	}

	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void SmFundOrderCenterWnd::OnBnClickedBtnAmount1()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("1");
}


void SmFundOrderCenterWnd::OnBnClickedBtnAmount2()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("2");
}


void SmFundOrderCenterWnd::OnBnClickedBtnAmount5()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("5");
}


void SmFundOrderCenterWnd::OnBnClickedBtnAmount3()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("3");
}


void SmFundOrderCenterWnd::OnBnClickedBtnAmount4()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("4");
}


void SmFundOrderCenterWnd::OnEnChangeEditAmount()
{
	CEdit* amount = (CEdit*)GetDlgItem(IDC_EDIT_AMOUNT);
	if (!amount) return;

	CString strValue;
	amount->GetWindowText(strValue);
	_OrderArea.OrderAmount(_ttoi(strValue));
}





void SmFundOrderCenterWnd::OnBnClickedBtnLiqSymbolPosition()
{
	if (!_Fund || !_Symbol) return;


	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);
		auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(account->No());
		const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();
		auto found = account_pos_map.find(_Symbol->SymbolCode());
		if (found == account_pos_map.end()) return;

		std::shared_ptr<SmOrderRequest> order_req = nullptr;
		if (found->second->Position == SmPositionType::Buy)
			order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), _Symbol->SymbolCode(), 0, abs(found->second->OpenQty), DarkHorse::SmPriceType::Market);
		else
			order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), _Symbol->SymbolCode(), 0, abs(found->second->OpenQty), DarkHorse::SmPriceType::Market);
		mainApp.Client()->NewOrder(order_req);
	}
}


void SmFundOrderCenterWnd::OnBnClickedCheckProfit()
{
	UpdateOrderSettings();
}


void SmFundOrderCenterWnd::OnBnClickedCheckLoss()
{
	UpdateOrderSettings();
}


void SmFundOrderCenterWnd::OnBnClickedRadioMarket()
{
	UpdateOrderSettings();
}


void SmFundOrderCenterWnd::OnBnClickedRadioPrice()
{
	UpdateOrderSettings();
}


void SmFundOrderCenterWnd::OnEnChangeEditProfit()
{
	UpdateOrderSettings();
}


void SmFundOrderCenterWnd::OnEnChangeEditLoss()
{
	UpdateOrderSettings();
}


void SmFundOrderCenterWnd::OnEnChangeEditSlip()
{
	UpdateOrderSettings();
}


void SmFundOrderCenterWnd::OnBnClickedBtnSearch()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	_SymbolTableDlg->FundOrderWnd = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

void SmFundOrderCenterWnd::OnBnClickedBtnSet()
{
	_OrderSetDlg = std::make_shared<SmOrderSetDialog>();
	_OrderSetDlg->Create(IDD_ORDER_SET, this);
	_OrderSetDlg->FundWnd(this);
	_OrderSetDlg->ShowWindow(SW_SHOW);
}

void SmFundOrderCenterWnd::OnBnClickedCheckFixHoga()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_FIX_HOGA))->GetCheck() == BST_CHECKED)
		_OrderArea.FixedMode(true);
	else
		_OrderArea.FixedMode(false);
}
