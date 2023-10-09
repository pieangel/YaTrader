#pragma once
#include <BCGCBProInc.h>
#include "SmFundGrid.h"
#include "SmFundAccountGrid.h"
#include <map>
#include <memory>
// SmFundDialog dialog
namespace DarkHorse {
	class SmFund;
}
class SmFundDialog : public CBCGPScrollDialog
{
	DECLARE_DYNAMIC(SmFundDialog)

public:
	SmFundDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmFundDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FUND };
#endif

	std::shared_ptr<DarkHorse::SmFund> CurFund() const { return _CurFund; }
	void CurFund(std::shared_ptr<DarkHorse::SmFund> val) { _CurFund = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	SmFundGrid _FundGrid;
	SmFundAccountGrid    _FundAccountGrid;
	afx_msg void OnBnClickedBtnFundNew();
	afx_msg void OnBnClickedBtnFundEdit();
	afx_msg void OnBnClickedBtnFundDel();
	virtual BOOL OnInitDialog();
	void FundClickEvent(const int& row);
private:
	void AddFund(std::shared_ptr<DarkHorse::SmFund> fund);
	void UpdateFundInfo(std::shared_ptr<DarkHorse::SmFund> fund);
	void SetFundList();
	HTREEITEM _SelItem;
	int _CurFundId{ -1 };
	int _OldFundId{ -1 };
	void SetFundAccount(std::shared_ptr<DarkHorse::SmFund> fund);
	void ClearFundGrid(const int& row);
	void ClearFundAccountGrid(const int& row);
	int _MaxFundRow = 0;
	int _MaxFundAccountRow = 0;
	std::shared_ptr<DarkHorse::SmFund> _CurFund = nullptr;
public:
	// key : row index, value : SmFund object
	std::map<int, std::shared_ptr<DarkHorse::SmFund>> _RowToFundMap;
	afx_msg void OnTvnSelchangedTreeFund(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedListFundAccount(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnUp();
	afx_msg void OnBnClickedBtnDown();
	afx_msg void OnBnClickedOk();
};
