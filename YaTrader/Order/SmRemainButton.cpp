#include "stdafx.h"
#include "SmRemainButton.h"
#include "AbAccountOrderCenterWindow.h"
#include "../Global/SmTotalManager.h"
#include <format>

#include "../Event/SmCallbackManager.h"


#include <functional>

using namespace std;
using namespace std::placeholders;


BEGIN_MESSAGE_MAP(SmRemainButton, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

SmRemainButton::SmRemainButton()
{

}

SmRemainButton::~SmRemainButton()
{
	mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmRemainButton::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SmRemainButton::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void SmRemainButton::CreateResource()
{
	LOGFONT lf;
	globalData.fontRegular.GetLogFont(&lf);

	_Resource.TextFormat = CBCGPTextFormat(lf);

	_Resource.TextFormat.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	_Resource.TextFormat.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	_Resource.TextFormat.SetWordWrap();
	_Resource.TextFormat.SetClipText();
}

void SmRemainButton::SetUp()
{
	_Selected = false;
	CreateResource();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	SetTimer(1, 40, NULL);

	mainApp.CallbackMgr()->SubscribeQuoteCallback((long)this, std::bind(&SmRemainButton::OnQuoteEvent, this, _1));
	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SmRemainButton::OnOrderEvent, this, _1, _2));

}

void SmRemainButton::SetTextRect(const CRect& rc)
{
	_TextRect = rc;
}


void SmRemainButton::OnPaint()
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
		m_pGM->DrawLine(rect.right - 1, rect.top, rect.right - 1, rect.bottom, _Resource.BorderBrush);
		m_pGM->DrawLine(rect.left, rect.bottom - 1, rect.right, rect.bottom - 1, _Resource.BorderBrush);
	}
	if (_DrawText)  m_pGM->DrawText(_Text.c_str(), _TextRect, _Resource.TextFormat, _Resource.TextBrush);

	m_pGM->EndDraw();
}

void SmRemainButton::OnMouseMove(UINT nFlags, CPoint point)
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

void SmRemainButton::OnMouseHover(UINT nFlags, CPoint point)
{
	//AfxMessageBox(_T("OnMouseHover()"));
	_Hover = true;
	Invalidate();
	CBCGPStatic::OnMouseHover(nFlags, point);
}

void SmRemainButton::OnMouseLeave()
{
	//AfxMessageBox(_T("OnMouseLeave()"));

	m_bTrackMouse = FALSE;
	_Hover = false;
	Invalidate();
	CBCGPStatic::OnMouseLeave();
}

void SmRemainButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	//AfxMessageBox("OnLButtonDown");
	CWnd* wnd = GetParent();
	//wnd->SendMessage(UM_CHANGE_SYMBOL, _Index, 0);
	//_Selected ? _Selected = false : _Selected = true;
	int position_count = 0;
	/*
	if (_FundOrderCenterWnd) {
		position_count = _FundOrderCenterWnd->GetFundPositionCount();
		if (position_count > 0) {
			_FundOrderCenterWnd->SetOrderAmount(position_count);
		}
	}
	*/
	if (_OrderCenterWnd) {
		position_count = _OrderCenterWnd->GetPositionCount();
		if (position_count > 0) {
			_OrderCenterWnd->SetOrderAmount(position_count);
		}
	}
}

void SmRemainButton::OnLButtonUp(UINT nFlags, CPoint point)
{

}


void SmRemainButton::OnTimer(UINT_PTR nIDEvent)
{
	int position_count = 0;
// 	if (_FundOrderCenterWnd) {
// 		position_count = _FundOrderCenterWnd->GetFundPositionCount();
// 	}

	if (_OrderCenterWnd) {
		position_count = _OrderCenterWnd->GetPositionCount();
	}

	if (_EnableOrderShow || _EnableQuoteShow) {
		if (position_count != 0) {
			_Text = std::format("¿‹∞Ì  {0}", position_count);
			//_Selected = true;
		}
		else {
			_Text = "¿‹∞Ì"; //_Selected = false;
		}
		Invalidate();
		_EnableOrderShow = false;
		_EnableQuoteShow = false;
	}
}

void SmRemainButton::PreSubclassWindow()
{
	/*
	UINT uiHHeight = GetSystemMetrics(SM_CYHSCROLL);
	UINT uiVWidth = GetSystemMetrics(SM_CXVSCROLL);
	CRect rectClient, rectH, rectV;
	GetClientRect(rectClient);
	rectH = rectClient;
	rectH.top = rectH.bottom - uiHHeight;
	rectH.right -= uiVWidth;

	rectV = rectClient;
	rectV.left = rectV.right - uiVWidth;
	rectV.bottom -= uiHHeight;

	m_HScroll.Create(SBS_HORZ | SBS_BOTTOMALIGN | WS_CHILD | WS_VISIBLE, rectH, this, 1100);
	m_VScroll.Create(SBS_VERT | SBS_RIGHTALIGN | WS_CHILD | WS_VISIBLE, rectV, this, 1101);

	SCROLLINFO sci;
	memset(&sci, 0, sizeof(sci));

	sci.cbSize = sizeof(SCROLLINFO);
	sci.fMask = SIF_ALL;
	sci.nMax = 1000;
	sci.nPage = 200;

	m_HScroll.SetScrollInfo(&sci, TRUE);
	m_VScroll.SetScrollInfo(&sci, TRUE);
	*/
	CBCGPStatic::PreSubclassWindow();
}


void SmRemainButton::OnDestroy()
{
	CBCGPStatic::OnDestroy();

	KillTimer(1);
}
