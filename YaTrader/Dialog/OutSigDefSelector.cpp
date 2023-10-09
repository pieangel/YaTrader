// OutSigDefSelector.cpp : implementation file
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "../resource.h"
#include "OutSigDefSelector.h"


// OutSigDefSelector dialog

IMPLEMENT_DYNAMIC(OutSigDefSelector, CBCGPDialog)

OutSigDefSelector::OutSigDefSelector(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_OUT_SIG_SELECTOR, pParent)
{

}

OutSigDefSelector::~OutSigDefSelector()
{
}

void OutSigDefSelector::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(OutSigDefSelector, CBCGPDialog)
END_MESSAGE_MAP()


// OutSigDefSelector message handlers


BOOL OutSigDefSelector::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_OUT_SIG_GRID);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	// Create the Windows control and attach it to the Grid object
	out_system_def_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, 0x0024);

	out_system_def_view_.init_grid();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
