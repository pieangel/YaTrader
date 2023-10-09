#include "stdafx.h"
#include "SmFilledRemainButton.h"
#include "../Fund/SmFundOrderCenterWnd.h"
#include "../Global/SmTotalManager.h"
#include "../Fund/SmFundOrderArea.h"
#include <format>

#include "../Event/SmCallbackManager.h"


#include <functional>

using namespace std;
using namespace std::placeholders;

BEGIN_MESSAGE_MAP(SmFilledRemainButton, CBCGPStatic)
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

SmFilledRemainButton::SmFilledRemainButton()
{

}

SmFilledRemainButton::~SmFilledRemainButton()
{
	mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmFilledRemainButton::SetFundOrderArea(SmFundOrderArea* fund_order_area)
{
	_FundOrderArea = fund_order_area;
}

void SmFilledRemainButton::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SmFilledRemainButton::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void SmFilledRemainButton::CreateResource()
{
	LOGFONT lf;
	globalData.fontRegular.GetLogFont(&lf);

	_Resource.TextFormat = CBCGPTextFormat(lf);

	_Resource.TextFormat.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	_Resource.TextFormat.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	_Resource.TextFormat.SetWordWrap();
	_Resource.TextFormat.SetClipText();
}

void SmFilledRemainButton::SetUp()
{
	_Selected = false;
	CreateResource();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	SetTimer(1, 40, NULL);
	mainApp.CallbackMgr()->SubscribeQuoteCallback((long)this, std::bind(&SmFilledRemainButton::OnQuoteEvent, this, _1));
	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SmFilledRemainButton::OnOrderEvent, this, _1, _2));
}

void SmFilledRemainButton::SetTextRect(const CRect& rc)
{
	_TextRect = rc;
}


void SmFilledRemainButton::OnPaint()
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
			m_pGM->FillRectangle(rect, _Resource.FilledBrush);
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

void SmFilledRemainButton::OnMouseMove(UINT nFlags, CPoint point)
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

void SmFilledRemainButton::OnMouseHover(UINT nFlags, CPoint point)
{
	//AfxMessageBox(_T("OnMouseHover()"));
	_Hover = true;
	Invalidate();
	CBCGPStatic::OnMouseHover(nFlags, point);
}

void SmFilledRemainButton::OnMouseLeave()
{
	//AfxMessageBox(_T("OnMouseLeave()"));

	m_bTrackMouse = FALSE;
	_Hover = false;
	Invalidate();
	CBCGPStatic::OnMouseLeave();
}

void SmFilledRemainButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	
	//AfxMessageBox("OnLButtonDown");
	CWnd* wnd = GetParent();
	//wnd->SendMessage(UM_CHANGE_SYMBOL, _Index, 0);
	int position_count = 0;
	if (_FundOrderCenterWnd) {
		position_count = _FundOrderCenterWnd->GetFilledOrderCount();
	}
	if (position_count != 0) {
		_EnableFilledOrder ? _EnableFilledOrder = false : _EnableFilledOrder = true;
		_EnableFilledOrder ? _Selected = true : _Selected = false;
		if (_FundOrderArea) {
			_FundOrderArea->EnableFilledOrder(_EnableFilledOrder);
		}
	}
	else {
		_EnableFilledOrder = false;
		_FundOrderArea->EnableFilledOrder(_EnableFilledOrder);
		_Selected = false;
	}
}

void SmFilledRemainButton::OnLButtonUp(UINT nFlags, CPoint point)
{

}


void SmFilledRemainButton::OnTimer(UINT_PTR nIDEvent)
{
	int position_count = 0;
	if (_FundOrderCenterWnd) {
		position_count = _FundOrderCenterWnd->GetFilledOrderCount();
	}

	if (_EnableOrderShow || _EnableQuoteShow) {
		if (position_count != 0) {
			_Text = std::format("√º∞·µ»¿‹∞Ì  {0}", position_count);
			//_Selected = true;
		}
		else {
			_EnableFilledOrder = false;
			if (_FundOrderArea)
				_FundOrderArea->EnableFilledOrder(_EnableFilledOrder);
			_Selected = false;
			_Text = "√º∞·µ»¿‹∞Ì"; //_Selected = false;
		}
		Invalidate();
		_EnableOrderShow = false;
		_EnableQuoteShow = false;
	}
}

void SmFilledRemainButton::PreSubclassWindow()
{
	
	CBCGPStatic::PreSubclassWindow();
}


void SmFilledRemainButton::OnDestroy()
{
	CBCGPStatic::OnDestroy();

	KillTimer(1);
}
