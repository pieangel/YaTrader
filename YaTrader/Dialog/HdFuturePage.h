#pragma once
#include "afxwin.h"
#include <BCGCBProInc.h>
#include "SymbolFutureView.h"
// HdFuturePage dialog
class HdSymbolSelecter;
class VtOrderConfigManager;
class VtUsdStrategyConfigDlg;
class SmAddConnectSignalDlg;
class VtSignalConnectionGrid;
class VtChartTimeToolBar;
class SmHftConfig;
class HdFuturePage : public CBCGPDialog
{
	DECLARE_DYNAMIC(HdFuturePage)

public:
	HdFuturePage(CWnd* pParent = NULL);   // standard constructor
	virtual ~HdFuturePage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FUT_PAGE };
#endif

	HdSymbolSelecter* SymSelecter() const { return _SymSelecter; }
	void SymSelecter(HdSymbolSelecter* val) { _SymSelecter = val; }
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) 
	{ 
		_OrderConfigMgr = val; 
		//_FutureGrid.OrderConfigMgr(_OrderConfigMgr);
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void set_source_window_id(const int window_id);
	CComboBox _ComboFutureMarket;
	//HdFutureGrid _FutureGrid;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboFutMarket();

private:
	HdSymbolSelecter* _SymSelecter;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	SymbolFutureView future_view_;
	void init_future_view();
public:
	void SetHftConfig(SmHftConfig* hftConfig);
	void GetSymbolMaster();
	void SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg);
	void SetAddConSigDlg(SmAddConnectSignalDlg* conSigDlg);
	void SetSigConGrid(VtSignalConnectionGrid* sigConGrid);
	void SetChartTimeToolBar(VtChartTimeToolBar* timeToolBar);
};
