//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a sample for BCGControlBar Library Professional Edition
// Copyright (C) 1998-2020 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// CustomCells.cpp: implementation of the CColorGridItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../DarkHorse.h"
#include "CustomCells.h"
#include "BCGPGridCtrl.h"
#include "BCGPDrawManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	BCGPGRIDCTRL_ID_INPLACE	1
#define BCGPGRIDCTRL_ID_INPLACE_COMBO (SHRT_MAX - 1)

#define PROP_HAS_LIST	0x0001
#define PROP_HAS_BUTTON	0x0002
#define PROP_HAS_SPIN	0x0004
#define PROP_AUTOGROUP	0x0100

//////////////////////////////////////////////////////////////////////
// CPasswordItem Class

CPasswordItem::CPasswordItem (const CString& strPassword, TCHAR cPassword) :
	CBCGPGridItem ((LPCTSTR) strPassword)
{
	m_cPassword = cPassword;
}
//******************************************************************************
CWnd* CPasswordItem::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
{
	CBCGPGridCtrl* pWndList = GetOwnerList ();
	ASSERT_VALID (pWndList);

	CBCGPEdit* pWndEdit = new CBCGPEdit;

	if (pWndList != NULL && pWndList->IsVisualManagerStyle())
	{
		pWndEdit->m_bVisualManagerStyle = TRUE;
	}

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_PASSWORD;

	switch (GetAlign())
	{
	case HDF_RIGHT:
		dwStyle |= ES_RIGHT;
		break;
		
	case HDF_CENTER:
		dwStyle |= ES_CENTER;
		break;
	}

	if (!m_bEnabled || !m_bAllowEdit)
	{
		dwStyle |= ES_READONLY;
	}

	pWndEdit->Create (dwStyle, rectEdit, pWndList, BCGPGRIDCTRL_ID_INPLACE);
	pWndEdit->EnablePasswordPreview();
	pWndEdit->SetPasswordChar (m_cPassword);

	pWndEdit->SetWindowText((LPCTSTR)(_bstr_t)GetValue());
	bDefaultFormat = FALSE;

	return pWndEdit;
}
//******************************************************************************
CString CPasswordItem::FormatItem ()
{
	CString strVal = (LPCTSTR)(_bstr_t)m_varValue;
	
	for (int i = 0; i < strVal.GetLength (); i++)
	{
		strVal.SetAt (i, m_cPassword);
	}

	return strVal;
}
//******************************************************************************
void CPasswordItem::WriteToArchive(CArchive& ar)
{
	if (m_varValue.vt != VT_BSTR)
	{
		CBCGPGridItem::WriteToArchive (ar);
		return;
	}

	// Copying real item's value is not allowed.
	// Serialize all visible characters using the password character
	_variant_t varOld = m_varValue;
	m_varValue = (LPCTSTR) FormatItem ();
	
	CBCGPGridItem::WriteToArchive (ar);
	
	m_varValue = varOld;
}

/////////////////////////////////////////////////////////////////////////////
// CIitemSliderCtrl

IMPLEMENT_DYNAMIC(CItemSliderCtrl, CBCGPSliderCtrl)

CItemSliderCtrl::CItemSliderCtrl(CSliderItem* pItem, COLORREF clrBack)
{
	m_clrBack = clrBack;
	m_brBackground.CreateSolidBrush (m_clrBack);
	m_pItem = pItem;
	m_bDrawFocus = FALSE;
	m_bVisualManagerStyle = TRUE;
}
//******************************************************************************
CItemSliderCtrl::~CItemSliderCtrl()
{
}

BEGIN_MESSAGE_MAP(CItemSliderCtrl, CBCGPSliderCtrl)
	//{{AFX_MSG_MAP(CItemSliderCtrl)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemSliderCtrl message handlers

HBRUSH CItemSliderCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/) 
{
	pDC->SetBkColor (m_clrBack);
	return m_brBackground;
}
//************************************************************************************
void CItemSliderCtrl::Redraw()
{
	ASSERT_VALID (m_pItem);

	long lCurrValue = (long) m_pItem->GetValue();
	long lNewValue = (long)GetPos ();

	if (lCurrValue != lNewValue)
	{
		m_pItem->SetValue (lNewValue);
	}
}

////////////////////////////////////////////////////////////////////////////////
// CSliderItem class

CSliderItem::CSliderItem (long lValue)
	: CBCGPGridItem  (lValue)
{
}
//************************************************************************************
CWnd* CSliderItem::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
{
	CBCGPGridCtrl* pWndList = GetOwnerList ();
	ASSERT_VALID (pWndList);
	
	CRect rectSpin;
	AdjustInPlaceEditRect (rectEdit, rectSpin);

	CClientDC dc(pWndList);
	CBCGPFontSelector fs(dc, pWndList->GetFont());

	CString strVal(_T("100"));
	const int cxVal = dc.GetTextExtent(strVal).cx;

	if (GetAlign() == HDF_RIGHT)
	{
		rectEdit.right -= cxVal;
	}
	else
	{
		rectEdit.left += cxVal;
	}

	CItemSliderCtrl* pWndSlider = new CItemSliderCtrl(this, pWndList->GetBkColor ());

	pWndSlider->EnableProgressMode();
	pWndSlider->Create (WS_VISIBLE | WS_CHILD | TBS_NOTICKS | TBS_BOTH, rectEdit, pWndList, BCGPGRIDCTRL_ID_INPLACE);
	pWndSlider->SetPos ((long) m_varValue);
	pWndSlider->m_bVisualManagerStyle = TRUE;
	
	bDefaultFormat = FALSE;
	
	return pWndSlider;
}
//******************************************************************************************
BOOL CSliderItem::OnUpdateValue ()
{
	ASSERT_VALID (this);

	ASSERT_VALID (m_pWndInPlace);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));

	CItemSliderCtrl* pWndSlider = (CItemSliderCtrl*) m_pWndInPlace;

	long lNewValue = (long) pWndSlider->GetPos ();
	long lCurrValue = m_varValue;

	if (lCurrValue != lNewValue)
	{
		SetValue (lNewValue);
	}

	return TRUE;
}
//************************************************************************************
void CSliderItem::SetValue (const _variant_t& varValue)
{
	ASSERT_VALID (this);

	if (m_varValue.vt != VT_EMPTY && m_varValue.vt != varValue.vt)
	{
		ASSERT (FALSE);
		return;
	}

	m_varValue = varValue;
	SetItemChanged ();
	Redraw ();
}
//************************************************************************************
int CSliderItem::GetAlign () const
{
	int nAlign = CBCGPGridItem::GetAlign();
	if (nAlign == HDF_CENTER)
	{
		nAlign = HDF_LEFT;
	}

	return nAlign;
}

/////////////////////////////////////////////////////////////////////////////
// CFileItem Class

CFileItem::CFileItem(const CString& strFileName) :
	CBCGPGridItem (_variant_t((LPCTSTR) strFileName))
{
	m_dwFlags = PROP_HAS_BUTTON;
}
//****************************************************************************************
void CFileItem::OnClickButton (CPoint /*point*/)
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList();

	pGridCtrl->EndEditItem(TRUE);

	m_bButtonIsDown = TRUE;
	Redraw ();

	CString strPath = (LPCTSTR)(_bstr_t)m_varValue;

#ifdef _BCGSUITE_INC_
	CFileDialog dlg(TRUE, NULL, strPath);
#else
	CBCGPFileDialogEx dlg(TRUE, NULL, strPath);
#endif

	if (dlg.DoModal () == IDOK)
	{
		strPath = dlg.GetPathName ();
		m_varValue = (LPCTSTR) strPath;
	}

	m_bButtonIsDown = FALSE;

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetWindowText (strPath);
		m_pWndInPlace->SetFocus ();
	}
	else
	{
		pGridCtrl->SetFocus ();
	}

	SetItemChanged ();
	Redraw ();
}

/////////////////////////////////////////////////////////////////////////////
// CFolderItem Class

CFolderItem::CFolderItem(const CString& strFolderName) :
	CBCGPGridItem (_variant_t((LPCTSTR) strFolderName))
{
	m_dwFlags = PROP_HAS_BUTTON;
}
//****************************************************************************************
void CFolderItem::OnClickButton (CPoint /*point*/)
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList ();

	pGridCtrl->EndEditItem(TRUE);

	m_bButtonIsDown = TRUE;
	Redraw ();

	CString strPath = (LPCTSTR)(_bstr_t)m_varValue;

	if (theApp.GetShellManager ()->BrowseForFolder (strPath, pGridCtrl, strPath, NULL, BIF_RETURNONLYFSDIRS, NULL
#ifndef _BCGSUITE_INC_
		, TRUE
#endif
		))
	{
		m_varValue = (LPCTSTR) strPath;
	}

	m_bButtonIsDown = FALSE;

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetWindowText (strPath);
		m_pWndInPlace->SetFocus ();
	}
	else
	{
		pGridCtrl->SetFocus ();
	}

	SetItemChanged ();
	Redraw ();
}

/////////////////////////////////////////////////////////////////////////////
// FontItem Class

CFontItem::CFontItem(LOGFONT& lf) :
	CBCGPGridItem (_variant_t((LPCTSTR)lf.lfFaceName))
{
	m_lf = lf;
	m_lfOrig = lf;
	m_dwFlags = PROP_HAS_BUTTON;
}
//*****************************************************************************************
void CFontItem::OnClickButton (CPoint /*point*/)
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList();
	ASSERT_VALID (pGridCtrl);

	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndInPlace);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));

	m_bButtonIsDown = TRUE;
	Redraw ();

	CFontDialog dlg (&m_lf);

	if (m_Color != (COLORREF)-1)
	{
		dlg.m_cf.rgbColors = m_Color;
	}

	if (dlg.DoModal () == IDOK)
	{
		dlg.GetCurrentFont (&m_lf);
		m_Color = dlg.GetColor ();

		if (m_pWndInPlace != NULL)
		{
			m_pWndInPlace->SetWindowText (FormatProperty ());
		}
		else
		{
			m_varValue = (LPCTSTR) FormatProperty ();
		}
	}

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus ();
	}
	else
	{
		pGridCtrl->SetFocus ();
	}

	SetItemChanged ();

	m_bButtonIsDown = FALSE;
	Redraw ();
}
//*****************************************************************************
CString CFontItem::FormatProperty ()
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList ();
	ASSERT_VALID (pGridCtrl);

	CWindowDC dc (pGridCtrl);

	int nLogY = dc.GetDeviceCaps (LOGPIXELSY);

	CString str;

	if (nLogY != 0)
	{
		str.Format( _T("%s(%i)"), m_lf.lfFaceName, 
			MulDiv (72, -m_lf.lfHeight, nLogY));
	}
	else
	{
		str = m_lf.lfFaceName;
	}

	return str;
}

////////////////////////////////////////////////////////////////////////////////
// CIconGridItem class

CIconGridItem::CIconGridItem(const CImageList& imageListIcons, int nItem)
	: CBCGPGridItem ("")
{
	m_nItem = nItem;

	m_imageListIcons.CreateFromImageList (imageListIcons);
	m_imageListIcons.SetTransparentColor (::GetSysColor (COLOR_3DFACE));
	
	globalUtils.ScaleByDPI(m_imageListIcons);

	AllowEdit (FALSE);
}
//***********************************************************************
void CIconGridItem::OnDrawValue (CDC* pDC, CRect rect)
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList();
	ASSERT_VALID (pGridCtrl);

	ASSERT_VALID (pDC);

	COLORREF clrTextOld = (COLORREF)-1;
	COLORREF clrText = OnFillBackground (pDC, rect);

	if (clrText != (COLORREF)-1)
	{
		clrTextOld = pDC->SetTextColor (clrText);
	}

	const double dblScale = pGridCtrl->GetScale();

	if (dblScale != 1.0)
	{
		CSize sizeImage = m_imageListIcons.GetImageSize ();
		sizeImage.cx = bcg_scale(sizeImage.cx, dblScale);
		sizeImage.cy = bcg_scale(sizeImage.cy, dblScale);
		
		CRect rectImage = CRect(
			rect.CenterPoint().x - sizeImage.cx / 2, rect.CenterPoint().y - sizeImage.cy / 2,
			rect.CenterPoint().x + sizeImage.cx / 2, rect.CenterPoint().y + sizeImage.cy / 2);

		m_imageListIcons.DrawEx (pDC, rectImage, m_nItem,
			CBCGPToolBarImages::ImageAlignHorzStretch, CBCGPToolBarImages::ImageAlignVertStretch);
	}
	else
	{
		m_imageListIcons.DrawEx (pDC, rect, m_nItem,
			CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
	}

	if (clrTextOld != (COLORREF)-1)
	{
		pDC->SetTextColor (clrTextOld);
	}
}
//*****************************************************************************************
void CIconGridItem::OnPrintValue (CDC* pDC, CRect rect)
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList ();
	ASSERT_VALID (pGridCtrl);

	// map to printer metrics
	HDC hDCFrom = ::GetDC(NULL);
	int nXMul = pDC->GetDeviceCaps(LOGPIXELSX); // pixels in print dc
	int nXDiv = ::GetDeviceCaps(hDCFrom, LOGPIXELSX); // pixels in screen dc
	::ReleaseDC(NULL, hDCFrom);
	
	const double dblScale = ::MulDiv (1, nXMul, nXDiv);
	const CRect& rectClip = pGridCtrl->GetPrintParams().m_pageInfo.m_rectPageItems;

	CSize sizeImage = m_imageListIcons.GetImageSize ();
	sizeImage.cx = bcg_scale(sizeImage.cx, dblScale);
	sizeImage.cy = bcg_scale(sizeImage.cy, dblScale);
	
	CRect rectImage = CRect(
		rect.CenterPoint().x - sizeImage.cx / 2, rect.CenterPoint().y - sizeImage.cy / 2,
		rect.CenterPoint().x + sizeImage.cx / 2, rect.CenterPoint().y + sizeImage.cy / 2);
	
	rectImage.NormalizeRect ();
	if (rectImage.IntersectRect (&rectImage, &rectClip))
	{
		m_imageListIcons.DrawEx (pDC, rectImage, m_nItem,
			CBCGPToolBarImages::ImageAlignHorzStretch, CBCGPToolBarImages::ImageAlignVertStretch);
	}
}

////////////////////////////////////////////////////////////////////////////////
// CIconListItem class

CIconListItem::CIconListItem(const _variant_t varValue, const CImageList& imageListIcons,
							CStringList* plstIconNames)
	: CBCGPGridItem (varValue)
{
	m_imageListIcons.CreateFromImageList (imageListIcons);
	m_imageListIcons.SetTransparentColor (::GetSysColor (COLOR_3DFACE));

	globalUtils.ScaleByDPI(m_imageListIcons);

	if (plstIconNames != NULL)
	{
		m_lstIconNames.AddTail (plstIconNames);
		ASSERT (m_lstIconNames.GetCount () == m_imageListIcons.GetCount ());
	}

	for (int i = 0; i < m_imageListIcons.GetCount (); i++)
	{
		CString strItem;
		strItem.Format (_T("%d"), i);

		AddOption (strItem);
	}

	AllowEdit(TRUE);
}
//*****************************************************************************************
CComboBox* CIconListItem::CreateCombo (CWnd* pWndParent, CRect rect)
{
	rect.bottom = rect.top + 400;

	CIconCombo* pWndCombo = new CIconCombo (m_imageListIcons, m_lstIconNames);

	CBCGPGridCtrl* pWndList = GetOwnerList ();
	if (pWndList != NULL && pWndList->IsVisualManagerStyle())
	{
		pWndCombo->m_bVisualManagerStyle = TRUE;
	}

	if (!pWndCombo->Create (WS_CHILD | CBS_NOINTEGRALHEIGHT | CBS_DROPDOWNLIST | WS_VSCROLL | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS,
		rect, pWndParent, BCGPGRIDCTRL_ID_INPLACE_COMBO))
	{
		delete pWndCombo;
		return NULL;
	}

	pWndCombo->SetIconListItem(this);
	return pWndCombo;
}
//*****************************************************************************************
void CIconListItem::OnDrawValue (CDC* pDC, CRect rect)
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList();
	ASSERT_VALID (pGridCtrl);

	ASSERT_VALID (pDC);

	CString strVal = (LPCTSTR)(_bstr_t) m_varValue;
	if (strVal.IsEmpty () || m_imageListIcons.GetCount () == 0)
	{
		return;
	}
	
	int nIndex = -1;
#if _MSC_VER < 1400
	_stscanf (strVal, _T("%d"), &nIndex);
#else
	_stscanf_s (strVal, _T("%d"), &nIndex);
#endif
	if (nIndex < 0)
	{
		return;
	}

	COLORREF clrTextOld = (COLORREF)-1;
	COLORREF clrText = OnFillBackground (pDC, rect);

	if (clrText != (COLORREF)-1)
	{
		clrTextOld = pDC->SetTextColor (clrText);
	}

	CString strLabel;
	CFont* pOldFont = NULL;
	if (!m_lstIconNames.IsEmpty ())
	{
		pOldFont = pDC->SelectObject (pGridCtrl->GetFont ());
		strLabel = m_lstIconNames.GetAt (m_lstIconNames.FindIndex (nIndex));
	}

	rect.right -= globalUtils.ScaleByDPI(pGridCtrl->GetTextMargin());
	if (!m_rectButton.IsRectEmpty())
	{
		rect.right -= m_rectButton.Width();
	}
	
	const double dblScale = pGridCtrl->GetScale();
	const int cxImage = pGridCtrl->GetBaseHeight() + 2 * bcg_scale(globalUtils.ScaleByDPI(pGridCtrl->GetImageMargin()), dblScale);

	CRect rectImage = rect;

	const int cxSpace = rect.Width() - (cxImage + pDC->GetTextExtent(strLabel).cx);
	if (cxSpace > 0)
	{
		switch(GetAlign())
		{
		case HDF_RIGHT:
			rectImage.left += cxSpace;
			break;

		case HDF_CENTER:
			rectImage.left += cxSpace / 2;
			break;
		}
	}
	rectImage.right = rectImage.left + cxImage;

	if (dblScale != 1.0)
	{
		CSize sizeImage = m_imageListIcons.GetImageSize ();
		sizeImage.cx = bcg_scale(sizeImage.cx, dblScale);
		sizeImage.cy = bcg_scale(sizeImage.cy, dblScale);
		
		CRect rectImage1 = CRect(
			rectImage.CenterPoint().x - sizeImage.cx / 2, rectImage.CenterPoint().y - sizeImage.cy / 2,
			rectImage.CenterPoint().x + sizeImage.cx / 2, rectImage.CenterPoint().y + sizeImage.cy / 2);
		
		m_imageListIcons.DrawEx (pDC, rectImage1, nIndex,
			CBCGPToolBarImages::ImageAlignHorzStretch, CBCGPToolBarImages::ImageAlignVertStretch);
	}
	else
	{
		m_imageListIcons.DrawEx (pDC, rectImage, nIndex,
			CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
	}

	if (!strLabel.IsEmpty())
	{
		rect.left = rectImage.right;

		pDC->DrawText (strLabel, rect, DT_SINGLELINE | DT_VCENTER);
	}

	if (pOldFont->GetSafeHandle() != NULL)
	{
		pDC->SelectObject (pOldFont);
	}

	m_bValueIsTrancated = FALSE;

	if (clrTextOld != (COLORREF)-1)
	{
		pDC->SetTextColor (clrTextOld);
	}
}
//*****************************************************************************************
void CIconListItem::OnPrintValue (CDC* pDC, CRect rect)
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList ();
	ASSERT_VALID (pGridCtrl);

	CString strVal = (LPCTSTR)(_bstr_t) m_varValue;
	if (strVal.IsEmpty () || m_imageListIcons.GetCount () == 0)
	{
		return;
	}

	int nIndex = -1;
#if _MSC_VER < 1400
	_stscanf (strVal, _T("%d"), &nIndex);
#else
	_stscanf_s (strVal, _T("%d"), &nIndex);
#endif
	if (nIndex < 0)
	{
		return;
	}

	// map to printer metrics
	HDC hDCFrom = ::GetDC(NULL);
	int nXMul = pDC->GetDeviceCaps(LOGPIXELSX); // pixels in print dc
	int nXDiv = ::GetDeviceCaps(hDCFrom, LOGPIXELSX); // pixels in screen dc
	::ReleaseDC(NULL, hDCFrom);
	
	const int CALCULATED_TEXT_MARGIN = ::MulDiv (pGridCtrl->GetTextMargin(), nXMul, nXDiv);
	const double dblScale = ::MulDiv (1, nXMul, nXDiv);
	const CRect& rectClip = pGridCtrl->GetPrintParams().m_pageInfo.m_rectPageItems;
	const int nPrintIconMargin = 2;

	// Draw icon:
	CRect rectImage = rect;
	rectImage.right = rectImage.left + pGridCtrl->GetPrintParams().m_nButtonWidth + 2 * bcg_scale(nPrintIconMargin, dblScale);

	CSize sizeImage = m_imageListIcons.GetImageSize ();
	sizeImage.cx = bcg_scale(sizeImage.cx, dblScale);
	sizeImage.cy = bcg_scale(sizeImage.cy, dblScale);
	
	CRect rectImage1 = CRect(
		rectImage.CenterPoint().x - sizeImage.cx / 2, rectImage.CenterPoint().y - sizeImage.cy / 2,
		rectImage.CenterPoint().x + sizeImage.cx / 2, rectImage.CenterPoint().y + sizeImage.cy / 2);
	
	rectImage1.NormalizeRect ();
	if (rectImage1.IntersectRect (&rectImage1, &rectClip))
	{
		m_imageListIcons.DrawEx (pDC, rectImage1, nIndex,
			CBCGPToolBarImages::ImageAlignHorzStretch, CBCGPToolBarImages::ImageAlignVertStretch);
	}

	// Draw text:
	COLORREF clrTextOld = pDC->SetTextColor (pGridCtrl->GetPrintTextColor());

	CRect rectText = rect;
	rectText.left = rectImage1.right;
	rectText.DeflateRect (CALCULATED_TEXT_MARGIN, 0);

	CRect rectClipText = rectText;
	rectClipText.NormalizeRect ();
	if (rectClipText.IntersectRect (&rectClipText, &rectClip))
	{
		CString strText = m_lstIconNames.GetAt (m_lstIconNames.FindIndex (nIndex));

		UINT uiTextFlags = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
		pGridCtrl->DoDrawText (pDC, strText, rectText, uiTextFlags, rectClipText);
	}
	
	pDC->SetTextColor (clrTextOld);
}
//*****************************************************************************************
CWnd* CIconListItem::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
{
	CWnd* pWnd = CBCGPGridItem::CreateInPlaceEdit (rectEdit, bDefaultFormat);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow (SW_HIDE);

		CEdit* pWndEdit = DYNAMIC_DOWNCAST (CEdit, pWnd);
		if (pWndEdit != NULL)
		{
			pWndEdit->SetReadOnly (TRUE);
		}
	}

	return pWnd;
}
//*****************************************************************************************
void CIconListItem::OnSelectCombo ()
{
	CIconCombo* pWndCombo = (CIconCombo*) m_pWndCombo;
	if (pWndCombo != NULL)
	{
		SetValue ((long) pWndCombo->GetCurSel ());
	}

	CBCGPGridItem::OnSelectCombo ();
}

/////////////////////////////////////////////////////////////////////////////
// CIconCombo

CIconCombo::CIconCombo(CBCGPToolBarImages& imageListIcons, CStringList& lstIconNames) :
	m_imageListIcons (imageListIcons),
	m_lstIconNames (lstIconNames)
{
	m_pIconListItem = NULL;
}
//*****************************************************************************************
CIconCombo::~CIconCombo()
{
}

BEGIN_MESSAGE_MAP(CIconCombo, CBCGPComboBox)
	//{{AFX_MSG_MAP(CIconCombo)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnCbnSelEndOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconCombo message handlers

void CIconCombo::OnDrawItem(int /*nIDCtl*/, LPDRAWITEMSTRUCT lpDIS) 
{
	CDC* pDC = CDC::FromHandle (lpDIS->hDC);
	ASSERT_VALID (pDC);

	CRect rect = lpDIS->rcItem;
	int nIcon = lpDIS->itemID;

	COLORREF clrText = OnFillLbItem(pDC, nIcon, rect, FALSE, lpDIS->itemState & ODS_SELECTED); 

	if (nIcon < 0)
	{
		return;
	}

	int nImageMargin = 0;
	
	if (m_pIconListItem != NULL)
	{
		CBCGPGridCtrl* pGridCtrl = m_pIconListItem->GetOwnerList();
		if (pGridCtrl != NULL)
		{
			nImageMargin = globalUtils.ScaleByDPI(pGridCtrl->GetImageMargin());
		}
	}

	rect.left += nImageMargin;

	m_imageListIcons.DrawEx (pDC, rect, nIcon,
		CBCGPToolBarImages::ImageAlignHorzLeft, CBCGPToolBarImages::ImageAlignVertCenter);

	if (!m_lstIconNames.IsEmpty ())
	{
		CString str = m_lstIconNames.GetAt (m_lstIconNames.FindIndex (nIcon));
		CFont* pOldFont = pDC->SelectObject (&globalData.fontRegular);

		pDC->SetBkMode (TRANSPARENT);

		if (clrText != (COLORREF)-1)
		{
			pDC->SetTextColor (clrText);
		}

		CRect rectText = rect;
		rectText.left += m_imageListIcons.GetImageSize ().cx + nImageMargin;

		pDC->DrawText (str, rectText, DT_SINGLELINE | DT_VCENTER);

		pDC->SelectObject (pOldFont);
	}
}
//*****************************************************************************************
void CIconCombo::OnMeasureItem(int /*nIDCtl*/, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	int nTextWidth = 0;
	int nTextHeight = 0;

	int nImageMargin = 0;
	
	if (m_pIconListItem != NULL)
	{
		CBCGPGridCtrl* pGridCtrl = m_pIconListItem->GetOwnerList();
		if (pGridCtrl != NULL)
		{
			nImageMargin = globalUtils.ScaleByDPI(pGridCtrl->GetImageMargin());
		}
	}

	if (!m_lstIconNames.IsEmpty ())
	{
		nTextHeight = globalData.GetTextHeight ();

		CClientDC dc (this);
		CFont* pOldFont = dc.SelectObject (&globalData.fontRegular);

		for (POSITION pos = m_lstIconNames.GetHeadPosition (); pos != NULL;)
		{
			CString str = m_lstIconNames.GetNext (pos);

			nTextWidth = max (nTextWidth, dc.GetTextExtent (str).cx + nImageMargin);
		}

		dc.SelectObject (pOldFont);
	}

	lpMeasureItemStruct->itemWidth = m_imageListIcons.GetImageSize ().cx + nTextWidth + 3 * nImageMargin;
	lpMeasureItemStruct->itemHeight = max(GetDropDownItemMinHeight(), max (nTextHeight, m_imageListIcons.GetImageSize ().cy + 2 * nImageMargin));
}
//*****************************************************************************************
void CIconCombo::OnCbnSelEndOk ()
{
	GetIconListItem ()->OnSelectCombo ();
}

////////////////////////////////////////////////////////////////////////////////
// CProgressItem Class

CProgressItem::CProgressItem (long lValue, long lTotal) : 
	CBCGPGridItem (lValue)
{
	m_bAllowEdit = FALSE;
	m_lTotal = lTotal;
}
//*****************************************************************************************
void CProgressItem::OnDrawValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);

	CBCGPGridCtrl* pGridCtrl = GetOwnerList();
	ASSERT_VALID (pGridCtrl);

	COLORREF clrTextOld = (COLORREF)-1;
	COLORREF clrText = OnFillBackground (pDC, rect);

	if (clrText != (COLORREF)-1)
	{
		clrTextOld = pDC->SetTextColor (clrText);
	}

	OnDrawIcon (pDC, rect);

	long nProgressCurr = (long) m_varValue;

	CString strText;
	strText.Format (_T("%d%%"), nProgressCurr * 100 / m_lTotal);

	rect.DeflateRect(globalUtils.ScaleByDPI(pGridCtrl->GetTextMargin()), 0);
	
	CRect rectProgress = rect;

	pDC->DrawText (strText, rect, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

	CString strMax = _T("100%");
	rectProgress.left += pDC->GetTextExtent (strMax).cx;

	rectProgress.DeflateRect(2, 4);

	if (rectProgress.right - rectProgress.left > 10)
	{
		CRect rectComplete = rectProgress;
		rectComplete.DeflateRect (1, 1);

		rectComplete.right = rectComplete.left + 
			nProgressCurr * rectComplete.Width () / m_lTotal;

#ifdef _BCGSUITE_INC_
		CMFCRibbonProgressBar dummy;
		dummy.SetRange(0, m_lTotal);
		dummy.SetPos(nProgressCurr, FALSE);

		CMFCVisualManager::GetInstance ()->OnDrawRibbonProgressBar (
				pDC, &dummy, rectProgress, rectComplete, FALSE);
#else
		CBCGPRibbonProgressBar dummy;
		dummy.SetRange(0, m_lTotal);
		dummy.SetPos(nProgressCurr, FALSE);

		CBCGPVisualManager::GetInstance ()->OnDrawRibbonProgressBar (
				pDC, &dummy, rectProgress, rectComplete, FALSE);
#endif
	}

	if (clrTextOld != (COLORREF)-1)
	{
		pDC->SetTextColor (clrTextOld);
	}
}

////////////////////////////////////////////////////////////////////////////////
// CButtonItem Class

CButtonItem::CButtonItem (LPCTSTR lpszText, UINT id) :
	CBCGPGridItem (lpszText)
{
	m_bAllowEdit = FALSE;
	m_id = id;
}
//*****************************************************************************************
void CButtonItem::OnDrawValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);

	OnFillBackground (pDC, rect);

	const int nMargin = globalUtils.ScaleByDPI(1);
	//rect.DeflateRect(nMargin, nMargin);

	COLORREF clrText = globalData.clrBarText;

#ifndef _BCGSUITE_INC_
	CBCGPGridCtrl* pWndList = GetOwnerList ();
	if (pWndList != NULL && pWndList->IsVisualManagerStyle())
	{
		CBCGPVisualManager::GetInstance()->OnDrawPushButton(pDC, rect, NULL, clrText);
	}
	else
	{
		pDC->FillRect (rect, &globalData.brBtnFace);
		
		pDC->Draw3dRect (rect, globalData.clrBtnLight, globalData.clrBtnDkShadow);
		rect.DeflateRect (1, 1);
		pDC->Draw3dRect (rect, globalData.clrBtnHilite, globalData.clrBtnShadow);

		clrText = globalData.clrBtnText;
	}
#else
	pDC->FillRect (rect, &globalData.brBarFace);

	pDC->Draw3dRect (rect, globalData.clrBarLight, globalData.clrBarDkShadow);
	rect.DeflateRect (1, 1);
	pDC->Draw3dRect (rect, globalData.clrBarHilite, globalData.clrBarShadow);
#endif

	COLORREF clrTextOld = pDC->SetTextColor(clrText);

	CString strText = (LPCTSTR)(_bstr_t) m_varValue;
	pDC->DrawText (strText, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

	pDC->SetTextColor (clrTextOld);
}
//*****************************************************************************************
BOOL CButtonItem::OnClickValue (UINT uiMsg, CPoint point)
{
	if (uiMsg != WM_LBUTTONDOWN)
	{
		return CBCGPGridItem::OnClickValue (uiMsg, point);
	}

	CBCGPGridCtrl* pGridCtrl = GetOwnerList();
	ASSERT_VALID (pGridCtrl);

	pGridCtrl->SendMessage (WM_COMMAND,
							MAKEWPARAM (m_id, BN_CLICKED));

	return TRUE;
}
//*****************************************************************************************
BOOL CButtonItem::PushChar (UINT nChar)
{
	if (nChar == VK_SPACE || nChar == VK_RETURN)
	{
		CBCGPGridCtrl* pGridCtrl = GetOwnerList();
		ASSERT_VALID (pGridCtrl);
		
		pGridCtrl->SendMessage (WM_COMMAND,
			MAKEWPARAM (m_id, BN_CLICKED));

		return TRUE;
	}
	
	return CBCGPGridItem::PushChar(nChar);
}


////////////////////////////////////////////////////////////////////////////////
// CButtonItem Class

CRadioButtonItem::CRadioButtonItem(LPCTSTR lpszText, UINT id) :
	CBCGPGridItem(lpszText)
{
	m_bAllowEdit = FALSE;
	m_id = id;

}
//*****************************************************************************************
void CRadioButtonItem::OnDrawValue(CDC* pDC, CRect rect)
{
	ASSERT_VALID(pDC);

	OnFillBackground(pDC, rect);

	const int nMargin = globalUtils.ScaleByDPI(1);
	//rect.DeflateRect(nMargin, nMargin);

	COLORREF clrText = globalData.clrBarText;

#ifndef _BCGSUITE_INC_
	CBCGPGridCtrl* pWndList = GetOwnerList();
	if (pWndList != NULL && pWndList->IsVisualManagerStyle())
	{
		//CBCGPVisualManager::GetInstance()->OnDrawPushButton(pDC, rect, NULL, clrText);

		CRect rect_button;
		rect_button.left = rect.left;
		rect_button.right = rect_button.left + rect.Height();
		rect_button.top = rect.top;
		rect_button.bottom = rect.bottom;

		rect_button.DeflateRect(5, 5);

		CBCGPDrawManager dm(*pDC);
		if (_Active) {
			dm.DrawEllipse(rect_button, RGB(255, 0, 0), RGB(255, 255, 255));
		}
		else {
			dm.DrawEllipse(rect_button, RGB(67, 67, 67), RGB(255, 255, 255));
		}

		//clrText = globalData.clrBtnText;
	}
	else
	{
		//pDC->FillRect(rect, &globalData.brBtnFace);

		//pDC->Draw3dRect(rect, globalData.clrBtnLight, globalData.clrBtnDkShadow);
		//rect.DeflateRect(1, 1);
		//pDC->Draw3dRect(rect, globalData.clrBtnHilite, globalData.clrBtnShadow);

		CRect rect_button;
		rect_button.left = rect.left;
		rect_button.right = rect_button.left + rect.Height();
		rect_button.top = rect.top;
		rect_button.bottom = rect.bottom;

		rect_button.DeflateRect(3, 3);

		CBCGPDrawManager dm(*pDC);
		dm.DrawEllipse(rect_button, RGB(255, 0, 0), globalData.clrBarShadow);

		clrText = globalData.clrBtnText;
	}
#else
	pDC->FillRect(rect, &globalData.brBarFace);

	pDC->Draw3dRect(rect, globalData.clrBarLight, globalData.clrBarDkShadow);
	rect.DeflateRect(1, 1);
	pDC->Draw3dRect(rect, globalData.clrBarHilite, globalData.clrBarShadow);
#endif

	COLORREF clrTextOld = pDC->SetTextColor(clrText);

	CString strText = (LPCTSTR)(_bstr_t)m_varValue;
	pDC->DrawText(strText, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

	pDC->SetTextColor(clrTextOld);
}
//*****************************************************************************************
BOOL CRadioButtonItem::OnClickValue(UINT uiMsg, CPoint point)
{
	if (uiMsg != WM_LBUTTONDOWN)
	{
		return CBCGPGridItem::OnClickValue(uiMsg, point);
	}

	CBCGPGridCtrl* pGridCtrl = GetOwnerList();
	ASSERT_VALID(pGridCtrl);

	pGridCtrl->SendMessage(WM_COMMAND,
		MAKEWPARAM(m_id, BN_CLICKED));

	return TRUE;
}
//*****************************************************************************************
BOOL CRadioButtonItem::PushChar(UINT nChar)
{
	if (nChar == VK_SPACE || nChar == VK_RETURN)
	{
		CBCGPGridCtrl* pGridCtrl = GetOwnerList();
		ASSERT_VALID(pGridCtrl);

		pGridCtrl->SendMessage(WM_COMMAND,
			MAKEWPARAM(m_id, BN_CLICKED));

		return TRUE;
	}

	return CBCGPGridItem::PushChar(nChar);
}

////////////////////////////////////////////////////////////////////////////////
// CAnimationItem Class

CAnimationItem::CAnimationItem (const CImageList& imageListIcons, LPCTSTR lpszCaption)
	:	CBCGPGridItem (lpszCaption)
{
	m_imageListIcons.CreateFromImageList (imageListIcons);
	m_imageListIcons.SetTransparentColor (::GetSysColor (COLOR_3DFACE));

	m_nItem = -1;	
	AllowEdit(FALSE);
}

CAnimationItem::CAnimationItem (UINT idBitmapRes, LPCTSTR lpszCaption)
	:	CBCGPGridItem (lpszCaption)
{
	m_imageListIcons.SetImageSize (CSize(16, 16));
	m_imageListIcons.Load (idBitmapRes);
	m_nItem = -1;	
	AllowEdit(FALSE);
}
//*****************************************************************************************
void CAnimationItem::OnDrawValue (CDC* pDC, CRect rect)
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList();
	ASSERT_VALID (pGridCtrl);

	ASSERT_VALID (pDC);
	
	CRect rectImage = rect;

	CString strText = GetLabel ();

	const double dblScale = pGridCtrl->GetScale();
	
	CSize sizeImage = m_imageListIcons.GetImageSize ();
	if (dblScale != 1.0)
	{
		sizeImage.cx = bcg_scale(sizeImage.cx, dblScale);
		sizeImage.cy = bcg_scale(sizeImage.cy, dblScale);
	}

	const int nImageWidth = sizeImage.cx + 2 * globalUtils.ScaleByDPI(pGridCtrl->GetImageMargin());

	const int nTextMargin = globalUtils.ScaleByDPI(pGridCtrl->GetTextMargin());
	const int nTextWidth = pDC->GetTextExtent (strText).cx;

	COLORREF clrTextOld = (COLORREF)-1;
	COLORREF clrText = OnFillBackground (pDC, rect);
	
	if (clrText != (COLORREF)-1)
	{
		clrTextOld = pDC->SetTextColor (clrText);
	}

	// Draw text:
	if (!strText.IsEmpty () && rect.Width () - nImageWidth > 10)
	{
		CRect rectText = rect;
		rectText.DeflateRect (nTextMargin, 0);
		rectText.right = min (rectText.right - nImageWidth, rectText.left + nTextWidth);

		pDC->DrawText (strText, rectText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
		
		rectImage.left = rectText.right;
	}

	// Draw image:
	if (m_nItem >= 0)
	{
		if (dblScale != 1.0)
		{
			CRect rectImage1 = CRect(
				rectImage.CenterPoint().x - sizeImage.cx / 2, rectImage.CenterPoint().y - sizeImage.cy / 2,
				rectImage.CenterPoint().x + sizeImage.cx / 2, rectImage.CenterPoint().y + sizeImage.cy / 2);

			m_imageListIcons.DrawEx (pDC, rectImage1, m_nItem,
				CBCGPToolBarImages::ImageAlignHorzStretch, CBCGPToolBarImages::ImageAlignVertStretch);
		}
		else
		{
			m_imageListIcons.DrawEx (pDC, rectImage, m_nItem,
				CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
		}
	}
	
	if (clrTextOld != (COLORREF)-1)
	{
		pDC->SetTextColor (clrTextOld);
	}
}
//*****************************************************************************************
void CAnimationItem::DoStep ()
{
	if (m_nItem >= 0 && m_nItem < m_imageListIcons.GetCount () - 1)
	{
		m_nItem ++;
	}
	else
	{
		m_nItem = 0;
	}
}
//*****************************************************************************************
void CAnimationItem::SetReady()
{
	m_nItem = -1;
}

////////////////////////////////////////////////////////////////////////////////
// CSubscriptItem Class

CSubscriptItem::CSubscriptItem (LPCTSTR lpszPostfix, const _variant_t& varValue, DWORD_PTR dwData)
	:	CBCGPGridItem (varValue, dwData), m_strPostfix (lpszPostfix)
{
	AllowEdit(FALSE);
}
//*****************************************************************************************
void CSubscriptItem::OnDrawValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);

	CBCGPGridCtrl* pGridCtrl = GetOwnerList();
	ASSERT_VALID (pGridCtrl);
	
	COLORREF clrTextOld = (COLORREF)-1;
	COLORREF clrText = OnFillBackground (pDC, rect);
	
	if (clrText != (COLORREF)-1)
	{
		clrTextOld = pDC->SetTextColor (clrText);
	}

	// Draw icon:
	OnDrawIcon (pDC, rect);
	
	CString strText = GetLabel ();
	const DWORD dwFlags = GetFlags ();
	int nTextAlign = GetAlign ();
	
	// Get subscript font:
	CFont* pFont = pGridCtrl->GetFont();
	ASSERT_VALID(pFont);
	
	LOGFONT lf;
	memset (&lf, 0, sizeof (LOGFONT));
	
	pFont->GetLogFont (&lf);
	
	const CPoint ptSubOffset (lf.lfWidth / 2, lf.lfHeight / 6);
	const int nErrY = lf.lfHeight % 6;
	
	lf.lfWidth /= 2;
	lf.lfHeight = lf.lfHeight * 2 / 3;

	CFont fontSub;
	fontSub.CreateFontIndirect (&lf);
	
	// Set vertical align:
	UINT uiTextFlags = DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
	int nYOffset = 0;

	if (dwFlags & BCGP_GRID_ITEM_VTOP)
	{
		uiTextFlags |= DT_TOP;

		if (IsSubscript ())
		{
			nYOffset = -2 * ptSubOffset.y - nErrY;
		}
	}
	else if (dwFlags & BCGP_GRID_ITEM_VBOTTOM)
	{
		uiTextFlags |= DT_BOTTOM;

		if (!IsSubscript ())
		{
			nYOffset = 2 * ptSubOffset.y + nErrY;
		}
	}
	else // dwFlags & BCGP_GRID_ITEM_VCENTER
	{
		uiTextFlags |= DT_VCENTER;
	
		if (IsSubscript ())
		{
			nYOffset = - ptSubOffset.y - (nErrY + 1) / 2;
		}
		else
		{
			nYOffset = ptSubOffset.y + (nErrY + 1) / 2;
		}
	}

	// Adjust positions:
	rect.DeflateRect (globalUtils.ScaleByDPI(pGridCtrl->GetTextMargin()), globalUtils.ScaleByDPI(pGridCtrl->GetTextVMargin()));

	const int nTextWidth = pDC->GetTextExtent (strText).cx;
	
	CFont* pFontOld = pDC->SelectObject (&fontSub);
	const int nPostfixWidth = ptSubOffset.x + pDC->GetTextExtent (m_strPostfix).cx;
	pDC->SelectObject (pFontOld);

	const int nTotalWidth = nTextWidth + nPostfixWidth;
	
	CPoint ptOrigin = rect.TopLeft();
	if (nTextAlign & HDF_CENTER)
	{
		ptOrigin.x = rect.left + rect.Width() / 2 - nTotalWidth / 2;
	}
	else if (nTextAlign & HDF_RIGHT)
	{
		ptOrigin.x = rect.right - nTotalWidth;
	}

	if (rect.Width() < nTotalWidth)
	{
		ptOrigin.x = rect.left;
	}

	BOOL bIsTrancated = FALSE;
	BOOL bEndEllipsis = FALSE;

	// Draw text (part 1):
	if (!strText.IsEmpty () && rect.right > ptOrigin.x && !bIsTrancated)
	{
		CRect rectText = rect;
		rectText.left = ptOrigin.x;
		bIsTrancated = pGridCtrl->DoDrawText (pDC, strText, rectText, uiTextFlags);

		ptOrigin.x += nTextWidth;
	}
	else
	{
		bIsTrancated = TRUE;
		bEndEllipsis = TRUE;
	}

	// Draw postfix text (part 2):
	if (!m_strPostfix.IsEmpty() && rect.right > ptOrigin.x + ptSubOffset.x && !bIsTrancated)
	{
		CRect rectText = rect;
		rectText.left = ptOrigin.x + ptSubOffset.x;
		rectText.OffsetRect(0, nYOffset);

		pDC->SelectObject (&fontSub);
		bIsTrancated = pGridCtrl->DoDrawText (pDC, m_strPostfix, rectText, uiTextFlags);
		pDC->SelectObject (pFontOld);
		
		ptOrigin.x += nTextWidth;
	}
	else
	{
		bIsTrancated = TRUE;
		bEndEllipsis = TRUE;
	}

	if (bIsTrancated && bEndEllipsis)
	{
		CRect rectText = rect;
		rectText.left = ptOrigin.x;
		pGridCtrl->DoDrawText (pDC, _T("..."), rectText, uiTextFlags);
	}

	m_bValueIsTrancated = bIsTrancated;

	if (clrTextOld != (COLORREF)-1)
	{
		pDC->SetTextColor (clrTextOld);
	}
}

void CSubscriptItem::OnPrintValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	CBCGPGridCtrl* pGridCtrl = GetOwnerList();
	ASSERT_VALID (pGridCtrl);
	ASSERT (pGridCtrl->GetPrinterFont() != NULL);

	// map to printer metrics
	HDC hDCFrom = ::GetDC(NULL);
	int nXMul = pDC->GetDeviceCaps(LOGPIXELSX); // pixels in print dc
	int nXDiv = ::GetDeviceCaps(hDCFrom, LOGPIXELSX); // pixels in screen dc
	::ReleaseDC(NULL, hDCFrom);

	const int CALCULATED_TEXT_MARGIN = ::MulDiv (pGridCtrl->GetTextMargin(), nXMul, nXDiv);
	const CRect& rectClip = pGridCtrl->GetPrintParams ().m_pageInfo.m_rectPageItems;

	COLORREF clrTextOld = pDC->SetTextColor (pGridCtrl->GetPrintTextColor ());

	CString strText = GetLabel ();
	const DWORD dwFlags = GetFlags ();
	int nTextAlign = GetAlign ();
	
	// Get subscript font:
	CFont* pFont = pGridCtrl->GetPrinterFont();
	ASSERT_VALID(pFont);
	
	LOGFONT lf;
	memset (&lf, 0, sizeof (LOGFONT));
	
	pFont->GetLogFont (&lf);
	
	const CPoint ptSubOffset (lf.lfWidth / 2, lf.lfHeight / 6);
	const int nErrY = lf.lfHeight % 6;
	
	lf.lfWidth /= 2;
	lf.lfHeight = lf.lfHeight * 2 / 3;

	CFont fontSub;
	fontSub.CreateFontIndirect (&lf);
	
	// Set vertical align:
	UINT uiTextFlags = DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
	int nYOffset = 0;

	if (dwFlags & BCGP_GRID_ITEM_VTOP)
	{
		uiTextFlags |= DT_TOP;

		if (IsSubscript ())
		{
			nYOffset = -2 * ptSubOffset.y - nErrY;
		}
	}
	else if (dwFlags & BCGP_GRID_ITEM_VBOTTOM)
	{
		uiTextFlags |= DT_BOTTOM;

		if (!IsSubscript ())
		{
			nYOffset = 2 * ptSubOffset.y + nErrY;
		}
	}
	else // dwFlags & BCGP_GRID_ITEM_VCENTER
	{
		uiTextFlags |= DT_VCENTER;
	
		if (IsSubscript ())
		{
			nYOffset = - ptSubOffset.y - (nErrY + 1) / 2;
		}
		else
		{
			nYOffset = ptSubOffset.y + (nErrY + 1) / 2;
		}
	}

	// Adjust positions:
	rect.DeflateRect (CALCULATED_TEXT_MARGIN, 0);

	const int nTextWidth = pDC->GetTextExtent (strText).cx;
	
	CFont* pFontOld = pDC->SelectObject (&fontSub);
	const int nPostfixWidth = ptSubOffset.x + pDC->GetTextExtent (m_strPostfix).cx;
	pDC->SelectObject (pFontOld);

	const int nTotalWidth = nTextWidth + nPostfixWidth;
	
	CPoint ptOrigin = rect.TopLeft();
	if (nTextAlign & HDF_CENTER)
	{
		ptOrigin.x = rect.left + rect.Width() / 2 - nTotalWidth / 2;
	}
	else if (nTextAlign & HDF_RIGHT)
	{
		ptOrigin.x = rect.right - nTotalWidth;
	}

	if (rect.Width() < nTotalWidth)
	{
		ptOrigin.x = rect.left;
	}

	BOOL bIsTrancated = FALSE;
	BOOL bEndEllipsis = FALSE;

	CRect rectClipText = rect;
	rectClipText.NormalizeRect ();
	if (rectClipText.IntersectRect (&rectClipText, &rectClip))
	{
		// Draw text (part 1):
		if (!strText.IsEmpty () && rect.right > ptOrigin.x && !bIsTrancated)
		{
			CRect rectText = rect;
			rectText.left = ptOrigin.x;
			bIsTrancated = pGridCtrl->DoDrawText (pDC, strText, rectText, uiTextFlags, rectClipText);

			ptOrigin.x += nTextWidth;
		}
		else
		{
			bIsTrancated = TRUE;
			bEndEllipsis = TRUE;
		}

		// Draw postfix text (part 2):
		if (!m_strPostfix.IsEmpty() && rect.right > ptOrigin.x + ptSubOffset.x && !bIsTrancated)
		{
			CRect rectText = rect;
			rectText.left = ptOrigin.x + ptSubOffset.x;
			rectText.OffsetRect(0, nYOffset);

			pDC->SelectObject (&fontSub);
			bIsTrancated = pGridCtrl->DoDrawText (pDC, m_strPostfix, rectText, uiTextFlags, rectClipText);
			pDC->SelectObject (pFontOld);
		
			ptOrigin.x += nTextWidth;
		}
		else
		{
			bIsTrancated = TRUE;
			bEndEllipsis = TRUE;
		}

		if (bIsTrancated && bEndEllipsis)
		{
			CRect rectText = rect;
			rectText.left = ptOrigin.x;
			pGridCtrl->DoDrawText (pDC, _T("..."), rectText, uiTextFlags, rectClipText);
		}
	}

	pDC->SetTextColor (clrTextOld);
}

CString CSubscriptItem::GetValueTooltip ()
{
	//ASSERT_VALID (this);
	//return m_bValueIsTrancated ? FormatItem () + m_strPostfix : _T("");
	return m_strPostfix;
}

#ifdef _BCGSUITE_INC_

////////////////////////////////////////////////////////////////////////////////
// CGridColorItem Class

CGridColorItem::CGridColorItem () :
	CBCGPGridItem (_variant_t(), 0),
	m_Color ((COLORREF) -1)
{
	m_varValue = (LONG) (COLORREF) -1;
	m_dwFlags = BCGP_GRID_ITEM_VCENTER | BCGP_GRID_ITEM_HAS_LIST;
}
//*****************************************************************************************
CGridColorItem::CGridColorItem (const COLORREF& color, DWORD_PTR dwData) :
	CBCGPGridItem (_variant_t(), dwData)
{
	m_varValue = (LONG)color;
	m_Color = color;
	m_dwFlags = BCGP_GRID_ITEM_VCENTER | BCGP_GRID_ITEM_HAS_LIST;
}
//*****************************************************************************************
void CGridColorItem::OnDrawValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);

	CRect rectColor = rect;

	rect.left += rect.Height ();
	CBCGPGridItem::OnDrawValue (pDC, rect);

	rectColor.right = rectColor.left + rectColor.Height ();
	rectColor.DeflateRect (1, 1);
	rectColor.top++;
	rectColor.left++;

	if (m_Color != (COLORREF)-1)
	{
		CBrush br(m_Color);
		pDC->FillRect (rectColor, &br);
	}

	pDC->Draw3dRect (rectColor, 0, 0);
}
//****************************************************************************************
void CGridColorItem::OnClickButton (CPoint /*point*/)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pGridRow);

	m_bButtonIsDown = TRUE;
	Redraw ();

	CMFCColorDialog dlg(m_Color);
	if (dlg.DoModal() == IDOK)
	{
		SetColor(dlg.GetColor());
	}

	m_bButtonIsDown = FALSE;
	Redraw ();
}
//******************************************************************************************
BOOL CGridColorItem::OnEdit (LPPOINT /*lptClick*/)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pGridRow);

	CBCGPGridCtrl* pWndList = GetOwnerList ();
	ASSERT_VALID (pWndList);

	m_pWndInPlace = NULL;

	CRect rectEdit;
	CRect rectSpin;

	AdjustInPlaceEditRect (rectEdit, rectSpin);

	CMFCMaskedEdit* pWndEdit = new CMFCMaskedEdit;
	DWORD dwStyle = WS_VISIBLE | WS_CHILD;

	if (!m_bEnabled)
	{
		dwStyle |= ES_READONLY;
	}

	pWndEdit->EnableMask(
		_T("AAAAAA"), 
		_T("______"), 
		_T(' ')); 
	pWndEdit->SetValidChars(_T("01234567890ABCDEFabcdef"));

	pWndEdit->Create (dwStyle, rectEdit, pWndList, /*BCGPGRIDCTRL_ID_INPLACE*/1);
	m_pWndInPlace = pWndEdit;

	m_pWndInPlace->SetWindowText (FormatItem ());

	m_pWndInPlace->SetFont (pWndList->GetFont ());
	m_pWndInPlace->SetFocus ();

	m_bInPlaceEdit = TRUE;
	return TRUE;
}
//****************************************************************************************
void CGridColorItem::AdjustInPlaceEditRect (CRect& rectEdit, CRect& rectSpin)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pGridRow);
	CBCGPGridCtrl* pWndList = GetOwnerList ();
	ASSERT_VALID (pWndList);

	rectSpin.SetRectEmpty ();

	rectEdit = m_Rect;

	if (rectEdit.right > pWndList->GetListRect ().right)
	{
		rectEdit.right = max (rectEdit.left, pWndList->GetListRect ().right);
	}

	rectEdit.DeflateRect (0, 2);
	rectEdit.left += m_Rect.Height () + 2;

	AdjustButtonRect ();
	rectEdit.right = m_rectButton.left - 1;
}
//****************************************************************************************
CString CGridColorItem::FormatItem ()
{
	ASSERT_VALID (this);

	CString str;
	str.Format (_T("%02x%02x%02x"),
		GetRValue (m_Color), GetGValue (m_Color), GetBValue (m_Color));

	return str;
}
//******************************************************************************************
CRect CGridColorItem::GetTooltipRect () const
{
	CRect rect = GetRect ();
	rect.left = rect.left + rect.Height ();
	return rect;
}
//******************************************************************************************
void CGridColorItem::SetColor (COLORREF color)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pGridRow);

	CBCGPGridCtrl* pWndList = GetOwnerList ();
	ASSERT_VALID (pWndList);

	m_Color = color;
	m_varValue = (LONG) color;

	if (::IsWindow (pWndList->GetSafeHwnd())) 
	{
		CRect rect = m_Rect;
		rect.DeflateRect (0, 1);

		pWndList->InvalidateRect (rect);
		pWndList->UpdateWindow ();
	}

	m_bIsChanged = TRUE;

	if (m_pWndInPlace != NULL)
	{
		ASSERT_VALID (m_pWndInPlace);
		m_pWndInPlace->SetWindowText (GetLabel ());
	}
}
//********************************************************************************
BOOL CGridColorItem::OnUpdateValue ()
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndInPlace);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));
	ASSERT_VALID (m_pGridRow);

	CString strText;
	m_pWndInPlace->GetWindowText (strText);

	COLORREF colorCurr = m_Color;
	UINT nR = 0, nG = 0, nB = 0;
#if _MSC_VER < 1400
	_stscanf (strText, _T("%2x%2x%2x"), &nR, &nG, &nB);
#else
	_stscanf_s (strText, _T("%2x%2x%2x"), &nR, &nG, &nB);
#endif
	if (CanUpdateData ())
	{
		SetColor (RGB (nR, nG, nB));
	}

	if (colorCurr != m_Color)
	{
		SetItemChanged ();
	}

	return TRUE;
}

#endif

////////////////////////////////////////////////////////////////////////////////
// CColorListItem class

#define NO_COLOR_LABEL _T("No color")

CColorListItem::CColorListItem(COLORREF color, const CArray<COLORREF, COLORREF>& arColors, const CStringArray& arColorNames, BOOL bAddNoColor)
{
	ASSERT(arColors.GetSize() == arColorNames.GetSize());

	if (bAddNoColor)
	{
		AddOption(NO_COLOR_LABEL, TRUE, (DWORD_PTR)-1);
	}

	for (int i = 0; i < arColorNames.GetSize(); i++)
	{
		AddOption(arColorNames[i], TRUE, (DWORD_PTR)arColors[i]);
	}

	SetData((DWORD_PTR)color);
	AllowEdit(TRUE);
}
//*****************************************************************************************
CComboBox* CColorListItem::CreateCombo (CWnd* pWndParent, CRect rect)
{
	rect.bottom = rect.top + globalUtils.ScaleByDPI(400);

	CBCGPColorComboBox* pWndCombo = new CBCGPColorComboBox;
	pWndCombo->m_bVisualManagerStyle = TRUE;

	if (!pWndCombo->Create (WS_CHILD | CBS_NOINTEGRALHEIGHT | CBS_DROPDOWNLIST | WS_VSCROLL | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS,
		rect, pWndParent, BCGPGRIDCTRL_ID_INPLACE_COMBO))
	{
		delete pWndCombo;
		return NULL;
	}

	return pWndCombo;
}
//*****************************************************************************************
void CColorListItem::OnDrawValue (CDC* pDC, CRect rect)
{
	COLORREF clrText = OnFillBackground (pDC, rect);

	COLORREF clrTextOld = (COLORREF)-1;
	if (clrText != (COLORREF)-1)
	{
		clrTextOld = pDC->SetTextColor(clrText);
	}

	CBCGPGridCtrl* pGridCtrl = GetOwnerList();

	m_bValueIsTrancated = !CBCGPColorComboBox::DoDrawItem(pDC, rect, pGridCtrl->GetFont(), GetColor(), FormatItem(), !IsEnabled());

	if (clrTextOld != NULL)
	{
		pDC->SetTextColor(clrTextOld);
	}
}
//*****************************************************************************************
void CColorListItem::OnPrintValue (CDC* pDC, CRect rect)
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList();

	COLORREF clrTextOld = pDC->SetTextColor(pGridCtrl->GetPrintTextColor());
	
	CBCGPColorComboBox::DoDrawItem(pDC, rect, pGridCtrl->GetPrinterFont(), GetColor(), FormatItem(), !IsEnabled());

	pDC->SetTextColor (clrTextOld);
}
//*****************************************************************************************
CWnd* CColorListItem::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
{
	CWnd* pWnd = CBCGPGridItem::CreateInPlaceEdit (rectEdit, bDefaultFormat);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow (SW_HIDE);

		CEdit* pWndEdit = DYNAMIC_DOWNCAST (CEdit, pWnd);
		if (pWndEdit != NULL)
		{
			pWndEdit->SetReadOnly (TRUE);
		}
	}

	return pWnd;
}
//*****************************************************************************************
void CColorListItem::OnSelectCombo()
{
	CBCGPGridItem::OnSelectCombo();

	CBCGPColorComboBox* pWndCombo = (CBCGPColorComboBox*)m_pWndCombo;
	if (pWndCombo != NULL)
	{
		SetData((DWORD_PTR)pWndCombo->GetItemColor(pWndCombo->GetCurSel()));
	}
}
//*****************************************************************************************
CString CColorListItem::FormatItem()
{
	ASSERT_VALID (this);
	
	CString str = CBCGPGridItem::FormatItem();
	//return str.IsEmpty() ? NO_COLOR_LABEL : str;
	return str;
}
//*****************************************************************************************
CRect CColorListItem::GetTooltipRect () const
{
	CRect rect = GetRect ();
	rect.left = rect.left + rect.Height ();
	return rect;
}

////////////////////////////////////////////////////////////////////////////////
// CTwoButtonsItem Class

CTwoButtonsItem::CTwoButtonsItem (const _variant_t& varValue) :
	CBCGPGridItem (varValue)
{
	m_images.SetImageSize(CSize(16, 16));
	m_images.SetTransparentColor(RGB(255, 0, 255));
	//m_images.Load(IDB_BUTTONS);

	globalUtils.ScaleByDPI(m_images);

	m_iClickedButton = -1;

	AllowEdit(FALSE);
}
//*******************************************************************************
void CTwoButtonsItem::AdjustButtonRect ()
{
	CBCGPGridItem::AdjustButtonRect ();

	CBCGPGridCtrl* pWndList = GetOwnerList();
	ASSERT_VALID(pWndList);
	
	m_rectButton.left = m_rectButton.right - (int)(0.5 + 2.0 * m_images.GetImageSize().cx * pWndList->GetScale());
}
//*******************************************************************************
void CTwoButtonsItem::OnClickButton (CPoint point)
{
	BOOL bIsLeft = point.x < m_rectButton.CenterPoint ().x;

	m_iClickedButton = bIsLeft ? 0 : 1;
	RedrawButton();

	BCGPMessageBox (bIsLeft ? _T("Left button clicked") : _T("Right button clicked"));

	m_iClickedButton = -1;
	RedrawButton();
}
//*******************************************************************************
void CTwoButtonsItem::OnDrawButton (CDC* pDC, CRect rectButton)
{
	CBCGPGridCtrl* pWndList = GetOwnerList();
	ASSERT_VALID(pWndList);

	for (int i = 0; i < 2; i++)
	{
		CBCGPToolbarButton button;

		CRect rect = rectButton;

		if (i == 0)
		{
			rect.right = rect.left + rect.Width () / 2;
		}
		else
		{
			rect.left = rect.right - rect.Width () / 2;
		}

#ifdef _BCGSUITE_INC_
		CMFCVisualManager::AFX_BUTTON_STATE state = 
			m_iClickedButton == i ? CMFCVisualManager::ButtonsIsHighlighted : CMFCVisualManager::ButtonsIsRegular;
		CMFCVisualManager::GetInstance ()->OnFillButtonInterior (pDC, &button, rect, state);
#else
		CBCGPVisualManager::BCGBUTTON_STATE state = 
			m_iClickedButton == i ? CBCGPVisualManager::ButtonsIsHighlighted : CBCGPVisualManager::ButtonsIsRegular;

		CBCGPVisualManager::GetInstance ()->OnFillButtonInterior (pDC, &button, rect, state);
#endif

		if (pWndList->GetScale() == 1.0)
		{
			m_images.DrawEx(pDC, rect, i, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
		}
		else
		{
			CSize sizeImage = m_images.GetImageSize();
			CRect rectImage = rect;

			if (sizeImage.cx != 0 && sizeImage.cy != 0)
			{
				double dAspect = min((double)rectImage.Width() / (double)sizeImage.cx, (double)rectImage.Height() / (double)sizeImage.cy);
				dAspect = min(dAspect, pWndList->GetScale());
				
				sizeImage.cx = bcg_clamp((long)((double)sizeImage.cx * dAspect), 0, rectImage.Width());
				sizeImage.cy = bcg_clamp((long)((double)sizeImage.cy * dAspect), 0, rectImage.Height());
				
				rectImage.OffsetRect((rectImage.Width() - sizeImage.cx) / 2, (rectImage.Height() - sizeImage.cy) / 2);
				rectImage.right = rectImage.left + sizeImage.cx;
				rectImage.bottom = rectImage.top + sizeImage.cy;
			}

			m_images.DrawEx(pDC, rectImage, i, CBCGPToolBarImages::ImageAlignHorzStretch, CBCGPToolBarImages::ImageAlignVertStretch);
		}

#ifdef _BCGSUITE_INC_
		CMFCVisualManager::GetInstance ()->OnDrawButtonBorder (pDC, &button, rect, state);
#else
		CBCGPVisualManager::GetInstance ()->OnDrawButtonBorder (pDC, &button, rect, state);
#endif
	}
}
//*******************************************************************************
void CTwoButtonsItem::RedrawButton()
{
	ASSERT_VALID (this);
	
	CBCGPGridCtrl* pWndList = GetOwnerList();
	CRect rectButton = GetButtonRect();

	if (pWndList != NULL && !rectButton.IsRectEmpty())
	{
		ASSERT_VALID (pWndList);
		pWndList->RedrawWindow(rectButton);
	}
}
//*******************************************************************************
void CTwoButtonsItem::OnDrawValue (CDC* pDC, CRect rect)
{
	// call the base implementation
	CBCGPGridItem::OnDrawValue (pDC, rect);
	
	// Always show the button: add the following code to update m_rectButton member
	if (HasButton())
	{
		AdjustButtonRect();
	}
}

////////////////////////////////////////////////////////////////////////////////
// CRatingItem Class

#define MAX_RATING	5

CRatingItem::CRatingItem(int nValue) : CBCGPGridItem((short)bcg_clamp(nValue, 0, MAX_RATING))
{
	AllowEdit(FALSE);

	m_nHighlightedValue = 0;

	m_images.SetImageSize(CSize(14, 14));
	//m_images.Load(IDB_STARS);
	
	globalUtils.ScaleByDPI(m_images);
}
//*******************************************************************************
void CRatingItem::OnDrawValue(CDC* pDC, CRect rect)
{
	CBCGPGridCtrl* pWndList = GetOwnerList ();
	ASSERT_VALID (pWndList);
	
	OnFillBackground (pDC, rect);

	const double dblScale = pWndList->GetScale();

	const int nValue = m_nHighlightedValue > 0 ? m_nHighlightedValue : bcg_clamp((int)(short)GetValue(), 0, MAX_RATING);

	CRect rectStar = PrepareStarsRect(rect);
	CSize sizeIcon = m_images.GetImageSize();
	
	if (dblScale != 1.0)
	{
		sizeIcon.cx = bcg_scale(sizeIcon.cx, dblScale);
		sizeIcon.cy = bcg_scale(sizeIcon.cy, dblScale);
	}

	rectStar.right = rectStar.left + sizeIcon.cx;

	for (int i = 1; i <= MAX_RATING; i++)
	{
		int nIconIndex = 0;

		if (i <= nValue)
		{
			nIconIndex = 2;
		}

		if (m_nHighlightedValue > 0)
		{
			nIconIndex++;
		}

		if (dblScale != 1.0)
		{
			m_images.DrawEx(pDC, rectStar, nIconIndex, CBCGPToolBarImages::ImageAlignHorzStretch, CBCGPToolBarImages::ImageAlignVertStretch);
		}
		else
		{
			m_images.DrawEx(pDC, rectStar, nIconIndex, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
		}

		rectStar.OffsetRect(sizeIcon.cx, 0);
	}
}
//*******************************************************************************
void CRatingItem::OnMouseMove(const CPoint& point)
{
	int nValue = CursorToValue(point);
	if (nValue != m_nHighlightedValue)
	{
		m_nHighlightedValue = nValue;
		Redraw();
	}
}
//*******************************************************************************
void CRatingItem::OnMouseLeave()
{
	if (m_nHighlightedValue > 0)
	{
		m_nHighlightedValue = 0;
		Redraw();
	}
}
//*******************************************************************************
BOOL CRatingItem::OnClickValue(UINT uiMsg, CPoint point)
{
	if (uiMsg == WM_LBUTTONDOWN)
	{
		short nValue = (short)CursorToValue(point);
		if (nValue > 0)
		{
			SetValue(nValue);
			Redraw();
		}
	}
	else
	{
		OnMouseLeave();
	}

	return CBCGPGridItem::OnClickValue(uiMsg, point);
}
//*******************************************************************************
CString CRatingItem::FormatItem()
{
	CString str;
	short nRating = (short)GetValue();

	if (nRating == 0)
	{
		str = _T("Unrated");
	}
	else
	{
		str.Format(_T("%d star(s)"), nRating);
	}

	return str;
}
//*******************************************************************************
BOOL CRatingItem::OnSetCursor() const
{
	if (m_nHighlightedValue > 0)
	{
		SetCursor(globalData.GetHandCursor());
		return TRUE;
	}

	return CBCGPGridItem::OnSetCursor();
}
//*******************************************************************************
CRect CRatingItem::PrepareStarsRect(const CRect& rectItem)
{
	CBCGPGridCtrl* pWndList = GetOwnerList ();
	ASSERT_VALID (pWndList);

	CRect rect = rectItem;

	rect.DeflateRect(globalUtils.ScaleByDPI(pWndList->GetTextMargin()), globalUtils.ScaleByDPI(pWndList->GetTextVMargin()));

	int cxIcon = m_images.GetImageSize().cx;
	const double dblScale = pWndList->GetScale();

	if (dblScale != 1.0)
	{
		cxIcon = bcg_scale(cxIcon, dblScale);
	}

	const int nWidth = MAX_RATING * cxIcon;

	switch (GetAlign())
	{
	case HDF_RIGHT:
		rect.left = rect.right - nWidth;
		break;
		
	case HDF_CENTER:
		rect.left = rect.CenterPoint().x - nWidth / 2;
		rect.right = rect.left + nWidth;
		break;

	default:
		rect.right = rect.left + nWidth;
		break;
	}

	return rect;
}
//*******************************************************************************
int CRatingItem::CursorToValue(const CPoint& point)
{
	CRect rectItem = PrepareStarsRect(GetRect());

	if (!rectItem.PtInRect(point))
	{
		return 0;
	}

	return bcg_clamp((int)((double)(point.x - rectItem.left) / rectItem.Width() * MAX_RATING) + 1, 1, MAX_RATING);
}
//*******************************************************************************
BOOL CRatingItem::PushChar(UINT nChar)
{
	short nValue = (short)GetValue();

	if (nChar == _T('+') || nChar == _T('-'))
	{
		if (nChar == _T('+'))
		{
			if (nValue < MAX_RATING)
			{
				SetValue(++nValue);
				Redraw();
			}
		}
		else
		{
			if (nValue > 0)
			{
				SetValue(--nValue);
				Redraw();
			}
		}

		return TRUE;
	}
	else if (nChar >= _T('1') && nChar <= _T('1') + MAX_RATING - 1)	// Assume MAX_RATING is less than 10!
	{
		short nNewValue = (short)(nChar - _T('1') + 1);

		if (nValue != nNewValue)
		{
			SetValue(nNewValue);
			Redraw();
		}
	}
	else if (nChar == VK_SPACE)
	{
		if (++nValue > MAX_RATING)
		{
			nValue = 0;
		}

		SetValue(nValue);
		Redraw();
	}
	else if (nChar == _T('0'))
	{
		if (nValue > 0)
		{
			if (BCGPMessageBox(_T("Do you really want to clear this rating?"), MB_YESNO) == IDYES)
			{
				nValue = 0;
				SetValue(nValue);
				Redraw();
			}
		}
	}

	return CBCGPGridItem::PushChar(nChar);
}
