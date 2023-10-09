#pragma once
#include "afxdialogex.h"
#include <BCGCBProInc.h>

// SetSpreadDialog dialog
class RealTimeMultiChart;
class SetSpreadHsDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(SetSpreadHsDialog)

public:
	SetSpreadHsDialog(CWnd* pParent = nullptr);   // standard constructor
	SetSpreadHsDialog(RealTimeMultiChart* parent);
	virtual ~SetSpreadHsDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABROAD_SET_HS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CBCGPButton _CheckVk1Hs;
	CBCGPButton _CheckVk1HsAvg;
	CBCGPButton _CheckVk1Nk;
	CBCGPButton _CheckVk1NkAvg;
	CBCGPButton _CheckVk1Nq;
	CBCGPButton _CheckVk1NqAvg;
	CBCGPButton _CheckVk2Hs;
	CBCGPButton _CheckVk2HsAvg;
	CBCGPButton _CheckVk2Nk;
	CBCGPButton _CheckVk2NkAvg;
	CBCGPButton _CheckVk2Nq;
	CBCGPButton _CheckVk2NqAvg;
	CBCGPButton _CheckVk3Hs;
	CBCGPButton _CheckVk3HsAvg;
	CBCGPButton _CheckVk3Nk;
	CBCGPButton _CheckVk3NkAvg;
	CBCGPButton _CheckVk3Nq;
	CBCGPButton _CheckVk3NqAvg;

	CBCGPEdit _EditVk1Hs;
	CBCGPEdit _EditVk1HsAvg;
	CBCGPEdit _EditVk1Nk;
	CBCGPEdit _EditVk1NkAvg;
	CBCGPEdit _EditVk1Nq;
	CBCGPEdit _EditVk1NqAvg;
	CBCGPEdit _EditVk2Hs;
	CBCGPEdit _EditVk2HsAvg;
	CBCGPEdit _EditVk2Nk;
	CBCGPEdit _EditVk2NkAvg;
	CBCGPEdit _EditVk2Nq;
	CBCGPEdit _EditVk2NqAvg;
	CBCGPEdit _EditVk3Hs;
	CBCGPEdit _EditVk3HsAvg;
	CBCGPEdit _EditVk3Nk;
	CBCGPEdit _EditVk3NkAvg;
	CBCGPEdit _EditVk3Nq;
	CBCGPEdit _EditVk3NqAvg;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckVk1Nq();
	afx_msg void OnBnClickedCheckVk1NqAvg();
	afx_msg void OnBnClickedCheckVk1Nk();
	afx_msg void OnBnClickedCheckVk1NkAvg();
	afx_msg void OnBnClickedCheckVk1Hs();
	afx_msg void OnBnClickedCheckVk1HsAvg();
	afx_msg void OnBnClickedCheckVk2Nq();
	afx_msg void OnBnClickedCheckVk2NqAvg();
	afx_msg void OnBnClickedCheckVk2Nk();
	afx_msg void OnBnClickedCheckVk2NkAvg();
	afx_msg void OnBnClickedCheckVk2Hs();
	afx_msg void OnBnClickedCheckVk2HsAvg();
	afx_msg void OnBnClickedCheckVk3Nq();
	afx_msg void OnBnClickedCheckVk3NqAvg();
	afx_msg void OnBnClickedCheckVk3Nk();
	afx_msg void OnBnClickedCheckVk3NkAvg();
	afx_msg void OnBnClickedCheckVk3Hs();
	afx_msg void OnBnClickedCheckVk3HsAvg();
	afx_msg void OnBnClickedBtnV1();
	afx_msg void OnBnClickedBtnV2();
	afx_msg void OnBnClickedBtnV3();
	CBCGPEdit _EditFirstRatio;
	CBCGPEdit _EditSecondRatio;
	afx_msg void OnBnClickedBtnRatio();
private:
	RealTimeMultiChart* _RealTimeChart = nullptr;
};
