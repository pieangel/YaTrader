// SmLoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmLoginDlg.h"
#include "../Global/SmTotalManager.h"
#include "../Client/ViStockClient.h"
#include "../Yuanta/YaStockClient.h"
#include "../Json/json.hpp"
#include "../Archieve/SmSaveManager.h"
#include "../Login/SmLoginManager.h"
#include <any>
#include <map>
#include <string>
#include <vector>

using namespace nlohmann;
// SmLoginDlg dialog

IMPLEMENT_DYNAMIC(SmLoginDlg, CBCGPDialog)

SmLoginDlg::SmLoginDlg(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_LOGIN_MAIN, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
}

SmLoginDlg::~SmLoginDlg()
{
}

void SmLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CERT, _EditCert);
	DDX_Control(pDX, IDC_EDIT_ID, _EditId);
	DDX_Control(pDX, IDC_EDIT_PWD, _EditPwd);
	DDX_Control(pDX, IDC_CHECK_SAVE, _CheckSave);
	DDX_Control(pDX, IDC_CHECK_SIMUL, _CheckSimul);
	DDX_Control(pDX, IDC_COMBO_LOGIN_SERVER, combol_log_in_server_);
}


BEGIN_MESSAGE_MAP(SmLoginDlg, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &SmLoginDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_CAN, &SmLoginDlg::OnBnClickedBtnCan)
	ON_CBN_SELCHANGE(IDC_COMBO_LOGIN_SERVER, &SmLoginDlg::OnCbnSelchangeComboLoginServer)
END_MESSAGE_MAP()


// SmLoginDlg message handlers


void SmLoginDlg::OnBnClickedBtnLogin()
{
	const int index = combol_log_in_server_.GetCurSel();
	if (index == 0) {
		AfxMessageBox("접속할 서버를 선택해 주세요!");
		return;
	}

	// TODO: Add your control notification handler code here
	CString id, pwd, cert;
	//-----------------------------------------------------------------------------
	// 아이디
	GetDlgItemText(IDC_EDIT_ID, id);

	//-----------------------------------------------------------------------------
	// 비밀번호
	GetDlgItemText(IDC_EDIT_PWD, pwd);

	//-----------------------------------------------------------------------------
	// 공인인증 비밀번호
	GetDlgItemText(IDC_EDIT_CERT, cert);

	std::map<std::string, std::any> login_info;
	login_info["id"] = std::string(id);
	login_info["pwd"] = std::string(pwd);
	login_info["cert"] = std::string(cert);

	int result = mainApp.Client()->Login(std::move(login_info));
	if (result < 0)
	{
		CDialog::EndDialog(IDCANCEL);
		return;
	}

	//CDialog::EndDialog(IDOK);
}


void SmLoginDlg::OnBnClickedBtnCan()
{
	// TODO: Add your control notification handler code here
	CDialog::EndDialog(IDCANCEL);
}


BOOL SmLoginDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	_EditCert.EnablePasswordPreview();
	_EditPwd.EnablePasswordPreview();

	mainApp.SaveMgr()->ReadLoginInfo();

#ifdef _DEBUG
	_EditId.SetWindowText(mainApp.LoginMgr()->id().c_str());
	_EditPwd.SetWindowText(mainApp.LoginMgr()->pwd().c_str());
	_EditCert.SetWindowText(mainApp.LoginMgr()->cert().c_str());
#else
	//_EditId.SetWindowText("jslight7");
	//_EditPwd.SetWindowText("live3404");
	//_EditCert.SetWindowText("!live340401");

	_EditId.SetWindowText(mainApp.LoginMgr()->id().c_str());
	_EditPwd.SetWindowText(mainApp.LoginMgr()->pwd().c_str());
	_EditCert.SetWindowText(mainApp.LoginMgr()->cert().c_str());
#endif

	std::vector<std::string> server_list;
	server_list.push_back("서버선택");
	server_list.push_back("국내모의");
	server_list.push_back("국내운영");
	server_list.push_back("해외모의");
	server_list.push_back("해외운영");
	for (const auto& server : server_list)
	{
		combol_log_in_server_.AddString(server.c_str());
	}

	combol_log_in_server_.SetCurSel(0);
	CRect desktopRect, dialogRect;
	GetDesktopWindow()->GetWindowRect(&desktopRect);

	int screenWidth = desktopRect.Width();
	int screenHeight = desktopRect.Height();

	GetWindowRect(&dialogRect);
	GetDesktopWindow()->ScreenToClient(dialogRect);
	//	# Get the dimensions of the dialog / window
	int	dialog_width = dialogRect.Width();
	int	dialog_height = dialogRect.Height();

	//	# Calculate the position to center the dialog
	int	x_position = (screenWidth - dialog_width) / 2;
	int	y_position = (screenHeight - dialog_height) / 2;

	//# Set the position of the dialog
	SetWindowPos(nullptr, x_position, y_position, dialog_width, dialog_height, SWP_NOSIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmLoginDlg::OnCbnSelchangeComboLoginServer()
{
	int cur_index = combol_log_in_server_.GetCurSel();
	if (cur_index < 0) {
		return;
	}

	if (cur_index == 0 || cur_index == 1) {
		mainApp.mode = 0;
	}
	else {
		mainApp.mode = 1;
	}

	if (cur_index == 0 || cur_index == 2) {
		mainApp.is_simul(true);
	}
	else {
		mainApp.is_simul(false);
	}

	mainApp.LoginMgr()->ya_server_index(cur_index - 1);
	mainApp.Client()->init();
}
