#pragma once
#include <BCGCBProInc.h>
#include "OutSystemDefView.h"

// OutSigDefSelector dialog

class OutSigDefSelector : public CBCGPDialog
{
	DECLARE_DYNAMIC(OutSigDefSelector)

public:
	OutSigDefSelector(CWnd* pParent = nullptr);   // standard constructor
	virtual ~OutSigDefSelector();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OUT_SIG_SELECTOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	OutSystemDefView out_system_def_view_;
public:
	virtual BOOL OnInitDialog();
};
