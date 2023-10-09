#pragma once
#include <BCGCBProInc.h>
#include <map>
#include "SmGrid/SmTotalAssetArea.h"
// SmTotalAssetDialog dialog
class SmTotalAssetDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmTotalAssetDialog)

public:
	SmTotalAssetDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmTotalAssetDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOTAL_ASSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	// key : combo index, value : account object.
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _ComboAccountMap;
	int _CurrentAccountIndex{ 0 };
	SmTotalAssetArea _AssetArea;
public:
	void SetAccount();
	virtual BOOL OnInitDialog();
	CBCGPComboBox _ComboAccount;
	afx_msg void OnBnClickedBtnGet();
	afx_msg void OnCbnSelchangeComboAccount();
	virtual void PostNcDestroy();
};
