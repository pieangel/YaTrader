// SmAccountPwdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmAccountPwdDlg.h"
#include "afxdialogex.h"
#include "../Grid/customcells.h"
#include "SmAccount.h"
#include "SmAccountManager.h"
#include "../Global/SmTotalManager.h"
#include <unordered_map>
#include <memory>
#include "../Event/SmCallbackManager.h"
#include <BCGCBProInc.h>
#include "../MessageDefine.h"
#include "../Task/SmTaskRequestMaker.h"
#include "../Client/ViClient.h"
#include "../Archieve/SmSaveManager.h"
#include "../MainFrm.h"
#include "../Yuanta/YaStockClient.h"
#include "../Task/YaServerDataReceiver.h"
using namespace DarkHorse;

// SmAccountPwdDlg dialog

IMPLEMENT_DYNAMIC(SmAccountPwdDlg, CBCGPDialog)

SmAccountPwdDlg::SmAccountPwdDlg(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ACCOUNT_PWD, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);

	
}

SmAccountPwdDlg::~SmAccountPwdDlg()
{
	mainApp.CallbackMgr()->UnsubscribePasswordCallback(GetSafeHwnd());
	//KillTimer(1);
}

void SmAccountPwdDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SmAccountPwdDlg, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_SAVE, &SmAccountPwdDlg::OnBtnConfirmAcntPwd)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &SmAccountPwdDlg::OnBnClickedBtnClose)
	ON_MESSAGE(WM_PASSWORD_CONFIRMED, &SmAccountPwdDlg::OnUmPasswordConfirmed)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// SmAccountPwdDlg message handlers


void SmAccountPwdDlg::SavePassword()
{
	for (auto it = _RowToAccountMap.begin(); it != _RowToAccountMap.end(); ++it) {
		const int row = it->first;
		CBCGPGridRow* pRow = m_wndGrid.GetRow(row);
		CString strValue = pRow->GetItem(2)->GetValue();
		it->second->Pwd(static_cast<const char*>(strValue));
	}

	mainApp.SaveMgr()->SaveAccountPasswords(_RowToAccountMap);
}

void SmAccountPwdDlg::handle_account_password_error()
{
	if (IDYES == AfxMessageBox("계좌비밀번호가 일치하지 않습니다.\n5회오류시 고객센터로 문의바랍니다.\n계속 진행하시겠습니까?", MB_YESNO)){
		//AfxMessageBox("OK선택");
		SetTimer(1, 700, NULL);
	}
	else if (IDNO) {
		//AfxMessageBox("NO선택");
		//EndDialog(IDOK);
		//CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		//pFrame->SendMessage(WM_CLOSE, 0, 0);
		KillTimer(1);
	}
}

BOOL SmAccountPwdDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableLayout();
	
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_PWD);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	m_wndGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, IDC_STATIC_PWD);


	m_wndGrid.EnableMarkSortedColumn(FALSE);
	m_wndGrid.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);
	m_wndGrid.EnableRowHeader(TRUE);
	//m_wndGrid.EnableLineNumbers();
	m_wndGrid.SetClearInplaceEditOnEnter(FALSE);
	m_wndGrid.EnableInvertSelOnCtrl();
	m_wndGrid.SetWholeRowSel(FALSE);
	
	m_wndGrid.SetRowHeaderWidth(1);
	m_wndGrid.SetVisualManagerColorTheme(TRUE);
	
	// Insert columns:
	m_wndGrid.InsertColumn(0, _T("계좌명"), 80);
	m_wndGrid.InsertColumn(1, _T("계좌번호"), 80);
	m_wndGrid.InsertColumn(2, _T("비밀번호"), 80);
	m_wndGrid.InsertColumn(3, _T("확인적용"), rect.Width() - 246);

	m_wndGrid.FreezeColumns(0);
	
	std::vector<std::shared_ptr<SmAccount>> main_acnt_vector;
	const int nColumns = m_wndGrid.GetColumnCount();

	mainApp.AcntMgr()->get_main_account_vector(main_acnt_vector);

	mainApp.SaveMgr()->LoadAccountPasswords();

	int row = 0;
	for (auto it = main_acnt_vector.begin(); it != main_acnt_vector.end(); it++) {
		auto account = *it;
		if (account->is_subaccount()) continue;
		// Create new row:
		CBCGPGridRow* pRow = m_wndGrid.CreateRow(nColumns);
		// Set each column data:
		for (int nColumn = 0; nColumn < nColumns; nColumn++)
		{
			
			pRow->GetItem(0)->SetValue(account->Name().c_str(), TRUE);
			pRow->GetItem(1)->SetValue(account->No().c_str(), TRUE);
			
			if (nColumn == 2)
				pRow->ReplaceItem(nColumn, new CPasswordItem(account->Pwd().c_str()));
			if (nColumn == 0 || nColumn == 1)
				pRow->GetItem(nColumn)->AllowEdit(FALSE);

			_RowToAccountMap[row] = account;
		}

		// Add row to grid:
		m_wndGrid.AddRow(pRow, FALSE /* Don't recal. layout */);

		row++;
	}
	
	m_wndGrid.EnableCheckBoxes(TRUE);
	m_wndGrid.SetHeaderAlign(0, 2);
	m_wndGrid.SetHeaderAlign(1, 2);
	m_wndGrid.SetHeaderAlign(2, 2);
	m_wndGrid.SetHeaderAlign(3, 2);
	//m_wndGrid.SetCheckBoxState(BST_CHECKED);

	m_wndGrid.SetColumnAlign(0, 2);
	m_wndGrid.SetColumnAlign(1, 2);
	m_wndGrid.SetColumnAlign(2, 2);
	m_wndGrid.SetColumnAlign(3, 2);
	
	m_wndGrid.CheckAll(TRUE);
	m_wndGrid.UpdateHeaderCheckbox();
	m_wndGrid.AdjustLayout();
	
	mainApp.CallbackMgr()->SubscribePasswordCallback(GetSafeHwnd());


	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
// fe, 2f, 19

void SmAccountPwdDlg::OnBtnConfirmAcntPwd()
{
	for (auto it = _RowToAccountMap.begin(); it != _RowToAccountMap.end(); ++it) {
		const int row = it->first;
		CBCGPGridRow* pRow = m_wndGrid.GetRow(row);
		CString strValue = pRow->GetItem(1)->GetValue();
		
		std::string account_no, pwd;
		account_no = static_cast<const char*>(strValue);
		strValue = pRow->GetItem(2)->GetValue();
		pwd = static_cast<const char*>(strValue);
		_ReqQ.push_back(std::make_pair(account_no, pwd));

		std::shared_ptr<SmAccount> account = mainApp.AcntMgr()->FindAccount(account_no);
		if (!account) continue;
		account->Pwd(pwd);
	}
	
	SetTimer(1, 700, NULL);
}


void SmAccountPwdDlg::OnBnClickedBtnClose()
{
	for (auto it = _RowToAccountMap.begin(); it != _RowToAccountMap.end(); ++it) {
		const int row = it->first;
		CBCGPGridRow* pRow = m_wndGrid.GetRow(row);
		CString strValue = pRow->GetItem(3)->GetValue();

		if (strValue.GetLength() == 0 || strValue == "X") {
			AfxMessageBox("계좌 비밀번호를 확인하여 주십시오!");
			return;
		}
	}

	SavePassword();
	CBCGPDialog::EndDialog(IDOK);
}

LRESULT SmAccountPwdDlg::OnUmPasswordConfirmed(WPARAM wParam, LPARAM lParam)
{
	const int account_id = static_cast<int>(wParam);
	const int result = static_cast<int>(lParam);
	for (auto it = _RowToAccountMap.begin(); it != _RowToAccountMap.end(); ++it) {
		auto account = it->second;
		if (account->id() == account_id) {
			CBCGPGridRow* pRow = m_wndGrid.GetRow(it->first);
			if (result == 1)
				pRow->GetItem(3)->SetValue("O");
			else {
				pRow->GetItem(3)->SetValue("X");
				KillTimer(1);
				handle_account_password_error();
			}
		}
	}
	return 1;
}


void SmAccountPwdDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (_ReqQ.empty()) { KillTimer(1); }
	else {
		auto account_pwd = _ReqQ.front();

		std::shared_ptr<SmAccount> account = mainApp.AcntMgr()->FindAccount(account_pwd.first);
		if (!account) return;
				
		DhTaskArg arg;
		arg.detail_task_description = account->No();
		arg.argument_id = YaServerDataReceiver::get_argument_id();
		arg.task_type = DhTaskType::DmAccountAsset;
		arg.parameter_map["account_no"] = account->No();
		arg.parameter_map["password"] = account->Pwd();
		mainApp.Client()->confirm_account_password(arg);
		_ReqQ.pop_front();
	}
	CBCGPDialog::OnTimer(nIDEvent);
}


void SmAccountPwdDlg::OnDestroy()
{
	CBCGPDialog::OnDestroy();

	KillTimer(1);
}
