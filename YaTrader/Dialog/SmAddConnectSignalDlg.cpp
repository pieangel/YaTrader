// VtAddConnectSignalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmAddConnectSignalDlg.h"
#include "afxdialogex.h"
#include "HdSymbolSelecter.h"
#include "../Global/SmTotalManager.h"
#include "../OutSystem/SmOutSystem.h"
#include "../OutSystem/SmOutSystemManager.h"
#include "../OutSystem/SmOutSignalDef.h"
#include "../Symbol/SmSymbol.h"
#include "../Account/SmAccount.h"
#include "../Fund/SmFund.h"
#include "../Fund/SmFundManager.h"
#include "../Account/SmAccountManager.h"
#include "../Order/SmOrderConst.h"
#include "SmAutoSignalManagerDialog.h"
#include "../Event/EventHub.h"
#include <functional>
#include "../OutSystem/VtAutoSignalManagerDialog.h"
#include "../Util/IdGenerator.h"
#include "../Symbol/AbFavoriteSymbolSelector.h"
// VtAddConnectSignalDlg dialog
using namespace DarkHorse;

IMPLEMENT_DYNAMIC(SmAddConnectSignalDlg, CBCGPDialog)

SmAddConnectSignalDlg::SmAddConnectSignalDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_ADD_SIG_CONNECT, pParent), id_(IdGenerator::get_id())
{
	mainApp.event_hub()->subscribe_symbol_event_handler(id_, std::bind(&SmAddConnectSignalDlg::set_symbol_from_out, this, std::placeholders::_1, std::placeholders::_2));
}

SmAddConnectSignalDlg::SmAddConnectSignalDlg(SmAutoSignalManagerDialog* source_dialog)
	: CBCGPDialog(IDD_ADD_SIG_CONNECT), source_dialog_(source_dialog), id_(IdGenerator::get_id())
{
	mainApp.event_hub()->subscribe_symbol_event_handler(id_, std::bind(&SmAddConnectSignalDlg::set_symbol_from_out, this, std::placeholders::_1, std::placeholders::_2));
}

SmAddConnectSignalDlg::~SmAddConnectSignalDlg()
{
	mainApp.event_hub()->unsubscribe_symbol_event_handler(id_);
}

void SmAddConnectSignalDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACNT, _ComboAcnt);
	DDX_Control(pDX, IDC_COMBO_SIGNAL, _ComboSignal);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, _ComboSymbol);
	DDX_Control(pDX, IDC_COMBO_TYPE, _ComboType);
	DDX_Control(pDX, IDC_EDIT_SEUNGSU, _EditSeungsu);
	DDX_Control(pDX, IDC_SPIN_SEUNGSU, _SpinSeungsu);
}


BEGIN_MESSAGE_MAP(SmAddConnectSignalDlg, CBCGPDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &SmAddConnectSignalDlg::OnCbnSelchangeComboType)
	ON_CBN_SELCHANGE(IDC_COMBO_ACNT, &SmAddConnectSignalDlg::OnCbnSelchangeComboAcnt)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &SmAddConnectSignalDlg::OnCbnSelchangeComboSymbol)
	ON_BN_CLICKED(IDC_BTN_FIND_SYMBOL, &SmAddConnectSignalDlg::OnBnClickedBtnFindSymbol)
	ON_CBN_SELCHANGE(IDC_COMBO_SIGNAL, &SmAddConnectSignalDlg::OnCbnSelchangeComboSignal)
	ON_BN_CLICKED(IDC_BTN_OK, &SmAddConnectSignalDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &SmAddConnectSignalDlg::OnBnClickedBtnCancel)
	ON_BN_CLICKED(IDC_BTN_ADD, &SmAddConnectSignalDlg::OnBnClickedBtnAdd)
END_MESSAGE_MAP()


// VtAddConnectSignalDlg message handlers


void SmAddConnectSignalDlg::OnCbnSelchangeComboType()
{
	int curSel = _ComboType.GetCurSel();
	if (curSel != -1) {
		_Mode = curSel;
		InitCombo();
	}
}


void SmAddConnectSignalDlg::OnCbnSelchangeComboAcnt()
{
	int selIndex = _ComboAcnt.GetCurSel();
	if (selIndex == -1) {
		return;
	}

	auto it = combo_to_account_map_.find(selIndex);
	if (it != combo_to_account_map_.end()) {
		account_ = it->second;
	}
}


void SmAddConnectSignalDlg::OnCbnSelchangeComboSymbol()
{
	int selIndex = _ComboSymbol.GetCurSel();
	if (selIndex == -1) {
		return;
	}

	auto it = combo_to_symbol_map_.find(selIndex);
	if (it != combo_to_symbol_map_.end()) {
		symbol_ = it->second;
	}
}


void SmAddConnectSignalDlg::OnBnClickedBtnFindSymbol()
{
	if (mainApp.mode == 0) {
		dm_symbol_selector_ = std::make_shared<HdSymbolSelecter>();
		dm_symbol_selector_->set_source_window_id(id_);
		dm_symbol_selector_->Create(IDD_SYMBOL_SELECTER_HD, this);
		dm_symbol_selector_->ShowWindow(SW_SHOW);
	}
	else {
		ab_symbol_selector_ = std::make_shared<AbFavoriteSymbolSelector>();
		ab_symbol_selector_->set_source_window_id(id_);
		ab_symbol_selector_->Create(IDD_AB_SYMBOL_SELECTOR, this);
		ab_symbol_selector_->ShowWindow(SW_SHOW);
	}
}


void SmAddConnectSignalDlg::OnCbnSelchangeComboSignal()
{
	int selIndex = _ComboSignal.GetCurSel();
	if (selIndex == -1) {
		return;
	}
	auto it = combo_to_out_sig_def_map_.find(selIndex);
	if (it != combo_to_out_sig_def_map_.end()) {
		out_sig_def_ = it->second;
	}
}


void SmAddConnectSignalDlg::OnBnClickedBtnOk()
{
	CBCGPDialog::OnOK();
}


void SmAddConnectSignalDlg::OnBnClickedBtnCancel()
{
	CBCGPDialog::OnCancel();
}


BOOL SmAddConnectSignalDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	_ComboType.AddString(_T("계좌"));
	_ComboType.AddString(_T("펀드"));
	_Mode = 0;
	_ComboType.SetCurSel(0);
	_EditSeungsu.SetWindowText(_T("1"));
	_SpinSeungsu.SetRange(0, 100);
	_ComboAcnt.SetDroppedWidth(200);
	InitCombo();
	InitOutSigDefCombo();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SmAddConnectSignalDlg::InitCombo()
{
	_ComboAcnt.ResetContent();
	int index = -1;
	if (_Mode == 0) {
		combo_to_account_map_.clear();
		std::vector<std::shared_ptr<SmAccount>> main_account_vector;
		mainApp.AcntMgr()->get_main_account_vector(main_account_vector);
		if (main_account_vector.empty()) return;
		for (auto ita = main_account_vector.begin(); ita != main_account_vector.end(); ++ita) {
			auto main_acnt = *ita;
			CString str;
			str.Format(_T("%s[%s]"), main_acnt->Name().c_str(), main_acnt->No().c_str());
			index = _ComboAcnt.AddString(str);
			combo_to_account_map_[index] = main_acnt;

			const std::vector<std::shared_ptr<SmAccount>>& sub_account_vector = main_acnt->get_sub_accounts();
			for (auto it = sub_account_vector.begin(); it != sub_account_vector.end(); it++) {
				auto account = *it;
				CString str;
				str.Format(_T("%s[%s]"), account->Name().c_str(), account->No().c_str());
				index = _ComboAcnt.AddString(str);
				combo_to_account_map_[index] = account;
			}
		}
		if (!combo_to_account_map_.empty()) {
			_ComboAcnt.SetCurSel(0);
			account_ = combo_to_account_map_.begin()->second;
		}
	}
	else {
		combo_to_fund_map_.clear();
		auto fund_map = mainApp.FundMgr()->GetFundMap();
		for (auto it = fund_map.begin(); it != fund_map.end(); ++it) {
			auto fund = it->second;
			CString str;
			str.Format(_T("%s"), fund->Name().c_str());
			index = _ComboAcnt.AddString(str);
			combo_to_fund_map_[index] = fund;
		}

		if (!combo_to_fund_map_.empty()) {
			_ComboAcnt.SetCurSel(0);
			fund_ = combo_to_fund_map_.begin()->second;
		}
	}
}

void SmAddConnectSignalDlg::InitOutSigDefCombo()
{
	auto signal_def_vector = mainApp.out_system_manager()->get_out_system_signal_map();
	int selIndex = -1;
	combo_to_out_sig_def_map_.clear();
	for (auto it = signal_def_vector.begin(); it != signal_def_vector.end(); ++it) {
		selIndex = _ComboSignal.AddString((*it)->name.c_str());
		combo_to_out_sig_def_map_[selIndex] = *it;
	}
	if (!combo_to_out_sig_def_map_.empty()) {
		out_sig_def_ = combo_to_out_sig_def_map_.begin()->second;
		_ComboSignal.SetCurSel(0);
	}
}

void SmAddConnectSignalDlg::set_symbol_from_out(const int window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (window_id != id_ || !symbol) return;
	symbol_ = symbol;
	int index = _ComboSymbol.AddString(symbol->SymbolCode().c_str());
	combo_to_symbol_map_[index] = symbol;
	_ComboSymbol.SetCurSel(index);
	if (dm_symbol_selector_) dm_symbol_selector_->SendMessage(WM_CLOSE);
}



void SmAddConnectSignalDlg::OnBnClickedBtnAdd()
{
	if (_Mode == 0 && !account_) { AfxMessageBox("계좌가 없습니다. 계좌를 선택하세요!"); return; }
	if (_Mode == 1 && !fund_) { AfxMessageBox("펀드가 없습니다. 펀드를 선택하세요!"); return; }
	if (!symbol_) { AfxMessageBox("종목이 없습니다. 종목을 선택하세요!"); return; }
	if (!out_sig_def_) { AfxMessageBox("목표 차트가 없습니다. 목표 차트를 선택하세요!"); return; }

	DarkHorse::OrderType order_type = DarkHorse::OrderType::None;
	if (_Mode == 0) {
		if (account_->is_subaccount())
			order_type = OrderType::SubAccount;
		else
			order_type = OrderType::MainAccount;
	}
	else {
		order_type = OrderType::Fund;
	}
	CString strSeungSu;
	_EditSeungsu.GetWindowText(strSeungSu);
	auto out_system = mainApp.out_system_manager()->create_out_system
	(
		out_sig_def_->name,
		_ttoi(strSeungSu),
		order_type,
		_Mode == 0 ? account_ : nullptr,
		_Mode == 1 ? fund_ : nullptr,
		symbol_
	);

	if (auto_connect_dialog_)
		auto_connect_dialog_->add_out_system(out_system);
}
