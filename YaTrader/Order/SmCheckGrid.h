#pragma once
#include <BCGCBProInc.h>

class SmCheckGrid : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(SmCheckGrid)

	// Construction
public:
	SmCheckGrid();

	// Attributes
protected:
	BOOL m_bExtendedPadding;

	// Operations
public:
	void ToggleExtendedPadding();
	BOOL IsExtendedPadding() const
	{
		return m_bExtendedPadding;
	}

	// Overrides
	virtual int GetTextMargin() const
	{
		return m_bExtendedPadding ? 8 : CBCGPGridCtrl::GetTextMargin();
	}

	virtual int GetTextVMargin() const
	{
		return m_bExtendedPadding ? 8 : CBCGPGridCtrl::GetTextVMargin();
	}

	virtual int GetImageMargin() const
	{
		return m_bExtendedPadding ? 8 : CBCGPGridCtrl::GetImageMargin();
	}

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicGridCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	
	virtual ~SmCheckGrid();

	virtual void OnHeaderCheckBoxClick(int 	nColumn);

	virtual void OnRowCheckBoxClick(CBCGPGridRow* pRow);


	// Generated message map functions
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool _HeaderCheck = false;
};