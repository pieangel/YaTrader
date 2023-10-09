// SmFundCompMainDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmFundCompMainDialog.h"
#include "afxdialogex.h"


#include "SmCompOrderDialog.h"
#include <set>
#include "../MainFrm.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccountManager.h"
#include "../Client/ViStockClient.h"
#include "../Yuanta/YaStockClient.h"
#include "../MessageDefine.h"
#include "../Event/SmCallbackManager.h"

#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmAccountPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Order/SmOrderRequest.h"
#include "../Client/ViStockClient.h"
#include "../Yuanta/YaStockClient.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Order/SmTotalOrderManager.h"
#include "SmCompOrderDialog.h"
#include "SmFundCompLeftDialog.h"
#include "SmFundCompRightDialog.h"
#include "../Symbol/SmSymbol.h"
#include "../Fund/SmFundManager.h"
#include "../Fund/SmFund.h"
#include "../Yuanta/YaStockClient.h"
using namespace DarkHorse;

const int CtrlHeight = 32;

int SmFundCompMainDialog::_Id = 0;

void SmFundCompMainDialog::SetAccount()
{
	const std::unordered_map<std::string, std::shared_ptr<DarkHorse::SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
	for (auto it = account_map.begin(); it != account_map.end(); ++it) {
		auto account = it->second;
		std::string account_info;
		account_info.append(account->Name());
		account_info.append(" : ");
		account_info.append(account->No());
		const int index = _ComboFund.AddString(account_info.c_str());
		_ComboAccountMap[index] = account;

	}

	if (!_ComboAccountMap.empty()) {
		_CurrentAccountIndex = 0;
		_ComboFund.SetCurSel(_CurrentAccountIndex);
		SetAccountInfo(_ComboAccountMap[_CurrentAccountIndex]);
		_LeftWnd->OnOrderChanged(0, 0);
	}
}

void SmFundCompMainDialog::SetFund()
{
	const std::map<std::string, std::shared_ptr<SmFund>>& fund_map = mainApp.FundMgr()->GetFundMap();

	for (auto it = fund_map.begin(); it != fund_map.end(); ++it) {
		auto fund = it->second;
		std::string account_info;
		account_info.append(fund->Name());
		const int index = _ComboFund.AddString(account_info.c_str());
		_ComboFundMap[index] = fund;

	}

	if (!_ComboFundMap.empty()) {
		_CurrentFundIndex = 0;
		_ComboFund.SetCurSel(_CurrentFundIndex);
		SetFundInfo(_ComboFundMap[_CurrentFundIndex]);
		_LeftWnd->OnOrderChanged(0, 0);
	}
}

// SmFundCompMainDialog dialog

IMPLEMENT_DYNAMIC(SmFundCompMainDialog, CBCGPDialog)

SmFundCompMainDialog::SmFundCompMainDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_MAIN, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmFundCompMainDialog::~SmFundCompMainDialog()
{
}

void SmFundCompMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboFund);
	//DDX_Control(pDX, IDC_STATIC_ACCOUNT_NAME, _StaticAccountName);
	DDX_Control(pDX, IDC_STATIC_LINE1, _Line1);
	DDX_Control(pDX, IDC_STATIC_LINE2, _Line2);
	DDX_Control(pDX, IDC_STATIC_LINE3, _Line3);
	DDX_Control(pDX, IDC_STATIC_LINE4, _Line4);
	DDX_Control(pDX, IDC_STATIC_LINE5, _Line5);
	DDX_Control(pDX, IDC_STATIC_LINE6, _Line6);
	DDX_Control(pDX, IDC_STATIC_LINE7, _Line7);
	DDX_Control(pDX, IDC_STATIC_LINE8, _Line8);
	DDX_Control(pDX, IDC_STATIC_LINE9, _Line9);
	DDX_Control(pDX, IDC_STATIC_LINE10, _Line10);
	DDX_Control(pDX, IDC_STATIC_LINE11, _Line11);
	DDX_Control(pDX, IDC_STATIC_LINE12, _Line12);
	DDX_Control(pDX, IDC_STATIC_LINE13, _Line13);
	DDX_Control(pDX, IDC_STATIC_LINE14, _Line14);
	DDX_Control(pDX, IDC_STATIC_LINE15, _Line15);
	DDX_Control(pDX, IDC_STATIC_LINE16, _Line16);
	DDX_Control(pDX, IDC_STATIC_LINE17, _Line17);
	DDX_Control(pDX, IDC_STATIC_LINE18, _Line18);
	DDX_Control(pDX, IDC_STATIC_LINE19, _Line19);
	DDX_Control(pDX, IDC_STATIC_LINE20, _Line20);
	DDX_Control(pDX, IDC_STATIC_LINE21, _Line21);
	DDX_Control(pDX, IDC_STATIC_MSG, _StaticMsg);
}


BEGIN_MESSAGE_MAP(SmFundCompMainDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD, &SmFundCompMainDialog::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_REMOVE, &SmFundCompMainDialog::OnBnClickedBtnRemove)
	ON_BN_CLICKED(IDC_BTN_LEFT, &SmFundCompMainDialog::OnBnClickedBtnLeft)
	ON_BN_CLICKED(IDC_BTN_RIGHT, &SmFundCompMainDialog::OnBnClickedBtnRight)
	ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	//ON_STN_CLICKED(IDC_STATIC_ACCOUNT_NAME, &SmFundCompMainDialog::OnStnClickedStaticAccountName)
	ON_BN_CLICKED(IDC_BUTTON6, &SmFundCompMainDialog::OnBnClickedButton6)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &SmFundCompMainDialog::OnCbnSelchangeComboAccount)
	ON_MESSAGE(WM_ORDER_UPDATE, &SmFundCompMainDialog::OnUmOrderUpdate)
	ON_MESSAGE(WM_SERVER_MSG, &SmFundCompMainDialog::OnUmServerMsg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &SmFundCompMainDialog::OnBnClickedBtnLiqAll)
END_MESSAGE_MAP()


// SmFundCompMainDialog message handlers


LRESULT SmFundCompMainDialog::OnEnterSizeMove(WPARAM wparam, LPARAM lparam)
{


	GetWindowRect(moveRect);
	// do stuff      
	return (LRESULT)0;
}
LRESULT SmFundCompMainDialog::OnExitSizeMove(WPARAM wparam, LPARAM lparam)
{


	CRect rcWnd;
	GetWindowRect(rcWnd);

	if (moveRect.Width() == rcWnd.Width() && moveRect.Height() == rcWnd.Height()) return 0;

	RecalcChildren(CM_REFRESH);
	//Invalidate(FALSE);
	// do stuff      
	return (LRESULT)0;
}


BOOL SmFundCompMainDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	CRect rcClient, rcWnd;

	_LineVector.push_back(&_Line1);
	_LineVector.push_back(&_Line2);
	_LineVector.push_back(&_Line3);
	_LineVector.push_back(&_Line4);
	_LineVector.push_back(&_Line5);
	_LineVector.push_back(&_Line6);
	_LineVector.push_back(&_Line7);
	_LineVector.push_back(&_Line8);
	_LineVector.push_back(&_Line9);
	_LineVector.push_back(&_Line10);
	_LineVector.push_back(&_Line11);
	_LineVector.push_back(&_Line12);
	_LineVector.push_back(&_Line13);
	_LineVector.push_back(&_Line14);
	_LineVector.push_back(&_Line15);
	_LineVector.push_back(&_Line16);
	_LineVector.push_back(&_Line17);
	_LineVector.push_back(&_Line18);
	_LineVector.push_back(&_Line19);
	_LineVector.push_back(&_Line20);
	_LineVector.push_back(&_Line21);


	GetWindowRect(rcWnd);


	_LeftWnd = std::make_shared<SmFundCompLeftDialog>(this);
	_LeftWnd->Create(IDD_ORDER_LEFT, this);
	_LeftWnd->_BtnAddFav.ShowWindow(SW_SHOW);
	_LeftWnd->_StaticFav.ShowWindow(SW_SHOW);
	_LeftWnd->_FavoriteGrid.ShowWindow(SW_SHOW);
	_LeftWnd->ShowWindow(SW_SHOW);
	_LeftWnd->SetMainWnd(this);


	std::shared_ptr<SmCompOrderDialog> center_wnd = std::make_shared<SmCompOrderDialog>(this);
	center_wnd->Create(IDD_COMP_ORDER_CENTER, this);
	center_wnd->ShowWindow(SW_SHOW);
	center_wnd->SetFundOrderWnd(this);
	center_wnd->SetSelected(true);
	_CenterWndMap.insert(std::make_pair(center_wnd->ID(), center_wnd));

	center_wnd = std::make_shared<SmCompOrderDialog>(this);
	center_wnd->Create(IDD_COMP_ORDER_CENTER, this);
	center_wnd->ShowWindow(SW_SHOW);
	center_wnd->SetFundOrderWnd(this);
	//center_wnd->Selected(true);
	_CenterWndMap.insert(std::make_pair(center_wnd->ID(), center_wnd));


	center_wnd = std::make_shared<SmCompOrderDialog>(this);
	center_wnd->Create(IDD_COMP_ORDER_CENTER, this);
	center_wnd->ShowWindow(SW_SHOW);
	center_wnd->SetFundOrderWnd(this);
	//center_wnd->Selected(true);
	_CenterWndMap.insert(std::make_pair(center_wnd->ID(), center_wnd));


	center_wnd = std::make_shared<SmCompOrderDialog>(this);
	center_wnd->Create(IDD_COMP_ORDER_CENTER, this);
	center_wnd->ShowWindow(SW_SHOW);
	center_wnd->SetFundOrderWnd(this);
	//center_wnd->Selected(true);
	_CenterWndMap.insert(std::make_pair(center_wnd->ID(), center_wnd));

	center_wnd = std::make_shared<SmCompOrderDialog>(this);
	center_wnd->Create(IDD_COMP_ORDER_CENTER, this);
	center_wnd->ShowWindow(SW_SHOW);
	center_wnd->SetFundOrderWnd(this);
	//center_wnd->Selected(true);
	_CenterWndMap.insert(std::make_pair(center_wnd->ID(), center_wnd));

	center_wnd = std::make_shared<SmCompOrderDialog>(this);
	center_wnd->Create(IDD_COMP_ORDER_CENTER, this);
	center_wnd->ShowWindow(SW_SHOW);
	center_wnd->SetFundOrderWnd(this);
	//center_wnd->Selected(true);
	_CenterWndMap.insert(std::make_pair(center_wnd->ID(), center_wnd));

	center_wnd->GetWindowRect(rcWnd);

	_RightWnd = std::make_shared<SmFundCompRightDialog>(this);
	_RightWnd->Create(IDD_ORDER_RIGHT_COMP, this);
	_RightWnd->ShowWindow(SW_SHOW);
	_RightWnd->MainWnd(this);


	SetFund();

	SetFundForOrderWnd();



	GetWindowRect(rcWnd);

	//rcWnd.bottom = rcWnd.top + 697;

	rcWnd.bottom = rcWnd.top + 1005;
	MoveWindow(rcWnd);
	//GetClientRect(rcClient);
	//_StaticAccountName.GetWindowRect(rcClient);

	RECT rcFrame = { 0 };
	//AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW, FALSE, 0);



	RecalcChildren(CM_REFRESH);



	_Init = true;

	CenterWindow();

	_StaticMsg.SetUp();

	mainApp.CallbackMgr()->SubscribeOrderUpdateCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->SubscribeServerMsgCallback(GetSafeHwnd());

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SmFundCompMainDialog::OnQuoteAreaShowHide()
{

}

void SmFundCompMainDialog::RecalcChildren(CmdMode mode)
{

	std::set<CWnd*> wnd_set;
	const int top_gap = 2;
	//const int hor_gap = 2;
	int max_height = 0, width_total = 0, start_x = 0;
	CRect rcWnd;
	CRect rcLine;
	CRect rcMain, rcMainWork, rcLeft, rcRight;
	_LeftWnd->GetWindowRect(rcLeft);
	_RightWnd->GetWindowRect(rcRight);
	GetWindowRect(rcMain);
	GetClientRect(rcMainWork);

	// 전체 길이 계산
	const int hor_gap = 5;
	int total_width = 0;


	start_x = hor_gap;
	std::vector<CRect> rcVector;
	int line_index = 0;
	if (_ShowLeft) {
		_LeftWnd->GetWindowRect(rcWnd);
		_rcLeft = rcWnd;
		_LeftWnd->MoveWindow(start_x, CtrlHeight + top_gap, rcWnd.Width(), rcWnd.Height());
		rcVector.push_back(rcWnd);

		_LeftWnd->ShowWindow(SW_SHOW);
		max_height = rcWnd.Height();
		start_x += rcWnd.Width();
		width_total += rcWnd.Width();
		wnd_set.insert(_LeftWnd.get());

		rcLine.left = start_x + 1;
		rcLine.right = start_x + _LineGap;
		rcLine.top = CtrlHeight + top_gap + 2;
		rcLine.bottom = 1600;
		_LineVector[line_index]->MoveWindow(rcLine);
		_LineVector[line_index]->ShowWindow(SW_SHOW);
		line_index++;

		start_x += hor_gap;
	}
	else {
		_LeftWnd->ShowWindow(SW_HIDE);
	}

	int i = 0;
	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); it++) {
		std::shared_ptr<SmCompOrderDialog> center_wnd = it->second;
		center_wnd->GetWindowRect(rcWnd);
		center_wnd->ShowWindow(SW_SHOW);
		i++;
		const int mod_val = 3;
		const int modu = i % mod_val;
		int top = 0, bottom = 0;
		if (modu != 0) {
			top = CtrlHeight + top_gap + rcWnd.Height() * (modu - 1);
			bottom = CtrlHeight + top_gap + rcWnd.Height() * modu;
			center_wnd->MoveWindow(start_x, top, rcWnd.Width(), rcWnd.Height());
			rcVector.push_back(rcWnd);

			rcLine.left = start_x - 1;
			rcLine.right = start_x + rcWnd.Width() + 1;
			rcLine.top = bottom + 1;
			rcLine.bottom = bottom + 3;
			_LineVector[line_index]->MoveWindow(rcLine);
			_LineVector[line_index]->ShowWindow(SW_SHOW);
			line_index++;
		}
		else {

			top = CtrlHeight + top_gap + rcWnd.Height() * (mod_val - 1);
			bottom = CtrlHeight + top_gap + rcWnd.Height() * mod_val;

			center_wnd->MoveWindow(start_x, top, rcWnd.Width(), rcWnd.Height());

			rcLine.left = start_x - 1;
			rcLine.right = start_x + rcWnd.Width() + 1;
			rcLine.top = bottom + 2;
			rcLine.bottom = bottom + 4;
			_LineVector[line_index]->MoveWindow(rcLine);
			_LineVector[line_index]->ShowWindow(SW_SHOW);
			line_index++;


			start_x += rcWnd.Width();
			width_total += rcWnd.Width();

			//center_wnd->RecalcOrderAreaHeight(this);

			wnd_set.insert(center_wnd.get());

			rcLine.left = start_x + 1;
			rcLine.right = start_x + _LineGap;
			rcLine.top = CtrlHeight + top_gap + 2;
			rcLine.bottom = 1600;
			_LineVector[line_index]->MoveWindow(rcLine);
			_LineVector[line_index]->ShowWindow(SW_SHOW);
			line_index++;
			start_x += hor_gap;
		}

	}


	if (_ShowRight) {
		_RightWnd->GetWindowRect(rcWnd);
		_rcRight = rcWnd;

		if (rcWnd.Height() > max_height) max_height = rcWnd.Height();
		_RightWnd->MoveWindow(start_x, CtrlHeight + top_gap, rcWnd.Width(), rcWnd.Height());
		rcVector.push_back(rcWnd);
		_RightWnd->ShowWindow(SW_SHOW);
		wnd_set.insert(_RightWnd.get());
		start_x += rcWnd.Width();
		width_total += rcWnd.Width();
		width_total += hor_gap;
	}
	else {
		_RightWnd->ShowWindow(SW_HIDE);
	}





	CRect rcClient;
	GetClientRect(&rcClient);

	if (mode != CM_REFRESH)
		Invalidate();




	GetWindowRect(rcWnd);
	rcWnd.right = rcWnd.left + width_total + 35;

	rcLine.left = 0;
	rcLine.right = rcWnd.Width() - 8;
	rcLine.top = CtrlHeight - 2;
	rcLine.bottom = CtrlHeight + _LineGap - 2;

	_LineVector[line_index]->MoveWindow(rcLine);
	_LineVector[line_index]->ShowWindow(SW_SHOW);
	line_index++;

	SetWindowPos(nullptr, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOZORDER | SWP_FRAMECHANGED);

	GetWindowRect(rcMain);
	_rcMain = rcMain;

	_StaticMsg.GetWindowRect(rcWnd);
	int len = rcMain.right - rcWnd.left - 13;
	rcWnd.right = rcWnd.left + len;
	ScreenToClient(rcWnd);
	//_StaticMsg.MoveWindow(rcWnd);
	_StaticMsg.SetWindowPos(nullptr, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOZORDER | SWP_FRAMECHANGED);
	SendMessage(WM_SERVER_MSG, 0, 0);
}


void SmFundCompMainDialog::OnDoOrder(const DarkHorse::SmPriceType price_type, const int slip)
{
	if (!_Fund) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);
		for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); it++) {
			std::shared_ptr<SmCompOrderDialog> center_wnd = it->second;
			if (!center_wnd->DoOrder()) continue;

			std::shared_ptr<DarkHorse::SmSymbol> symbol = center_wnd->Symbol();
			if (!symbol) continue;

			const DarkHorse::SmPositionType position_type = center_wnd->OrderPosition();
			const int order_amount = center_wnd->OrderAmount();
			const int int_tick_size = static_cast<int>(symbol->TickSize() * pow(10, symbol->decimal()));

			int price = 0;

			if (price_type == SmPriceType::Price) {
				if (position_type == DarkHorse::SmPositionType::Buy)
					price = symbol->Qoute.close + slip * int_tick_size;
				else
					price = symbol->Qoute.close - slip * int_tick_size;
			}

			std::shared_ptr<SmOrderRequest> order_req = nullptr;
			if (position_type == DarkHorse::SmPositionType::Sell)
				order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), price, order_amount, price_type);
			else
				order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), price, order_amount, price_type);
			if (order_req) {
				order_req->CutSlip = slip;
				mainApp.Client()->NewOrder(order_req);
			}
		}
	}
}

void SmFundCompMainDialog::SetAccountForOrderWnd()
{
	if (_ComboAccountMap.size() > 0) {
		const int cur_sel = _ComboFund.GetCurSel();
		if (cur_sel >= 0) {
			for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); ++it) {
				it->second->Account(_ComboAccountMap[cur_sel]);
			}
		}
	}
}


void SmFundCompMainDialog::SetAccountInfo(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account || _CurrentAccountIndex < 0) return;

	std::string account_info;
	account_info.append(account->Name());
	account_info.append(" : ");
	account_info.append(account->No());

	mainApp.Client()->RegisterAccount(account->No());
	_Account = account;

	_LeftWnd->SetAccount(_ComboAccountMap[_CurrentAccountIndex]);
	_RightWnd->SetAccount(_ComboAccountMap[_CurrentAccountIndex]);
}

void SmFundCompMainDialog::SetFundInfo(std::shared_ptr<DarkHorse::SmFund> fund)
{
	if (!fund || _CurrentFundIndex < 0) return;

	std::string account_info;
	account_info.append(fund->Name());
	//account_info.append(" : ");
	//account_info.append(account->No());
	//_StaticAccountName.SetWindowText(account_info.c_str());
	const std::vector<std::shared_ptr<SmAccount>>& account_vec = fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);
		mainApp.Client()->RegisterAccount(account->No());
	}

	_Fund = fund;

	
}

void SmFundCompMainDialog::SetFundForOrderWnd()
{
	if (_ComboFundMap.size() > 0) {
		const int cur_sel = _ComboFund.GetCurSel();
		if (cur_sel >= 0) {
			for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); ++it) {
				it->second->Fund(_ComboFundMap[cur_sel]);
			}

			_LeftWnd->SetFund(_ComboFundMap[cur_sel]);
			_RightWnd->SetFund(_ComboFundMap[cur_sel]);
		}
	}
}

void SmFundCompMainDialog::OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); it++) {
		if (it->second->Selected()) {
			it->second->OnSymbolClicked(symbol->SymbolCode());
			break;
		}
	}
	_RightWnd->SetSymbol(symbol);
}

void SmFundCompMainDialog::OnSymbolClicked(const std::string& symbol_code)
{
	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); it++) {
		if (it->second->Selected()) {
			it->second->OnSymbolClicked(symbol_code);
			break;
		}
	}
}

void SmFundCompMainDialog::OnBnClickedBtnAdd()
{
	std::shared_ptr<SmCompOrderDialog> center_wnd = std::make_shared<SmCompOrderDialog>();
	center_wnd->Create(IDD_COMP_ORDER_CENTER, this);
	center_wnd->ShowWindow(SW_HIDE);
	center_wnd->SetFundOrderWnd(this);
	center_wnd->SetSelected(false);
	center_wnd->Account(_ComboAccountMap[_CurrentAccountIndex]);
	_CenterWndMap.insert(std::make_pair(center_wnd->ID(), center_wnd));

	center_wnd = std::make_shared<SmCompOrderDialog>();
	center_wnd->Create(IDD_COMP_ORDER_CENTER, this);
	center_wnd->ShowWindow(SW_HIDE);
	center_wnd->SetFundOrderWnd(this);
	center_wnd->SetSelected(false);
	center_wnd->Account(_ComboAccountMap[_CurrentAccountIndex]);
	_CenterWndMap.insert(std::make_pair(center_wnd->ID(), center_wnd));

	center_wnd = std::make_shared<SmCompOrderDialog>();
	center_wnd->Create(IDD_COMP_ORDER_CENTER, this);
	center_wnd->ShowWindow(SW_HIDE);
	center_wnd->SetFundOrderWnd(this);
	center_wnd->SetSelected(true);
	center_wnd->Account(_ComboAccountMap[_CurrentAccountIndex]);
	_CenterWndMap.insert(std::make_pair(center_wnd->ID(), center_wnd));

	RecalcChildren(CM_ADD_CENTER);
}


void SmFundCompMainDialog::OnBnClickedBtnRemove()
{
	if (_CenterWndMap.size() == 1) return;

	//LockWindowUpdate();
	auto it = std::prev(_CenterWndMap.end());
	_CenterWndMap.erase(it);

	it = std::prev(_CenterWndMap.end());
	_CenterWndMap.erase(it);

	it = std::prev(_CenterWndMap.end());
	_CenterWndMap.erase(it);

	RecalcChildren(CM_DEL_CENTER);
	//UnlockWindowUpdate();
	Invalidate(FALSE);
}


void SmFundCompMainDialog::OnBnClickedBtnLeft()
{
	_ShowLeft ? _ShowLeft = false : _ShowLeft = true;
	//LockWindowUpdate();
	RecalcChildren(_ShowLeft ? CM_SHOW_LEFT : CM_HIDE_LEFT);
	//UnlockWindowUpdate();
	Invalidate(FALSE);
}


void SmFundCompMainDialog::OnBnClickedBtnRight()
{
	_ShowRight ? _ShowRight = false : _ShowRight = true;
	//LockWindowUpdate();
	RecalcChildren(_ShowRight ? CM_SHOW_RIGHT : CM_HIDE_RIGHT);
	//UnlockWindowUpdate();
	Invalidate(FALSE);
}


void SmFundCompMainDialog::OnStnClickedStaticAccountName()
{

}


void SmFundCompMainDialog::OnBnClickedButton6()
{
	int max_delta_height = 0;
	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); ++it) {
		std::shared_ptr<SmCompOrderDialog> center_wnd = it->second;
		//int delta_height = center_wnd->RecalcOrderAreaHeight(this);
		//if (delta_height > max_delta_height) max_delta_height = delta_height;
	}


	if (max_delta_height > 2) {
		CRect rcWnd;
		GetWindowRect(rcWnd);
		rcWnd.bottom += max_delta_height;
		SetWindowPos(nullptr, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOMOVE);
	}

	Invalidate(FALSE);
}


void SmFundCompMainDialog::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if (!_Init) return;

	//LockWindowUpdate();
	RecalcChildren(CM_REFRESH);
	//UnlockWindowUpdate();
	//Invalidate(FALSE);
}


BOOL SmFundCompMainDialog::OnEraseBkgnd(CDC* pDC)
{
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void SmFundCompMainDialog::OnCbnSelchangeComboAccount()
{
	_CurrentFundIndex = _ComboFund.GetCurSel();
	if (_CurrentFundIndex < 0) return;

	SetFundInfo(_ComboFundMap[_CurrentFundIndex]);

	SetFundForOrderWnd();
}

LRESULT SmFundCompMainDialog::OnUmOrderUpdate(WPARAM wParam, LPARAM lParam)
{
	const int account_id = (int)wParam;
	const int symbol_id = (int)lParam;

	_LeftWnd->OnOrderChanged(account_id, symbol_id);
	_RightWnd->OnOrderChanged(account_id, symbol_id);

	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); it++) {
		//it->second->OnOrderChanged(account_id, symbol_id);
	}

	return 1;
}

void SmFundCompMainDialog::ChangedSymbol(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	_RightWnd->SetSymbol(symbol);
}

void SmFundCompMainDialog::ChangedCenterWindow(const int& center_wnd_id)
{
	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); ++it) {
		it->second->ID() == center_wnd_id ? it->second->SetSelected(true) : it->second->SetSelected(false);
	}
}


void SmFundCompMainDialog::OnClose()
{
	mainApp.CallbackMgr()->UnsubscribeOrderUpdateCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->UnsubscribeServerMsgCallback(GetSafeHwnd());

	CBCGPDialog::OnClose();
}


void SmFundCompMainDialog::OnBnClickedBtnLiqAll()
{
	if (!_Fund) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);
		auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(account->No());
		const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();
		for (auto it = account_pos_map.begin(); it != account_pos_map.end(); ++it) {
			std::shared_ptr<SmOrderRequest> order_req = nullptr;
			if (it->second->Position == SmPositionType::Buy)
				order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
			else
				order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
			mainApp.Client()->NewOrder(order_req);
		}
	}
}

LRESULT SmFundCompMainDialog::OnUmServerMsg(WPARAM wParam, LPARAM lParam)
{
	_StaticMsg.Text(mainApp.TotalOrderMgr()->ServerMsg.c_str());
	_StaticMsg.Invalidate();
	return 1;
}
