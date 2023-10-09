// SetSpreadDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "afxdialogex.h"
#include "SetSpreadHsDialog.h"
#include "../TimeData/SmCompData.h"
#include "RealTimeMultiChart.h"


// SetSpreadHsDialog dialog

IMPLEMENT_DYNAMIC(SetSpreadHsDialog, CBCGPDialog)

SetSpreadHsDialog::SetSpreadHsDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ABROAD_SET_HS, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	SetWhiteBackground(FALSE);
}

SetSpreadHsDialog::SetSpreadHsDialog(RealTimeMultiChart* parent)
	: CBCGPDialog(IDD_ABROAD_SET_HS, (CWnd*)parent)
{
	_RealTimeChart = parent;
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	SetWhiteBackground(FALSE);
}
SetSpreadHsDialog::~SetSpreadHsDialog()
{
}

void SetSpreadHsDialog::DoDataExchange(CDataExchange* pDX)
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


BEGIN_MESSAGE_MAP(SetSpreadHsDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_CHECK_VK1_NQ, &SetSpreadHsDialog::OnBnClickedCheckVk1Nq)
	ON_BN_CLICKED(IDC_CHECK_VK1_NQ_AVG, &SetSpreadHsDialog::OnBnClickedCheckVk1NqAvg)
	ON_BN_CLICKED(IDC_CHECK_VK1_NK, &SetSpreadHsDialog::OnBnClickedCheckVk1Nk)
	ON_BN_CLICKED(IDC_CHECK_VK1_NK_AVG, &SetSpreadHsDialog::OnBnClickedCheckVk1NkAvg)
	ON_BN_CLICKED(IDC_CHECK_VK1_HS, &SetSpreadHsDialog::OnBnClickedCheckVk1Hs)
	ON_BN_CLICKED(IDC_CHECK_VK1_HS_AVG, &SetSpreadHsDialog::OnBnClickedCheckVk1HsAvg)
	ON_BN_CLICKED(IDC_CHECK_VK2_NQ, &SetSpreadHsDialog::OnBnClickedCheckVk2Nq)
	ON_BN_CLICKED(IDC_CHECK_VK2_NQ_AVG, &SetSpreadHsDialog::OnBnClickedCheckVk2NqAvg)
	ON_BN_CLICKED(IDC_CHECK_VK2_NK, &SetSpreadHsDialog::OnBnClickedCheckVk2Nk)
	ON_BN_CLICKED(IDC_CHECK_VK2_NK_AVG, &SetSpreadHsDialog::OnBnClickedCheckVk2NkAvg)
	ON_BN_CLICKED(IDC_CHECK_VK2_HS, &SetSpreadHsDialog::OnBnClickedCheckVk2Hs)
	ON_BN_CLICKED(IDC_CHECK_VK2_HS_AVG, &SetSpreadHsDialog::OnBnClickedCheckVk2HsAvg)
	ON_BN_CLICKED(IDC_CHECK_VK3_NQ, &SetSpreadHsDialog::OnBnClickedCheckVk3Nq)
	ON_BN_CLICKED(IDC_CHECK_VK3_NQ_AVG, &SetSpreadHsDialog::OnBnClickedCheckVk3NqAvg)
	ON_BN_CLICKED(IDC_CHECK_VK3_NK, &SetSpreadHsDialog::OnBnClickedCheckVk3Nk)
	ON_BN_CLICKED(IDC_CHECK_VK3_NK_AVG, &SetSpreadHsDialog::OnBnClickedCheckVk3NkAvg)
	ON_BN_CLICKED(IDC_CHECK_VK3_HS, &SetSpreadHsDialog::OnBnClickedCheckVk3Hs)
	ON_BN_CLICKED(IDC_CHECK_VK3_HS_AVG, &SetSpreadHsDialog::OnBnClickedCheckVk3HsAvg)
	ON_BN_CLICKED(IDC_BTN_V1, &SetSpreadHsDialog::OnBnClickedBtnV1)
	ON_BN_CLICKED(IDC_BTN_V2, &SetSpreadHsDialog::OnBnClickedBtnV2)
	ON_BN_CLICKED(IDC_BTN_V3, &SetSpreadHsDialog::OnBnClickedBtnV3)
	ON_BN_CLICKED(IDC_BTN_RATIO, &SetSpreadHsDialog::OnBnClickedBtnRatio)
END_MESSAGE_MAP()


// SetSpreadHsDialog message handlers


BOOL SetSpreadHsDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	DarkHorse::SmCompData& chart_data = _RealTimeChart->GetChartData();
	CString text;
	text.Format("%.1f", chart_data.NqRatio1());
	_EditVk1Nq.SetWindowText(text);
	text.Format("%.1f", chart_data.NkRatio1());
	_EditVk1Nk.SetWindowText(text);
	text.Format("%.1f", chart_data.KpRatio1());
	_EditVk1Hs.SetWindowText(text);

	text.Format("%.1f", chart_data.NqAvgRatio1());
	_EditVk1NqAvg.SetWindowText(text);
	text.Format("%.1f", chart_data.NkAvgRatio1());
	_EditVk1NkAvg.SetWindowText(text);
	text.Format("%.1f", chart_data.KpAvgRatio1());
	_EditVk1HsAvg.SetWindowText(text);


	text.Format("%.1f", chart_data.NqRatio2());
	_EditVk2Nq.SetWindowText(text);
	text.Format("%.1f", chart_data.NkRatio2());
	_EditVk2Nk.SetWindowText(text);
	text.Format("%.1f", chart_data.KpRatio2());
	_EditVk2Hs.SetWindowText(text);

	text.Format("%.1f", chart_data.NqAvgRatio2());
	_EditVk2NqAvg.SetWindowText(text);
	text.Format("%.1f", chart_data.NkAvgRatio2());
	_EditVk2NkAvg.SetWindowText(text);
	text.Format("%.1f", chart_data.KpAvgRatio2());
	_EditVk2HsAvg.SetWindowText(text);

	text.Format("%.1f", chart_data.NqRatio3());
	_EditVk3Nq.SetWindowText(text);
	text.Format("%.1f", chart_data.NkRatio3());
	_EditVk3Nk.SetWindowText(text);
	text.Format("%.1f", chart_data.KpRatio3());
	_EditVk3Hs.SetWindowText(text);

	text.Format("%.1f", chart_data.NqAvgRatio3());
	_EditVk3NqAvg.SetWindowText(text);
	text.Format("%.1f", chart_data.NkAvgRatio3());
	_EditVk3NkAvg.SetWindowText(text);
	text.Format("%.1f", chart_data.KpAvgRatio3());
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


void SetSpreadHsDialog::OnBnClickedCheckVk1Nq()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk1NqAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk1Nk()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk1NkAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk1Hs()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk1HsAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk2Nq()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk2NqAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk2Nk()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk2NkAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk2Hs()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk2HsAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk3Nq()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk3NqAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk3Nk()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk3NkAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk3Hs()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedCheckVk3HsAvg()
{
	// TODO: Add your control notification handler code here
}


void SetSpreadHsDialog::OnBnClickedBtnV1()
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
		chart_data.KpRatio1(_ttof(text));
	}
	else chart_data.KpRatio1(0);

	double nq_avg_ratio = 0, nk_avg_ratio = 0, kp_avg_ratio = 0;
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
		kp_avg_ratio = _ttof(text);
	}
	else {
		kp_avg_ratio = 0;
	}

	if (nq_avg_ratio + nk_avg_ratio + kp_avg_ratio > 1.0) {
		AfxMessageBox("세 평균비율 합이 1.0을 넘을 수 없습니다.");
		return;
	}

	chart_data.NqAvgRatio1(nq_avg_ratio);
	chart_data.NkAvgRatio1(nk_avg_ratio);
	chart_data.KpAvgRatio1(kp_avg_ratio);

	chart_data.ReprocessChartData();

	if (!_RealTimeChart) return;

	_RealTimeChart->RefreshChart();
}


void SetSpreadHsDialog::OnBnClickedBtnV2()
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
		chart_data.KpRatio2(_ttof(text));
	}
	else chart_data.KpRatio2(0);

	double nq_avg_ratio = 0, nk_avg_ratio = 0, kp_avg_ratio = 0;
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
		kp_avg_ratio = _ttof(text);
	}
	else {
		kp_avg_ratio = 0;
	}

	if (nq_avg_ratio + nk_avg_ratio + kp_avg_ratio > 1.0) {
		AfxMessageBox("세 평균비율 합이 1.0을 넘을 수 없습니다.");
		return;
	}

	chart_data.NqAvgRatio2(nq_avg_ratio);
	chart_data.NkAvgRatio2(nk_avg_ratio);
	chart_data.KpAvgRatio2(kp_avg_ratio);

	chart_data.ReprocessChartData();

	if (!_RealTimeChart) return;

	_RealTimeChart->RefreshChart();
}


void SetSpreadHsDialog::OnBnClickedBtnV3()
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
		chart_data.KpRatio3(_ttof(text));
	}
	else chart_data.KpRatio3(0);

	double nq_avg_ratio = 0, nk_avg_ratio = 0, kp_avg_ratio = 0;
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
		kp_avg_ratio = _ttof(text);
	}
	else {
		kp_avg_ratio = 0;
	}

	if (nq_avg_ratio + nk_avg_ratio + kp_avg_ratio > 1.0) {
		AfxMessageBox("세 평균비율 합이 1.0을 넘을 수 없습니다.");
		return;
	}

	chart_data.NqAvgRatio3(nq_avg_ratio);
	chart_data.NkAvgRatio3(nk_avg_ratio);
	chart_data.KpAvgRatio3(kp_avg_ratio);

	chart_data.ReprocessChartData();

	if (!_RealTimeChart) return;

	_RealTimeChart->RefreshChart();
}


void SetSpreadHsDialog::OnBnClickedBtnRatio()
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
