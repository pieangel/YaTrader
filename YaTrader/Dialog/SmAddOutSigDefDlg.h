#pragma once
#include "afxwin.h"
#include <BCGCBProInc.h>

// VtAddOutSigDefDlg dialog
class VtSignalDefinitionGrid;
class SmAddOutSigDefDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmAddOutSigDefDlg)

public:
	SmAddOutSigDefDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SmAddOutSigDefDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_SIG_DEF };
#endif

	VtSignalDefinitionGrid* SigDefGrid() const { return _SigDefGrid; }
	void SigDefGrid(VtSignalDefinitionGrid* val) { _SigDefGrid = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit _EditSignal;
private:
	VtSignalDefinitionGrid* _SigDefGrid = nullptr;
};
