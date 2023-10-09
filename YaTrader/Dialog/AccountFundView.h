#pragma once

#include <BCGCBProInc.h>
#include <map>
#include <memory>	
namespace DarkHorse
{
	class SmSymbol;
	class SmAccount;
	class SmFund;
}
const int grid_row_count5 = 300;
class AccountFundView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(AccountFundView)
public:
	AccountFundView();
	virtual ~AccountFundView();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	int mode() const { return mode_; }
	void mode(int val) { mode_ = val; }
	void init_grid();
	void set_source_id(const int source_id) { source_id_ = source_id; }
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	COLORREF _DefaultBackColor;
	void ClearGrid();
	bool init_ = false;
	int mode_ = 0;
	// key : row, value : account
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>>  row_to_account_map_;
	// key : row, value : fund
	std::map<int, std::shared_ptr<DarkHorse::SmFund>>  row_to_fund_map_;
	// selected account
	std::shared_ptr<DarkHorse::SmAccount> account_;
	// selected fund
	std::shared_ptr<DarkHorse::SmFund> fund_;
	int source_id_ = 0;
};
