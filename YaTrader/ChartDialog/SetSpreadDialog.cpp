// SetSpreadDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "afxdialogex.h"
#include "SetSpreadDialog.h"
#include "../TimeData/SmCompData.h"
#include "RealTimeMultiChart.h"


// SetSpreadDialog dialog

IMPLEMENT_DYNAMIC(SetSpreadDialog, CBCGPDialog)

SetSpreadDialog::SetSpreadDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ABROAD_SET, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	SetWhiteBackground(FALSE);
}

SetSpreadDialog::SetSpreadDialog(RealTimeMultiChart* parent)
	: CBCGPDialog(IDD_ABROAD_SET, (CWnd*)parent)
{
	_RealTimeChart = parent;
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	SetWhiteBackground(FALSE);
}
SetSpreadDialog::~SetSpreadDialog()
{
}

void SetSpreadDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_VK1_HS, _CheckVk1Hs);
	DDX_Control(pDX, IDC_CHECK_VK1_HS_AVG, _CheckVk1HsAvg);
	DDX_Control(pDX, IDC_CHECK_VK1_NK, _CheckVk1Nk);
	DDX_Control(pDX, IDC_CHECK_VK1_NK_AVG, _CheckVk1NkAvg);
	DDX_Control(pDX, IDC_CHECK_VK1_NQ, _CheckVk1Nq);
	DDX_Control(pDX, IDC_CHECK_VK1_NQ_AVG, _CheckVk1NqAvg);
	DDX_Control(pDX, IDC_CHECK_VK2_HS, _CheckVk2Hs);
	DDX_Control(pDX, IDC_CHECK_VK2_HS_AVG, _CheckVk2HsAvg);
	DDX_Control(pDX, IDC_CHECK_VK2_NK, _CheckVk2Nk);
	DDX_Control(pDX, IDC_CHECK_VK2_NK_AVG, _CheckVk2NkAvg);
	DDX_Control(pDX, IDC_CHECK_VK2_NQ, _CheckVk2Nq);
	DDX_Control(pDX, IDC_CHECK_VK2_NQ_AVG, _CheckVk2NqAvg);
	DDX_Control(pDX, IDC_CHECK_VK3_HS, _CheckVk3Hs);
	DDX_Control(pDX, IDC_CHECK_VK3_HS_AVG, _CheckVk3HsAvg);
	DDX_Control(pDX, IDC_CHECK_VK3_NK, _CheckVk3Nk);
	DDX_Control(pDX, IDC_CHECK_VK3_NK_AVG, _CheckVk3NkAvg);
	DDX_Control(pDX, IDC_CHECK_VK3_NQ, _CheckVk3Nq);
	DDX_Control(pDX, IDC_CHECK_VK3_NQ_AVG, _CheckVk3NqAvg);

	DDX_Control(pDX, IDC_EDIT_VK1_HS, _EditVk1Hs);
	DDX_Control(pDX, IDC_EDIT_VK1_HS_AVG, _EditVk1HsAvg);
	DDX_Control(pDX, IDC_EDIT_VK1_NK, _EditVk1Nk);
	DDX_Control(pDX, IDC_EDIT_VK1_NK_AVG, _EditVk1NkAvg);
	DDX_Control(pDX, IDC_EDIT_VK1_NQ, _EditVk1Nq);
	DDX_Control(pDX, IDC_EDIT_VK1_NQ_AVG, _EditVk1NqAvg);
	DDX_Control(pDX, IDC_EDIT_VK2_HS, _EditVk2Hs);
	DDX_Control(pDX, IDC_EDIT_VK2_HS_AVG, _EditVk2HsAvg);
	DDX_Control(pDX, IDC_EDIT_VK2_NK, _EditVk2Nk);
	DDX_Control(pDX, IDC_EDIT_VK2_NK_AVG, _EditVk2NkAvg);
	DDX_Control(pDX, IDC_EDIT_VK2_NQ, _EditVk2Nq);
	DDX_Control(pDX, IDC_EDIT_VK2_NQ_AVG, _EditVk2NqAvg);
	DDX_Control(pDX, IDC_EDIT_VK3_HS, _EditVk3Hs);
	DDX_Control(pDX, IDC_EDIT_VK3_HS_AVG, _EditVk3HsAvg);
	DDX_Control(pDX, IDC_EDIT_VK3_NK, _EditVk3Nk);
	DDX_Control(pDX, IDC_EDIT_VK3_NK_AVG, _EditVk3NkAvg);
	DDX_Control(pDX, IDC_EDIT_VK3_NQ, _EditVk3Nq);
	DDX_Control(pDX, IDC_EDIT_VK3_NQ_AVG, _EditVk3NqAvg);
	DDX_Control(pDX, IDC_EDIT_CHART_RATIO1, _EditFirstRatio);
	DDX_Control(pDX, IDC_EDIT_CHART_RATIO2, _EditSecondRatio);
}


BEGIN_MESSAGE_MAP(SetSpreadDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_CHECK_VK1_NQ, &SetSpreadDialog::OnBnClickedCheckVk1Nq)
	ON_BN_CLICKED(IDC_CHECK_VK1_NQ_AVG, &SetSpreadDialog::OnBnClickedCheckVk1NqAvg)
	ON_BN_CLICKED(IDC_CHECK_VK1_NK, &SetSpreadDialog::OnBnClickedCheckVk1Nk)
	ON_BN_CLICKED(IDC_CHECK_VK1_NK_AVG, &SetSpreadDialog::OnBnClickedCheckVk1NkAvg)
	ON_BN_CLICKED(IDC_CHECK_VK1_HS, &SetSpreadDialog::OnBnClickedCheckVk1Hs)
	ON_BN_CLICKED(IDC_CHECK_VK1_HS_AVG, &SetSpreadDialog::OnBnClickedCheckVk1HsAvg)
	ON_BN_CLICKED(IDC_CHECK_VK2_NQ, &SetSpreadDialog::OnBnClickedCheckVk2Nq)
	ON_BN_CLICKED(IDC_CHECK_VK2_NQ_AVG, &SetSpreadDialog::OnBnClickedCheckVk2NqAvg)
	ON_BN_CLICKED(IDC_CHECK_VK2_NK, &SetSpreadDialog::OnBnClickedCheckVk2Nk)
	ON_BN_CLICKED(IDC_CHECK_VK2_NK_AVG, &SetSpreadDialog::OnBnClickedCheckVk2NkAvg)
	ON_BN_CLICKED(IDC_CHECK_VK2_HS, &SetSpreadDialog::OnBnClickedCheckVk2Hs)
	ON_BN_CLICKED(IDC_CHECK_VK2_HS_AVG, &SetSpreadDialog::OnBnClickedCheckVk2HsAvg)
	ON_BN_CLICKED(IDC_CHECK_VK3_NQ, &SetSpreadDialog::OnBnClickedCheckVk3Nq)
	ON_BN_CLICKED(IDC_CHECK_VK3_NQ_AVG, &SetSpreadDialog::OnBnClickedCheckVk3NqAvg)
	ON_BN_CLICKED(IDC_CHECK_VK3_NK, &SetSpreadDialog::OnBnClickedCheckVk3Nk)
	ON_BN_CLICKED(IDC_CHECK_VK3_NK_AVG, &SetSpreadDialog::OnBnClickedCheckVk3NkAvg)
	ON_BN_CLICKED(IDC_CHECK_VK3_HS, &SetSpreadDialog::OnBnClickedCheckVk3Hs)
	ON_BN_CLICKED(IDC_CHECK_VK3_HS_AVG, &SetSpreadDialog::OnBnClickedCheckVk3HsAvg)
	ON_BN_CLICKED(IDC_BTN_V1, &SetSpreadDialog::OnBnClickedBtnV1)
	ON_BN_CLICKED(IDC_BTN_V2, &SetSpreadDialog::OnBnClickedBtnV2)
	ON_BN_CLICKED(IDC_BTN_V3, &SetSpreadDialog::OnBnClickedBtnV3)
	ON_BN_CLICKED(IDC_BTN_RATIO, &SetSpreadDialog::OnBnClickedBtnRatio)
END_MESSAGE_MAP()


// SetSpreadDialog message handlers


BOOL SetSpreadDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	DarkHorse::SmCompData& chart_data = _RealTimeChart->GetChartData();
	CString text;
	text.Format("%.1f", chart_data.NqRatio1());
	_EditVk1Nq.SetWindowText(text);
	text.Format("%.1f", chart_data.NkRatio1());
	_EditVk1Nk.SetWindowText(text);
	text.Format("%.1f", chart_data.HsRatio1());
	_EditVk1Hs.SetWindowText(text);

	text.Format("%.1f", chart_data.NqAvgRatio1());
	_EditVk1NqAvg.SetWindowText(text);
	text.Format("%.1f", chart_data.NkAvgRatio1());
	_EditVk1NkAvg.SetWindowText(text);
	text.Format("%.1f", chart_data.HsAvgRatio1());
	_EditVk1HsAvg.SetWindowText(text);


	text.Format("%.1f", chart_data.NqRatio2());
	_EditVk2Nq.SetWindowText(text);
	text.Format("%.1f", chart_data.NkRatio2());
	_EditVk2Nk.SetWindowText(text);
	text.Format("%.1f", chart_data.HsRatio2());
	_EditVk2Hs.SetWindowText(text);

	text.Format("%.1f", chart_data.NqAvgRatio2());
	_EditVk2NqAvg.SetWindowText(text);
	text.Format("%.1f", chart_data.NkAvgRatio2());
	_EditVk2NkAvg.SetWindowText(text);
	text.Format("%.1f", chart_data.HsAvgRatio2());
	_EditVk2HsAvg.SetWindowText(text);

	text.Format("%.1f", chart_data.NqRatio3());
	_EditVk3Nq.SetWindowText(text);
	text.Format("%.1f", chart_data.NkRatio3());
	_EditVk3Nk.SetWindowText(text);
	text.Format("%.1f", chart_data.HsRatio3());
	_EditVk3Hs.SetWindowText(text);

	text.Format("%.1f", chart_data.NqAvgRatio3());
	_EditVk3NqAvg.SetWindowText(text);
	text.Format("%.1f", chart_data.NkAvgRatio3());
	_EditVk3NkAvg.SetWindowText(text);
	text.Format("%.1f", chart_data.HsAvgRatio3());
	_EditVk3HsAvg.SetWindowText(text);

	text.Format("%.1f", _RealTimeChart->FirstChartRatio());
	_EditFirstRatio.SetWindowText(text);

	text.Format("%.1f", _RealTimeChart->SecondChartRatio());
	_EditSecondRatio.SetWindowText(text);

	_CheckVk1Nq.SetCheck(BST_CHECKED);
	_CheckVk1NqAvg.SetCheck(BST_CHECKED);

	_CheckVk2Nq.SetCheck(BST_CHECKED);
	_CheckVk2NqAvg.SetCheck(BST_CHECKED);

	_CheckVk2Nk.SetCheck(BST_CHECKED);
	_CheckVk2NkAvg.SetCheck(BST_CHECKED);

	_CheckVk3Nq.SetCheck(BST_CHECKED);
	_CheckVk3NqAvg.SetCheck(BST_CHECKED);

	_CheckVk3Nk.SetCheck(BST_CHECKED);
	_CheckVk3NkAvg.SetCheck(BST_CHECKED);

	_CheckVk3Hs.SetCheck(BST_CHECKED);
	_CheckVk3HsAvg.SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SetSpreadDialog::OnBnClickedCheckVk1Nq()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk1NqAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk1Nk()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk1NkAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk1Hs()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk1HsAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk2Nq()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk2NqAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk2Nk()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk2NkAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk2Hs()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk2HsAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk3Nq()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk3NqAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk3Nk()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk3NkAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk3Hs()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedCheckVk3HsAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadDialog::OnBnClickedBtnV1()
{
	// TODO: Add your control notification handler code here
	CString text;
	DarkHorse::SmCompData& chart_data = _RealTimeChart->GetChartData();
	if (_CheckVk1Nq.GetCheck() == BST_CHECKED) {
		_EditVk1Nq.GetWindowTextA(text);
		chart_data.NqRatio1(_ttof(text));
	}
	else chart_data.NqRatio1(0);

	if (_CheckVk1Nk.GetCheck() == BST_CHECKED) {
		_EditVk1Nk.GetWindowTextA(text);
		chart_data.NkRatio1(_ttof(text));
	}
	else chart_data.NkRatio1(0);

	if (_CheckVk1Hs.GetCheck() == BST_CHECKED) {
		_EditVk1Hs.GetWindowTextA(text);
		chart_data.HsRatio1(_ttof(text));
	}
	else chart_data.HsRatio1(0);

	double nq_avg_ratio = 0, nk_avg_ratio = 0, hs_avg_ratio = 0;
	if (_CheckVk1NqAvg.GetCheck() == BST_CHECKED) {
		_EditVk1NqAvg.GetWindowTextA(text);
		nq_avg_ratio = _ttof(text);
	}
	else {
		nq_avg_ratio = 0;
	}

	if (_CheckVk1NkAvg.GetCheck() == BST_CHECKED) {
		_EditVk1NkAvg.GetWindowTextA(text);
		nk_avg_ratio = _ttof(text);
	}
	else {
		nk_avg_ratio = 0;
	}

	if (_CheckVk1HsAvg.GetCheck() == BST_CHECKED) {
		_EditVk1HsAvg.GetWindowTextA(text);
		hs_avg_ratio = _ttof(text);
	}
	else {
		hs_avg_ratio = 0;
	}

	if (nq_avg_ratio + nk_avg_ratio + hs_avg_ratio > 1.0) {
		AfxMessageBox("세 평균비율 합이 1.0을 넘을 수 없습니다.");
		return;
	}

	chart_data.NqAvgRatio1(nq_avg_ratio);
	chart_data.NkAvgRatio1(nk_avg_ratio);
	chart_data.HsAvgRatio1(hs_avg_ratio);

	chart_data.ReprocessChartData();

	if (!_RealTimeChart) return;

	_RealTimeChart->RefreshChart();
}


void SetSpreadDialog::OnBnClickedBtnV2()
{
	// TODO: Add your control notification handler code here
	CString text;
	DarkHorse::SmCompData& chart_data = _RealTimeChart->GetChartData();
	if (_CheckVk2Nq.GetCheck() == BST_CHECKED) {
		_EditVk2Nq.GetWindowTextA(text);
		chart_data.NqRatio2(_ttof(text));
	}
	else chart_data.NqRatio2(0);

	if (_CheckVk2Nk.GetCheck() == BST_CHECKED) {
		_EditVk2Nk.GetWindowTextA(text);
		chart_data.NkRatio2(_ttof(text));
	}
	else chart_data.NkRatio2(0);

	if (_CheckVk2Hs.GetCheck() == BST_CHECKED) {
		_EditVk2Hs.GetWindowTextA(text);
		chart_data.HsRatio2(_ttof(text));
	}
	else chart_data.HsRatio2(0);

	double nq_avg_ratio = 0, nk_avg_ratio = 0, hs_avg_ratio = 0;
	if (_CheckVk2NqAvg.GetCheck() == BST_CHECKED) {
		_EditVk2NqAvg.GetWindowTextA(text);
		nq_avg_ratio = _ttof(text);
	}
	else {
		nq_avg_ratio = 0;
	}

	if (_CheckVk2NkAvg.GetCheck() == BST_CHECKED) {
		_EditVk2NkAvg.GetWindowTextA(text);
		nk_avg_ratio = _ttof(text);
	}
	else {
		nk_avg_ratio = 0;
	}

	if (_CheckVk2HsAvg.GetCheck() == BST_CHECKED) {
		_EditVk2HsAvg.GetWindowTextA(text);
		hs_avg_ratio = _ttof(text);
	}
	else {
		hs_avg_ratio = 0;
	}

	if (nq_avg_ratio + nk_avg_ratio + hs_avg_ratio > 1.0) {
		AfxMessageBox("세 평균비율 합이 1.0을 넘을 수 없습니다.");
		return;
	}

	chart_data.NqAvgRatio2(nq_avg_ratio);
	chart_data.NkAvgRatio2(nk_avg_ratio);
	chart_data.HsAvgRatio2(hs_avg_ratio);

	chart_data.ReprocessChartData();

	if (!_RealTimeChart) return;

	_RealTimeChart->RefreshChart();
}


void SetSpreadDialog::OnBnClickedBtnV3()
{
	// TODO: Add your control notification handler code here
	CString text;
	DarkHorse::SmCompData& chart_data = _RealTimeChart->GetChartData();
	if (_CheckVk3Nq.GetCheck() == BST_CHECKED) {
		_EditVk3Nq.GetWindowTextA(text);
		chart_data.NqRatio3(_ttof(text));
	}
	else chart_data.NqRatio3(0);

	if (_CheckVk3Nk.GetCheck() == BST_CHECKED) {
		_EditVk3Nk.GetWindowTextA(text);
		chart_data.NkRatio3(_ttof(text));
	}
	else chart_data.NkRatio3(0);

	if (_CheckVk3Hs.GetCheck() == BST_CHECKED) {
		_EditVk3Hs.GetWindowTextA(text);
		chart_data.HsRatio3(_ttof(text));
	}
	else chart_data.HsRatio3(0);

	double nq_avg_ratio = 0, nk_avg_ratio = 0, hs_avg_ratio = 0;
	if (_CheckVk3NqAvg.GetCheck() == BST_CHECKED) {
		_EditVk3NqAvg.GetWindowTextA(text);
		nq_avg_ratio = _ttof(text);
	}
	else {
		nq_avg_ratio = 0;
	}

	if (_CheckVk3NkAvg.GetCheck() == BST_CHECKED) {
		_EditVk3NkAvg.GetWindowTextA(text);
		nk_avg_ratio = _ttof(text);
	}
	else {
		nk_avg_ratio = 0;
	}

	if (_CheckVk3HsAvg.GetCheck() == BST_CHECKED) {
		_EditVk3HsAvg.GetWindowTextA(text);
		hs_avg_ratio = _ttof(text);
	}
	else {
		hs_avg_ratio = 0;
	}

	if (nq_avg_ratio + nk_avg_ratio + hs_avg_ratio > 1.0) {
		AfxMessageBox("세 평균비율 합이 1.0을 넘을 수 없습니다.");
		return;
	}

	chart_data.NqAvgRatio3(nq_avg_ratio);
	chart_data.NkAvgRatio3(nk_avg_ratio);
	chart_data.HsAvgRatio3(hs_avg_ratio);

	chart_data.ReprocessChartData();

	if (!_RealTimeChart) return;

	_RealTimeChart->RefreshChart();
}


void SetSpreadDialog::OnBnClickedBtnRatio()
{
	// TODO: Add your control notification handler code here
	if (!_RealTimeChart) return;
	CString text;
	_EditFirstRatio.GetWindowTextA(text);
	double first_ratio = _ttof(text);
	_EditSecondRatio.GetWindowTextA(text);
	double second_ratio = _ttof(text);
	if (first_ratio + second_ratio > 1.0) {
		AfxMessageBox("두 차트 비율의 합이 1.0을 넘을 수 없습니다.");
		return;
	}
	_RealTimeChart->FirstChartRatio(first_ratio);
	_RealTimeChart->SecondChartRatio(second_ratio);
	_RealTimeChart->RecalLayout();
	_RealTimeChart->SetEnableUpdate(true);
	_RealTimeChart->RefreshChart();
	//_RealTimeChart->SetEnableUpdate(false);
}
