#pragma once
#include "afxwin.h"
#include "../View/DmOptionView.h"
#include <BCGCBProInc.h>
// HdOptionPage dialog
class HdSymbolSelecter;
class VtOrderConfigManager;
class VtUsdStrategyConfigDlg;
class SmAddConnectSignalDlg;
class VtSignalConnectionGrid;
class VtChartTimeToolBar;
class SmHftConfig;
namespace DarkHorse {
	class DmOptionView;
}
class HdOptionPage : public CBCGPDialog
{
	DECLARE_DYNAMIC(HdOptionPage)

public:
	HdOptionPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~HdOptionPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPT_PAGE };
#endif

	HdSymbolSelecter* SymSelecter() const { return _SymSelecter; }
	void SymSelecter(HdSymbolSelecter* val) { _SymSelecter = val; }
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) 
	{ 
		_OrderConfigMgr = val;
		//_OptionGrid.OrderConfigMgr(_OrderConfigMgr);
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void set_source_window_id(const int window_id);
	CComboBox combo_option_market_;
	CComboBox combo_option_month_;

	DmOptionView option_view_;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboOptMarket();
	afx_msg void OnCbnSelchangeComboOptMonth();

private:
	void set_option_view();
	// key : option year-month combo index, value : year-month name
	std::map<int, std::string> option_yearmonth_index_map;
	int year_month_index{ 0 };
	int option_market_index{ 0 };
	void init_option_market();

	HdSymbolSelecter* _SymSelecter;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	void GetSymbolMaster();
	void SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg);
	void SetAddConSigDlg(SmAddConnectSignalDlg* conSigDlg);
	void SetSigConGrid(VtSignalConnectionGrid* sigConGrid);
	void SetChartTimeToolBar(VtChartTimeToolBar* timeToolBar);
	void SetHftConfig(SmHftConfig* hftConfig);
};
