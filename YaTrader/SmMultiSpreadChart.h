#pragma once


// SmMultiSpreadChart dialog

class SmMultiSpreadChart : public CDialogEx
{
	DECLARE_DYNAMIC(SmMultiSpreadChart)

public:
	SmMultiSpreadChart(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmMultiSpreadChart();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MULTI_SPREAD_CHART };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
