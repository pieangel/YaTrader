// VtAddConnectSignalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmAddUsdSystemDlg.h"
#include "afxdialogex.h"
#include "../Dialog/HdSymbolSelecter.h"
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
#include "SmUSDSystemDialog.h"
#include "../Event/EventHub.h"
#include <functional>
#include "../OutSystem/VtAutoSignalManagerDialog.h"
#include "../Util/IdGenerator.h"
#include "../Util/VtTime.h"
#include "../OutSystem/SmUsdSystem.h"
// SmAddUsdSystemDlg dialog
using namespace DarkHorse;

IMPLEMENT_DYNAMIC(SmAddUsdSystemDlg, CBCGPDialog)

SmAddUsdSystemDlg::SmAddUsdSystemDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_ADD_USD_SYSTEM, pParent), id_(IdGenerator::get_id())
{
	mainApp.event_hub()->subscribe_symbol_event_handler(id_, std::bind(&SmAddUsdSystemDlg::set_symbol_from_out, this, std::placeholders::_1, std::placeholders::_2));
}

SmAddUsdSystemDlg::SmAddUsdSystemDlg(SmUSDSystemDialog* source_dialog)
	: CBCGPDialog(IDD_ADD_USD_SYSTEM), source_dialog_(source_dialog), id_(IdGenerator::get_id())
{
	mainApp.event_hub()->subscribe_symbol_event_handler(id_, std::bind(&SmAddUsdSystemDlg::set_symbol_from_out, this, std::placeholders::_1, std::placeholders::_2));
}

SmAddUsdSystemDlg::~SmAddUsdSystemDlg()
{
	mainApp.event_hub()->unsubscribe_symbol_event_handler(id_);
}

void SmAddUsdSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACNT, _ComboAcnt);
	DDX_Control(pDX, IDC_COMBO_SIGNAL, combo_usd_strategy_);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, _ComboSymbol);
	DDX_Control(pDX, IDC_COMBO_TYPE, _ComboType);
	DDX_Control(pDX, IDC_EDIT_SEUNGSU, _EditSeungsu);
	DDX_Control(pDX, IDC_SPIN_SEUNGSU, _SpinSeungsu);

	DDX_Control(pDX, IDC_DP_ENT_BEGIN, _DpEntBegin);
	DDX_Control(pDX, IDC_DP_ENT_END, _DpEntEnd);
	DDX_Control(pDX, IDC_DP_LIQ, _DpLiq);
	DDX_Control(pDX, IDC_EDIT_ENT_MAX, _EditEntMax);

}


BEGIN_MESSAGE_MAP(SmAddUsdSystemDlg, CBCGPDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &SmAddUsdSystemDlg::OnCbnSelchangeComboType)
	ON_CBN_SELCHANGE(IDC_COMBO_ACNT, &SmAddUsdSystemDlg::OnCbnSelchangeComboAcnt)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &SmAddUsdSystemDlg::OnCbnSelchangeComboSymbol)
	ON_BN_CLICKED(IDC_BTN_FIND_SYMBOL, &SmAddUsdSystemDlg::OnBnClickedBtnFindSymbol)
	ON_CBN_SELCHANGE(IDC_COMBO_SIGNAL, &SmAddUsdSystemDlg::OnCbnSelchangeComboSignal)
	ON_BN_CLICKED(IDC_BTN_OK, &SmAddUsdSystemDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &SmAddUsdSystemDlg::OnBnClickedBtnCancel)
	ON_BN_CLICKED(IDC_BTN_ADD, &SmAddUsdSystemDlg::OnBnClickedBtnAdd)
END_MESSAGE_MAP()


// VtAddConnectSignalDlg message handlers


void SmAddUsdSystemDlg::OnCbnSelchangeComboType()
{
	int curSel = _ComboType.GetCurSel();
	if (curSel != -1) {
		_Mode = curSel;
		InitCombo();
	}
}


void SmAddUsdSystemDlg::OnCbnSelchangeComboAcnt()
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


void SmAddUsdSystemDlg::OnCbnSelchangeComboSymbol()
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


void SmAddUsdSystemDlg::OnBnClickedBtnFindSymbol()
{
	_SymbolSelecter = std::make_shared<HdSymbolSelecter>();
	_SymbolSelecter->set_source_window_id(id_);
	_SymbolSelecter->Create(IDD_SYMBOL_SELECTER_HD, this);
	_SymbolSelecter->ShowWindow(SW_SHOW);
}


void SmAddUsdSystemDlg::OnCbnSelchangeComboSignal()
{
	int selIndex = combo_usd_strategy_.GetCurSel();
	if (selIndex == -1) {
		return;
	}
	auto it = combo_usd_strategy_map_.find(selIndex);
	if (it != combo_usd_strategy_map_.end()) {
		strategy_type_ = it->second;

		set_strategy_type();
	}
}


void SmAddUsdSystemDlg::OnBnClickedBtnOk()
{
	CBCGPDialog::OnOK();
}


void SmAddUsdSystemDlg::OnBnClickedBtnCancel()
{
	CBCGPDialog::OnCancel();
}


BOOL SmAddUsdSystemDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	_ComboType.AddString(_T("계좌"));
	_ComboType.AddString(_T("펀드"));
	_Mode = 0;
	_ComboType.SetCurSel(0);
	_EditSeungsu.SetWindowText(_T("1"));
	_SpinSeungsu.SetRange(0, 100);
	_ComboAcnt.SetDroppedWidth(200);

	_EntGrid.Type(0);
	_EntGrid.AttachGrid(this, IDC_STATIC_ENT_GRID);
	_LiqGrid.Type(1);
	_LiqGrid.AttachGrid(this, IDC_STATIC_LIQ_GRID);

	_EditEntMax.SetWindowText("0");

	InitCombo();
	InitUsdStrategyCombo();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SmAddUsdSystemDlg::InitCombo()
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

void SmAddUsdSystemDlg::InitUsdStrategyCombo()
{
	auto usd_strategy_vector = mainApp.out_system_manager()->get_usd_strategy_vec();
	int selIndex = -1;
	combo_usd_strategy_map_.clear();
	for (auto it = usd_strategy_vector.begin(); it != usd_strategy_vector.end(); ++it) {
		selIndex = combo_usd_strategy_.AddString((*it).c_str());
		combo_usd_strategy_map_[selIndex] = *it;
	}
	if (!combo_usd_strategy_map_.empty()) {
		strategy_type_ = combo_usd_strategy_map_.begin()->second;
		combo_usd_strategy_.SetCurSel(0);
		set_strategy_type();
	}
}

void SmAddUsdSystemDlg::set_strategy_type()
{
	const int cur_sel = combo_usd_strategy_.GetCurSel();
	if (cur_sel < 0) return;
	CString str_usd_strategy;
	combo_usd_strategy_.GetLBText(cur_sel, str_usd_strategy);

	DarkHorse::SmUsdStrategy usd_strategy = mainApp.out_system_manager()->get_usd_strategy(static_cast<const char *>(str_usd_strategy));

	strategy_ = usd_strategy;

	_EntGrid.ClearArgMap();
	_LiqGrid.ClearArgMap();
	//_EntGrid.System(_System);
	//_LiqGrid.System(_System);
	const std::vector<DarkHorse::GroupArg>& argGrpVec = usd_strategy.group_args;
	for (auto it = argGrpVec.begin(); it != argGrpVec.end(); ++it) {
		const GroupArg& argGrp = *it;
		if (argGrp.name.compare(_T("매수진입")) == 0 ||
			argGrp.name.compare(_T("매도진입")) == 0) {
			_EntGrid.SetArg(argGrp);
		}

		if (argGrp.name.compare(_T("매수청산")) == 0 ||
			argGrp.name.compare(_T("매도청산")) == 0) {
			_LiqGrid.SetArg(argGrp);
		}
	}

}

void SmAddUsdSystemDlg::set_symbol_from_out(const int window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (window_id != id_ || !symbol) return;
	symbol_ = symbol;
	int index = _ComboSymbol.AddString(symbol->SymbolCode().c_str());
	combo_to_symbol_map_[index] = symbol;
	_ComboSymbol.SetCurSel(index);
	if (_SymbolSelecter) _SymbolSelecter->SendMessage(WM_CLOSE);
}



void SmAddUsdSystemDlg::OnBnClickedBtnAdd()
{
	if (_Mode == 0 && !account_) { AfxMessageBox("계좌가 없습니다. 계좌를 선택하세요!"); return; }
	if (_Mode == 1 && !fund_) { AfxMessageBox("펀드가 없습니다. 펀드를 선택하세요!"); return; }
	if (!symbol_) { AfxMessageBox("종목이 없습니다. 종목을 선택하세요!"); return; }
	if (strategy_type_.empty()) { AfxMessageBox("전략이 없습니다. 전략을 선택하세요!"); return; }

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
	const std::string usd_system_name = mainApp.out_system_manager()->get_usd_strategy_name();
	std::shared_ptr<DarkHorse::SmUsdSystem> usd_system = mainApp.out_system_manager()->create_usd_system
	(
		usd_system_name,
		_ttoi(strSeungSu),
		order_type,
		_Mode == 0 ? account_ : nullptr,
		_Mode == 1 ? fund_ : nullptr,
		symbol_
	);


	CTime esTime;
	_DpEntBegin.GetTime(esTime);
	VtTime startTime;
	startTime.hour = esTime.GetHour();
	startTime.min = esTime.GetMinute();
	startTime.sec = esTime.GetSecond();

	usd_system->start_time_begin(startTime);

	_DpEntEnd.GetTime(esTime);
	VtTime endTime;//
	endTime.hour = esTime.GetHour();
	endTime.min = esTime.GetMinute();
	endTime.sec = esTime.GetSecond();

	usd_system->start_time_end(endTime);

	_DpLiq.GetTime(esTime);
	VtTime ligTime; //
	ligTime.hour = esTime.GetHour();
	ligTime.min = esTime.GetMinute();
	ligTime.sec = esTime.GetSecond();

	usd_system->end_time(ligTime);

	CString value;
	_EditEntMax.GetWindowText(value);
	usd_system->order_limit_count(_ttoi(value));


	CUGCell cell;
	// 매수 진입
	for (size_t i = 0; i < 4; ++i) {
		CString strValue;
		_EntGrid.QuickGetText(2, i, &strValue);
		strategy_.group_args[0].sys_args[i].param = (LPCTSTR)strValue;
		_EntGrid.GetCell(1, i, &cell);
		double num = cell.GetNumber();
		num == 1.0 ? strategy_.group_args[0].sys_args[i].enable = true : strategy_.group_args[0].sys_args[i].enable = false;
	}
	// 매도 진입
	for (size_t i = 0; i < 4; ++i) {
		CString strValue;
		_EntGrid.QuickGetText(7, i, &strValue);
		strategy_.group_args[1].sys_args[i].param = (LPCTSTR)strValue;
		_EntGrid.GetCell(6, i, &cell);
		double num = cell.GetNumber();
		num == 1.0 ? strategy_.group_args[1].sys_args[i].enable = true : strategy_.group_args[1].sys_args[i].enable = false;
	}
	// 매수 청산
	for (size_t i = 0; i < 4; ++i) {
		CString strValue;
		_EntGrid.QuickGetText(2, i, &strValue);
		strategy_.group_args[2].sys_args[i].param = (LPCTSTR)strValue;
		_EntGrid.GetCell(1, i, &cell);
		double num = cell.GetNumber();
		num == 1.0 ? strategy_.group_args[2].sys_args[i].enable = true : strategy_.group_args[2].sys_args[i].enable = false;
	}
	// 매도 청산
	for (size_t i = 0; i < 4; ++i) {
		CString strValue;
		_EntGrid.QuickGetText(7, i, &strValue);
		strategy_.group_args[3].sys_args[i].param = (LPCTSTR)strValue;
		_EntGrid.GetCell(6, i, &cell);
		double num = cell.GetNumber();
		num == 1.0 ? strategy_.group_args[3].sys_args[i].enable = true : strategy_.group_args[3].sys_args[i].enable = false;
	}

	

	usd_system->strategy(strategy_);

	if (auto_connect_dialog_)
		auto_connect_dialog_->add_usd_system(usd_system);


 	
}
