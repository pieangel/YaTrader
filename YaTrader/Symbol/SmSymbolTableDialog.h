#pragma once
#include "../SmGrid/SmSymbolTableArea.h"
#include "../Util/SmButton.h"
#include <map>
#include <vector>

// SmSymbolTableDialog dialog
namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmMarket;
}
class AbAccountOrderCenterWindow;
class SmFundOrderCenterWnd;
class AbAccountOrderLeftWindow;
class SmFavoriteGrid;
class SmFundOrderLeft;
class SmCompOrderDialog;
class SmOrderLeftComp;
class GaSpreadChart;
class SmMultiSpreadChart;
class FavoriteSymbolView;
class SmSymbolTableDialog : public CBCGPScrollDialog
{
	DECLARE_DYNAMIC(SmSymbolTableDialog)

public:
	SmSymbolTableDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmSymbolTableDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYMBOL_TABLE };
#endif


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	SmButton _Btn1;
	SmButton _Btn2;
	SmButton _Btn3;
	SmButton _Btn4;
	SmButton _Btn5;
	SmButton _Btn6;
	SmButton _Btn7;

	CBCGPScrollBar _HScrollBar;
	CBCGPScrollBar _VScrollBar;
	
	std::map<std::string, std::shared_ptr<DarkHorse::SmMarket>> _MarketMap;
	std::vector<SmButton*> _ButtonVec;
	std::vector<std::string> _MarketNameVec;

	SmSymbolTableArea _SymbolTableArea;
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnUmChangeMarket(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmProductWheeled(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmSymbolSelected(WPARAM wParam, LPARAM lParam);

	AbAccountOrderCenterWindow* OrderWnd = nullptr;
	SmFundOrderCenterWnd* FundOrderWnd = nullptr;
	AbAccountOrderLeftWindow* OrderLeftWnd = nullptr;
	SmFavoriteGrid* FavoriteGrid = nullptr;
	SmFundOrderLeft* FundOrderLeft = nullptr;
	SmCompOrderDialog* CompOrderWnd = nullptr;
	SmOrderLeftComp* CompOrderLeft = nullptr;
	GaSpreadChart* SpreadChart = nullptr;
	FavoriteSymbolView* favorite_symbol_view_ = nullptr;
	SmMultiSpreadChart* MultiSpreadChart = nullptr;
	int order_window_id() const { return order_window_id_; }
	void order_window_id(int val) { order_window_id_ = val; }
private:
	int order_window_id_ = 0;
	bool _MoreSymbol{ false };
	int _SelectedMarketIndex{ 0 };
	int _StartMarketRowIndex{ 0 };
	void SetMarket(std::shared_ptr<DarkHorse::SmMarket> market);
	void ShowProductList();
	
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
