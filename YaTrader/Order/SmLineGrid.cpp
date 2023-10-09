#include "stdafx.h"
#include "SmLineGrid.h"

BEGIN_MESSAGE_MAP(SmLineGrid, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

SmLineGrid::SmLineGrid()
{
	m_pGM = CBCGPGraphicsManager::CreateInstance();
}

SmLineGrid::~SmLineGrid()
{
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmLineGrid::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CBCGPMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rect;
	GetClientRect(rect);

	if (m_pGM == NULL)
	{
		return;
	}

	m_pGM->BindDC(pDC, rect);

	if (!m_pGM->BeginDraw())
	{
		return;
	}

	rect.right -= 1;
	rect.bottom -= 1;

	m_pGM->FillRectangle(rect, LineBrush);
	m_pGM->EndDraw();
}
