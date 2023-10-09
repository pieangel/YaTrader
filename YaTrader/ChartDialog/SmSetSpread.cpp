// SmSetSpread.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmSetSpread.h"
#include "afxdialogex.h"
#include "SmMultiSpreadChart.h"

// SmSetSpread dialog

IMPLEMENT_DYNAMIC(SmSetSpread, CBCGPDialog)

SmSetSpread::SmSetSpread(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_SPRD_CHART_SET, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	SetWhiteBackground(FALSE);
}

SmSetSpread::SmSetSpread(SmMultiSpreadChart* pParent)
	: CBCGPDialog(IDD_SPRD_CHART_SET, (CWnd*)pParent)
{
	_SpreadChart = pParent;
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	SetWhiteBackground(FALSE);
}

SmSetSpread::~SmSetSpread()
{
}

void SmSetSpread::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_DELTA1, _ChechDelta1);
	DDX_Control(pDX, IDC_CHECK_DELTA2, _CheckDelta2);
	DDX_Control(pDX, IDC_CHECK_KP, _CheckKp);
	//DDX_Control(pDX, IDC_CHECK_NQ, _CheckNq);
	DDX_Control(pDX, IDC_CHECK_VIR1, _CheckVir1);
	DDX_Control(pDX, IDC_CHECK_VIR2, _CheckVir2);
}


BEGIN_MESSAGE_MAP(SmSetSpread, CBCGPDialog)
	//ON_BN_CLICKED(IDC_CHECK_NQ, &SmSetSpread::OnBnClickedCheckNq)
	ON_BN_CLICKED(IDC_CHECK_KP, &SmSetSpread::OnBnClickedCheckKp)
	ON_BN_CLICKED(IDC_CHECK_VIR1, &SmSetSpread::OnBnClickedCheckVir1)
	ON_BN_CLICKED(IDC_CHECK_VIR2, &SmSetSpread::OnBnClickedCheckVir2)
	ON_BN_CLICKED(IDC_CHECK_DELTA1, &SmSetSpread::OnBnClickedCheckDelta1)
	ON_BN_CLICKED(IDC_CHECK_DELTA2, &SmSetSpread::OnBnClickedCheckDelta2)
END_MESSAGE_MAP()


// SmSetSpread message handlers


BOOL SmSetSpread::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmSetSpread::OnBnClickedCheckNq()
{
	// TODO: Add your control notification handler code here
}


void SmSetSpread::OnBnClickedCheckKp()
{
	// TODO: Add your control notification handler code here
}


void SmSetSpread::OnBnClickedCheckVir1()
{
	// TODO: Add your control notification handler code here
}


void SmSetSpread::OnBnClickedCheckVir2()
{
	// TODO: Add your control notification handler code here
}


void SmSetSpread::OnBnClickedCheckDelta1()
{
	// TODO: Add your control notification handler code here
}


void SmSetSpread::OnBnClickedCheckDelta2()
{
	// TODO: Add your control notification handler code here
}
