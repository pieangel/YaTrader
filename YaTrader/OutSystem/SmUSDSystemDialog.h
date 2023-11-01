#pragma once
#include "VtTotalSignalGrid.h"
#include "UsdSystemDefGrid.h"
#include "afxwin.h"
#include <BCGCBProInc.h>
namespace DarkHorse {
	class SmOutSystem;
}
class SmUSDSystemDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmUSDSystemDialog)

public:
	SmUSDSystemDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SmUSDSystemDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYS_AUTO_CONNECT2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();

private:
	VtTotalSignalGrid _TotalSigGrid;
	UsdSystemDefGrid _ConnectGrid;
	void RefreshOrder();
	void Resize();
public:
	void add_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
	afx_msg void OnBnClickedBtnAddConnect();
	afx_msg void OnBnClickedBtnAddSignal();
	afx_msg void OnBnClickedBtnDelConnect();
	afx_msg void OnBnClickedBtnDelSignal();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnOrderConfig();
	afx_msg void OnBnClickedCheckAll();
	CButton _CheckAll;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
