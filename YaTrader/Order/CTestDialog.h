#pragma once
#include <unordered_map>
#include <memory>
#include "../Order/SmCheckGrid.h"
#include "../Util/SmButton.h"
// CTestDialog dialog
class AbAccountOrderCenterWindow;
class AbAccountOrderLeftWindow;
class AbAccountOrderRightWindow;
class CTestDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(CTestDialog)

public:
	CTestDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTestDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	std::shared_ptr<AbAccountOrderLeftWindow> _LeftWnd = nullptr;
	std::shared_ptr<AbAccountOrderRightWindow> _RightWnd = nullptr;
	std::unordered_map<HWND, std::shared_ptr<AbAccountOrderCenterWindow>> _CenterWndMap;
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnEnterSizeMove(WPARAM, LPARAM);
	afx_msg LRESULT OnExitSizeMove(WPARAM, LPARAM);
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	SmButton _Button1;
	bool _ShowLeft = true;
	bool _ShowRight = true;

	SmCheckGrid m_wndGrid;
	
	int _LineGap = 2;
};
