// ExtLabel.cpp : implementation file
//

#include "stdafx.h"
#include "ExtLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtLabel

CExtLabel::CExtLabel()
{
}

CExtLabel::~CExtLabel()
{
}


BEGIN_MESSAGE_MAP(CExtLabel, CStatic)
	//{{AFX_MSG_MAP(CExtLabel)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtLabel message handlers


BOOL CExtLabel::OnEraseBkgnd(CDC* pDC) 
{
	pDC;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

void CExtLabel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect( &rcClient );

	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcClient);
	dc.SelectClipRgn(&rgn);
	
	dc.FillSolidRect( &rcClient, GetSysColor(COLOR_3DFACE) );

	//////////////////////////////////////////////////////////////////////////
	// paint text
	//////////////////////////////////////////////////////////////////////////
	
	CString strText;
	GetWindowText(strText);

	DWORD style = GetStyle();
	DWORD dwFlags = 0;
				
	switch (style & SS_TYPEMASK)
	{
	case SS_RIGHT: 
		dwFlags = DT_RIGHT | DT_WORDBREAK; 
		break; 
	case SS_CENTER: 
		dwFlags = SS_CENTER | DT_WORDBREAK;
		break;
	case SS_LEFTNOWORDWRAP: 
		dwFlags = DT_LEFT; 
		break;
	default: // treat other types as left
	case SS_LEFT: 
		dwFlags = DT_LEFT | DT_WORDBREAK; 
		break;
	}	
	// Added to expand tabs...
	if(strText.Find(_T('\t')) != -1)
		dwFlags |= DT_EXPANDTABS;
	// If the text centered make an assumtion that
	// the will want to center verticly as well
	if (style & SS_CENTERIMAGE){
		dwFlags = DT_CENTER;
		// Apply 
		if (strText.Find(_T("\r\n")) == -1){
			dwFlags |= DT_VCENTER;
			// And because DT_VCENTER only works with single lines
			dwFlags |= DT_SINGLELINE; 
		}
	}	
	dwFlags |= (DT_VCENTER|DT_END_ELLIPSIS); 


	COLORREF clrOldText;
	INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
	
	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));
	lf.lfQuality = PROOF_QUALITY;
	lf.lfHeight = 16;
	lf.lfWeight = 600;
	lstrcpy(lf.lfFaceName, _T("Arial"));
	CFont f;
	f.CreateFontIndirect(&lf);
	CFont * pOldFont = dc.SelectObject(&f);

	CRect rc;
	
	rc = rcClient;
	clrOldText = dc.SetTextColor( GetSysColor( COLOR_3DHILIGHT) );
	rc.top -= 3;
	dc.DrawText(strText,rc,dwFlags);
	
	rc = rcClient;
	rc.top -= 1;
	rc.left += 2;
	dc.SetTextColor(GetSysColor(COLOR_3DSHADOW));
	dc.DrawText(strText,rc,dwFlags);
	
	rc = rcClient;
	rc.left += 1;
	rc.top -= 2;
	dc.SetTextColor( RGB(0,0,255) );
	dc.DrawText(strText,rc,dwFlags);
	
	dc.SelectObject( pOldFont );
	f.DeleteObject();
	dc.SetBkMode( nOldBkMode );
	dc.SetTextColor( clrOldText );
}
