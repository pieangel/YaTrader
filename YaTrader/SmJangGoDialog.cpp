// SmJangGoDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DarkHorse.h"
#include "SmJangGoDialog.h"
#include "afxdialogex.h"
#include <unordered_map>

#include "Account/SmAccount.h"
#include "Global/SmTotalManager.h"
#include "Account/SmAccountManager.h"
#include "Fund/SmFundManager.h"
#include "Fund/SmFund.h"
#include "MainFrm.h"
// SmJangGoDialog dialog
using namespace DarkHorse;
IMPLEMENT_DYNAMIC(SmJangGoDialog, CBCGPDialog)

SmJangGoDialog::SmJangGoDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_JANGO, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmJangGoDialog::~SmJangGoDialog()
{
}

void SmJangGoDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, _StaticAccount);
	DDX_Control(pDX, IDC_STATIC_COMBO, _StaticCombo);
	DDX_Control(pDX, IDC_STATIC_POSITION, _StaticPosition);
}


BEGIN_MESSAGE_MAP(SmJangGoDialog, CBCGPDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &SmJangGoDialog::OnCbnSelchangeComboAccount)
END_MESSAGE_MAP()


// SmJangGoDialog message handlers


BOOL SmJangGoDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	_StaticAccount.Mode(_Mode);
	_StaticAccount.SetUp();
	_StaticPosition.SetUp();
	_StaticAccount.Mode(_Mode);
	_StaticPosition.Mode(_Mode);
	_Mode == 0 ? SetAccount() : SetFund();
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SmJangGoDialog::SetAccount()
{
	
	const std::unordered_map<std::string, std::shared_ptr<DarkHorse::SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
	for (auto it = account_map.begin(); it != account_map.end(); ++it) {
		auto account = it->second;
		std::string account_info;
		account_info.append(account->Name());
		account_info.append(" : ");
		account_info.append(account->No());
		const int index = _ComboAccount.AddString(account_info.c_str());
		_ComboAccountMap[index] = account;

	}

	if (!_ComboAccountMap.empty()) {
		_CurrentAccountIndex = 0;
		_ComboAccount.SetCurSel(_CurrentAccountIndex);
		_StaticAccount.Account(_ComboAccountMap[_CurrentAccountIndex]);
		_StaticPosition.Account(_ComboAccountMap[_CurrentAccountIndex]);
		_StaticAccount.UpdateAccountAssetInfo();
		_StaticPosition.UpdateAccountPositionInfo();
	}
}


void SmJangGoDialog::SetFund()
{
	const std::map<std::string, std::shared_ptr<SmFund>>& fund_map = mainApp.FundMgr()->GetFundMap();

	for (auto it = fund_map.begin(); it != fund_map.end(); ++it) {
		auto fund = it->second;
		std::string account_info;
		account_info.append(fund->Name());
		const int index = _ComboAccount.AddString(account_info.c_str());
		_ComboFundMap[index] = fund;

	}

	if (!_ComboFundMap.empty()) {
		_CurrentAccountIndex = 0;
		_ComboAccount.SetCurSel(_CurrentAccountIndex);
		_StaticAccount.Fund(_ComboFundMap[_CurrentAccountIndex]);
		_StaticPosition.Fund(_ComboFundMap[_CurrentAccountIndex]);
		_StaticAccount.UpdateFundAssetInfo();
		_StaticPosition.UpdateFundPositionInfo();
	}
}

void SmJangGoDialog::OnCbnSelchangeComboAccount()
{
	const int cur_sel = _ComboAccount.GetCurSel();
	if (cur_sel < 0) return;

	_CurrentAccountIndex = cur_sel;
	if (_Mode == 0) {
		_StaticAccount.Account(_ComboAccountMap[_CurrentAccountIndex]);
		_StaticPosition.Account(_ComboAccountMap[_CurrentAccountIndex]);
		_StaticAccount.UpdateAccountAssetInfo();
		_StaticPosition.UpdateAccountPositionInfo();
		_StaticAccount.Invalidate();
		_StaticPosition.Invalidate();
	}
	else {
		_StaticAccount.Fund(_ComboFundMap[_CurrentAccountIndex]);
		_StaticPosition.Fund(_ComboFundMap[_CurrentAccountIndex]);
		_StaticAccount.UpdateFundAssetInfo();
		_StaticPosition.UpdateFundPositionInfo();
		_StaticAccount.Invalidate();
		_StaticPosition.Invalidate();
	}
}


void SmJangGoDialog::PostNcDestroy()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->RemoveJangoWnd(GetSafeHwnd());

	CBCGPDialog::PostNcDestroy();
}
