// MainFrm.cpp : implementation of the CMainFrame class
//
#include "stdafx.h"
#include "DarkHorse.h"
#include "resource.h"
#include "MainFrm.h"
#include "DarkHorseDoc.h"
#include "DarkHorseView.h"
#include <memory>
#include <vector>
#include "Task/SmServerDataReceiver.h"
#include "Login/SmLoginDlg.h"
#include "Account/SmAccountPwdDlg.h"
#include "Global/SmTotalManager.h"
#include "Task/DataProgressDialog.h"
#include "Json/json.hpp"
#include "Order/AbAccountOrderCenterWindow.h"
#include "MfcMdiAppGlue.h"
#include "Order/AbAccountOrderLeftWindow.h"
#include "Order/AbAccountOrderRightWindow.h"
#include "Order/AbAccountOrderWindow.h"
#include "Util/Delegate.h"
#include "Symbol/SmSymbolTableDialog.h"
#include "Order/CTestDialog.h"
#include "Log/MyLogger.h"
#include "Fund/SmFundDialog.h"
#include "Order/AbFundOrderWindow.h"
#include "Symbol/SmSymbolManager.h"
#include "Symbol/SmSymbol.h"
#include "Client/ViStockClient.h"
#include "Yuanta/YaStockClient.h"
#include "Fund/SmFundOrderDialog.h"
#include "Archieve/SmSaveManager.h"
#include "Account/SmAccountManager.h"
#include "CompOrder/SmCompOrderDialog.h"
#include "CompOrder/SmOrderCompMainDialog.h"
#include "Util/SmUtil.h"
#include "OrderState/SmFilledListDialog.h"
#include "SmTotalAssetDialog.h"
#include "SmJangGoDialog.h"
#include "CompOrder/SmFundCompMainDialog.h"
#include "Hoga/SmHogaManager.h"
#include "Hoga/SmHogaProcessor.h"
#include "Quote/SmQuoteManager.h"
#include "Quote/SmQuoteProcessor.h"
#include "ChartDialog/GaSpreadChart.h"
#include "ChartDialog/SmMultiSpreadChart.h"
#include "ChartDialog/RealTimeMultiChart.h"
#include "ChartDialog/SmTrendChart.h"
#include "resource.h"

#include "DataFrame/DataFrame.h"  // Main DataFrame header
#include "DataFrame/DataFrameFinancialVisitors.h"  // Financial algorithms
#include "DataFrame/DataFrameMLVisitors.h"  // Machine-learning algorithms
#include "DataFrame/DataFrameStatsVisitors.h"  // Statistical algorithms

#include <iostream>

#include "ChartDialog/SmStockChart.h"
#include "System/SmSystemManager.h"

#include "Order/OrderUi/DmAccountOrderWindow.h"
#include "Order/OrderUi/DmFundOrderWindow.h"
#include "Order/OrderRequest/OrderRequestManager.h"
#include "Task/VtProgressDlg.h"
#include "Task/ViServerDataReceiver.h"
#include "Dialog/MiniJangoDialog.h"
#include "Dialog/TotalAssetProfitLossDialog.h"
#include "Task/SmTaskRequestManager.h"
#include "Account/SubAccountEditor.h"
// -----------------------------------------------------------------------------

// DataFrame library is entirely under hmdf name-space
//
using namespace hmdf;

#include "DataFrame/Utils/DateTime.h"
#include "DataFrame/Utils/FixedSizePriorityQueue.h"
#include "DataFrame/Utils/FixedSizeString.h"
#include "Dialog/SmAutoSignalManagerDialog.h"
#include "OutSystem/VtAutoSignalManagerDialog.h"
#include "BCGPWorkspace.h"
#include "Config/SystemConfig.h"
#include "Config/SmConfigManager.h"
#include "FileWatch/VtFileEventMonitor.h"
#include "Task/YaServerDataReceiver.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include "OutSystem/SmUSDSystemDialog.h"
//using namespace hmdf;

using namespace DarkHorse;


// A DataFrame with ulong index type
//
using ULDataFrame = hmdf::StdDataFrame<unsigned long>;

// A DataFrame with string index type
//
using StrDataFrame = hmdf::StdDataFrame<std::string>;

// This is just some arbitrary type to show how any type could be in DataFrame
//
struct  MyData {
	int         i{ 10 };
	double      d{ 5.5 };
	std::string s{ "Boo" };

	MyData() = default;
};


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CMainFrame::add_dm_order_wnd(DmAccountOrderWindow* wnd)
{
	if (!wnd) return;
	dm_account_order_wnd_map_[wnd->GetSafeHwnd()] = wnd;
}

void CMainFrame::start_login()
{
// 	SmLoginDlg loginDlg;
// 	if (loginDlg.DoModal() == IDCANCEL)
// 	{
// 		SendMessage(WM_CLOSE);
// 		return;
// 	}
	login_dialog_ = std::make_shared<SmLoginDlg>();
	login_dialog_->Create(IDD_LOGIN_MAIN, nullptr);
	login_dialog_->ShowWindow(SW_SHOW);
	
	//StartDataRequest();
}

void CMainFrame::start_timer(int milisecond)
{
	SetTimer(DATA_REQ_TIMER, milisecond, NULL);
}

void CMainFrame::stop_timer()
{
	KillTimer(DATA_REQ_TIMER);
}

void CMainFrame::LoadAfterServerData()
{
	stop_timer();
	HideProgress();
	StartLoad();
	//mainApp.SymbolMgr().MrktMgr().MakeFavoriteList();
	//mainApp.AcntMgr().RegisterAllAccounts();
}

void CMainFrame::HideProgress()
{
	if (ProgressDlg) {
		ProgressDlg->ShowWindow(SW_HIDE);
		//ProgressDlg->DestroyWindow();
	}
}

int CMainFrame::_Id = 0;

#define UM_REDRAW (WM_USER + 101)

#ifdef _BCGSUITE_INC_
#define CBCGPToolbarComboBoxButton CMFCToolBarComboBoxButton
#define CBCGPToolbarEditBoxButton CMFCToolBarEditBoxButton
#endif

//////////////////////////////////////////////////////////////////////
// CToolbarAnimationCombo class

class CToolbarAnimationCombo : public CBCGPToolbarEditBoxButton
{
	DECLARE_SERIAL(CToolbarAnimationCombo)

public:
	CToolbarAnimationCombo(UINT uiID = 0) :
		CBCGPToolbarEditBoxButton(uiID, (UINT)-1, ES_AUTOHSCROLL, globalUtils.ScaleByDPI(90))
	{
		m_strText = _T("&Animation Options...");
	}

	virtual CEdit* CreateEdit(CWnd* pWndParent, const CRect& rect)
	{
#ifdef _BCGSUITE_INC_
		CBCGPEdit* pToolbarEditCtrl = new CBCGPEdit;
		if (!pToolbarEditCtrl->Create(m_dwStyle, rect, pWndParent, m_nID))
		{
			delete pToolbarEditCtrl;
			return NULL;
		}
#else
		CEdit* pEditCtrl = CBCGPToolbarEditBoxButton::CreateEdit(pWndParent, rect);

		CBCGPEdit* pToolbarEditCtrl = DYNAMIC_DOWNCAST(CBCGPEdit, pEditCtrl);
		if (pToolbarEditCtrl == NULL)
		{
			return pEditCtrl;
		}
#endif

		//pToolbarEditCtrl->EnablePopupDialog(RUNTIME_CLASS(CAnimationOptionsDlg), IDD_ANIMATION_OPTIONS, FALSE, TRUE);
		//pToolbarEditCtrl->SetWindowText(theApp.m_arAnimationStyleNames[(int)theApp.m_animationStyle]);

		return pToolbarEditCtrl;
	}
};

IMPLEMENT_SERIAL(CToolbarAnimationCombo, CBCGPToolbarEditBoxButton, 1)

//////////////////////////////////////////////////////////////////////
// CToolbarLabel class

class CToolbarLabel : public CBCGPToolbarButton
{
	DECLARE_SERIAL(CToolbarLabel)

public:
	CToolbarLabel(UINT nID = 0, LPCTSTR lpszText = NULL)
	{
		if (lpszText != NULL)
		{
			m_strText = lpszText;
		}

		m_bText = TRUE;
		m_nID = nID;
		m_iImage = -1;
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CBCGPToolBarImages* pImages,
		BOOL bHorz = TRUE, BOOL /*bCustomizeMode*/ = FALSE,
		BOOL /*bHighlight*/ = FALSE,
		BOOL /*bDrawBorder*/ = TRUE,
		BOOL /*bGrayDisabledButtons*/ = TRUE)
	{
		UINT nStyle = m_nStyle;
		m_nStyle &= ~TBBS_DISABLED;

		CBCGPToolbarButton::OnDraw(pDC, rect, pImages, bHorz, FALSE,
			FALSE, FALSE, FALSE);

		m_nStyle = nStyle;
	}

	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz)
	{
		if (m_strText.IsEmpty())
		{
			return CSize(0, 0);
		}

		return CBCGPToolbarButton::OnCalculateSize(pDC, sizeDefault, bHorz);
	}
};

IMPLEMENT_SERIAL(CToolbarLabel, CBCGPToolbarButton, 1)

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CBCGPMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CBCGPMDIFrameWnd)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_WINDOW_MANAGER, OnWindowManager)
	ON_REGISTERED_MESSAGE(BCGM_RESETTOOLBAR, OnToolbarReset)
	ON_COMMAND(ID_MDI_MOVE_TO_NEXT_GROUP, OnMdiMoveToNextGroup)
	ON_COMMAND(ID_MDI_MOVE_TO_PREV_GROUP, OnMdiMoveToPrevGroup)
	ON_COMMAND(ID_MDI_NEW_HORZ_TAB_GROUP, OnMdiNewHorzTabGroup)
	ON_COMMAND(ID_MDI_NEW_VERT_GROUP, OnMdiNewVertGroup)
	ON_COMMAND(ID_STOCK_CHART, OnStockChart)
	ON_COMMAND(ID_MDI_CANCEL, OnMdiCancel)
	ON_COMMAND(ID_COLOR_THEME_COMBO, OnColorThemeCombo)
	ON_CBN_SELENDOK(ID_COLOR_THEME_COMBO, OnColorThemeCombo)
	ON_COMMAND(ID_TEST_ORDERWND, &CMainFrame::OnTestOrderwnd)
	ON_COMMAND(ID_TEST_LEFTORDERDIALOG, &CMainFrame::OnTestLeftorderdialog)
	ON_COMMAND(ID_TEST_RIGHTORDERDIALOG, &CMainFrame::OnTestRightorderdialog)
	ON_COMMAND(ID_TEST_ORDERMAIN, &CMainFrame::OnTestOrdermain)
	ON_COMMAND(ID_TEST_SYMBOLTABLE, &CMainFrame::OnTestSymboltable)
	ON_COMMAND(ID_TEST_FUND, &CMainFrame::OnTestFund)
	ON_COMMAND(ID_ORDER_ORDERMAIN, &CMainFrame::OnOrderOrdermain)
	ON_COMMAND(ID_ORDER_ORDERFUND, &CMainFrame::OnOrderOrderfund)
	ON_COMMAND(ID_ORDER_EDITFUND, &CMainFrame::OnOrderEditfund)
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_FILLED_LIST, &CMainFrame::OnFilledList)
	ON_COMMAND(ID_MULTI_ORDER_FOR_ACCOUNT, &CMainFrame::OnMultiOrderForAccount)
	ON_COMMAND(ID_SERVER_MSG, &CMainFrame::OnServerMsg)
	ON_COMMAND(ID_OPEN_SETTINGS, &CMainFrame::OnOpenSettings)
	ON_COMMAND(ID_ASSET, &CMainFrame::OnAsset)
	ON_COMMAND(ID_MANAGE_PWD, &CMainFrame::OnManagePwd)
	ON_COMMAND(ID_ORDER_WND, &CMainFrame::OnOrderWnd)
	ON_COMMAND(ID_COMPLEX_ORDER, &CMainFrame::OnComplexOrder)
	ON_COMMAND(ID_ORDER_REMAIN, &CMainFrame::OnOrderRemain)
	ON_COMMAND(ID_EDIT_FUND, &CMainFrame::OnEditFund)
	ON_COMMAND(ID_FUND_ORDER, &CMainFrame::OnFundOrder)
	ON_COMMAND(ID_FUND_REMAIN, &CMainFrame::OnFundRemain)
	ON_COMMAND(ID_FUND_PL, &CMainFrame::OnFundPl)
	ON_COMMAND(ID_NEW_CHART, &CMainFrame::OnNewChart)
	ON_COMMAND(ID_STRATEGE1, &CMainFrame::OnStratege1)
	ON_COMMAND(ID_FUND_COMPLEX_ORDER, &CMainFrame::OnFundComplexOrder)
	ON_COMMAND(ID_SPREAD_CHART, &CMainFrame::OnSpreadChart)
	ON_COMMAND(ID_CHART_COMP, &CMainFrame::OnChartComp)
	ON_COMMAND(ID_GIGA_SPREAD_CHART, &CMainFrame::OnGigaSpreadChart)
	ON_COMMAND(ID_GIGA_COMP_CHART, &CMainFrame::OnGigaCompChart)
	ON_COMMAND(ID_REALTIME_MULTI_CHART, &CMainFrame::OnRealtimeMultiChart)
	ON_COMMAND(ID_MULTICOLOR_CHART, &CMainFrame::OnMulticolorChart)
	ON_COMMAND(ID_32927, &CMainFrame::OnDomesticAccountOrder)
	ON_COMMAND(ID_32929, &CMainFrame::OnDomesticFundOrder)
	ON_COMMAND(ID_DM_ACNT_ORDER, &CMainFrame::OnDmAcntOrder)
	ON_COMMAND(ID_SET_SIMULATION_MODE, &CMainFrame::OnSetSimulationMode)
	ON_COMMAND(ID_32934, &CMainFrame::OnEnableSimulationFilledOrder)
	ON_COMMAND(ID_DM_FUND_ORDER, &CMainFrame::OnDmFundOrder)
	ON_COMMAND(ID_SUB_ACCOUNT, &CMainFrame::OnSubAccount)
	ON_COMMAND(ID_OUT_SYSTEM, &CMainFrame::OnOutSystem)
	ON_COMMAND(ID_THEME_DARKGRAY, &CMainFrame::OnThemeDarkgray)
	ON_COMMAND(ID_THEME_BLUE, &CMainFrame::OnThemeBlue)
	ON_COMMAND(ID_THEME_DARK, &CMainFrame::OnThemeDark)
	ON_COMMAND(ID_THEME_COLORFUL, &CMainFrame::OnThemeColorful)
	ON_COMMAND(ID_DOMESTIC_REMAIN, &CMainFrame::OnDomesticRemain)
	ON_COMMAND(ID_ABROAD_REMAIN, &CMainFrame::OnAbroadRemain)
	ON_COMMAND(ID_DM_FUND_REMAIN, &CMainFrame::OnDmFundRemain)
	ON_COMMAND(ID_AB_FUND_REMAIN, &CMainFrame::OnAbFundRemain)
	ON_WM_DESTROY()
	ON_COMMAND(ID_SIMUL_STARTSIMUL, &CMainFrame::OnSimulStartsimul)
	ON_COMMAND(ID_SIMUL_STOPSIMUL, &CMainFrame::OnSimulStopsimul)
	ON_COMMAND(ID_USD_SYSTEM, &CMainFrame::OnUsdSystem)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	//ID_SERVER_MSG,
	//ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CBCGPToolBar::EnableQuickCustomization();

	CBCGPMDITabParams mdiTabParams;

	mdiTabParams.m_bTabIcons = TRUE;
	mdiTabParams.m_tabLocation = CBCGPTabWnd::LOCATION_TOP;
	mdiTabParams.m_bDocumentMenu = TRUE;
	mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_SCROLLED;
	mdiTabParams.m_closeButtonMode = CBCGPTabWnd::TAB_CLOSE_BUTTON_HIGHLIGHTED_COMPACT;

	EnableMDITabbedGroups(TRUE, mdiTabParams);

	EnableTearOffMDIChildren();
	// Menu will not take the focus on activation:
	CBCGPPopupMenu::SetForceMenuFocus(FALSE);

// 	if (!m_wndMenuBar.Create(this))
// 	{
// 		TRACE0("Failed to create menubar\n");
// 		return -1;      // fail to create
// 	}
// 
// 	m_wndMenuBar.SetBarStyle(m_wndMenuBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);

	/*
	m_wndToolBar.SetSizes(CSize(32, 32), CSize(24, 24));
	m_wndToolBar.SetMenuSizes(globalUtils.ScaleByDPI(CSize(32, 32)), CSize(24, 24));

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME, 0, 0, FALSE, 0, 0, IDB_TOOLBAR_HC))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	// Detect color depth. 256 color toolbars can be used in the
		// high or true color modes only (bits per pixel is > 8):
	CClientDC dc(this);
	BOOL bIsHighColor = dc.GetDeviceCaps(BITSPIXEL) > 8;

	UINT uiToolbarHotID = bIsHighColor ? IDB_TOOLBAR_HC : 0;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME, uiToolbarHotID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	//SetMenu(NULL);
	//-----------------------
	// Setup toolbar buttons:
	//-----------------------
	CMenu menuView;
	menuView.LoadMenu(IDR_MENU_VIEW);

	m_wndToolBar.ReplaceButton(ID_TOOLBAR_VIEW,
		CBCGPToolbarMenuButton((UINT)-1, menuView.Detach(), -1, _T("&View")));

	m_wndToolBar.ReplaceButton(ID_COLOR_LABEL,
		CToolbarLabel(ID_COLOR_LABEL, _T("Color theme:")));

	CBCGPToolbarComboBoxButton comboTheme(ID_COLOR_THEME_COMBO,
#ifdef _BCGSUITE_INC_
		GetCmdMgr()->GetCmdImage(ID_COLOR_THEME_COMBO, FALSE),
#else
		CImageHash::GetImageOfCommand(ID_COLOR_THEME_COMBO, FALSE),
#endif
		CBS_DROPDOWNLIST, globalUtils.ScaleByDPI(150));

	comboTheme.AddItem(_T("Default Theme"));
	comboTheme.AddItem(_T("Pastel Theme"));
	comboTheme.AddItem(_T("Spring Theme"));
	comboTheme.AddItem(_T("Forest Green Theme"));
	comboTheme.AddItem(_T("Sea Blue Theme"));
	comboTheme.AddItem(_T("Golden Theme"));
	comboTheme.AddItem(_T("Dark Rose Theme"));
	comboTheme.AddItem(_T("Black and Gold Theme"));
	comboTheme.AddItem(_T("Rainbow Theme"));
	comboTheme.AddItem(_T("Gray Theme"));
	comboTheme.AddItem(_T("Arctic Theme"));
	comboTheme.AddItem(_T("Black and Red Theme"));
	comboTheme.AddItem(_T("Plum Theme"));
	comboTheme.AddItem(_T("Sunny Theme"));
	comboTheme.AddItem(_T("Violet Theme"));
	comboTheme.AddItem(_T("Flower Theme"));
	comboTheme.AddItem(_T("Steel Theme"));
	comboTheme.AddItem(_T("Gray and Green Theme"));
	comboTheme.AddItem(_T("Olive Theme"));
	comboTheme.AddItem(_T("Autumn Theme"));
	comboTheme.AddItem(_T("Black and Green Theme"));
	comboTheme.AddItem(_T("Black and Blue Theme"));
	comboTheme.AddItem(_T("Flat 2014 1-st Theme"));
	comboTheme.AddItem(_T("Flat 2014 2-nd Theme"));
	comboTheme.AddItem(_T("Flat 2014 3-rd Theme"));
	comboTheme.AddItem(_T("Flat 2014 4-th Theme"));
	comboTheme.AddItem(_T("Flat 2015 1-st Theme"));
	comboTheme.AddItem(_T("Flat 2015 2-nd Theme"));
	comboTheme.AddItem(_T("Flat 2015 3-rd Theme"));
	comboTheme.AddItem(_T("Flat 2015 4-th Theme"));
	comboTheme.AddItem(_T("Flat 2015 5-th Theme"));
	comboTheme.AddItem(_T("Flat 2016 1-st Theme"));
	comboTheme.AddItem(_T("Flat 2016 2-nd Theme"));
	comboTheme.AddItem(_T("Flat 2016 3-rd Theme"));
	comboTheme.AddItem(_T("Flat 2016 4-th Theme"));
	comboTheme.AddItem(_T("Flat 2016 5-th Theme"));
	comboTheme.AddItem(_T("SPB Theme"));
	comboTheme.AddItem(_T("Palace Theme"));
	comboTheme.AddItem(_T("Ice Theme"));
	comboTheme.AddItem(_T("Custom Theme (Color)"));
	comboTheme.AddItem(_T("Custom Theme (Textures)"));

	comboTheme.SelectItem(m_nColorTheme < 0 ? (int)comboTheme.GetCount() + m_nColorTheme : m_nColorTheme);

	m_wndToolBar.ReplaceButton(ID_COLOR_THEME_COMBO, comboTheme);

	m_wndToolBar.ReplaceButton(ID_ANIMATION_LABEL,
		CToolbarLabel(ID_ANIMATION_LABEL, _T("Animation:")));

	m_wndToolBar.ReplaceButton(ID_ANIMATION_COMBO, CToolbarAnimationCombo(ID_ANIMATION_COMBO));

	m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(ID_DARK_THEME), NULL, TRUE, FALSE);
	m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(ID_CHART_EXPORT), NULL, TRUE, TRUE);
	m_wndToolBar.SetToolBarBtnText(m_wndToolBar.CommandToIndex(ID_CHART_COPY), NULL, TRUE, TRUE);

	m_wndToolBar.EnableCustomizeButton(TRUE, (UINT)-1, _T("More Items"));
	


	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	
	// Load control bar icons:
	CBCGPToolBarImages imagesWorkspace;
	imagesWorkspace.SetImageSize(CSize(16, 16));
	imagesWorkspace.Load(IDB_WORKSPACE);
	globalUtils.ScaleByDPI(imagesWorkspace);

	const int nPaneSize = globalUtils.ScaleByDPI(200);

	if (!m_wndWorkSpace.Create(_T("View 1"), this, CRect(0, 0, nPaneSize, nPaneSize),
		TRUE, ID_VIEW_WORKSPACE,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Workspace bar\n");
		return -1;      // fail to create
	}

	m_wndWorkSpace.SetIcon(imagesWorkspace.ExtractIcon(0), FALSE);

	if (!m_wndWorkSpace2.Create(_T("View 2"), this, CRect(0, 0, nPaneSize, nPaneSize),
		TRUE, ID_VIEW_WORKSPACE2,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Workspace bar 2\n");
		return -1;      // fail to create
	}

	m_wndWorkSpace2.SetIcon(imagesWorkspace.ExtractIcon(1), FALSE);


	const int nOutputPaneSize = globalUtils.ScaleByDPI(150);

	if (!m_wndOutput.Create(_T("Output"), this, CSize(nOutputPaneSize, nOutputPaneSize),
		TRUE , ID_VIEW_OUTPUT,
		WS_CHILD | WS_VISIBLE | CBRS_BOTTOM))
	{
		TRACE0("Failed to create output bar\n");
		return -1;      // fail to create
	}
	m_wndOutput.SetIcon(imagesWorkspace.ExtractIcon(2), FALSE);

	if (!m_wndPropGrid.Create(_T("Properties"), this, CRect(0, 0, nPaneSize, nPaneSize),
		TRUE,
		ID_VIEW_PROPERTIES,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties Bar\n");
		return FALSE;		// fail to create
	}

	m_wndPropGrid.SetIcon(imagesWorkspace.ExtractIcon(3), FALSE);


	CString strMainToolbarTitle;
	strMainToolbarTitle.LoadString(IDS_MAIN_TOOLBAR);
	m_wndToolBar.SetWindowText(strMainToolbarTitle);
	*/

	try {
		// TODO: delete these three lines if you don't want the toolbar to be dockable
		//m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
		//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
		//m_wndWorkSpace.EnableDocking(CBRS_ALIGN_ANY);
		//m_wndWorkSpace2.EnableDocking(CBRS_ALIGN_ANY);
		//m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
		//m_wndPropGrid.EnableDocking(CBRS_ALIGN_ANY);
		EnableDocking(CBRS_ALIGN_ANY);
		EnableAutoHideBars(CBRS_ALIGN_ANY);
		//DockControlBar(&m_wndMenuBar);
		//DockControlBar(&m_wndToolBar);
		//DockControlBar(&m_wndWorkSpace);
		//m_wndWorkSpace2.AttachToTabWnd(&m_wndWorkSpace, BCGP_DM_STANDARD, FALSE, NULL);
		//DockControlBar(&m_wndOutput);
		//DockControlBar(&m_wndPropGrid);

		// Enable windows manager:
		EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE, FALSE, TRUE);

		// Enable windows navigator (activated by Ctrl+Tab/Ctrl+Shift+Tab):
		EnableWindowsNavigator();
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CBCGPMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CBCGPMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

// CMainFrame message handlers


LRESULT CMainFrame::OnToolbarReset(WPARAM /*wp*/,LPARAM)
{
	// TODO: reset toolbar with id = (UINT) wp to its initial state:
	//
	// UINT uiToolBarId = (UINT) wp;
	// if (uiToolBarId == IDR_MAINFRAME)
	// {
	//		do something with m_wndToolBar
	// }

	return 0;
}

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

BOOL CMainFrame::OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bDrop)
{
	CMenu menu;
	VERIFY(menu.LoadMenu(bDrop ? IDR_POPUP_DROP_MDITABS : IDR_POPUP_MDITABS));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	if ((dwAllowedItems & BCGP_MDI_CREATE_HORZ_GROUP) == 0)
	{
		pPopup->DeleteMenu(ID_MDI_NEW_HORZ_TAB_GROUP, MF_BYCOMMAND);
	}

	if ((dwAllowedItems & BCGP_MDI_CREATE_VERT_GROUP) == 0)
	{
		pPopup->DeleteMenu(ID_MDI_NEW_VERT_GROUP, MF_BYCOMMAND);
	}

	if ((dwAllowedItems & BCGP_MDI_CAN_MOVE_NEXT) == 0)
	{
		pPopup->DeleteMenu(ID_MDI_MOVE_TO_NEXT_GROUP, MF_BYCOMMAND);
	}

	if ((dwAllowedItems & BCGP_MDI_CAN_MOVE_PREV) == 0)
	{
		pPopup->DeleteMenu(ID_MDI_MOVE_TO_PREV_GROUP, MF_BYCOMMAND);
	}

	CBCGPPopupMenu* pPopupMenu = new CBCGPPopupMenu;
	pPopupMenu->SetAutoDestroy(FALSE);

	CWnd* pMenuOwner = GetActiveTearOffFrame() == NULL ? this : GetActiveTearOffFrame();
	pPopupMenu->Create(pMenuOwner, point.x, point.y, pPopup->GetSafeHmenu ());

	return TRUE;
}

void CMainFrame::OnMdiMoveToNextGroup()
{
	MDITabMoveToNextGroup();
}

void CMainFrame::OnMdiMoveToPrevGroup()
{
	MDITabMoveToNextGroup(FALSE);
}

void CMainFrame::OnMdiNewHorzTabGroup()
{
	MDITabNewGroup(FALSE);
}

void CMainFrame::OnMdiNewVertGroup()
{
	MDITabNewGroup();
}

void CMainFrame::OnMdiCancel()
{
	// TODO: add your command handler code here

}

CBCGPMDIChildWnd* CMainFrame::CreateDocumentWindow(LPCTSTR lpcszDocName, CObject* /*pObj*/)
{
	if (lpcszDocName != NULL && lpcszDocName [0] != '\0')
	{
		CDocument* pDoc = AfxGetApp()->OpenDocumentFile(lpcszDocName);
		if (pDoc != NULL)
		{
			POSITION pos = pDoc->GetFirstViewPosition();

			if (pos != NULL)
			{
				CView* pView = pDoc->GetNextView(pos);
				if (pView == NULL)
				{
					return NULL;
				}

				return DYNAMIC_DOWNCAST(CBCGPMDIChildWnd, pView->GetParent());
			}
		}
	}

	return NULL;
}

void CMainFrame::RemoveOrderWnd(HWND wnd)
{
	if (!wnd) return;

	auto found = _OrderWndMap.find(wnd);
	if (found == _OrderWndMap.end()) return;
	found->second->DestroyWindow();
	_OrderWndMap.erase(found);
}

void CMainFrame::RemoveFundOrderWnd(HWND wnd)
{
	if (!wnd) return;

	auto found = _FundOrderWndMap.find(wnd);
	if (found == _FundOrderWndMap.end()) return;

	_FundOrderWndMap.erase(found);
}

void CMainFrame::RemoveJangoWnd(HWND wnd)
{
	if (!wnd) return;

	auto found = _JangoWndMap.find(wnd);
	if (found == _JangoWndMap.end()) return;

	_JangoWndMap.erase(found);
}

void CMainFrame::RemoveFilledWnd(HWND wnd)
{
	if (!wnd) return;

	auto found = _FilledWndMap.find(wnd);
	if (found == _FilledWndMap.end()) return;

	_FilledWndMap.erase(found);
}

void CMainFrame::RemoveCompWnd(HWND wnd)
{
	if (!wnd) return;

	auto found = _CompOrderWndMap.find(wnd);
	if (found == _CompOrderWndMap.end()) return;

	_CompOrderWndMap.erase(found);
}

void CMainFrame::RemoveFundCompWnd(HWND wnd)
{
	if (!wnd) return;

	auto found = _FundCompOrderWndMap.find(wnd);
	if (found == _FundCompOrderWndMap.end()) return;

	_FundCompOrderWndMap.erase(found);
}

void CMainFrame::RemoveAssetWnd(HWND wnd)
{
	if (!wnd) return;

	auto found = _AssetWndMap.find(wnd);
	if (found == _AssetWndMap.end()) return;

	_AssetWndMap.erase(found);
}


void CMainFrame::StartDataRequest()
{
	/*
	std::shared_ptr<DataProgressDialog> ProgressDlg = std::make_shared<DataProgressDialog>();
	ProgressDlg->Create(IDD_DATA_PROGRESS, nullptr);
	ProgressDlg->ShowWindow(SW_SHOW);
	ProgressDlg->SetForegroundWindow();
	ProgressDlg->BringWindowToTop();
	mainApp.SvrDataRcvr()->ProgressDlg(ProgressDlg);
	mainApp.SvrDataRcvr()->StartDataRequest();
	*/


	// 서버 데이터 가져오기를 표시하는 대화 상자를 생성한다.
	ProgressDlg = std::make_shared<VtProgressDlg>();
	ProgressDlg->Create(IDD_PROGRESS, this);
	ProgressDlg->MainFrm = this;
	ProgressDlg->ShowWindow(SW_SHOW);
	//ProgressDlg->SetForegroundWindow();
	//ProgressDlg->BringWindowToTop();

	// 파일에서 국내 선물/옵션 상품 정보를 읽어 온다.
	//mainApp.LoadProductInfo();


	// 진행상황 표시 대화상자를 할당해 준다.
	mainApp.ya_server_data_receiver()->progress_dialog(ProgressDlg);
	// 심볼 코드를 가져오기 시작한다.
	mainApp.ya_server_data_receiver()->start_dm_account_asset();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	mainApp.ya_server_data_receiver()->execute_next();

	std::vector<int> lc = SmUtil::GetLocalDateTime();
	CString msg;
	//msg.Format("hour = %d, min = %d, sec = %d\n", lc[3], lc[4], lc[5]);
	//TRACE(msg);
}

void CMainFrame::SetMarketTree()
{
	m_wndWorkSpace.SetMarketTree();
}

void CMainFrame::SetAccountInfo()
{
	if (login_dialog_) {
		login_dialog_->SendMessage(WM_CLOSE);
	}
	SmAccountPwdDlg dlg;
	dlg.DoModal();
}

void CMainFrame::UpdateChart()
{
	CView* pActiveView = GetActiveView();
	CDarkHorseView* pActiveViewChartView = DYNAMIC_DOWNCAST(CDarkHorseView, pActiveView);
	
}

void CMainFrame::SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& window_id, const int& series_index)
{
	if (!chart_data) return;

	for (auto it = _SpreadChartMap.begin(); it != _SpreadChartMap.end(); ++it) {
		it->second->SetChartData(chart_data, window_id, series_index);
	}
	
}

void CMainFrame::OnClose()
{
	try {
		
		mainApp.Client()->Enable(false);
		mainApp.Client()->UnRegAll();

		mainApp.HogaMgr()->StopAllHogaProcess();
		mainApp.HogaMgr()->StopProcess();
		mainApp.QuoteMgr()->StopAllQuoteProcess();
		mainApp.QuoteMgr()->StopProcess();
		mainApp.order_request_manager()->stop_handle_order_request();
		//SystemConfig config;
		//config.app_name = "DarkHorse";
		//config.version = 1.0;
		//config.yes_path = "C:\\예스트레이더\\Spot\\Export";

		//mainApp.config_manager()->set_system_config(config);
		mainApp.SaveMgr()->WriteSettings();
		mainApp.SaveMgr()->save_system_config("system_config.json");
		mainApp.SaveMgr()->save_account("account_list.json");
		mainApp.SaveMgr()->save_fund("fund_list.json");
		mainApp.SaveMgr()->save_out_system("out_system_list.json");

		mainApp.SaveMgr()->save_dm_account_order_windows("dm_account_order_windows", dm_account_order_wnd_map_);
		mainApp.SaveMgr()->save_dm_fund_order_windows("dm_fund_order_windows", dm_fund_order_wnd_map_);
		mainApp.SaveMgr()->save_dm_mini_jango_windows("dm_mini_jango_windows.json", mini_jango_wnd_map_);
		mainApp.SaveMgr()->save_total_asset_windows("dm_total_asset_windows.json", total_asset_profit_loss_map_);

		total_asset_profit_loss_map_.clear();
		if (auto_signal_manager_dlg_) {
			auto_signal_manager_dlg_->DestroyWindow();
			delete auto_signal_manager_dlg_;
			auto_signal_manager_dlg_ = nullptr;
		}

		mainApp.TaskReqMgr()->StopProcess();
		mainApp.file_watch_monitor()->Stop();
		mainApp.out_system_manager()->StopProcess();

		std::vector<int> date_time = SmUtil::GetLocalDateTime();

		CString msg;
		msg.Format(_T("%d년 %d월 %d일 %d시 %d분 %d초에 종료합니다."), date_time[0], date_time[1], date_time[2], date_time[3], date_time[4], date_time[5]);
		AfxMessageBox(msg, MB_ICONEXCLAMATION);

		//SaveMDIState(theApp.GetRegSectionPath());
		//Sleep(1000);
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
	catch (...) {
		LOGINFO(CMyLogger::getInstance(), "error = %s", "unknown error");
	}

	CBCGPMDIFrameWnd::OnClose();
}


void CMainFrame::StartLoad()
{
	//mainApp.AcntMgr()->AddTestAccounts();
	//mainApp.SaveMgr()->ReadSettings();
	mainApp.AcntMgr()->register_accounts();
	mainApp.SaveMgr()->restore_account("account_list.json");
	mainApp.SaveMgr()->restore_fund("fund_list.json");
	mainApp.SaveMgr()->restore_out_system("out_system_list.json");
	mainApp.SaveMgr()->restore_dm_account_order_windows(this, "dm_account_order_windows", dm_account_order_wnd_map_);
	mainApp.SaveMgr()->restore_dm_fund_order_windows(this, "dm_fund_order_windows", dm_fund_order_wnd_map_);
	mainApp.SaveMgr()->restore_dm_mini_jango_windows_from_json(this, "dm_mini_jango_windows.json", mini_jango_wnd_map_);
	mainApp.SaveMgr()->restore_total_asset_windows_from_json(this, "dm_total_asset_windows.json", total_asset_profit_loss_map_);
	//mainApp.SystemMgr()->AddSystem("KillNasdaq");
	//mainApp.SystemMgr()->AddSystem("KillKospi");

	const std::map<int, std::shared_ptr<DarkHorse::SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	//std::vector<std::string> symbol_code_vec;
	for (auto it = favorite_map.begin(); it != favorite_map.end(); ++it) {
		//symbol_code_vec.push_back(it->second->SymbolCode());
		mainApp.Client()->RegisterSymbol(it->second->SymbolCode());
	}
	mainApp.SaveMgr()->restore_system_config("system_config.json");

	mainApp.file_watch_monitor()->AddMonDir(mainApp.config_manager()->system_config().yes_path.c_str(), true);
	//_FildMonitor->AddMonDir(_T("C:\\WRFutures\\YesGlobalPro\\YesLang"), true);
	//_FildMonitor->AddMonDir(_T("C:\\WRFutures\\YesGlobalPro\\Spot\\Export"), true);
	mainApp.file_watch_monitor()->Start();
	mainApp.out_system_manager()->StartProcess();
}


void CMainFrame::OnColorThemeCombo()
{
	CWaitCursor wait;

	CView* pActiveView = GetActiveView();

	if (pActiveView != NULL)
	{
		pActiveView->SetRedraw(FALSE);

	}

	mainApp.use_dark_theme() ? m_bIsDarkTheme = TRUE : m_bIsDarkTheme = FALSE;

	CBCGPToolbarComboBoxButton* pCombobox = DYNAMIC_DOWNCAST(CBCGPToolbarComboBoxButton, m_wndToolBar.GetButton(m_wndToolBar.CommandToIndex(ID_COLOR_THEME_COMBO)));
	ASSERT_VALID(pCombobox);

	m_nColorTheme = pCombobox->GetCurSel();

	if (m_nColorTheme == pCombobox->GetCount() - 2)
	{
		m_nColorTheme = -2;	// Custom color theme
	}
	else if (m_nColorTheme == pCombobox->GetCount() - 1)
	{
		m_nColorTheme = -1;	// Custom textures theme
	}

	

	if (pActiveView != NULL)
	{
		((CDarkHorseView*)pActiveView)->UpdateChartColorTheme(m_nColorTheme, m_bIsDarkTheme);

		pActiveView->SetRedraw();
		pActiveView->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_ERASE);
	}

	

	theApp.WriteInt(_T("ColorTheme"), m_nColorTheme);
}



void CMainFrame::OnTestOrderwnd()
{
	// TODO: Add your command handler code here

	//m_wndWorkSpace.symbol_click.Connect(&dlg, &SmOrderWnd::OnClickSymbol);
	

	_OrderWnd = std::make_shared<AbAccountOrderCenterWindow>(this);
	m_wndWorkSpace.symbol_click.Connect(_OrderWnd.get(), &AbAccountOrderCenterWindow::OnClickSymbol);
	_OrderWnd->Create(IDD_ORDER_MAIN, this);
	_OrderWnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnTestLeftorderdialog()
{
	_LeftOrderWnd = std::make_shared<AbAccountOrderLeftWindow>(this);
	//m_wndWorkSpace.symbol_click.Connect(_LeftOrderWnd.get(), &SmOrderWnd::OnClickSymbol);
	_LeftOrderWnd->Create(IDD_ORDER_LEFT, this);
	_LeftOrderWnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnTestRightorderdialog()
{
	_RightOrderWnd = std::make_shared<AbAccountOrderRightWindow>(this);
	//m_wndWorkSpace.symbol_click.Connect(_LeftOrderWnd.get(), &SmOrderWnd::OnClickSymbol);
	_RightOrderWnd->Create(IDD_ORDER_RIGHT, this);
	_RightOrderWnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnTestOrdermain()
{
	_MainOrderWnd = std::make_shared<AbAccountOrderWindow>();
	_MainOrderWnd->Create(IDD_ORDER_MAIN, this);
	const std::map<int, std::shared_ptr<AbAccountOrderCenterWindow>>& center_wnd_map = _MainOrderWnd->GetCenterWndMap();
	for (auto it = center_wnd_map.begin(); it != center_wnd_map.end(); ++it) {
		std::shared_ptr<AbAccountOrderCenterWindow> order_wnd = it->second;
		m_wndWorkSpace.symbol_click.Connect(order_wnd.get(), &AbAccountOrderCenterWindow::OnClickSymbol);
	}
	_MainOrderWnd->ShowWindow(SW_SHOW);
}

void CMainFrame::ReconnectOrderEvent()
{
	
	
}


void CMainFrame::OnTestSymboltable()
{
	//SmSymbolTableDialog dlg;
	//dlg.DoModal();
	//CTestDialog dlg;
	//dlg.DoModal();
	_TestDlg = std::make_shared<CTestDialog>();
	_TestDlg->Create(IDD_DIALOG1, this);
	_TestDlg->ShowWindow(SW_SHOW);
}


void CMainFrame::OnTestFund()
{
	//SmFundDialog dlg;
	//dlg.DoModal();
	//SmFundOrderDialog dlg;
	//dlg.DoModal();
	//_FundOrderWnd = std::make_shared<SmFundOrderDialog>();
	//_FundOrderWnd->Create(IDD_ORDER_FUND, this);
	//_FundOrderWnd->ShowWindow(SW_SHOW);
	//SmOrderCompMainDialog dlg;
	//dlg.DoModal();

	_CompOrderWnd = std::make_shared<SmOrderCompMainDialog>();
	_CompOrderWnd->Create(IDD_ORDER_COMP);
	_CompOrderWnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnOrderOrdermain()
{
	_MainOrderWnd = std::make_shared<AbAccountOrderWindow>();
	_MainOrderWnd->Create(IDD_ORDER_MAIN, this);
	
	_MainOrderWnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnOrderOrderfund()
{
	_FundOrderWnd = std::make_shared<SmFundOrderDialog>();
	_FundOrderWnd->Create(IDD_ORDER_FUND, this);
	_FundOrderWnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnOrderEditfund()
{
	_FundDialog = std::make_shared<SmFundDialog>();
	_FundDialog->Create(IDD_FUND, this);
	_FundDialog->ShowWindow(SW_SHOW);
}


void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	int i = 0;
	i = i + 0;
	CBCGPMDIFrameWnd::OnShowWindow(bShow, nStatus);
}


void CMainFrame::OnFilledList()
{
	std::shared_ptr< SmFilledListDialog> filledDialog = std::make_shared<SmFilledListDialog>();
	filledDialog->Create(IDD_FILLED_LIST, this);
	_FilledWndMap[filledDialog->GetSafeHwnd()] = filledDialog;
	filledDialog->ShowWindow(SW_SHOW);
}


void CMainFrame::OnMultiOrderForAccount()
{
	_CompOrderWnd = std::make_shared<SmOrderCompMainDialog>();
	_CompOrderWnd->Create(IDD_ORDER_COMP);
	_CompOrderWnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnServerMsg()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnOpenSettings()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnAsset()
{
	std::shared_ptr< TotalAssetProfitLossDialog>  totalAssetDialog = std::make_shared<TotalAssetProfitLossDialog>();
	totalAssetDialog->Create(IDD_TOTAL_ASSET, this);
	total_asset_profit_loss_map_[totalAssetDialog->GetSafeHwnd()] = totalAssetDialog;
	totalAssetDialog->ShowWindow(SW_SHOW);
}


void CMainFrame::OnManagePwd()
{
	SmAccountPwdDlg dlg;
	dlg.DoModal();
}


void CMainFrame::OnOrderWnd()
{
	AbAccountOrderWindow* mainOrderWnd = new AbAccountOrderWindow();
	mainOrderWnd->Create(IDD_ORDER_MAIN, this);
	_OrderWndMap[mainOrderWnd->GetSafeHwnd()] = mainOrderWnd;
	mainOrderWnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnComplexOrder()
{
	std::shared_ptr< SmOrderCompMainDialog> compOrderWnd = std::make_shared<SmOrderCompMainDialog>();
	compOrderWnd->Create(IDD_ORDER_COMP);
	_CompOrderWndMap[compOrderWnd->GetSafeHwnd()] = compOrderWnd;
	compOrderWnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnOrderRemain()
{
	std::shared_ptr<MiniJangoDialog> accountJangoDialog = std::make_shared<MiniJangoDialog>(this, "1");
	accountJangoDialog->Mode(0);
	accountJangoDialog->Create(IDD_JANGO, this);
	mini_jango_wnd_map_[accountJangoDialog->GetSafeHwnd()] = accountJangoDialog;
	accountJangoDialog->ShowWindow(SW_SHOW);
}


void CMainFrame::OnEditFund()
{
	_FundDialog = std::make_shared<SmFundDialog>();
	_FundDialog->Create(IDD_FUND, this);
	_FundDialog->ShowWindow(SW_SHOW);
}


void CMainFrame::OnFundOrder()
{
	std::shared_ptr<AbFundOrderWindow> fundOrderWnd = std::make_shared<AbFundOrderWindow>();
	fundOrderWnd->Create(IDD_ORDER_FUND, this);
	_FundOrderWndMap[fundOrderWnd->GetSafeHwnd()] = fundOrderWnd;
	fundOrderWnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnFundRemain()
{
	std::shared_ptr< MiniJangoDialog> fundJangoDialog = std::make_shared<MiniJangoDialog>(this, "1");
	fundJangoDialog->Mode(1);
	fundJangoDialog->Create(IDD_JANGO, this);
	_JangoWndMap[fundJangoDialog->GetSafeHwnd()] = fundJangoDialog;
	fundJangoDialog->ShowWindow(SW_SHOW);
}


void CMainFrame::OnFundPl()
{
	// TODO: Add your command handler code here
}




void CMainFrame::OnNewChart()
{


	StrDataFrame    ibm_df;

	// Also, you can load data into a DataFrame from a file, suporting a few
	// different formats.
	// If the file cannot be found, an exception will be thrown.
	// If the DataFrame test directory is your current directory when running
	// this, it should work
	// fine.
	//
	ibm_df.read("data/SHORT_IBM.csv", io_format::csv2);

	// To access a column, you must know its name (or index) and its type
	//
	const auto& close_const_ref = ibm_df.get_column<double>("IBM_Close");
	const auto& index_vec = ibm_df.get_index();

	std::cout << "There are " << close_const_ref.size() << " IBM close prices"
		<< std::endl;
	std::cout << "There are " << index_vec.size() << " IBM indices"
		<< std::endl;

	hmdf::StdDataFrame<hmdf::DateTime> _DataFrame;

	std::vector<hmdf::DateTime> dt_v;
	dt_v.resize(4);
	hmdf::DateTime    chart_time(20211026, 10, 1, 1, 0);
	dt_v[0] = chart_time;
	hmdf::DateTime chart_time2(20211026, 10, 1, 1, 1);
	dt_v[1] = chart_time2;
	hmdf::DateTime chart_time3(20211026, 10, 1, 1, 2);
	dt_v[2] = chart_time3;
	hmdf::DateTime chart_time4(20211026, 10, 1, 1, 3);
	dt_v[3] = chart_time4;

	std::vector<double> h_v = { 1, 2, 3, 4 };
	std::vector<double> l_v = { 5, 6, 7, 8 };
	std::vector<double> o_v = { 9, 10, 11, 12 };
	std::vector<double> c_v = { 13, 14, 15, 16 };
	std::vector<double> v_v = { 17, 18, 19, 20 };

	size_t count = _DataFrame.load_data(std::move(dt_v),
		std::make_pair("high", h_v),
		std::make_pair("low", l_v),
		std::make_pair("open", o_v),
		std::make_pair("close", c_v),
		std::make_pair("volume", v_v));

	std::vector<double>& h = _DataFrame.get_column<double>("high");
	std::vector<double>& l = _DataFrame.get_column<double>("low");
	std::vector<double>& o = _DataFrame.get_column<double>("open");
	std::vector<double>& c = _DataFrame.get_column<double>("close");

	std::vector<hmdf::DateTime> dt = _DataFrame.get_index();

	memmove(dt.data(), dt.data() + 1, sizeof(hmdf::DateTime) * (dt.size() - 1));
	hmdf::DateTime chart_time5(20211026, 10, 1, 1, 4);
	dt.data()[dt.size() - 1] = chart_time5;


	memmove(c_v.data(), c_v.data() + 1, sizeof(double) * (c_v.size() - 1));
	c_v.data()[c_v.size() - 1] = 21;

	o[1] = 22;

	_DataFrame.get_column<double>("high")[2] = 23;

	std::vector<double>& h2 = _DataFrame.get_column<double>("high");

	
}


void CMainFrame::OnStratege1()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnFundComplexOrder()
{
	std::shared_ptr< SmFundCompMainDialog> fundCompOrderDialog = std::make_shared<SmFundCompMainDialog>();
	fundCompOrderDialog->Create(IDD_ORDER_COMP_FUND, this);
	_FundCompOrderWndMap[fundCompOrderDialog->GetSafeHwnd()] = fundCompOrderDialog;
	fundCompOrderDialog->ShowWindow(SW_SHOW);
}


void CMainFrame::OnSpreadChart()
{
	std::shared_ptr<GaSpreadChart> spreadChart = std::make_shared<GaSpreadChart>();
	spreadChart->Create(IDD_GA_MULTI_SPREAD_CHART, this);
	_ChartMap[spreadChart->GetSafeHwnd()] = spreadChart;
	spreadChart->ShowWindow(SW_SHOW);
}


void CMainFrame::OnChartComp()
{
	
}


void CMainFrame::OnGigaSpreadChart()
{
	std::shared_ptr<SmMultiSpreadChart> spreadChart = std::make_shared<SmMultiSpreadChart>();
	spreadChart->Create(IDD_MULTI_SPREAD_CHART, this);
	_SpreadChartMap[spreadChart->GetSafeHwnd()] = spreadChart;
	spreadChart->ShowWindow(SW_SHOW);
}


void CMainFrame::OnGigaCompChart()
{
// 	std::shared_ptr<SmGigaCompChart> spreadChart = std::make_shared<SmGigaCompChart>();
// 	spreadChart->Create(IDD_MULTI_SPREAD_CHART, this);
// 	_GigaCompChartMap[spreadChart->GetSafeHwnd()] = spreadChart;
// 	spreadChart->ShowWindow(SW_SHOW);

}

void CMainFrame::OnStockChart()
{
	std::shared_ptr<SmStockChart> spreadChart = std::make_shared<SmStockChart>();
	spreadChart->Create(IDD_SYSTEM_CHART, this);
	_StockChartMap[spreadChart->GetSafeHwnd()] = spreadChart;
	spreadChart->ShowWindow(SW_SHOW);
}


void CMainFrame::OnRealtimeMultiChart()
{
	std::shared_ptr<RealTimeMultiChart> spreadChart = std::make_shared<RealTimeMultiChart>();
	spreadChart->Create(IDD_REALTIME_MULTICHART, this);
	_RealTimeChartMap[spreadChart->GetSafeHwnd()] = spreadChart;
	spreadChart->ShowWindow(SW_SHOW);
}


void CMainFrame::OnMulticolorChart()
{
	std::shared_ptr<SmTrendChart> spreadChart = std::make_shared<SmTrendChart>();
	spreadChart->Create(IDD_MULTI_COLOR, this);
	_TrendChartMap[spreadChart->GetSafeHwnd()] = spreadChart;
	spreadChart->ShowWindow(SW_SHOW);
}


void CMainFrame::OnDomesticAccountOrder()
{
	DmAccountOrderWindow* acnt_order_wnd = new DmAccountOrderWindow();
	acnt_order_wnd->Create(IDD_DM_ACNT_ORDER_MAIN, this);
	dm_account_order_wnd_map_[acnt_order_wnd->GetSafeHwnd()] = acnt_order_wnd;
	acnt_order_wnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnDomesticFundOrder()
{

}

void CMainFrame::remove_dm_account_order_window(HWND handle)
{
	if (!handle) return;
	auto found = dm_account_order_wnd_map_.find(handle);
	if (found == dm_account_order_wnd_map_.end()) return;
	found->second->DestroyWindow();
	dm_account_order_wnd_map_.erase(found);
}

void CMainFrame::remove_dm_fund_order_window(HWND handle)
{
	if (!handle) return;
	auto found = dm_fund_order_wnd_map_.find(handle);
	if (found == dm_fund_order_wnd_map_.end()) return;
	found->second->DestroyWindow();
	dm_fund_order_wnd_map_.erase(found);
}

void CMainFrame::OnDmAcntOrder()
{
	DmAccountOrderWindow* acnt_order_wnd = new DmAccountOrderWindow();
	acnt_order_wnd->Create(IDD_DM_ACNT_ORDER_MAIN, this);
	dm_account_order_wnd_map_[acnt_order_wnd->GetSafeHwnd()] = acnt_order_wnd;
	acnt_order_wnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnSetSimulationMode()
{
	if (mainApp.order_request_manager()->get_simulation())
		mainApp.order_request_manager()->set_simulation(false);
	else
		mainApp.order_request_manager()->set_simulation(true);
}


void CMainFrame::OnEnableSimulationFilledOrder()
{
	if (mainApp.order_request_manager()->get_enable_simulation_filled_order())
		mainApp.order_request_manager()->set_enable_simulation_filled_order(false);
	else
		mainApp.order_request_manager()->set_enable_simulation_filled_order(true);
}


void CMainFrame::OnDmFundOrder()
{
	DmFundOrderWindow* fund_order_wnd = new DmFundOrderWindow();
	fund_order_wnd->Create(IDD_DM_FUND_ORDER_MAIN, this);
	dm_fund_order_wnd_map_[fund_order_wnd->GetSafeHwnd()] = fund_order_wnd;
	fund_order_wnd->ShowWindow(SW_SHOW);
}


void CMainFrame::OnSubAccount()
{
	SubAccountEditor sub_account_editor;
	sub_account_editor.DoModal();
}


void CMainFrame::OnOutSystem()
{
	if (auto_signal_manager_dlg_) {
		auto_signal_manager_dlg_->DestroyWindow();
		delete auto_signal_manager_dlg_;
		auto_signal_manager_dlg_ = nullptr;
	}
	auto_signal_manager_dlg_ = new VtAutoSignalManagerDialog();
	auto_signal_manager_dlg_->Create(IDD_SYS_AUTO_CONNECT1, this);
	auto_signal_manager_dlg_->ShowWindow(SW_SHOW);
}


void CMainFrame::OnThemeDarkgray()
{
	theApp.SetVisualTheme(CBCGPWinApp::BCGP_VISUAL_THEME_OFFICE_2013_DARK_GRAY);
}


void CMainFrame::OnThemeBlue()
{
	theApp.SetVisualTheme(CBCGPWinApp::BCGP_VISUAL_THEME_VS_2013_BLUE);
}


void CMainFrame::OnThemeDark()
{
	theApp.SetVisualTheme(CBCGPWinApp::BCGP_VISUAL_THEME_VS_2013_DARK);
}


void CMainFrame::OnThemeColorful()
{
	theApp.SetVisualTheme(CBCGPWinApp::BCGP_VISUAL_THEME_OFFICE_2019_COLORFUL);
}


void CMainFrame::OnDomesticRemain()
{
	std::shared_ptr<MiniJangoDialog> accountJangoDialog = std::make_shared<MiniJangoDialog>(this, "9");
	accountJangoDialog->Mode(0);
	accountJangoDialog->Create(IDD_JANGO, this);
	mini_jango_wnd_map_[accountJangoDialog->GetSafeHwnd()] = accountJangoDialog;
	accountJangoDialog->ShowWindow(SW_SHOW);
}


void CMainFrame::OnAbroadRemain()
{
	std::shared_ptr<MiniJangoDialog> accountJangoDialog = std::make_shared<MiniJangoDialog>(this, "1");
	accountJangoDialog->Mode(0);
	accountJangoDialog->Create(IDD_JANGO, this);
	mini_jango_wnd_map_[accountJangoDialog->GetSafeHwnd()] = accountJangoDialog;
	accountJangoDialog->ShowWindow(SW_SHOW);
}


void CMainFrame::OnDmFundRemain()
{
	std::shared_ptr< MiniJangoDialog> fundJangoDialog = std::make_shared<MiniJangoDialog>(this, "9");
	fundJangoDialog->Mode(1);
	fundJangoDialog->Create(IDD_JANGO, this);
	_JangoWndMap[fundJangoDialog->GetSafeHwnd()] = fundJangoDialog;
	fundJangoDialog->ShowWindow(SW_SHOW);
}


void CMainFrame::OnAbFundRemain()
{
	std::shared_ptr< MiniJangoDialog> fundJangoDialog = std::make_shared<MiniJangoDialog>(this, "1");
	fundJangoDialog->Mode(1);
	fundJangoDialog->Create(IDD_JANGO, this);
	_JangoWndMap[fundJangoDialog->GetSafeHwnd()] = fundJangoDialog;
	fundJangoDialog->ShowWindow(SW_SHOW);
}


void CMainFrame::OnDestroy()
{
	CBCGPMDIFrameWnd::OnDestroy();

	
}


void CMainFrame::OnSimulStartsimul()
{
	mainApp.Client()->start_timer();
}


void CMainFrame::OnSimulStopsimul()
{
	mainApp.Client()->stop_timer();
}


void CMainFrame::OnUsdSystem()
{
	SmUSDSystemDialog dlg;
	dlg.DoModal();
}
