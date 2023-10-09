// SmTotalAssetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DarkHorse.h"
#include "SmTotalAssetDialog.h"
#include "afxdialogex.h"
#include <unordered_map>
#include "Account/SmAccount.h"
#include "Global/SmTotalManager.h"
#include "Account/SmAccountManager.h"
#include "MainFrm.h"
#include "Task/SmTaskArg.h"


// SmTotalAssetDialog dialog

IMPLEMENT_DYNAMIC(SmTotalAssetDialog, CBCGPDialog)

SmTotalAssetDialog::SmTotalAssetDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_TOTAL_ASSET, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmTotalAssetDialog::~SmTotalAssetDialog()
{
}

void SmTotalAssetDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ASSET, _AssetArea);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
}


BEGIN_MESSAGE_MAP(SmTotalAssetDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_GET, &SmTotalAssetDialog::OnBnClickedBtnGet)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &SmTotalAssetDialog::OnCbnSelchangeComboAccount)
END_MESSAGE_MAP()


// SmTotalAssetDialog message handlers


void SmTotalAssetDialog::SetAccount()
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
		_AssetArea.Account(_ComboAccountMap[_CurrentAccountIndex]);
	}
}

BOOL SmTotalAssetDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	SetAccount();
	_AssetArea.SetUp();
	_AssetArea.SetAssetInfo();
	_AssetArea.Invalidate();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmTotalAssetDialog::OnBnClickedBtnGet()
{
	
}


void SmTotalAssetDialog::OnCbnSelchangeComboAccount()
{
	const int cur_sel = _ComboAccount.GetCurSel();
	if (cur_sel < 0) return;

	_CurrentAccountIndex = cur_sel;
	_AssetArea.Account(_ComboAccountMap[_CurrentAccountIndex]);

	_AssetArea.SetAssetInfo();
	_AssetArea.Invalidate();
}


void SmTotalAssetDialog::PostNcDestroy()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->RemoveAssetWnd(GetSafeHwnd());

	CBCGPDialog::PostNcDestroy();
}
