// SmFundEditDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmFundEditDialog.h"
#include "afxdialogex.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Global/SmTotalManager.h"
#include "SmFund.h"
#include "SmFundManager.h"
#include <format>

#define WND_ID5 0x00000005
#define WND_ID6 0x00000006
// SmFundEditDialog dialog
using namespace DarkHorse;
IMPLEMENT_DYNAMIC(SmFundEditDialog, CBCGPScrollDialog)

SmFundEditDialog::SmFundEditDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPScrollDialog(IDD_FUND_EDIT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmFundEditDialog::SmFundEditDialog(CWnd* pParent, const int& fund_id)
	: CBCGPScrollDialog(IDD_FUND_EDIT, pParent)
{
	_FundId = fund_id;

	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmFundEditDialog::~SmFundEditDialog()
{
}

void SmFundEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPScrollDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT_ACNT_SEUNSU, _EditAcntSeungSu);
	DDX_Control(pDX, IDC_EDIT_FUND_NAME, _EditFundName);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT_TYPE, _ComboAccountType);
	//DDX_Control(pDX, IDC_LIST_UNREG_ACCOUNT, _ListUnregAcnt);
	//DDX_Control(pDX, IDC_STATIC_ACNT_DETAIL, _StaticAcntDetail);
	//DDX_Control(pDX, IDC_STATIC_ACNT_RATIO, _StaticAcntRatio);
	//DDX_Control(pDX, IDC_STATIC_ACNT_NAME, _StaticAcntName);
}


BEGIN_MESSAGE_MAP(SmFundEditDialog, CBCGPScrollDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_REG_ACCOUNT, &SmFundEditDialog::OnLvnItemchangedListRegAccount)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_UNREG_ACCOUNT, &SmFundEditDialog::OnLvnItemchangedListUnregAccount)
	ON_BN_CLICKED(IDC_BTN_IN, &SmFundEditDialog::OnBnClickedBtnIn)
	ON_BN_CLICKED(IDC_BTN_IN_ALL, &SmFundEditDialog::OnBnClickedBtnInAll)
	ON_BN_CLICKED(IDC_BTN_OUT, &SmFundEditDialog::OnBnClickedBtnOut)
	ON_BN_CLICKED(IDC_BTN_OUT_ALL, &SmFundEditDialog::OnBnClickedBtnOutAll)
	ON_EN_CHANGE(IDC_EDIT_FUND_NAME, &SmFundEditDialog::OnEnChangeEditFundName)
	ON_BN_CLICKED(IDOK, &SmFundEditDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &SmFundEditDialog::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT_ACNT_SEUNSU, &SmFundEditDialog::OnEnChangeEditAcntSeunsu)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT_TYPE, &SmFundEditDialog::OnCbnSelchangeComboAccountType)
END_MESSAGE_MAP()


// SmFundEditDialog message handlers


void SmFundEditDialog::OnLvnItemchangedListRegAccount(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void SmFundEditDialog::OnLvnItemchangedListUnregAccount(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void SmFundEditDialog::OnClose()
{
	CString fund_name;
	_EditFundName.GetWindowText(fund_name);

	auto fund = mainApp.FundMgr()->FindFund(static_cast<const char*>(fund_name));
	if (fund && fund != _CurFund) {
		AfxMessageBox("같은 이름이 펀드가 있습니다. 다른 이름을 사용해 주세요!");
		return;
	}

	CString strText;
	_EditFundName.GetWindowText(strText);

	_CurFund->Name(static_cast<const char*>(strText));
	CBCGPScrollDialog::OnClose();
}

void SmFundEditDialog::UnregAccountDoubleClickEvent(const int& row)
{
	auto found = _RowToAccountMap.find(row);
	if (found == _RowToAccountMap.end()) return;
	
	_CurFund->AddAccount(found->second);

	

	//InitFund(_CurFund);
	//_EditFundName.SetWindowText(_CurFund->Name().c_str());
	_FundAccountGrid.InitFund(_CurFund);
	//RefreshUnregAccounts();
	_AccountGrid.InitUnregAccount(account_type_);
}

void SmFundEditDialog::UnregAccountDoubleClickEvent(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;

	_CurFund->AddAccount(account);
	_FundAccountGrid.InitFund(_CurFund);
	_AccountGrid.InitUnregAccount(account_type_);
}

void SmFundEditDialog::UnregAccountClickEvent(const int& row)
{

}

void SmFundEditDialog::UnregAccountClickEvent(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;
}

void SmFundEditDialog::FundAccountDoubleClickEvent(const int& row)
{
	auto found = _RowToFundAccountMap.find(row);
	if (found == _RowToFundAccountMap.end()) return;
	
	_CurFund->RemoveAccount(found->second->No());

	

	//InitFund(_CurFund);
	//RefreshUnregAccounts();

	//InitFund(_CurFund);
	_EditFundName.SetWindowText(_CurFund->Name().c_str());
	_FundAccountGrid.InitFund(_CurFund);
	//RefreshUnregAccounts();
	_AccountGrid.InitUnregAccount(account_type_);
}

void SmFundEditDialog::FundAccountDoubleClickEvent(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;

	_CurFund->RemoveAccount(account->No());



	//InitFund(_CurFund);
	//RefreshUnregAccounts();

	//InitFund(_CurFund);
	//_EditFundName.SetWindowText(_CurFund->Name().c_str());
	_FundAccountGrid.InitFund(_CurFund);
	//RefreshUnregAccounts();
	_AccountGrid.InitUnregAccount(account_type_);
}

void SmFundEditDialog::FundAccountClickEvent(const int& row)
{
	
}

void SmFundEditDialog::FundAccountClickEvent(std::shared_ptr<DarkHorse::SmAccount> account)
{

}

void SmFundEditDialog::RemoveFromFund(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;
	if (!_CurFund) return;
	
	_CurFund->RemoveAccount(account->No());
	//InitFund(_CurFund);
	_EditFundName.SetWindowText(_CurFund->Name().c_str());
	_FundAccountGrid.InitFund(_CurFund);
}

void SmFundEditDialog::AddToFund(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;
	if (!_CurFund) return;
	
	_CurFund->AddAccount(account);
	//InitFund(_CurFund);
	_EditFundName.SetWindowText(_CurFund->Name().c_str());
	_FundAccountGrid.InitFund(_CurFund);
}



void SmFundEditDialog::RefreshUnregAccounts()
{
	//InitUnregAccount();
	_AccountGrid.InitUnregAccount(account_type_);
}


BOOL SmFundEditDialog::OnInitDialog()
{
	CBCGPScrollDialog::OnInitDialog();


	_CurFund = mainApp.FundMgr()->FindFundById(_FundId);

	_ComboAccountType.SetCurSel(0);

	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_FUND_ACCOUNT);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	_FundAccountGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID5);
	//_FundAccountGrid.StartTimer();


	pWnd = GetDlgItem(IDC_STATIC_ACCOUNT);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	_AccountGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID6);
	
	_AccountGrid.Account_type(account_type_);
	_FundAccountGrid.Account_type(account_type_);
	_FundAccountGrid.AccountGrid = &_AccountGrid;
	_AccountGrid.FundAccountGrid = &_FundAccountGrid;
	_AccountGrid.Fund(_CurFund);
	_FundAccountGrid.Fund(_CurFund);

	_FundAccountGrid.SetFundEditDialog(this);
	_AccountGrid.SetFundEditDialog(this);


	_EditFundName.SetWindowText(_CurFund->Name().c_str());
	_FundAccountGrid.InitFund();
	_AccountGrid.InitUnregAccount(account_type_);

	// 취소를 위해 백업을 받아 놓는다.
	for (auto it = _RowToFundAccountMap.begin(); it != _RowToFundAccountMap.end(); ++it) {
		_FundAccountsBackup.push_back(it->second);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmFundEditDialog::OnBnClickedBtnIn()
{
	_AccountGrid.MoveSelectedAccounts();
}


void SmFundEditDialog::OnBnClickedBtnInAll()
{
	_AccountGrid.MoveAllAccounts();
}


void SmFundEditDialog::OnBnClickedBtnOut()
{
	_FundAccountGrid.MoveSelectedAccounts();
}


void SmFundEditDialog::OnBnClickedBtnOutAll()
{
	_FundAccountGrid.MoveAllAccounts();
}


void SmFundEditDialog::OnEnChangeEditFundName()
{
	if (!_CurFund) return;
	if (!_EditFundName.GetSafeHwnd()) return;

	CString strText;
	_EditFundName.GetWindowText(strText);

	_CurFund->Name(static_cast<const char*>(strText));
}


void SmFundEditDialog::OnBnClickedOk()
{
	CString fund_name;
	_EditFundName.GetWindowText(fund_name);

	auto fund = mainApp.FundMgr()->FindFund(static_cast<const char*>(fund_name));
	if (fund && fund != _CurFund) {
		AfxMessageBox("같은 이름이 펀드가 있습니다. 다른 이름을 사용해 주세요!");
		return;
	}

	CString strText;
	_EditFundName.GetWindowText(strText);

	_CurFund->Name(static_cast<const char*>(strText));

	CBCGPDialog::OnOK();
}


void SmFundEditDialog::OnBnClickedCancel()
{
	CBCGPDialog::OnCancel();
}


void SmFundEditDialog::OnEnChangeEditAcntSeunsu()
{
	
}



void SmFundEditDialog::OnCbnSelchangeComboAccountType()
{
	if (_ComboAccountType.GetCurSel() < 0) return;
	int index = _ComboAccountType.GetCurSel();
	if (index == 0)
		account_type_ = "9";
	else
		account_type_ = "1";

	_AccountGrid.Account_type(account_type_);
	_FundAccountGrid.Account_type(account_type_);
	_AccountGrid.InitUnregAccount(account_type_);
	_FundAccountGrid.InitFund();
}
