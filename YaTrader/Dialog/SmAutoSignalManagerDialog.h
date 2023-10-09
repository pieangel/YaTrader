#pragma once

#include "afxwin.h"
#include <BCGCBProInc.h>
#include <memory>
#include "OutSystemView.h"
#include "ActiveOutSystemView.h"
#include "OutSystemDefView.h"

namespace DarkHorse
{
	class SmOutSystem;
}
class SmAutoSignalManagerDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmAutoSignalManagerDialog)

public:
	SmAutoSignalManagerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SmAutoSignalManagerDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYS_AUTO_CONNECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();

private:
	OutSystemView out_system_view_;
	ActiveOutSystemView active_out_system_view_;
	OutSystemDefView out_system_def_view_;
	void RefreshOrder();
	void Resize();
public:
	void add_active_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
	void remove_active_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
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
