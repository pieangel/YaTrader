// workspace.cpp : implementation of the CWorkSpaceBar class
//

#include "stdafx.h"
#include "DarkHorse.h"
#include "SymbolBar.h"
#include "Global/SmTotalManager.h"
#include "Symbol/SmSymbolManager.h"
#include "Symbol/SmMarket.h"
#include "Symbol/SmMarketConst.h"
#include "Symbol/SmProduct.h"
#include "Symbol/SmSymbol.h"
#include "Chart/SmChartDataManager.h"
#include "Util/VtStringUtil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define IDC_MARKET_TREE 1
/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar

BEGIN_MESSAGE_MAP(CSymbolBar, CBCGPDockingControlBar)
	ON_WM_CREATE()
	ON_NOTIFY(NM_DBLCLK, IDC_MARKET_TREE, doubleClickedTree)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar construction/destruction

CSymbolBar::CSymbolBar()
{
	// TODO: add one-time construction code here

}

CSymbolBar::~CSymbolBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar message handlers

int CSymbolBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tree control:
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	m_wndTree.m_bVisualManagerStyle = TRUE;

	if (!m_wndTree.Create(dwViewStyle, rectDummy, this, IDC_MARKET_TREE))
	{
		TRACE0("Failed to create workspace view\n");
		return -1;      // fail to create
	}

	// Setup tree content:
// 	HTREEITEM hRoot = m_wndTree.InsertItem(_T("Root 1"));
// 	m_wndTree.InsertItem(_T("Item 1"), hRoot);
// 	m_wndTree.InsertItem(_T("Item 2"), hRoot);

	//m_wndTree.Expand(hRoot, TVE_EXPAND);

	return 0;
}

void CSymbolBar::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	// Tree control should cover a whole client area:
	m_wndTree.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CSymbolBar::SetMarketTree()
{
	const HTREEITEM hRoot = m_wndTree.InsertItem(DarkHorse::AbroadMarketName.c_str());
	const std::map<std::string, std::shared_ptr<DarkHorse::SmMarket>>& market_map = mainApp.SymMgr()->GetMarketMap();
	for(auto it = market_map.begin(); it != market_map.end(); ++it) {
		const auto market = it->second;
		const std::map<std::string, std::shared_ptr<DarkHorse::SmProduct>>& product_map = market->GetProductMap();
		const HTREEITEM hMarket = m_wndTree.InsertItem(market->MarketName().c_str(), hRoot);
		for(auto it2 = product_map.begin(); it2 != product_map.end(); ++it2) {
			const auto product = it2->second;
			const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec = product->GetSymbolVec();
			if (symbol_vec.empty()) continue;
			
			const HTREEITEM hProduct = m_wndTree.InsertItem(product->ProductNameKr().c_str(), hMarket);
			for(auto it3 = symbol_vec.begin(); it3 != symbol_vec.end(); ++it3) {
				const auto symbol = *it3;
				std::string symbol_info(symbol->SymbolNameKr());
				symbol_info.append(" [");
				symbol_info.append(symbol->SymbolCode());
				symbol_info.append("]");
				m_wndTree.InsertItem(symbol_info.c_str(), hProduct);
			}
			m_wndTree.Expand(hProduct, TVE_EXPAND);
		}
		m_wndTree.Expand(hMarket, TVE_EXPAND);
	}

	m_wndTree.Expand(hRoot, TVE_EXPAND);
}

void CSymbolBar::doubleClickedTree(NMHDR* pNmhdr, LRESULT* pResult)
{
	const HTREEITEM hItem = m_wndTree.GetSelectedItem();
	if (m_wndTree.GetChildItem(hItem) == nullptr)
	{
		const CString str_symbol_info = m_wndTree.GetItemText(hItem);
		//::MessageBox(nullptr, symbol_info, symbol_info, MB_OK);
		const std::string symbol_info = std::string(static_cast<const char*>(str_symbol_info));
		symbol_click(symbol_info);
		const std::string symbol_code = DarkHorse::VtStringUtil::get_str_between_two_str(symbol_info, "[", "]");
		mainApp.ChartDataMgr()->RequestChartData(symbol_code, -1, -1);
	}
}

