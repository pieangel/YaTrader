#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <BCGCBProInc.h>
#include "../Order/SmOrderConst.h"
// VtSystemOrderConfig dialog

class SmSystemOrderConfig : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmSystemOrderConfig)

public:
	SmSystemOrderConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~SmSystemOrderConfig();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYS_ORDER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox _ComboPrice;
	CComboBox _ComboPriceType;
	CEdit _EditTick;
	CSpinButtonCtrl _SpinTick;
	virtual BOOL OnInitDialog();

	DarkHorse::SmPriceType _PriceType = DarkHorse::SmPriceType::Price;
	int _OrderTick = 10;
	afx_msg void OnCbnSelchangeComboPriceType();
	afx_msg void OnCbnSelchangeComboPrice();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnCancel();
};
