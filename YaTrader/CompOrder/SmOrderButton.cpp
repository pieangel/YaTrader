#include "stdafx.h"
#include "SmOrderButton.h"
#include "SmOrderRightComp.h"
#include "SmFundCompRightDialog.h"

BEGIN_MESSAGE_MAP(SmOrderButton, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

SmOrderButton::SmOrderButton()
{

}

SmOrderButton::~SmOrderButton()
{
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmOrderButton::CreateResource()
{
	LOGFONT lf;
	globalData.fontRegular.GetLogFont(&lf);

	_Resource.TextFormat = CBCGPTextFormat(lf);

	_Resource.TextFormat.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	_Resource.TextFormat.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	_Resource.TextFormat.SetWordWrap();
	_Resource.TextFormat.SetClipText();
}

void SmOrderButton::SetUp()
{
	_Selected = false;
	CreateResource();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
}

void SmOrderButton::SetTextRect(const CRect& rc)
{
	_TextRect = rc;
}


void SmOrderButton::OnPaint()
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
		if (_Hover) {
			if (_Selected)
				m_pGM->FillRectangle(rect, _Resource.DownBrush);
			else
				m_pGM->FillRectangle(rect, _Resource.HoverBrush);
		}
		else
			m_pGM->FillRectangle(rect, _Resource.OrderBrush);
	}
	if (_DrawBorder) {
		m_pGM->DrawLine(rect.left, rect.top, rect.right, rect.top, _Resource.BorderBrush);
		m_pGM->DrawLine(rect.left, rect.top, rect.left, rect.bottom, _Resource.BorderBrush);
		m_pGM->DrawLine(rect.right - 1, rect.top, rect.right - 1, rect.bottom, _Resource.BorderBrush);
		m_pGM->DrawLine(rect.left, rect.bottom - 1, rect.right, rect.bottom - 1, _Resource.BorderBrush);
	}
	if (_DrawText)  m_pGM->DrawText(_Text.c_str(), _TextRect, _Resource.TextFormat, _Resource.TextBrush);

	m_pGM->EndDraw();
}

void SmOrderButton::OnMouseMove(UINT nFlags, CPoint point)
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

void SmOrderButton::OnMouseHover(UINT nFlags, CPoint point)
{
	//AfxMessageBox(_T("OnMouseHover()"));
	_Hover = true;
	Invalidate();
	CBCGPStatic::OnMouseHover(nFlags, point);
}

void SmOrderButton::OnMouseLeave()
{
	//AfxMessageBox(_T("OnMouseLeave()"));

	m_bTrackMouse = FALSE;
	_Hover = false;
	Invalidate();
	CBCGPStatic::OnMouseLeave();
}

void SmOrderButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	_Selected = true;
	Invalidate(TRUE);
	//AfxMessageBox("OnLButtonDown");
	if (_RightWnd) _RightWnd->DoOrder();
	if (_RightCompFund) _RightCompFund->DoOrder();
}

void SmOrderButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	_Selected = false;
	Invalidate(TRUE);
}


void SmOrderButton::PreSubclassWindow()
{

	CBCGPStatic::PreSubclassWindow();
}
