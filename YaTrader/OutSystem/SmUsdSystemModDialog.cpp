// VtAddConnectSignalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmUsdSystemModDialog.h"
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
// SmUsdSystemModDialog dialog
using namespace DarkHorse;

IMPLEMENT_DYNAMIC(SmUsdSystemModDialog, CBCGPDialog)

SmUsdSystemModDialog::SmUsdSystemModDialog(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_MOD_USD_SYSTEM, pParent), id_(IdGenerator::get_id())
{
}

SmUsdSystemModDialog::SmUsdSystemModDialog(SmUSDSystemDialog* source_dialog)
	: CBCGPDialog(IDD_MOD_USD_SYSTEM), source_dialog_(source_dialog), id_(IdGenerator::get_id())
{
}

SmUsdSystemModDialog::SmUsdSystemModDialog(SmUSDSystemDialog* source_dialog, std::shared_ptr<DarkHorse::SmUsdSystem> usd_system)
	: CBCGPDialog(IDD_MOD_USD_SYSTEM), source_dialog_(source_dialog), id_(IdGenerator::get_id()), usd_system_(usd_system)
{

}

SmUsdSystemModDialog::~SmUsdSystemModDialog()
{
	mainApp.event_hub()->unsubscribe_symbol_event_handler(id_);
}

void SmUsdSystemModDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DP_ENT_BEGIN, _DpEntBegin);
	DDX_Control(pDX, IDC_DP_ENT_END, _DpEntEnd);
	DDX_Control(pDX, IDC_DP_LIQ, _DpLiq);
	DDX_Control(pDX, IDC_EDIT_ENT_MAX, _EditEntMax);

}


BEGIN_MESSAGE_MAP(SmUsdSystemModDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_MOD, &SmUsdSystemModDialog::OnBnClickedBtnMod)
	ON_BN_CLICKED(IDC_BTN_OK, &SmUsdSystemModDialog::OnBnClickedBtnOk)
END_MESSAGE_MAP()


// VtAddConnectSignalDlg message handlers



BOOL SmUsdSystemModDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	_Mode = 0;

	_EntGrid.Type(0);
	_EntGrid.AttachGrid(this, IDC_STATIC_ENT_GRID);
	_LiqGrid.Type(1);
	_LiqGrid.AttachGrid(this, IDC_STATIC_LIQ_GRID);

	_EditEntMax.SetWindowText("0");
	set_usd_system(usd_system_);
	InitCombo();
	InitUsdStrategyCombo();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SmUsdSystemModDialog::InitCombo()
{
	
}

void SmUsdSystemModDialog::InitUsdStrategyCombo()
{

}

void SmUsdSystemModDialog::set_usd_system(std::shared_ptr<DarkHorse::SmUsdSystem> usd_system)
{
	if (!usd_system) return;

	CString orderLimitCount;
	orderLimitCount.Format("%d", usd_system->order_limit_count());
	_EditEntMax.SetWindowText(orderLimitCount);

	COleDateTime NewDate;
	CTime curTime = CTime::GetCurrentTime();
	VtTime esTime = usd_system->start_time_begin();
	NewDate.SetDateTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), esTime.hour, esTime.min, esTime.sec);
	_DpEntBegin.SetTime(NewDate);

	COleDateTime NewDate2;
	esTime = usd_system->start_time_end();
	NewDate2.SetDateTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), esTime.hour, esTime.min, esTime.sec);
	_DpEntEnd.SetTime(NewDate2);

	COleDateTime NewDate3;
	esTime = usd_system->end_time();
	NewDate3.SetDateTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), esTime.hour, esTime.min, esTime.sec);
	_DpLiq.SetTime(NewDate3);


	DarkHorse::SmUsdStrategy usd_strategy = usd_system->strategy();
	_EntGrid.ClearArgMap();
	_LiqGrid.ClearArgMap();

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

void SmUsdSystemModDialog::OnBnClickedBtnMod()
{
	if (usd_system_ == nullptr) return;

	CTime esTime;
	_DpEntBegin.GetTime(esTime);
	VtTime startTime;
	startTime.hour = esTime.GetHour();
	startTime.min = esTime.GetMinute();
	startTime.sec = esTime.GetSecond();

	usd_system_->start_time_begin(startTime);

	CTime esTime2;
	_DpEntEnd.GetTime(esTime2);
	VtTime endTime;//
	endTime.hour = esTime2.GetHour();
	endTime.min = esTime2.GetMinute();
	endTime.sec = esTime2.GetSecond();

	usd_system_->start_time_end(endTime);

	CTime esTime3;
	_DpLiq.GetTime(esTime3);
	VtTime ligTime; //
	ligTime.hour = esTime3.GetHour();
	ligTime.min = esTime3.GetMinute();
	ligTime.sec = esTime3.GetSecond();

	usd_system_->end_time(ligTime);

	CString value;
	_EditEntMax.GetWindowText(value);
	usd_system_->order_limit_count(_ttoi(value));


	DarkHorse::SmUsdStrategy  strategy = usd_system_->strategy();
	CUGCell cell;
	// 매수 진입
	for (size_t i = 0; i < 4; ++i) {
		CString strValue;
		_EntGrid.QuickGetText(2, i, &strValue);
		strategy.group_args[0].sys_args[i].param = (LPCTSTR)strValue;
		_EntGrid.GetCell(1, i, &cell);
		double num = cell.GetNumber();
		num == 1.0 ? strategy.group_args[0].sys_args[i].enable = true : strategy.group_args[0].sys_args[i].enable = false;
	}
	// 매도 진입
	for (size_t i = 0; i < 4; ++i) {
		CString strValue;
		_EntGrid.QuickGetText(7, i, &strValue);
		strategy.group_args[1].sys_args[i].param = (LPCTSTR)strValue;
		_EntGrid.GetCell(6, i, &cell);
		double num = cell.GetNumber();
		num == 1.0 ? strategy.group_args[1].sys_args[i].enable = true : strategy.group_args[1].sys_args[i].enable = false;
	}
	// 매수 청산
	for (size_t i = 0; i < 4; ++i) {
		CString strValue;
		_LiqGrid.QuickGetText(2, i, &strValue);
		strategy.group_args[2].sys_args[i].param = (LPCTSTR)strValue;
		_LiqGrid.GetCell(1, i, &cell);
		double num = cell.GetNumber();
		num == 1.0 ? strategy.group_args[2].sys_args[i].enable = true : strategy.group_args[2].sys_args[i].enable = false;
	}
	// 매도 청산
	for (size_t i = 0; i < 4; ++i) {
		CString strValue;
		_LiqGrid.QuickGetText(7, i, &strValue);
		strategy.group_args[3].sys_args[i].param = (LPCTSTR)strValue;
		_LiqGrid.GetCell(6, i, &cell);
		double num = cell.GetNumber();
		num == 1.0 ? strategy.group_args[3].sys_args[i].enable = true : strategy.group_args[3].sys_args[i].enable = false;
	}

	usd_system_->strategy(strategy);

	if (source_dialog_)
		source_dialog_->update_usd_system(usd_system_);
}


void SmUsdSystemModDialog::OnBnClickedBtnOk()
{
	CBCGPDialog::OnOK();
}
