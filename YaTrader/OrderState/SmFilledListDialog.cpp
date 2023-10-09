// SmFilledListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmFilledListDialog.h"
#include "afxdialogex.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccountManager.h"
#include "../MainFrm.h"

#define WND_ID10 0x00000010

// SmFilledListDialog dialog

IMPLEMENT_DYNAMIC(SmFilledListDialog, CBCGPDialog)

SmFilledListDialog::SmFilledListDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_FILLED_LIST, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmFilledListDialog::~SmFilledListDialog()
{
	KillTimer(1);
}

void SmFilledListDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
	DDX_Control(pDX, IDC_STATIC_LIST, _AcceptedArea);
}


BEGIN_MESSAGE_MAP(SmFilledListDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_RELOAD, &SmFilledListDialog::OnBnClickedBtnReload)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &SmFilledListDialog::OnCbnSelchangeComboAccount)
END_MESSAGE_MAP()


// SmFilledListDialog message handlers


void SmFilledListDialog::SetAccount()
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
		_AcceptedArea.Account(_ComboAccountMap[_CurrentAccountIndex]);
	}
}

void SmFilledListDialog::OnBnClickedBtnReload()
{
	// TODO: Add your control notification handler code here
}


BOOL SmFilledListDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_LIST);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	//_FilledGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID10);
	_AcceptedArea.SetUp();

	SetAccount();


	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
	if (pLayout != NULL && _AcceptedArea.GetSafeHwnd())
	{
		pLayout->AddAnchor(_AcceptedArea.GetSafeHwnd(), CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);
		//pLayout->AddAnchor(IDC_STATIC_LIST, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);
		pLayout->AddAnchor(IDC_BTN_RELOAD, CBCGPStaticLayout::e_MoveTypeVert, CBCGPStaticLayout::e_SizeTypeNone);
		pLayout->AddAnchor(IDOK, CBCGPStaticLayout::e_MoveTypeVert, CBCGPStaticLayout::e_SizeTypeNone);
	}

	SetTimer(1, 40, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmFilledListDialog::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CBCGPDialog::OnClose();
}


void SmFilledListDialog::OnTimer(UINT_PTR nIDEvent)
{
	_Count++;
	if (_Count == 20) {
		_AcceptedArea.OnOrderEvent("", "");
		_Count = 0;
	}
	_AcceptedArea.Update();

	CBCGPDialog::OnTimer(nIDEvent);
}


void SmFilledListDialog::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	//if (!_FilledGrid.GetSafeHwnd()) return;

	CRect rect;
	//CWnd* pWnd = GetDlgItem(IDC_STATIC_LIST);
	//pWnd->GetWindowRect(&rect);
	//ScreenToClient(&rect);
	//_FilledGrid.MoveWindow(rect);
	if (_AcceptedArea.GetSafeHwnd()) {
		_AcceptedArea.RecalSize();
	}
}


void SmFilledListDialog::OnCbnSelchangeComboAccount()
{
	const int cur_sel = _ComboAccount.GetCurSel();
	if (cur_sel < 0) return;
	_CurrentAccountIndex = cur_sel;
	_AcceptedArea.Account(_ComboAccountMap[_CurrentAccountIndex]);
}


void SmFilledListDialog::PostNcDestroy()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->RemoveFilledWnd(GetSafeHwnd());

	CBCGPDialog::PostNcDestroy();
}
