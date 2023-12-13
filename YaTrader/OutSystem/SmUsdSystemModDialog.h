#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <BCGCBProInc.h>
#include <map>
#include <memory>
#include <string>

#include "../Dialog/VtUsdStrategyConfigGrid.h"
#include "../Dialog/VtUsdEntConfigGrid.h"
//#include "VtOutSignalDefManager.h"

// VtAddConnectSignalDlg dialog
namespace DarkHorse {
	class SmAccount;
	class SmFund;
	class SmSymbol;
	class SmOutSignalDef;
	class SmUsdSystem;
}
class SmUSDSystemDialog;
class HdSymbolSelecter;
class VtAutoSignalManagerDialog;
class SmUsdSystemModDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmUsdSystemModDialog)

public:
	SmUsdSystemModDialog(SmUSDSystemDialog* source_dialog, std::shared_ptr<DarkHorse::SmUsdSystem> usd_system);
	SmUsdSystemModDialog(SmUSDSystemDialog* source_dialog);
	SmUsdSystemModDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SmUsdSystemModDialog();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOD_USD_SYSTEM };
#endif
	SmUSDSystemDialog* source_dialog() const { return source_dialog_; }
	void source_dialog(SmUSDSystemDialog* val) { source_dialog_ = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	CDateTimeCtrl _DpEntBegin;
	CDateTimeCtrl _DpEntEnd;
	CDateTimeCtrl _DpLiq;
	CBCGPEdit _EditEntMax;

	CBCGPEdit _EditSeungsu;
	virtual BOOL OnInitDialog();
	int _Mode = 0;
	void InitCombo();
	void InitUsdStrategyCombo();
private:
	 void set_usd_system();
	VtUsdEntConfigGrid _EntGrid;
	VtUsdStrategyConfigGrid _LiqGrid;
	int id_ = 0;
	SmUSDSystemDialog* source_dialog_ = nullptr;
	// key: combo index, value: strategy type
	std::map<int, std::string> combo_usd_strategy_map_;

	std::shared_ptr<DarkHorse::SmUsdSystem> usd_system_ = nullptr;

public:
	afx_msg void OnBnClickedBtnMod();
	afx_msg void OnBnClickedBtnOk();
};
