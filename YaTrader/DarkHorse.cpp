// DarkHorse.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DarkHorse.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "DarkHorseDoc.h"
#include "DarkHorseView.h"

#include "Log/MyLogger.h"
#include "Util/VtStringUtil.h"
#include <filesystem>
#include <string>
#include "Global/SmTotalManager.h"
#include "Client/ViStockClient.h"
#include "Yuanta/YaStockClient.h"
#include "Login/SmLoginDlg.h"
#include "Dump/CrashHandler.h"
#include "Config/SystemConfig.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fs = std::filesystem;
using namespace DarkHorse;

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

// CDarkHorseApp

BEGIN_MESSAGE_MAP(CDarkHorseApp, CBCGPWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CBCGPWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CBCGPWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CDarkHorseApp construction

CDarkHorseApp::CDarkHorseApp()
{

	// Support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;

	CBCGPVisualManagerVS2012::m_bAutoGrayscaleImages = FALSE;

	//SetApplicationLookMenu(ID_VIEW_APPLOOK, BCGP_THEMES_LIST_WIN_NATIVE | BCGP_THEMES_LIST_SCENIC | BCGP_THEMES_LIST_OFFICE_2007 | BCGP_THEMES_LIST_OFFICE_2010 | BCGP_THEMES_LIST_VS_2010 | BCGP_THEMES_LIST_OFFICE_2013 | BCGP_THEMES_LIST_VS_2013 | BCGP_THEMES_LIST_OFFICE_2016 | BCGP_THEMES_LIST_VS_2019);

	if (mainApp.use_dark_theme())
		SetVisualTheme(BCGP_VISUAL_THEME_VS_2019_DARK);
	else
		SetVisualTheme(/*BCGP_VISUAL_THEME_VS_2013_DARK */BCGP_VISUAL_THEME_OFFICE_2013_DARK_GRAY);

	CBCGPToolbarOptions toolbarOptions;

	toolbarOptions.m_nViewToolbarsMenuEntryID = ID_VIEW_TOOLBARS;
	toolbarOptions.m_nCustomizeCommandID = ID_VIEW_CUSTOMIZE;
	toolbarOptions.m_strCustomizeCommandLabel = _T("Customize...");
	toolbarOptions.m_nUserToolbarCommandIDFirst = uiFirstUserToolBarId;
	toolbarOptions.m_nUserToolbarCommandIDLast = uiLastUserToolBarId;
	toolbarOptions.m_strToolbarCustomIconsPath = _T(".\\UserImages.png");

	SetToolbarOptions(toolbarOptions);

	m_bThemedFileDialog = TRUE;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDarkHorseApp object

CDarkHorseApp theApp;


// CDarkHorseApp initialization

BOOL CDarkHorseApp::InitInstance()
{
	if (!CheckExpire()) { return FALSE; }
	InitLog();
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CBCGPWinApp::InitInstance();

	// Remove this line if you don't need themed product dialogs:
	globalData.m_bUseVisualManagerInBuiltInDialogs = TRUE;

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: you should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("BCGP AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	SetRegistryBase(_T("Settings"));

	//RegisterOCX();

	// Initialize all Managers for usage. They are automatically constructed
	// if not yet present
	// Enable user-defined tools. If you want allow more than 10 tools,
	// add tools entry to resources (ID_USER_TOOL11, ID_USER_TOOL12,...)
	EnableUserTools(ID_TOOLS_ENTRY, ID_USER_TOOL1, ID_USER_TOOL10);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_DarkHorseTYPE,
		RUNTIME_CLASS(CDarkHorseDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CDarkHorseView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
	{
		if (!pMainFrame->LoadMDIState(GetRegSectionPath()) ||
			DYNAMIC_DOWNCAST(CMDIChildWnd, pMainFrame->GetActiveFrame()) == NULL)
		{
			if (!ProcessShellCommand(cmdInfo))
				return FALSE;
		}
	}
	else
	{
		// Dispatch commands specified on the command line
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	}

	CRect rect;
	int ScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	rect.left = (ScreenWidth) / 2 - 200;
	rect.top = 20;
	int width = 376;
	rect.right = rect.left + width;
	rect.bottom = rect.top + 78;
	pMainFrame->MoveWindow(rect);
	// 주 창이 초기화되었으므로 이를 표시하고 업데이트합니다.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	mainApp.CreateManagers();
	mainApp.Client()->CreateControl();
	mainApp.Client()->ConnectToServer();

	pMainFrame->start_login();
	return TRUE;
}

// CDarkHorseApp message handlers

int CDarkHorseApp::ExitInstance()
{
	return CBCGPWinApp::ExitInstance();
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CBCGPDialog
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif
	CBCGPURLLinkButton m_btnURL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CBCGPDialog(IDD_ABOUTBOX)
{
	EnableVisualManagerStyle(TRUE, TRUE);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPANY_URL, m_btnURL);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CBCGPDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CDarkHorseApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CDarkHorseApp message handlers


void CDarkHorseApp::PreLoadState()
{
	GetContextMenuManager()->AddMenu(_T("My menu"), IDR_CONTEXT_MENU);

	// TODO: add another context menus here
}

BOOL CDarkHorseApp::SaveAllModified()
{
	if (!CBCGPWinApp::SaveAllModified())
	{
		return FALSE;
	}

	CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST(CBCGPMDIFrameWnd, m_pMainWnd);
	if (pMainFrame != NULL)
	{
		pMainFrame->SaveMDIState(GetRegSectionPath());
	}

	return TRUE;
}

void CDarkHorseApp::InitLog()
{
	std::string log_file = VtStringUtil::get_application_path();
	log_file.append("\\log\\");
	std::string app_name = VtStringUtil::get_application_name();
	CMyLogger::getInstance().setLogFile((char*)log_file.c_str(), app_name.c_str());
	CMyLogger::getInstance().SetLoggerInfo(true);
	CMyLogger::getInstance().Run();

	LOGINFO(CMyLogger::getInstance(), "%s Started!", app_name.c_str());
}


int CDarkHorseApp::RegisterOCX()
{
#ifdef LOGGING
	CommsDbgLog(0, "RegisterOCX() - start");
#endif

	// 챠트 OCX 파일 등록 처리
	TCHAR iniFileName[500] = { 0 };

	GetModuleFileName(NULL, iniFileName, MAX_PATH);
	CString path = iniFileName;
	CString strFileName = path.Left(path.ReverseFind('\\') + 1);
	strFileName = strFileName += "HDFCommAgent.ocx";


	int			iReturn = 1;
	CString		szErrorMsg;

	strFileName.Replace("'\'", "\\");
	// Initialize OLE.
	if (FAILED(OleInitialize(NULL))) {
		AfxMessageBox("DLLRegister OleInitialize 실패");
		return 1;
	}

	SetErrorMode(SEM_FAILCRITICALERRORS);       // Make sure LoadLib fails.
												// Load the library.
	HINSTANCE hLib = LoadLibraryEx(strFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (hLib == NULL) {
		szErrorMsg.Format("File Name=%s, GetLastError() NO = 0x%08lx\n", strFileName, GetLastError());
		AfxMessageBox(szErrorMsg);
		iReturn = 0;
		goto CleanupOle;
	}

	HRESULT(STDAPICALLTYPE * lpDllEntryPoint)(void);
	// Find the entry point.
	(FARPROC&)lpDllEntryPoint = GetProcAddress(hLib, "DllRegisterServer");
	if (lpDllEntryPoint == NULL) {
		// 		TCHAR szExt[_MAX_EXT];
		// 		_tsplitpath(strFileName, NULL, NULL, NULL, szExt);

		TCHAR drive[255];
		TCHAR szExt[255];
		TCHAR path[MAX_PATH];
		TCHAR filename[MAX_PATH];
		_tsplitpath_s((LPTSTR)(LPCTSTR)strFileName, drive, _countof(drive), path, _countof(path), filename, _countof(filename), szExt, _countof(szExt));

		if ((_stricmp(szExt, ".dll") != 0) && (_stricmp(szExt, ".ocx") != 0)) {
			szErrorMsg.Format("File Name=%s, GetProcAddress Fail\n", strFileName);
			AfxMessageBox(szErrorMsg);
		}

		iReturn = 0;
		goto CleanupLibrary;
	}

	// Call the entry point.
	if (FAILED((*lpDllEntryPoint)())) {
		szErrorMsg.Format("File Name=%s, lpDllEntryPoint Fail\n", strFileName);
		AfxMessageBox(szErrorMsg);
		iReturn = 0;
		goto CleanupLibrary;
	}
	return iReturn;

CleanupLibrary:
	FreeLibrary(hLib);

CleanupOle:
	OleUninitialize();

#ifdef LOGGING
	CommsDbgLog(0, "RegisterOCX() - end");
#endif

	return iReturn;
}

void CDarkHorseApp::CreateCrashHandler()
{
	// 챠트 OCX 파일 등록 처리
	TCHAR iniFileName[500] = { 0 };

	GetModuleFileName(NULL, iniFileName, MAX_PATH);
	CString path = iniFileName;
	CString fileName = path.Left(path.ReverseFind('\\') + 1);
	CString dumpPath = fileName;
	fileName = fileName += "HDFCommAgent.ocx";

	dumpPath += "hdtrader.dmp";

	CCrashHandler ch((LPCTSTR)dumpPath);

	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();
}

bool CDarkHorseApp::CheckExpire()
{
	std::string now = VtStringUtil::getCurentDate();

	int curDate = std::stoi(now);

	if (curDate > 20231031)
		return false;

	return true;
}

