// DarkHorseView.h : interface of the CDarkHorseView class
//


#pragma once
#include <memory>
#include <BCGCBProInc.h>
#define YEARS_NUM 5

inline double Rand(double dblStart, double dblFinish)
{
	double minVal = min(dblStart, dblFinish);
	double maxVal = max(dblStart, dblFinish);

	return (maxVal - minVal) * (double)rand() / (RAND_MAX + 1) + minVal;
}

namespace DarkHorse {
	class SmChartData;
}
class CChartSliderCtrl : public CBCGPSliderCtrl
{
public:
	CChartSliderCtrl()
	{
		m_bDrawFocus = FALSE;
		EnableZoomButtons();
	}
};
namespace DarkHorse {
	class SmChartData;
}
class CDarkHorseView : public CBCGPFormView
{
protected: // create from serialization only
	CDarkHorseView();
	DECLARE_DYNCREATE(CDarkHorseView)

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DARKHORSE_FORM };
#endif
	CBCGPChartCtrl	m_wndChart;
	int		m_nChartType;
	int		m_nLineType;
	int		m_nOverlayLine;
	int		m_nOverlayBand;

	CBCGPButton	m_wndStart;
	CBCGPButton	m_wndCancel;
	BOOL	m_bAutoScroll;
	BOOL	m_bDataTable;

	virtual CBCGPChartVisualObject* GetChart()
	{
		return m_wndChart.GetChart();
	}

	virtual CBCGPBaseVisualCtrl* GetCtrl()
	{
		return &m_wndChart;
	}

	void UpdateChartData();

// Attributes
public:
	CDarkHorseDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void UpdateChartColorTheme(int nTheme, BOOL bIsDarkTheme);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnChartAnimation();
	virtual void SetupLayout();
	virtual void GetTitle(CString& strTitle);
	
	virtual void OnChangeBackgroundDarkness() {}
// Implementation
public:
	virtual ~CDarkHorseView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	afx_msg void OnUpdateChart();
	afx_msg void OnUpdateChartType();
	afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);

protected:

// Generated message map functions
protected:
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	void CreateChart();
// 	void SetupIndicatorObjects(double dblValue, CBCGPChartAxis* pIndicatorAxis,
// 		const CString& strIndicatorInfo = _T(""));
	void LoadStockData(CBCGPChartStockSeries* pStockSeries, COleDateTime& dtStartRange, COleDateTime& dtEndRange);

	CBCGPChartSeries* m_pOverlaySeriesLine;
	CBCGPChartSeries* m_pOverlaySeriesBand;

	CString					m_strTitle;
	BOOL					m_bIsReady;
	//CString					m_arYears[YEARS_NUM];
	BOOL					m_bIsDarkBackground;
	BOOL					m_bIsTexturedTheme;
	BOOL					m_bIsFlatTheme;
	UINT					m_nThumbnailFlags;

	double				m_dblUpTrendLimit;
	CBCGPChartSeries* m_pRSISeries;
	CBCGPChartObject* m_pRSIInfoObject;
	CBCGPChartObject* m_pOHLCInfo;
	std::shared_ptr<DarkHorse::SmChartData> _ChartData = nullptr;
	void SetBarLineWidth();
public:
	afx_msg void OnBnClickedButton1();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnStart();
	afx_msg void OnCancel();
	//afx_msg void OnUpdateChart();
	afx_msg LRESULT OnChartAxisZoomed(WPARAM wp, LPARAM lp);
	afx_msg void OnClear();
	void SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data);

	//void SetBarLineWidth();
	CBCGPChartObject* SetupIndicatorObjects(double dblValue, CBCGPChartAxis* pIndicatorAxis,
		const CString& strIndicatorName = _T(""));
	void GenerateDataPoint(double& dblOpen, double& dblHigh, double& dblLow, double& dblClose, double& dblTime);

};

#ifndef _DEBUG  // debug version in DarkHorseView.cpp
inline CDarkHorseDoc* CDarkHorseView::GetDocument() const
	{ return reinterpret_cast<CDarkHorseDoc*>(m_pDocument); }

#endif

