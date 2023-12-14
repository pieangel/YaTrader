// AbFavoriteSymbolSelector.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "AbFavoriteSymbolSelector.h"


// AbFavoriteSymbolSelector dialog

IMPLEMENT_DYNAMIC(AbFavoriteSymbolSelector, CBCGPDialog)

AbFavoriteSymbolSelector::AbFavoriteSymbolSelector(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_AB_SYMBOL_SELECTOR, pParent)
{

}

AbFavoriteSymbolSelector::~AbFavoriteSymbolSelector()
{
}

void AbFavoriteSymbolSelector::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AbFavoriteSymbolSelector, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &AbFavoriteSymbolSelector::OnBnClickedBtnClose)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// AbFavoriteSymbolSelector message handlers


void AbFavoriteSymbolSelector::set_source_window_id(const int window_id)
{
	favorite_symbol_view_.set_order_window_id(window_id);
}

BOOL AbFavoriteSymbolSelector::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	CWnd* pWnd = GetDlgItem(IDC_STATIC_SYMBOL);
	CRect rect;
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	SetTimer(1, 500, NULL);
	// Create the Windows control and attach it to the Grid object
	favorite_symbol_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, 0x00001234);

	favorite_symbol_view_.SetFavorite();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void AbFavoriteSymbolSelector::OnBnClickedBtnClose()
{
	CBCGPDialog::OnOK();
}


void AbFavoriteSymbolSelector::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	favorite_symbol_view_.OnQuoteEvent("");
	favorite_symbol_view_.Update();
	CBCGPDialog::OnTimer(nIDEvent);
}
