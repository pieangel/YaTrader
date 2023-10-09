// TotalAssetProfitLossDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "TotalAssetProfitLossDialog.h"
#include "afxdialogex.h"
#include <unordered_map>
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccountManager.h"
#include "../MainFrm.h"
#include "../Task/SmTaskArg.h"
#include "../Global/SmTotalManager.h"
#include "../Task/SmTaskRequestManager.h"

// TotalAssetProfitLossDialog dialog

IMPLEMENT_DYNAMIC(TotalAssetProfitLossDialog, CBCGPDialog)

TotalAssetProfitLossDialog::TotalAssetProfitLossDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_TOTAL_ASSET, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

TotalAssetProfitLossDialog::TotalAssetProfitLossDialog(CWnd* pParent, const std::string& type, const std::string& account_no)
	: CBCGPDialog(IDD_TOTAL_ASSET, pParent), type_(type), account_no_(account_no)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}


TotalAssetProfitLossDialog::~TotalAssetProfitLossDialog()
{
}

void TotalAssetProfitLossDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ASSET, total_asset_profit_loss_view_);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
}


BEGIN_MESSAGE_MAP(TotalAssetProfitLossDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_GET, &TotalAssetProfitLossDialog::OnBnClickedBtnGet)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &TotalAssetProfitLossDialog::OnCbnSelchangeComboAccount)
END_MESSAGE_MAP()


// TotalAssetProfitLossDialog message handlers


void TotalAssetProfitLossDialog::SetAccount()
{
	std::vector<std::shared_ptr<DarkHorse::SmAccount>> account_vector;
	mainApp.AcntMgr()->get_main_account_vector(account_vector);
	int selected_index = 0;
	for (auto it = account_vector.begin(); it != account_vector.end(); ++it) {
		auto account = *it;
		if (account->is_subaccount()) continue;
		std::string account_info;
		if (account->Type() == "9")
			account_info.append("[국내]");
		else
			account_info.append("[해외]");
		account_info.append(account->Name());
		account_info.append(" : ");
		account_info.append(account->No());
		const int index = _ComboAccount.AddString(account_info.c_str());
		_ComboAccountMap[index] = account;
		if (account_no_ == account->No()) {
			selected_index = index;
		}
	}

	if (!_ComboAccountMap.empty()) {
		_CurrentAccountIndex = selected_index;
		_ComboAccount.SetCurSel(_CurrentAccountIndex);
		total_asset_profit_loss_view_.Account(_ComboAccountMap[_CurrentAccountIndex]);
	}
}

BOOL TotalAssetProfitLossDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	SetAccount();
	total_asset_profit_loss_view_.SetUp();
	total_asset_profit_loss_view_.SetAssetInfo();
	total_asset_profit_loss_view_.Invalidate();
	_ComboAccount.SetDroppedWidth(250);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void TotalAssetProfitLossDialog::OnBnClickedBtnGet()
{
	if (_CurrentAccountIndex < 0) return;
	auto account = _ComboAccountMap[_CurrentAccountIndex];

	DhTaskArg arg;
	arg.detail_task_description = account->No();
	arg.task_type = DhTaskType::AccountProfitLoss;
	arg.parameter_map["account_no"] = account->No();
	arg.parameter_map["password"] = account->Pwd();
	arg.parameter_map["account_type"] = account->Type();

	mainApp.TaskReqMgr()->AddTask(std::move(arg));
}


void TotalAssetProfitLossDialog::OnCbnSelchangeComboAccount()
{
	const int cur_sel = _ComboAccount.GetCurSel();
	if (cur_sel < 0) return;

	_CurrentAccountIndex = cur_sel;
	auto account = _ComboAccountMap[_CurrentAccountIndex];
	if (!account) return;
	account_no_ = account->No();
	type_ = account->Type();
	total_asset_profit_loss_view_.Account(account);

	total_asset_profit_loss_view_.SetAssetInfo();
	total_asset_profit_loss_view_.Invalidate();
	DhTaskArg arg;
	arg.detail_task_description = account->No();
	arg.task_type = DhTaskType::AccountProfitLoss;
	arg.parameter_map["account_no"] = account->No();
	arg.parameter_map["password"] = account->Pwd();
	arg.parameter_map["account_type"] = account->Type();

	mainApp.TaskReqMgr()->AddTask(std::move(arg));
}


void TotalAssetProfitLossDialog::PostNcDestroy()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->RemoveAssetWnd(GetSafeHwnd());

	CBCGPDialog::PostNcDestroy();
}
