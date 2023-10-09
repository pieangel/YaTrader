// HdOptionPage.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "HdOptionPage.h"
#include "afxdialogex.h"
#include "HdSymbolSelecter.h"
#include "../Global/SmTotalManager.h"

#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmProductYearMonth.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Symbol/SmProduct.h"
#include "../Util/SmUtil.h"
#include <format>

// HdOptionPage dialog
//extern TApplicationFont g_Font;

IMPLEMENT_DYNAMIC(HdOptionPage, CBCGPDialog)

HdOptionPage::HdOptionPage(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_OPT_PAGE, pParent)
{
	_SymSelecter = nullptr;
}

HdOptionPage::~HdOptionPage()
{
}

void HdOptionPage::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_OPT_MARKET, combo_option_market_);
	DDX_Control(pDX, IDC_COMBO_OPT_MONTH, combo_option_month_);
	DDX_Control(pDX, IDC_STATIC_OPTION, option_view_);
}


BEGIN_MESSAGE_MAP(HdOptionPage, CBCGPDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_OPT_MARKET, &HdOptionPage::OnCbnSelchangeComboOptMarket)
	ON_CBN_SELCHANGE(IDC_COMBO_OPT_MONTH, &HdOptionPage::OnCbnSelchangeComboOptMonth)
END_MESSAGE_MAP()


// HdOptionPage message handlers


void HdOptionPage::set_source_window_id(const int window_id)
{
	option_view_.order_window_id(window_id);
}

BOOL HdOptionPage::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	//::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());

	//_OptionGrid.OptionPage(this);
	// TODO:  Add extra initialization here
	//_OptionGrid.AttachGrid(this, IDC_STATIC_OPT_GRID);
	//_OptionGrid.SymSelecter(_SymSelecter);
	//if (_SymSelecter && _SymSelecter->SelTab == 1)
	//	_OptionGrid.GetSymbolMasterLately();

	option_view_.SetUp();

	init_option_market();
	set_option_view();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void HdOptionPage::OnCbnSelchangeComboOptMarket()
{
	option_market_index = combo_option_market_.GetCurSel();
	if (option_market_index < 0) return;

	combo_option_month_.ResetContent();
	option_yearmonth_index_map.clear();
	std::vector<DarkHorse::DmOption>& option_vec = mainApp.SymMgr()->get_dm_option_vec();
	const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map = option_vec[option_market_index].call_product->get_yearmonth_map();
	for (auto it = year_month_map.begin(); it != year_month_map.end(); it++) {
		const std::string& name = it->second->get_name();
		int index = combo_option_month_.AddString(name.c_str());
		option_yearmonth_index_map[index] = name;
	}
	year_month_index = 0;
	combo_option_month_.SetCurSel(year_month_index);
	set_option_view();
}


void HdOptionPage::OnCbnSelchangeComboOptMonth()
{
	if (option_market_index < 0) return;

	year_month_index = combo_option_month_.GetCurSel();
	set_option_view();
}

void HdOptionPage::set_option_view()
{
	const std::string year_month_name = option_yearmonth_index_map[year_month_index];
	option_view_.set_option_view(option_market_index, year_month_name);
}

void HdOptionPage::init_option_market()
{
	std::vector<DarkHorse::DmOption>& option_vec = mainApp.SymMgr()->get_dm_option_vec();
	for (size_t i = 0; i < option_vec.size(); i++) {
		int index = combo_option_market_.AddString(option_vec[i].option_name.c_str());
	}
	option_market_index = 0;
	combo_option_market_.SetCurSel(option_market_index);
	const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map = option_vec[option_market_index].call_product->get_yearmonth_map();
	for (auto it = year_month_map.begin(); it != year_month_map.end(); it++) {
		const std::string& name = it->second->get_name();
		int index = combo_option_month_.AddString(name.c_str());
		option_yearmonth_index_map[index] = name;
	}
	year_month_index = 0;
	combo_option_month_.SetCurSel(year_month_index);
}

void HdOptionPage::GetSymbolMaster()
{
	//_OptionGrid.GetSymbolMasterLately();
}

void HdOptionPage::SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg)
{
	//_OptionGrid.SetConfigDlg(ConfigDlg);
}

void HdOptionPage::SetAddConSigDlg(SmAddConnectSignalDlg* conSigDlg)
{
	//_OptionGrid.SetAddConSigDlg(conSigDlg);
}

void HdOptionPage::SetSigConGrid(VtSignalConnectionGrid* sigConGrid)
{
	//_OptionGrid.SetSigConGrid(sigConGrid);
}

void HdOptionPage::SetChartTimeToolBar(VtChartTimeToolBar* timeToolBar)
{
	//_OptionGrid.SetChartTimeToolBar(timeToolBar);
}

void HdOptionPage::SetHftConfig(SmHftConfig* hftConfig)
{
	//_OptionGrid.SetHftConfig(hftConfig);
}
