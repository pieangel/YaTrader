// SmFundDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmFundDialog.h"
#include "afxdialogex.h"
#include "SmFundEditDialog.h"
#include "SmFundNewDialog.h"
#include "../Global/SmTotalManager.h"
#include "SmFundManager.h"
#include "SmFund.h"
#include "../Account/SmAccount.h"
#include <CommCtrl.h>
#include <vector>
#include "../resource.h"
#include "../Account/SmAccountManager.h"
#include "../Event/SmCallbackManager.h"


#define WND_ID4 0x00000004
#define WND_ID7 0x00000007

// SmFundDialog dialog

IMPLEMENT_DYNAMIC(SmFundDialog, CBCGPScrollDialog)

SmFundDialog::SmFundDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPScrollDialog(IDD_FUND, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmFundDialog::~SmFundDialog()
{
}

void SmFundDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPScrollDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_TREE_FUND, _TreeFund);
	//DDX_Control(pDX, IDC_LIST_FUND_ACCOUNT, _ListViewFund);
}


BEGIN_MESSAGE_MAP(SmFundDialog, CBCGPScrollDialog)
	ON_BN_CLICKED(IDC_BTN_FUND_NEW, &SmFundDialog::OnBnClickedBtnFundNew)
	ON_BN_CLICKED(IDC_BTN_FUND_EDIT, &SmFundDialog::OnBnClickedBtnFundEdit)
	ON_BN_CLICKED(IDC_BTN_FUND_DEL, &SmFundDialog::OnBnClickedBtnFundDel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FUND, &SmFundDialog::OnTvnSelchangedTreeFund)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FUND_ACCOUNT, &SmFundDialog::OnLvnItemchangedListFundAccount)
	ON_BN_CLICKED(IDC_BTN_UP, &SmFundDialog::OnBnClickedBtnUp)
	ON_BN_CLICKED(IDC_BTN_DOWN, &SmFundDialog::OnBnClickedBtnDown)
	ON_BN_CLICKED(IDOK, &SmFundDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// SmFundDialog message handlers


void SmFundDialog::OnBnClickedBtnFundNew()
{
	SmFundNewDialog dlg;
	int res = dlg.DoModal();
	if (res == IDOK) {
		std::string fund_name = static_cast<const char*>( dlg.FundName);
		std::string fund_type = dlg.account_type_;
		auto fund_new = mainApp.FundMgr()->FindAddFund(fund_name, fund_type);
		AddFund(fund_new);
	}
}


void SmFundDialog::OnBnClickedBtnFundEdit()
{
	if (!_CurFund) return;

	SmFundEditDialog dlg(this, _CurFund->Id());
	int res = dlg.DoModal();
	if (res == IDOK) {
		SetFundAccount(_CurFund);
		UpdateFundInfo(_CurFund);
	}
}


void SmFundDialog::OnBnClickedBtnFundDel()
{
	if (!_CurFund) return;

	mainApp.FundMgr()->RemoveFund(_CurFund->Name());
	_FundGrid.ClearAll();
	_FundGrid.SetFundList();

	_CurFund = _FundGrid.GetFirstFund();

	if (_CurFund) {
		_FundAccountGrid.InitFund(_CurFund);
		_FundGrid.SelectRow(0);
	}
}


BOOL SmFundDialog::OnInitDialog()
{
	CBCGPScrollDialog::OnInitDialog();

	

	

	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_FUND_ACCOUNT);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	_FundAccountGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID4);
	_FundAccountGrid.StartTimer();


	pWnd = GetDlgItem(IDC_STATIC_FUND);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	_FundGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID7);
	_FundGrid.StartTimer();

	_FundGrid.SetWholeRowSel(TRUE);

	_FundGrid.SetFundDialog(this);
	_FundAccountGrid.SetWholeRowSel(TRUE);


	_FundAccountGrid.FundGrid = &_FundGrid;
	_FundGrid.FundAccountGrid = &_FundAccountGrid;


	_FundGrid.SetFundList();

	_CurFund = _FundGrid.GetFirstFund();

	if (_CurFund) {
		_FundAccountGrid.InitFund(_CurFund);
		_FundGrid.SelectRow(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SmFundDialog::FundClickEvent(const int& row)
{

}

void SmFundDialog::AddFund(std::shared_ptr<DarkHorse::SmFund> fund)
{
	if (!fund) return;

	_FundGrid.SetFundList();
	_CurFund = _FundGrid.GetLastFund();
	SetFundAccount(_CurFund);	
}

void SmFundDialog::UpdateFundInfo(std::shared_ptr<DarkHorse::SmFund> fund)
{
	if (!fund) return;

	_FundGrid.UpdateFundInfo(fund);
}

void SmFundDialog::SetFundList()
{

}




void SmFundDialog::SetFundAccount(std::shared_ptr<DarkHorse::SmFund> fund)
{
	if (!fund) return;

	_FundAccountGrid.InitFund(fund);
}

void SmFundDialog::ClearFundGrid(const int& row)
{

}

void SmFundDialog::ClearFundAccountGrid(const int& row)
{

}

void SmFundDialog::OnTvnSelchangedTreeFund(NMHDR* pNMHDR, LRESULT* pResult)
{
	
}


void SmFundDialog::OnLvnItemchangedListFundAccount(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void SmFundDialog::OnBnClickedBtnUp()
{
	if (!_CurFund) return;

	_FundAccountGrid.MoveUp();

	SetFundAccount(_CurFund);
}


void SmFundDialog::OnBnClickedBtnDown()
{
	if (!_CurFund) return;

	_FundAccountGrid.MoveDown();

	SetFundAccount(_CurFund);
}


void SmFundDialog::OnBnClickedOk()
{
	mainApp.CallbackMgr()->OnFundChanged();

	CBCGPScrollDialog::OnOK();
}
