#pragma once
#include <BCGCBProInc.h>

// SmCompOrderDialog dialog
#include "SmCompOrderArea.h"
#include "../Order/SmOrderConst.h"
// SmOrderWnd dialog
namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}

class SmSymbolTableDialog;
class SmOrderCompMainDialog;
class SmFundCompMainDialog;
class SmCompOrderDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmCompOrderDialog)

public:
	SmCompOrderDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmCompOrderDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COMP_ORDER_CENTER };
#endif
	
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return _Fund; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val) { _Fund = val; }
public:
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	int OrderAmount() const { return _OrderAmount; }
	void OrderAmount(int val) { _OrderAmount = val; }
	DarkHorse::SmPositionType OrderPosition() const { return _OrderPosition; }
	void OrderPosition(DarkHorse::SmPositionType val) { _OrderPosition = val; }

	bool DoOrder() const { return _DoOrder; }
	void DoOrder(bool val) { _DoOrder = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int ID() const { return _ID; }
	void ID(int val) { _ID = val; }
private:
	bool _Selected = false;
	int _ID{ 0 };
	std::shared_ptr< SmSymbolTableDialog> _SymbolTableDlg = nullptr;
	int _CurrentIndex = -1;
	// key : combobox index, value : symbol object
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _IndexToSymbolMap;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	void SetInfo(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;
public:
	virtual BOOL OnInitDialog();
	bool Selected() const { return _Selected; }
	void Selected(bool val) { _Selected = val; }
private:
	void SetSymbolInfo(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	SmCompOrderArea _OrderArea;

	CBCGPButton _BtnSearch;
	CBCGPComboBox _ComboSymbol;
	CBCGPStatic _StaticSymbolName;
private:
	bool _DoOrder = false;
	int _OrderAmount = 0;
	DarkHorse::SmPositionType _OrderPosition = DarkHorse::SmPositionType::None;
public:
	void OnSymbolClicked(const std::string& symbol_code);
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val) { _Account = val; }
	void OnClickSymbol(const std::string& symbol_info);
	void SetSelected(const bool& selected);
	void SetMainOrderWnd(SmOrderCompMainDialog* wnd);
	void SetFundOrderWnd(SmFundCompMainDialog* wnd);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnSearch();
	afx_msg LRESULT OnUmSymbolSelected(WPARAM wParam, LPARAM lParam);
	CBCGPButton _CheckReal;
	CBCGPEdit _EditAmount;
	CBCGPSpinButtonCtrl _SpinAmount;
	CBCGPButton _RadioBuy;
	CBCGPButton _RadioSell;
	afx_msg void OnCbnSelchangeComboSymbol();
	afx_msg void OnBnClickedRadioBuy();
	afx_msg void OnBnClickedRadioSell();
	afx_msg void OnEnChangeEditAmount();
	afx_msg void OnBnClickedCheckReal();
};
