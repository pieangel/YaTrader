// SmCompOrderDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "../resource.h"
#include "SmCompOrderDialog.h"
#include "afxdialogex.h"
#include "SmOrderCompMainDialog.h"
#include "../Symbol/SmSymbolTableDialog.h"
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Client/ViStockClient.h"
#include "../Yuanta/YaStockClient.h"
#include "../Util/VtStringUtil.h"
#include "../Log/MyLogger.h"

// SmCompOrderDialog dialog

using namespace DarkHorse;

IMPLEMENT_DYNAMIC(SmCompOrderDialog, CBCGPDialog)

SmCompOrderDialog::SmCompOrderDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_COMP_ORDER_CENTER, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	
	EnableLayout();
}

SmCompOrderDialog::~SmCompOrderDialog()
{
	KillTimer(1);
}

void SmCompOrderDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ORDER, _OrderArea);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_NAME_KR, _StaticSymbolName);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, _ComboSymbol);
	DDX_Control(pDX, IDC_BTN_SEARCH, _BtnSearch);
	DDX_Control(pDX, IDC_CHECK_REAL, _CheckReal);
	DDX_Control(pDX, IDC_EDIT_AMOUNT, _EditAmount);
	DDX_Control(pDX, IDC_SPIN_AMOUNT, _SpinAmount);

	DDX_Control(pDX, IDC_RADIO_BUY, _RadioBuy);
	DDX_Control(pDX, IDC_RADIO_SELL, _RadioSell);
}


BEGIN_MESSAGE_MAP(SmCompOrderDialog, CBCGPDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SEARCH, &SmCompOrderDialog::OnBnClickedBtnSearch)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &SmCompOrderDialog::OnUmSymbolSelected)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &SmCompOrderDialog::OnCbnSelchangeComboSymbol)
	ON_BN_CLICKED(IDC_RADIO_BUY, &SmCompOrderDialog::OnBnClickedRadioBuy)
	ON_BN_CLICKED(IDC_RADIO_SELL, &SmCompOrderDialog::OnBnClickedRadioSell)
	ON_EN_CHANGE(IDC_EDIT_AMOUNT, &SmCompOrderDialog::OnEnChangeEditAmount)
	ON_BN_CLICKED(IDC_CHECK_REAL, &SmCompOrderDialog::OnBnClickedCheckReal)
END_MESSAGE_MAP()


// SmCompOrderDialog message handlers


void SmCompOrderDialog::SetInfo(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	_OrderArea.Symbol(symbol);
	_OrderArea.SetHoga(symbol);
	_OrderArea.SetQuote(symbol);
}

BOOL SmCompOrderDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	_ID = SmOrderCompMainDialog::GetId();

	_OrderArea.SetUp();

	_ComboSymbol.SetDroppedWidth(250);
	
	if (_BtnSearch.GetSafeHwnd()) {
		_BtnSearch.SetImage(IDB_BITMAP_SEARCH, IDB_BITMAP_SEARCH, 0, IDB_BITMAP_SEARCH);
		_BtnSearch.m_bRighImage = FALSE;
		_BtnSearch.m_bTopImage = FALSE;
		//_BtnSearch.SizeToContent();
		_BtnSearch.RedrawWindow();
	}

	_RadioBuy.SetCheck(BST_CHECKED);

	if (_EditAmount.GetSafeHwnd()) {
		_EditAmount.SetWindowText("1");
	}

	_SpinAmount.SetRange32(0, 100);

	SetTimer(1, 10, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmCompOrderDialog::SetSymbolInfo(std::shared_ptr<DarkHorse::SmSymbol> symbol)
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

void SmCompOrderDialog::OnSymbolClicked(const std::string& symbol_code)
{
	auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (symbol) SetSymbolInfo(symbol);
}

void SmCompOrderDialog::OnClickSymbol(const std::string& symbol_info)
{
	
}

void SmCompOrderDialog::SetSelected(const bool& selected)
{
	_Selected = selected;
	_OrderArea.Selected(selected);
	_OrderArea.Invalidate();
}

void SmCompOrderDialog::SetMainOrderWnd(SmOrderCompMainDialog* wnd)
{
	_OrderArea.SetMainOrderWnd(wnd);
}

void SmCompOrderDialog::SetFundOrderWnd(SmFundCompMainDialog* wnd)
{
	_OrderArea.SetFundCompOrderWnd(wnd);
}

void SmCompOrderDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	_OrderArea.Refresh();
	CBCGPDialog::OnTimer(nIDEvent);
}


void SmCompOrderDialog::OnBnClickedBtnSearch()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	_SymbolTableDlg->CompOrderWnd = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

LRESULT SmCompOrderDialog::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = (int)wParam;
	std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbolById(symbol_id);
	
	if (symbol) SetSymbolInfo(symbol);

	return 1;
}


void SmCompOrderDialog::OnCbnSelchangeComboSymbol()
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


void SmCompOrderDialog::OnBnClickedRadioBuy()
{
	_OrderPosition = DarkHorse::SmPositionType::Buy;
}


void SmCompOrderDialog::OnBnClickedRadioSell()
{
	_OrderPosition = DarkHorse::SmPositionType::Sell;
}


void SmCompOrderDialog::OnEnChangeEditAmount()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBCGPDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!_EditAmount.GetSafeHwnd()) return;
	CString strAmount;
	_EditAmount.GetWindowText(strAmount);
	_OrderAmount = _ttoi(strAmount);
}


void SmCompOrderDialog::OnBnClickedCheckReal()
{
	if (_CheckReal.GetCheck() == BST_CHECKED)
		_DoOrder = true;
	else
		_DoOrder = false;
}
