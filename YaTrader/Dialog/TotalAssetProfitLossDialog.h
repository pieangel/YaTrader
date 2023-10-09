#pragma once
#include <BCGCBProInc.h>
#include <map>
#include <string>
#include "../SmGrid/SmTotalAssetArea.h"
#include "../View/TotalAssetProfitLossView.h"
// SmTotalAssetDialog dialog
class TotalAssetProfitLossDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(TotalAssetProfitLossDialog)

public:
	TotalAssetProfitLossDialog(CWnd* pParent = nullptr);   // standard constructor
	TotalAssetProfitLossDialog(CWnd* pParent, const std::string& type, const std::string& account_no);   // standard constructor
	virtual ~TotalAssetProfitLossDialog();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOTAL_ASSET };
#endif

	std::string account_no() const { return account_no_; }
	void account_no(std::string val) { account_no_ = val; }
	std::string type() const { return type_; }
	void type(std::string val) { type_ = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	// key : combo index, value : account object.
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _ComboAccountMap;
	int _CurrentAccountIndex{ 0 };
	TotalAssetProfitLossView total_asset_profit_loss_view_;
	std::string account_no_;
	std::string type_;
public:
	void SetAccount();
	virtual BOOL OnInitDialog();
	CBCGPComboBox _ComboAccount;
	afx_msg void OnBnClickedBtnGet();
	afx_msg void OnCbnSelchangeComboAccount();
	virtual void PostNcDestroy();
};
