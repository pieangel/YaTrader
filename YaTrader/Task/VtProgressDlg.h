#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "GradientStatic.h"
#include <afxcmn.h>
#include <afxdialogex.h>
#include <afxwin.h>
#include <BCGPDialog.h>
#include <BCGPProgressCtrl.h>
#include <map>
#include <memory>
#include <string>

// VtProgressDlg dialog
UINT threadfunc(LPVOID pp);
class CMainFrame;
class VtProgressDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(VtProgressDlg)

public:
	VtProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtProgressDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGRESS };
#endif

	size_t TotalCount() const { return _TotalCount; }
	void TotalCount(size_t val) { _TotalCount = val; }
	size_t RemainCount() const { return _RemainCount; }
	void RemainCount(size_t val) { _RemainCount = val; }
	std::string TaskTitle() const { return _TaskTitle; }
	void TaskTitle(std::string val) { _TaskTitle = val; }
	std::string TaskDetail() const { return _TaskDetail; }
	void TaskDetail(std::string val) { _TaskDetail = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CBrush _BrushBackNor;
	virtual BOOL OnInitDialog();
	CGradientStatic _StaticState;
	CGradientStatic _StaticTask;
	void CloseDialog();
	CBCGPProgressCtrl _PrgCtrl;
	void SetPrgressPos(int pos);
	CMainFrame* MainFrm = nullptr;
	virtual void PostNcDestroy();
	virtual void OnCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void SetTaskInfo(std::string msg);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void RefreshProgress();
private:
	size_t _TotalCount = 0;
	size_t _RemainCount = 0;
	std::string _TaskTitle;
	std::string _TaskDetail;
};
