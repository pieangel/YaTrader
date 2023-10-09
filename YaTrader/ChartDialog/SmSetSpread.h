#pragma once
#include <BCGCBProInc.h>

// SmSetSpread dialog
class SmMultiSpreadChart;
class SmSetSpread : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmSetSpread)

public:
	SmSetSpread(SmMultiSpreadChart* pParent);
	SmSetSpread(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmSetSpread();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPRD_CHART_SET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CBCGPButton _ChechDelta1;
	CBCGPButton _CheckDelta2;
	CBCGPButton _CheckKp;
	CBCGPButton _CheckNq;
	CBCGPButton _CheckVir1;
	CBCGPButton _CheckVir2;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckNq();
	afx_msg void OnBnClickedCheckKp();
	afx_msg void OnBnClickedCheckVir1();
	afx_msg void OnBnClickedCheckVir2();
	afx_msg void OnBnClickedCheckDelta1();
	afx_msg void OnBnClickedCheckDelta2();
private:
	SmMultiSpreadChart* _SpreadChart = nullptr;
};
