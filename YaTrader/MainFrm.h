// MainFrm.h : interface of the CMainFrame class
//


#pragma once
#include "SymbolBar.h"
#include "WorkSpaceBar2.h"
#include "OutputBar.h"
#include "PropertiesViewBar.h"
#include <memory>
#include <map>

#define DATA_REQ_TIMER 100
namespace DarkHorse {
class SmChartData;
}

class CTestDialog;
class AbAccountOrderCenterWindow;
class AbAccountOrderLeftWindow;
class AbAccountOrderRightWindow;
class AbAccountOrderWindow;
class SmFundOrderDialog;
class SmFundDialog;
class SmOrderCompMainDialog;
class SmFilledListDialog;
class SmTotalAssetDialog;
class SmJangGoDialog;
class SmFundCompMainDialog;
class GaSpreadChart;
class SmMultiSpreadChart;
class SmCompChart;
class SmGigaSpreadChart;
class SmGigaCompChart;
class RealTimeMultiChart;
class SmStockChart;
class SmTrendChart;
class DmAccountOrderWindow;
class DmFundOrderWindow;
class VtProgressDlg;
class SmLoginDlg;
class MiniJangoDialog;
class TotalAssetProfitLossDialog;
class AbFundOrderWindow;
class VtAutoSignalManagerDialog;
class CMainFrame : public CBCGPMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CBCGPStatusBar			m_wndStatusBar;
	CBCGPMenuBar			m_wndMenuBar;
	CBCGPExplorerToolBar			m_wndToolBar;
	CSymbolBar			    m_wndWorkSpace;
	CWorkSpaceBar2			m_wndWorkSpace2;
	COutputBar				m_wndOutput;
	CBCGPPropBar			m_wndPropGrid;
	std::shared_ptr<VtProgressDlg> ProgressDlg = nullptr;

// Generated message map functions
protected:
	int						m_nColorTheme;
	BOOL					m_bIsDarkTheme;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);
	afx_msg void OnWindowManager();
	afx_msg void OnMdiMoveToNextGroup();
	afx_msg void OnMdiMoveToPrevGroup();
	afx_msg void OnMdiNewHorzTabGroup();
	afx_msg void OnMdiNewVertGroup();
	afx_msg void OnMdiCancel();
	afx_msg void OnClose();
	afx_msg void OnColorThemeCombo();
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bDrop);
	virtual CBCGPMDIChildWnd* CreateDocumentWindow(LPCTSTR lpcszDocName, CObject* /*pObj*/);

public:
	void add_dm_order_wnd(DmAccountOrderWindow* wnd);
	void start_login();
	void start_timer(int milisecond);
	void stop_timer();
	void LoadAfterServerData();
	void HideProgress();
	// window id
	static int _Id;
	static int GetId() { return ++_Id; }

	void StartLoad();
	int	GetColorTheme() const
	{
		return m_nColorTheme;
	}

	BOOL IsDarkTheme() const
	{
		return m_bIsDarkTheme;
	}
	void StartDataRequest();
	void SetMarketTree();
	void SetAccountInfo();
	void UpdateChart();
	void SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& window_id, const int& series_index = 0);
	afx_msg void OnTestOrderwnd();
	std::shared_ptr<AbAccountOrderCenterWindow> _OrderWnd = nullptr;
	afx_msg void OnTestLeftorderdialog();

	std::shared_ptr<SmLoginDlg> login_dialog_ = nullptr;

	std::shared_ptr<AbAccountOrderLeftWindow> _LeftOrderWnd = nullptr;
	std::shared_ptr<AbAccountOrderRightWindow> _RightOrderWnd = nullptr;
	std::shared_ptr<AbAccountOrderWindow> _MainOrderWnd = nullptr;
	std::shared_ptr<SmFundOrderDialog> _FundOrderWnd = nullptr;
	std::shared_ptr< SmOrderCompMainDialog> _CompOrderWnd = nullptr;
	std::shared_ptr<CTestDialog> _TestDlg = nullptr;
	std::shared_ptr< SmFundDialog> _FundDialog = nullptr;
	std::shared_ptr< SmFilledListDialog> _FilledDialog = nullptr;
	std::shared_ptr< SmTotalAssetDialog> _TotalAssetDialog = nullptr;
	std::shared_ptr< SmJangGoDialog> _AccountJangoDialog = nullptr;
	std::shared_ptr< SmJangGoDialog> _FundJangoDialog = nullptr;
	std::shared_ptr< SmFundCompMainDialog> _FundCompOrderDialog = nullptr;

	DmAccountOrderWindow* dm_account_order_wnd_p_ = nullptr;
	std::shared_ptr<DmFundOrderWindow> dm_fund_order_wnd_p = nullptr;
	std::map<HWND, DmAccountOrderWindow*> dm_account_order_wnd_map_;
	std::map<HWND, DmFundOrderWindow*> dm_fund_order_wnd_map_;

	std::map<HWND, AbAccountOrderWindow*> _OrderWndMap;

	std::map<HWND, std::shared_ptr<SmOrderCompMainDialog>> _CompOrderWndMap;
	std::map<HWND, std::shared_ptr<MiniJangoDialog>> _JangoWndMap;
	std::map<HWND, std::shared_ptr<MiniJangoDialog>> mini_jango_wnd_map_;
	std::map<HWND, std::shared_ptr<SmFilledListDialog>> _FilledWndMap;

	std::map<HWND, std::shared_ptr<AbFundOrderWindow>> _FundOrderWndMap;

	std::map<HWND, std::shared_ptr<SmFundCompMainDialog>> _FundCompOrderWndMap;

	std::map<HWND, std::shared_ptr<SmTotalAssetDialog>> _AssetWndMap;

	std::map<HWND, std::shared_ptr<TotalAssetProfitLossDialog>> total_asset_profit_loss_map_;

	std::map<HWND, std::shared_ptr<GaSpreadChart>> _ChartMap;
	std::map<HWND, std::shared_ptr<SmMultiSpreadChart>> _SpreadChartMap;

	std::map<HWND, std::shared_ptr<SmCompChart>> _CompChartMap;
	std::map<HWND, std::shared_ptr<SmGigaSpreadChart>> _GigaSpreadChartMap;
	std::map<HWND, std::shared_ptr<SmStockChart>> _StockChartMap;

	std::map<HWND, std::shared_ptr<RealTimeMultiChart>> _RealTimeChartMap;

	std::map<HWND, std::shared_ptr<SmTrendChart>> _TrendChartMap;

	VtAutoSignalManagerDialog* auto_signal_manager_dlg_ = nullptr;
	void remove_dm_account_order_window(HWND handle);
	void remove_dm_fund_order_window(HWND handle);
	void RemoveOrderWnd(HWND wnd);
	void RemoveFundOrderWnd(HWND wnd);
	void RemoveJangoWnd(HWND wnd);
	void RemoveFilledWnd(HWND wnd);
	void RemoveCompWnd(HWND wnd);
	void RemoveFundCompWnd(HWND wnd);
	void RemoveAssetWnd(HWND wnd);
	afx_msg void OnTestRightorderdialog();
	afx_msg void OnTestOrdermain();
	void ReconnectOrderEvent();
	afx_msg void OnTestSymboltable();
	afx_msg void OnTestFund();
	afx_msg void OnOrderOrdermain();
	afx_msg void OnOrderOrderfund();
	afx_msg void OnOrderEditfund();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnFilledList();
	afx_msg void OnMultiOrderForAccount();
	afx_msg void OnServerMsg();
	afx_msg void OnOpenSettings();
	afx_msg void OnAsset();
	afx_msg void OnManagePwd();
	afx_msg void OnOrderWnd();
	afx_msg void OnComplexOrder();
	afx_msg void OnOrderRemain();
	afx_msg void OnEditFund();
	afx_msg void OnFundOrder();
	afx_msg void OnFundRemain();
	afx_msg void OnFundPl();
	afx_msg void OnNewChart();
	afx_msg void OnStratege1();
	afx_msg void OnFundComplexOrder();
	afx_msg void OnSpreadChart();
	afx_msg void OnChartComp();
	afx_msg void OnGigaSpreadChart();
	afx_msg void OnGigaCompChart();
	afx_msg void OnStockChart();
	afx_msg void OnRealtimeMultiChart();
	afx_msg void OnMulticolorChart();
	afx_msg void OnDomesticAccountOrder();
	afx_msg void OnDomesticFundOrder();
	afx_msg void OnDmAcntOrder();
	afx_msg void OnSetSimulationMode();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnableSimulationFilledOrder();
	afx_msg void OnDmFundOrder();
	afx_msg void OnSubAccount();
	afx_msg void OnOutSystem();
	afx_msg void OnThemeDarkgray();
	afx_msg void OnThemeBlue();
	afx_msg void OnThemeDark();
	afx_msg void OnThemeColorful();
	afx_msg void OnDomesticRemain();
	afx_msg void OnAbroadRemain();
	afx_msg void OnDmFundRemain();
	afx_msg void OnAbFundRemain();
	afx_msg void OnDestroy();
	afx_msg void OnSimulStartsimul();
	afx_msg void OnSimulStopsimul();
	afx_msg void OnUsdSystem();
	void copyLastLineAndAppend(const std::string& filename);
	afx_msg void OnSimulYestest();
};
