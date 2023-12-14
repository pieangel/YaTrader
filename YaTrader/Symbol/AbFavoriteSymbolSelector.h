#pragma once
#include "afxdialogex.h"
#include <BCGCBProInc.h>
#include "AbFavoriteSymbolView.h"
// AbFavoriteSymbolSelector dialog

class AbFavoriteSymbolSelector : public CBCGPDialog
{
	DECLARE_DYNAMIC(AbFavoriteSymbolSelector)

public:
	AbFavoriteSymbolSelector(CWnd* pParent = nullptr);   // standard constructor
	virtual ~AbFavoriteSymbolSelector();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AB_SYMBOL_SELECTOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	AbFavoriteSymbolView favorite_symbol_view_;
public:
	void set_source_window_id(const int window_id);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
