#pragma once
#include <afxcmn.h>
#include <afxdialogex.h>
#include <afxwin.h>
#include "IProgressDialog.h"
#include <BCGPDialog.h>
#include <BCGPProgressCtrl.h>
// DataProgressDialog dialog

class DataProgressDialog : public CBCGPDialog, public DarkHorse::IProgressDialog
{
	DECLARE_DYNAMIC(DataProgressDialog)

public:
	DataProgressDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DataProgressDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATA_PROGRESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CBCGPProgressCtrl _ProgressData;
	CBCGPStatic _StaticDetail;

	void SetTotalCount(const int& count) override;
	void SetRemainCount(const int& count) override;
	void SetTaskTitle(const std::string& title) override;
	void SetTaskDetail(const std::string& detail) override;
	void RefreshProgress() override;
	void ShowHide(const bool show) override;
	BOOL OnInitDialog() override;
	bool GetShowState() const override;

private:
	int _TotalCount = 0;
	int _RemainCount = 0;
	void SetPrgressPos(const int& pos);
};
