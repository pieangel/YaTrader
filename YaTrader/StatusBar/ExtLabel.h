#if !defined(AFX_EXTLABEL_H__698A45B4_27B1_4AE6_B651_0DA439882A9B__INCLUDED_)
#define AFX_EXTLABEL_H__698A45B4_27B1_4AE6_B651_0DA439882A9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtLabel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExtLabel window

class CExtLabel : public CStatic
{
// Construction
public:
	CExtLabel();

// Attributes
public:
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtLabel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtLabel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtLabel)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTLABEL_H__698A45B4_27B1_4AE6_B651_0DA439882A9B__INCLUDED_)
