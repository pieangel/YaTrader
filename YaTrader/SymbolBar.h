// workspacebar.h : interface of the CWorkSpaceBar class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Util/Signal.h"
#include <string>
class SmSymbol;
class CSymbolBar : public CBCGPDockingControlBar
{
public:
	CSymbolBar();

// Attributes
protected:
	CBCGPTreeCtrl m_wndTree;

// Operations
public:

// Overrides

// Implementation
public:
	virtual ~CSymbolBar();

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
public:
	void SetMarketTree();

	void doubleClickedTree(NMHDR* pNmhdr, LRESULT* pResult);

	Gallant::Signal1<const std::string&> symbol_click;
};

/////////////////////////////////////////////////////////////////////////////
