#include "stdafx.h"
#include "SmButton.h"

BEGIN_MESSAGE_MAP(SmButton, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

SmButton::SmButton()
{
	
}

SmButton::~SmButton()
{
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmButton::CreateResource()
{
	LOGFONT lf;
	globalData.fontRegular.GetLogFont(&lf);

	_Resource.TextFormat = CBCGPTextFormat(lf);

	_Resource.TextFormat.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	_Resource.TextFormat.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	_Resource.TextFormat.SetWordWrap();
	_Resource.TextFormat.SetClipText();
}

void SmButton::SetUp()
{
	_Selected = false;
	CreateResource();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
}

void SmButton::SetTextRect(const CRect& rc)
{
	_TextRect = rc;
}


void SmButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CBCGPMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rect;
	GetClientRect(rect);

	_TextRect = rect;

	if (m_pGM == NULL) return;

	m_pGM->BindDC(pDC, rect);

	if (!m_pGM->BeginDraw()) return;

	::DrawFocusRect(pDC->GetSafeHdc(), rect);

	if (_Fill) {
		if (_Hover)
			m_pGM->FillRectangle(rect, _Resource.HoverBrush);
		else if (_Selected)
			m_pGM->FillRectangle(rect, _Resource.SelectBrush);
		else
			m_pGM->FillRectangle(rect, _Resource.BackBrush);
	}
	if (_DrawBorder) {
		m_pGM->DrawLine(rect.left, rect.top, rect.right, rect.top, _Resource.BorderBrush);
		m_pGM->DrawLine(rect.left, rect.top, rect.left, rect.bottom, _Resource.BorderBrush);
		m_pGM->DrawLine(rect.right - 1, rect.top, rect.right -1, rect.bottom, _Resource.BorderBrush);
		m_pGM->DrawLine(rect.left, rect.bottom - 1, rect.right, rect.bottom - 1, _Resource.BorderBrush);
	}
	if (_DrawText)  m_pGM->DrawText(_Text.c_str(), _TextRect, _Resource.TextFormat, _Resource.TextBrush);

	m_pGM->EndDraw();
}

void SmButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTrackMouse)

	{

		TRACKMOUSEEVENT tme;

		tme.cbSize = sizeof(tme);

		tme.hwndTrack = m_hWnd;

		tme.dwFlags = TME_LEAVE | TME_HOVER;

		tme.dwHoverTime = 1;

		if (TrackMouseEvent(&tme))

		{

			m_bTrackMouse = TRUE;

		}

	}

	CBCGPStatic::OnMouseMove(nFlags, point);
}

void SmButton::OnMouseHover(UINT nFlags, CPoint point)
{
	//AfxMessageBox(_T("OnMouseHover()"));
	_Hover = true;
	Invalidate();
	CBCGPStatic::OnMouseHover(nFlags, point);
}

void SmButton::OnMouseLeave()
{
	//AfxMessageBox(_T("OnMouseLeave()"));

	m_bTrackMouse = FALSE;
	_Hover = false;
	Invalidate();
	CBCGPStatic::OnMouseLeave();
}

void SmButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	//AfxMessageBox("OnLButtonDown");
	CWnd* wnd = GetParent();
	wnd->SendMessage(UM_CHANGE_SYMBOL, _Index, 0);
}

void SmButton::OnLButtonUp(UINT nFlags, CPoint point)
{

}


void SmButton::PreSubclassWindow()
{
	
	CBCGPStatic::PreSubclassWindow();
}
