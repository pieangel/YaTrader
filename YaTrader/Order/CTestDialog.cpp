// CTestDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "CTestDialog.h"
#include "afxdialogex.h"

#include "../Order/AbAccountOrderLeftWindow.h"
#include "../Order/AbAccountOrderRightWindow.h"
#include "../Order/AbAccountOrderCenterWindow.h"
#include <set>
#include "../MainFrm.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccountManager.h"
#include "../Client/ViStockClient.h"
#include "../Yuanta/YaStockClient.h"
#include "../Util/SmUtil.h"

const int CtrlHeight = 32;


// CTestDialog dialog

IMPLEMENT_DYNAMIC(CTestDialog, CBCGPDialog)

CTestDialog::CTestDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_DIALOG1, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

CTestDialog::~CTestDialog()
{
}

void CTestDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_GRID, _Button1);
}


BEGIN_MESSAGE_MAP(CTestDialog, CBCGPDialog)
	ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTestDialog message handlers


BOOL CTestDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	_Button1.SetUp();

	/*
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_GRID);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	DarkHorse::SmUtil::GetMacAddress();

	// Create the Windows control and attach it to the Grid object
	m_wndGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, IDC_GRID);

	m_wndGrid.EnableMarkSortedColumn(FALSE);
	m_wndGrid.EnableHeader(FALSE, BCGP_GRID_HEADER_MOVE_ITEMS);
	m_wndGrid.EnableRowHeader(TRUE);
	//m_wndGrid.EnableLineNumbers();
	m_wndGrid.SetClearInplaceEditOnEnter(FALSE);
	m_wndGrid.EnableInvertSelOnCtrl();
	m_wndGrid.SetWholeRowSel(FALSE);

	m_wndGrid.SetRowHeaderWidth(1);
	m_wndGrid.SetVisualManagerColorTheme(TRUE);

	// Insert columns:
	m_wndGrid.InsertColumn(0, _T("계좌명"), 100);
	m_wndGrid.InsertColumn(1, _T("계좌번호"), 100);
	m_wndGrid.InsertColumn(2, _T("비밀번호"), 100);

	m_wndGrid.FreezeColumns(0);

	const int nColumns = m_wndGrid.GetColumnCount();

	
	for (int row = 0; row < 100; row++) {
		// Create new row:
		CBCGPGridRow* pRow = m_wndGrid.CreateRow(nColumns);
		// Set each column data:
		for (int nColumn = 0; nColumn < nColumns; nColumn++)
		{
			long lValue = (nColumn + 1) * (row + 1);
			//pRow->GetItem(nColumn)->SetValue(lValue);

			
			pRow->GetItem(nColumn)->SetValue(lValue);
			if (nColumn == 0 || nColumn == 1)
				pRow->GetItem(nColumn)->AllowEdit(FALSE);
		}

		// Add row to grid:
		m_wndGrid.AddRow(pRow, FALSE );

		row++;
	}

	m_wndGrid.EnableCheckBoxes(TRUE);
	m_wndGrid.SetHeaderAlign(0, 2);
	m_wndGrid.SetHeaderAlign(1, 2);
	m_wndGrid.SetHeaderAlign(2, 2);
	//m_wndGrid.SetCheckBoxState(BST_CHECKED);

	m_wndGrid.CheckAll(FALSE);
	m_wndGrid.UpdateHeaderCheckbox();
	m_wndGrid.AdjustLayout();
	*/

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CTestDialog::OnEnterSizeMove(WPARAM, LPARAM)
{
	CRect rcWnd;
	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); ++it) {
		std::shared_ptr<AbAccountOrderCenterWindow> center_wnd = it->second;
		center_wnd->GetWindowRect(rcWnd);


		ScreenToClient(rcWnd);
		center_wnd->MoveWindow(rcWnd.left + 10, rcWnd.top, rcWnd.Width(), rcWnd.Height());

		
	}

	return 1;
}

LRESULT CTestDialog::OnExitSizeMove(WPARAM, LPARAM)
{
	return 1;
}


void CTestDialog::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

// 	CRect rcWnd;
// 	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); ++it) {
// 		std::shared_ptr<SmOrderWnd> center_wnd = it->second;
// 		center_wnd->GetWindowRect(rcWnd);
// 
// 
// 		ScreenToClient(rcWnd);
// 		center_wnd->MoveWindow(rcWnd.left + 1, rcWnd.top, rcWnd.Width(), rcWnd.Height());
// 
// 
// 	}

	if (!_LeftWnd) return;

	std::set<CWnd*> wnd_set;
	int max_height = 0, width_total = 0, start_x = 0;
	CRect rcWnd;
	CRect rcLine;


	int line_index = 0;
	if (_ShowLeft) {
		_LeftWnd->GetWindowRect(rcWnd);
		_LeftWnd->MoveWindow(0, CtrlHeight + 1, rcWnd.Width(), rcWnd.Height());
		_LeftWnd->ShowWindow(SW_SHOW);
		max_height = rcWnd.Height();
		start_x = rcWnd.Width();
		width_total += rcWnd.Width();
		wnd_set.insert(_LeftWnd.get());

		rcLine.left = start_x;
		rcLine.right = start_x + _LineGap;
		rcLine.top = CtrlHeight;
		rcLine.bottom = 1600;
		
	}
	else {
		_LeftWnd->ShowWindow(SW_HIDE);
	}
	start_x++;

	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); ++it) {
		std::shared_ptr<AbAccountOrderCenterWindow> center_wnd = it->second;
		center_wnd->GetWindowRect(rcWnd);


		
		//center_wnd->MoveWindow(start_x, CtrlHeight + 1, rcWnd.Width(), rcWnd.Height());

		_CenterWndMap.insert(std::make_pair(center_wnd->GetSafeHwnd(), center_wnd));
		start_x += rcWnd.Width();
		width_total += rcWnd.Width();

		//center_wnd->RecalcOrderAreaHeight(this);

		wnd_set.insert(center_wnd.get());

		rcLine.left = start_x;
		rcLine.right = start_x + _LineGap;
		rcLine.top = CtrlHeight;
		rcLine.bottom = 1600;
	
		start_x++;
	}


	if (_ShowRight) {
		_RightWnd->GetWindowRect(rcWnd);
		width_total += rcWnd.Width();
		if (rcWnd.Height() > max_height) max_height = rcWnd.Height();
		_RightWnd->MoveWindow(start_x, CtrlHeight + 1, rcWnd.Width(), rcWnd.Height());
		_RightWnd->ShowWindow(SW_SHOW);
		wnd_set.insert(_RightWnd.get());
		start_x += rcWnd.Width();

		start_x++;
	}
	else {
		_RightWnd->ShowWindow(SW_HIDE);
	}



	GetWindowRect(rcWnd);
	//MoveWindow(rcWnd.left, rcWnd.top, width_total + 21, rcWnd.Height());

	CRect rcMain(rcWnd);
	CRect rcClient;
	GetClientRect(&rcClient);

	for (auto it = wnd_set.begin(); it != wnd_set.end(); ++it) {

		(*it)->GetWindowRect(&rcWnd);
		ScreenToClient(rcWnd);
		rcWnd.bottom = rcClient.Height();
		//(*it)->MoveWindow(rcWnd);

		(*it)->SetWindowPos(nullptr, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOZORDER | SWP_NOREDRAW);
		//(*it)->ShowWindow(SW_SHOW);
	}




	rcLine.left = 0;
	rcLine.right = width_total + 4;
	rcLine.top = CtrlHeight;
	rcLine.bottom = CtrlHeight + _LineGap;


	SetWindowPos(nullptr, rcMain.left, rcMain.top, width_total + 21, rcMain.Height(), SWP_NOZORDER | SWP_NOREDRAW);

	CRect rcUpdate(rcWnd.right, rcWnd.bottom, 50, 50);
}
