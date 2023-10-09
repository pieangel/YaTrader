// SmSymbolTableDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmSymbolTableDialog.h"
#include "afxdialogex.h"
#include "../Global/SmTotalManager.h"
#include "SmSymbolManager.h"
#include "SmMarket.h"
#include "SmMarketConst.h"
#include "SmProduct.h"
#include "SmSymbol.h"
#include "../Order/AbAccountOrderCenterWindow.h"
#include "../Fund/SmFundOrderCenterWnd.h"
#include "../Order/SmFavoriteGrid.h"
#include "../Order/AbAccountOrderLeftWindow.h"
#include "../CompOrder/SmCompOrderDialog.h"
#include "../CompOrder/SmOrderLeftComp.h"
#include "../ChartDialog/GaSpreadChart.h"
#include "../ChartDialog/SmMultiSpreadChart.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"


// SmSymbolTableDialog dialog

IMPLEMENT_DYNAMIC(SmSymbolTableDialog, CBCGPScrollDialog)

SmSymbolTableDialog::SmSymbolTableDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPScrollDialog(IDD_SYMBOL_TABLE, pParent)
{
	//_OrderWnd = (SmOrderWnd*)pParent;
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmSymbolTableDialog::~SmSymbolTableDialog()
{
}

void SmSymbolTableDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPScrollDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SYMBOL, _SymbolTableArea);
	DDX_Control(pDX, IDC_STATIC_TOOL1, _Btn1);
	DDX_Control(pDX, IDC_STATIC_TOOL2, _Btn2);
	DDX_Control(pDX, IDC_STATIC_TOOL3, _Btn3);
	DDX_Control(pDX, IDC_STATIC_TOOL4, _Btn4);
	DDX_Control(pDX, IDC_STATIC_TOOL5, _Btn5);
	DDX_Control(pDX, IDC_STATIC_TOOL6, _Btn6);
	DDX_Control(pDX, IDC_STATIC_TOOL7, _Btn7);
	DDX_Control(pDX, IDC_SCROLLBAR_HOR, _HScrollBar);
	DDX_Control(pDX, IDC_SCROLLBAR_VER, _VScrollBar);
}


BEGIN_MESSAGE_MAP(SmSymbolTableDialog, CBCGPScrollDialog)
	ON_MESSAGE(UM_CHANGE_SYMBOL, &SmSymbolTableDialog::OnUmChangeMarket)
	ON_MESSAGE(UM_PRODUCT_WHEELED, &SmSymbolTableDialog::OnUmProductWheeled)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &SmSymbolTableDialog::OnUmSymbolSelected)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// SmSymbolTableDialog message handlers


BOOL SmSymbolTableDialog::OnInitDialog()
{
	CBCGPScrollDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	_SymbolTableArea.SetUp();
	_Btn1.SetUp();
	_Btn1.Index(0);
	_Btn2.SetUp();
	_Btn2.Index(1);
	_Btn3.SetUp();
	_Btn3.Index(2);
	_Btn4.SetUp();
	_Btn4.Index(3);
	_Btn5.SetUp();
	_Btn5.Index(4);
	_Btn6.SetUp();
	_Btn6.Index(5);
	_Btn7.SetUp();
	_Btn7.Index(6);

	_ButtonVec.push_back(&_Btn1);
	_ButtonVec.push_back(&_Btn2);
	_ButtonVec.push_back(&_Btn3);
	_ButtonVec.push_back(&_Btn4);
	_ButtonVec.push_back(&_Btn5);
	_ButtonVec.push_back(&_Btn6);
	_ButtonVec.push_back(&_Btn7);

	_MarketNameVec.push_back("금리");
	_MarketNameVec.push_back("금속");
	_MarketNameVec.push_back("농산물");
	_MarketNameVec.push_back("에너지");
	_MarketNameVec.push_back("지수");
	_MarketNameVec.push_back("축산물");
	_MarketNameVec.push_back("통화");

	_Btn1.Text(_MarketNameVec[0]);
	_Btn1.Selected(true);
	_Btn2.Text(_MarketNameVec[1]);
	_Btn3.Text(_MarketNameVec[2]);
	_Btn4.Text(_MarketNameVec[3]);
	_Btn5.Text(_MarketNameVec[4]);
	_Btn6.Text(_MarketNameVec[5]);
	_Btn7.Text(_MarketNameVec[6]);

	auto market = mainApp.SymMgr()->FindMarket(_MarketNameVec[0]);
	SetMarket(market);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT SmSymbolTableDialog::OnUmChangeMarket(WPARAM wParam, LPARAM lParam)
{
	_SymbolTableArea.FixedSelectedCell(nullptr);
	size_t index = static_cast<size_t>(wParam);
	for (size_t i = 0; i < _ButtonVec.size(); i++) {
		if (i == index) _ButtonVec[i]->Selected(true);
		else _ButtonVec[i]->Selected(false);
		_ButtonVec[i]->Invalidate();
	}
	_SelectedMarketIndex = index;
	_SymbolTableArea.ClearValues();
	auto market = mainApp.SymMgr()->FindMarket(_MarketNameVec[_SelectedMarketIndex]);
	SetMarket(market);

	return 1;
}

LRESULT SmSymbolTableDialog::OnUmProductWheeled(WPARAM wParam, LPARAM lParam)
{
	if (!_VScrollBar.GetSafeHwnd())	return 0;
	if (!_MoreSymbol) return 0;

	_SymbolTableArea.FixedSelectedCell(nullptr);
	int pos;

	int delta = -1 * (int)wParam;

	SCROLLINFO  scrinfo;
	_VScrollBar.GetScrollInfo(&scrinfo);

	if (delta != 0) {
		pos = _VScrollBar.GetScrollPos();
		_VScrollBar.SetScrollPos(pos + delta);


	}

	_StartMarketRowIndex = scrinfo.nPos;
	ShowProductList();

	return 1;
}

LRESULT SmSymbolTableDialog::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = (int)wParam;
	std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbolById(symbol_id);
	if (symbol) {
		std::string title;
		title = "종목선택 : [";
		title.append(symbol->SymbolNameKr());
		title.append("]");
		SetWindowText(title.c_str());

		mainApp.event_hub()->process_symbol_event(order_window_id_, symbol);

	}
	else {
		SetWindowText("종목선택");
	}



	if (OrderWnd) OrderWnd->SendMessage(UM_SYMBOL_SELECTED, symbol_id, 0);
	if (FundOrderWnd) FundOrderWnd->SendMessage(UM_SYMBOL_SELECTED, symbol_id, 0);
	if (OrderLeftWnd) OrderLeftWnd->SendMessage(UM_SYMBOL_SELECTED, symbol_id, 0);
	if (favorite_symbol_view_) favorite_symbol_view_->SendMessage(UM_SYMBOL_SELECTED, symbol_id, 0);
	if (CompOrderWnd) CompOrderWnd->SendMessage(UM_SYMBOL_SELECTED, symbol_id, 0);
	if (CompOrderLeft) CompOrderLeft->SendMessage(UM_SYMBOL_SELECTED, symbol_id, 0);
	if (SpreadChart) SpreadChart->SendMessage(UM_SYMBOL_SELECTED, symbol_id, 0);
	if (MultiSpreadChart) MultiSpreadChart->SendMessage(UM_SYMBOL_SELECTED, symbol_id, 0);

	
	CBCGPScrollDialog::OnOK();

	return 1;
}

void SmSymbolTableDialog::SetMarket(std::shared_ptr<DarkHorse::SmMarket> market)
{
	if (!market) return;

	int row_count = _SymbolTableArea.GetRowCount();
	int product_count = market->GetValidProductCount();
	_StartMarketRowIndex = 0;
	
	ShowProductList();
	if (product_count > row_count - 1) {
		_VScrollBar.ShowWindow(SW_SHOW);
		SCROLLINFO  scrinfo;
		scrinfo.cbSize = sizeof(scrinfo);
		scrinfo.fMask = SIF_ALL;
		scrinfo.nMin = 0;          // 최소값
		scrinfo.nMax = product_count;      // 최대값
		scrinfo.nPage = row_count;      // 전체 스크롤바에서 스크롤박스가 차지하는 범위 
		scrinfo.nTrackPos = 0;  // 트랙바가 움직일때의 위치값
		scrinfo.nPos = 0;        // 위치
		_VScrollBar.SetScrollInfo(&scrinfo);
		_MoreSymbol = true;
	}
	else {
		_VScrollBar.ShowWindow(SW_HIDE);
		_MoreSymbol = false;
	}
}


void SmSymbolTableDialog::ShowProductList()
{
	_SymbolTableArea.ClearValues();
	auto market = mainApp.SymMgr()->FindMarket(_MarketNameVec[_SelectedMarketIndex]);
	const std::map<std::string, std::shared_ptr<DarkHorse::SmProduct>>& product_map = market->GetProductMap();
	std::vector<std::shared_ptr<DarkHorse::SmProduct>> product_vector;
	int row_count = _SymbolTableArea.GetRowCount();
	market->GetProductVector(_StartMarketRowIndex, row_count, product_vector);

	int row = 1;
	for (auto it2 = product_vector.begin(); it2 != product_vector.end(); ++it2) {
		const auto product = *it2;
		const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec = product->GetSymbolVec();
		_SymbolTableArea.SetProductInfo(row, product->ExchangeName(), product->ProductNameKr(), product->ProductCode());
		_SymbolTableArea.SetSymbolList(row, symbol_vec);
		row++;
	}

	_SymbolTableArea.Invalidate();
}

void SmSymbolTableDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	
	if (!pScrollBar) return;
	if (pScrollBar->GetSafeHwnd() != _VScrollBar.GetSafeHwnd()) return;

	int pos;

	int delta = 0;
	// 어디를 눌러서 이동하였는가?
	int prev = pScrollBar->GetScrollPos();
	switch (nSBCode)
	{
	case SB_THUMBTRACK: // 스크롤 바를 잡고 움직인 경우
		pScrollBar->SetScrollPos(nPos); // 누른 위치로 스크롤 바 셋팅
		break;
	case SB_LINEUP: // 스크롤의 화살표를 누름
		delta = -1;
		break;
	case SB_LINEDOWN:
		delta = 1;
		break;
	case SB_PAGEUP: // 스크롤의 임의의 부분을 누름
		delta = -3;
		break;
	case SB_PAGEDOWN:
		delta = 3;
		break;
	}

	SCROLLINFO  scrinfo;
	pScrollBar->GetScrollInfo(&scrinfo);

	if (delta != 0) {
		pos = pScrollBar->GetScrollPos();
		pScrollBar->SetScrollPos(pos + delta);

		
	}

	_StartMarketRowIndex = scrinfo.nPos;
	ShowProductList();

	CBCGPScrollDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void SmSymbolTableDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (!pScrollBar) return;
	if (pScrollBar->GetSafeHwnd() != _HScrollBar.GetSafeHwnd()) return;

	int pos;

	int delta = 0;
	// 어디를 눌러서 이동하였는가?

	switch (nSBCode)
	{
	case SB_THUMBTRACK: // 스크롤 바를 잡고 움직인 경우
		pScrollBar->SetScrollPos(nPos); // 누른 위치로 스크롤 바 셋팅
		break;
	case SB_LINEUP: // 스크롤의 화살표를 누름
		delta = -1;
		break;
	case SB_LINEDOWN:
		delta = 1;
		break;
	case SB_PAGEUP: // 스크롤의 임의의 부분을 누름
		delta = -4;
		break;
	case SB_PAGEDOWN:
		delta = 4;
		break;
	}

	if (delta != 0) {
		pos = pScrollBar->GetScrollPos();
		pScrollBar->SetScrollPos(pos + delta);
	}
	//Invalidate(); // 다시 그려주기

	CBCGPScrollDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
