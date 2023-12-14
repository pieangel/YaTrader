// HdFuturePage.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "HdFuturePage.h"
#include "afxdialogex.h"
#include "HdSymbolSelecter.h"
#include "../Global/SmTotalManager.h"
#include "../Symbol/MarketDefine.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"

// HdFuturePage dialog
//extern TApplicationFont g_Font;

#define WND_ID10 0x00000010
using namespace DarkHorse;

IMPLEMENT_DYNAMIC(HdFuturePage, CBCGPDialog)

HdFuturePage::HdFuturePage(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_FUT_PAGE, pParent)
{
	_SymSelecter = nullptr;
}

HdFuturePage::~HdFuturePage()
{
}

void HdFuturePage::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FUT_MARKET, _ComboFutureMarket);
}


BEGIN_MESSAGE_MAP(HdFuturePage, CBCGPDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_FUT_MARKET, &HdFuturePage::OnCbnSelchangeComboFutMarket)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// HdFuturePage message handlers


void HdFuturePage::set_source_window_id(const int window_id)
{
	future_view_.set_source_window_id(window_id);
}

BOOL HdFuturePage::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_FUT_GRID);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	// Create the Windows control and attach it to the Grid object
	future_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID10);
	SetTimer(1, 500, NULL);
	init_future_view();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void HdFuturePage::OnCbnSelchangeComboFutMarket()
{
	// TODO: Add your control notification handler code here
	//_FutureGrid.SetProductSection();
	//_FutureGrid.InitGrid();
	//if (_SymSelecter && _SymSelecter->SelTab == 0)
	//	_FutureGrid.GetSymbolMaster();
	int cur_sel = _ComboFutureMarket.GetCurSel();
	if (cur_sel < 0) return;
	future_view_.init_symbol(cur_sel);
}

void HdFuturePage::OnTimer(UINT_PTR nIDEvent)
{
	//future_view_.OnQuoteEvent("");
	//future_view_.Update();
	CBCGPDialog::OnTimer(nIDEvent);
}

void HdFuturePage::init_future_view()
{
	const std::vector<DmFuture>& future_vec = mainApp.SymMgr()->get_dm_future_vec();
	for (auto& future : future_vec)
	{
		_ComboFutureMarket.AddString(future.future_name.c_str());
	}
	_ComboFutureMarket.SetCurSel(0);
	future_view_.init_symbol(0);
}

void HdFuturePage::SetHftConfig(SmHftConfig* hftConfig)
{
	//_FutureGrid.SetHftConfig(hftConfig);
}

void HdFuturePage::GetSymbolMaster()
{
	//_FutureGrid.GetSymbolMaster();
}

void HdFuturePage::SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg)
{
	//_FutureGrid.SetConfigDlg(ConfigDlg);
}

void HdFuturePage::SetAddConSigDlg(SmAddConnectSignalDlg* conSigDlg)
{
	//_FutureGrid.SetAddConSigDlg(conSigDlg);
}

void HdFuturePage::SetSigConGrid(VtSignalConnectionGrid* sigConGrid)
{
	//_FutureGrid.SetSigConGrid(sigConGrid);
}

void HdFuturePage::SetChartTimeToolBar(VtChartTimeToolBar* timeToolBar)
{
	//_FutureGrid.SetChartTimeToolBar(timeToolBar);
}
