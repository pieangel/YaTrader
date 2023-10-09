// SubAccountEditor.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "afxdialogex.h"
#include "SubAccountEditor.h"
#include "SmAccount.h"
#include "SmAccountManager.h"
#include "../Global/SmTotalManager.h"
#include "../Archieve/SmSaveManager.h"

#define WND_ID8 0x00000008
#define WND_ID9 0x00000009
// SubAccountEditor dialog

IMPLEMENT_DYNAMIC(SubAccountEditor, CBCGPScrollDialog)

SubAccountEditor::SubAccountEditor(CWnd* pParent /*=nullptr*/)
	: CBCGPScrollDialog(IDD_SUB_ACCOUNT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SubAccountEditor::~SubAccountEditor()
{
}

BOOL SubAccountEditor::OnInitDialog()
{
	CBCGPScrollDialog::OnInitDialog();


	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_ACCOUNT_LIST);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	main_account_grid_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID8);
	main_account_grid_.StartTimer();
	//main_account_grid_.SetAccountList();

	pWnd = GetDlgItem(IDC_SUB_ACCOUNT_LIST);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	_ComboAccountType.SetCurSel(0);
	// Create the Windows control and attach it to the Grid object
	sub_account_grid_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID9);
	sub_account_grid_.StartTimer();
	sub_account_grid_.SetSubAccountEditor(this);
	//sub_account_grid_.set_default_account();
	main_account_grid_.SetWholeRowSel(TRUE);
	main_account_grid_.SetSubAccountEditor(this);
	set_default_account();
	_BtnModify.EnableWindow(FALSE);
	return TRUE;
}

void SubAccountEditor::DoDataExchange(CDataExchange* pDX)
{
	CBCGPScrollDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_SUB_ACCOUNT_NAME, _EditSubAcntName);
	DDX_Control(pDX, IDC_EDIT_SUB_ACCOUNT_NO, _EditSubAcntCode);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT_TYPE, _ComboAccountType);
	DDX_Control(pDX, IDC_STATIC_SEL_ACCOUNT, _StaticSelAcntName);
	DDX_Control(pDX, IDC_BTN_MODIFY, _BtnModify);
}


void SubAccountEditor::set_sub_account_grid()
{
	if (!selected_account_) return;
	const std::vector<std::shared_ptr<DarkHorse::SmAccount>>& sub_account_vector = selected_account_->get_sub_accounts();
	if (sub_account_vector.empty()) {
		CString strAcntNo, strAcntName;
		strAcntNo = selected_account_->No().c_str();
		strAcntNo.Append(_T("_1"));
		strAcntName = selected_account_->Name().c_str();
		strAcntName.Append(_T("_1"));
		_EditSubAcntCode.SetWindowText(strAcntNo);
		_EditSubAcntName.SetWindowText(strAcntName);
		sub_account_grid_.ClearOldContents();
		return;
	}
	sub_account_grid_.InitAccount(selected_account_);
	auto sub_account = sub_account_vector[sub_account_vector.size() - 1];
	set_sub_account(sub_account);
}

void SubAccountEditor::set_sel_account_name()
{
	if (!selected_account_) return;
	CString sel_account_name;
	sel_account_name.Format("%s[%s]", selected_account_->Name().c_str(), selected_account_->No().c_str());
	_StaticSelAcntName.SetWindowText(sel_account_name);
}

void SubAccountEditor::set_account(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;

	selected_account_ = account;
	set_sel_account_name();
	set_sub_account_grid();
}

void SubAccountEditor::set_sub_account(std::shared_ptr<DarkHorse::SmAccount> sub_account)
{
	if (!sub_account) return;

	CString strAcntNo, strAcntName;
	strAcntNo = sub_account->No().c_str();
	strAcntName = sub_account->Name().c_str();
	_EditSubAcntCode.SetWindowText(strAcntNo);
	_EditSubAcntName.SetWindowText(strAcntName);
}

void SubAccountEditor::set_default_account()
{
	std::vector<std::shared_ptr<DarkHorse::SmAccount>> account_vector;
	mainApp.AcntMgr()->get_main_account_vector(account_type_, account_vector);
	if (account_vector.empty()) {
		_EditSubAcntCode.SetWindowText(_T(""));
		_EditSubAcntName.SetWindowText(_T(""));
		return;
	}
	main_account_grid_.SetAccountList(account_vector);
	//selected_account_ = account_vector[0];
	//set_sub_account_grid();
	set_account(account_vector[0]);
}

BEGIN_MESSAGE_MAP(SubAccountEditor, CBCGPScrollDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT_TYPE, &SubAccountEditor::OnCbnSelchangeComboAccountType)
	ON_BN_CLICKED(IDC_BTN_CREATE, &SubAccountEditor::OnBnClickedBtnCreate)
	ON_BN_CLICKED(IDC_BTN_MODIFY, &SubAccountEditor::OnBnClickedBtnModify)
	ON_BN_CLICKED(IDC_BTN_APPLY, &SubAccountEditor::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_SAVE, &SubAccountEditor::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_DELETE, &SubAccountEditor::OnBnClickedBtnDelete)
END_MESSAGE_MAP()


// SubAccountEditor message handlers


void SubAccountEditor::OnCbnSelchangeComboAccountType()
{
	if (_ComboAccountType.GetCurSel() < 0) return;
	int index = _ComboAccountType.GetCurSel();
	if (index == 0)
		account_type_ = "9";
	else
		account_type_ = "1";

	set_default_account();
}


void SubAccountEditor::OnBnClickedBtnCreate()
{
	CString strAcntNo, strAcntName;
	_EditSubAcntCode.GetWindowText(strAcntNo);
	_EditSubAcntName.GetWindowText(strAcntName);

	if (selected_account_)
	{
		auto found = selected_account_->find_sub_account_by_no((LPCTSTR)strAcntNo);
		if (found)
		{
			AfxMessageBox(_T("동일한 계좌 번호를 가진 서브 계좌가 이미 있습니다. 다른 계좌 번호를 사용하십시오.!"));
			return;
		}
		auto subAcnt = selected_account_->CreateSubAccount((LPCTSTR)strAcntNo, (LPCTSTR)strAcntName, selected_account_->id(), selected_account_->Type());
		set_sub_account_grid();
	}
}


void SubAccountEditor::OnBnClickedBtnModify()
{
	CString strAcntNo, strAcntName;
	_EditSubAcntCode.GetWindowText(strAcntNo);
	_EditSubAcntName.GetWindowText(strAcntName);

	if (selected_account_) {
		auto subAcnt = sub_account_grid_.get_selected_account();
		if (!subAcnt)
			return;

		auto anotherSubAcnt = selected_account_->find_sub_account_by_no((LPCTSTR)strAcntNo);
// 		if (anotherSubAcnt) {
// 			AfxMessageBox(_T("동일한 이름의 서브 계좌가 이미 있습니다. 다른 이름을 사용하십시오.!"));
// 			return;
// 		}
		subAcnt->No((LPCTSTR)strAcntNo);
		subAcnt->Name((LPCTSTR)strAcntName);
		//set_sub_account_grid();
		sub_account_grid_.update_sub_account(subAcnt);
	}
}

void SubAccountEditor::OnBnClickedBtnDelete()
{
	if (!selected_account_)
		return;
	if (selected_account_->get_sub_account_count() == 1) { // 하나일 때는 삭제하지 못한다.
		return;
	}
	auto subAcnt = sub_account_grid_.get_selected_account();
	if (!subAcnt)
		return;

// 	VtFundManager* fundMgr = VtFundManager::GetInstance();
// 	VtAccount* foundAcnt = fundMgr->FindFromUsedAccountList(subAcnt->AccountNo);
// 	if (foundAcnt) {
// 		AfxMessageBox(_T("현재 펀드에 사용중이므로 삭제할 수 없습니다."));
// 		return;
// 	}

	mainApp.AcntMgr()->RemoveAccount(subAcnt->No());
	selected_account_->remove_sub_account(subAcnt->No());
	set_sub_account_grid();
}


void SubAccountEditor::OnBnClickedBtnApply()
{
	CBCGPScrollDialog::EndDialog(IDOK);
}


void SubAccountEditor::OnBnClickedBtnSave()
{
	mainApp.SaveMgr()->save_account("account_list.json");
}


