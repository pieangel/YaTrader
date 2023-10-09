#pragma once
#include <BCGCBProInc.h>
#include "SmFilledGrid.h"
#include "../SmGrid/SmAcceptedArea.h"
// SmFilledListDialog dialog

class SmFilledListDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmFilledListDialog)

public:
	SmFilledListDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmFilledListDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILLED_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	SmAcceptedArea _AcceptedArea;
	void SetAccount();

	// key : combo index, value : account object.
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _ComboAccountMap;
	int _CurrentAccountIndex{ 0 };
	int _Count = 0;
public:

	afx_msg void OnBnClickedBtnReload();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CBCGPComboBox _ComboAccount;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeComboAccount();
	virtual void PostNcDestroy();
};
