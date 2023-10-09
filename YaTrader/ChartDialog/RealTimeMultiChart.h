#pragma once
#include "afxdialogex.h"
#include "../Chart/ChartViewer.h"
#include "../Chart/SmChartConsts.h"
#include "../TimeData/SmCompData.h"
#include "../Chart/SmExternalStorage.h"
#include "../DataFrame/DataFrame.h"

#include <memory>
#include <map>
#include <vector>

// RealTimeMultiChart dialog

namespace DarkHorse {
	class SmChartData;
	class SmSymbol;
	class SmTimeData;
	class SmCompData;
}

class RealTimeMultiChart : public CBCGPDialog
{
	DECLARE_DYNAMIC(RealTimeMultiChart)

public:
	RealTimeMultiChart(CWnd* pParent = nullptr);   // standard constructor
	virtual ~RealTimeMultiChart();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REALTIME_MULTICHART };
#endif

	double FirstChartRatio() const { return _FirstChartRatio; }
	void FirstChartRatio(double val) { _FirstChartRatio = val; }
	double SecondChartRatio() const { return _SecondChartRatio; }
	void SecondChartRatio(double val) { _SecondChartRatio = val; }
	void SetEnableUpdate(bool enableUpdate);
private:
	//CChartViewer m_ChartViewer;
	//CScrollBar m_HScrollBar;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CViewPortControl m_ViewPortControl;
	CChartViewer m_ChartViewer;
	CBCGPComboBox _ComboSymbolMain;
	//CBCGPScrollBar m_HScrollBar;

	bool _InitDlg = false;

	int _EntireChartWidth = 1800;
	int _EntireChartHeight = 800;

	int _FirstChartHeight = 300;
	int _SecondChartHeight = 245;
	int _ThirdChartHeight = 200;
	int _TitleHeight = 30;
	int _RightMargin = 0;
	int _LeftMargin = 0;
	int _ToolBarHeight = 25;
	int _ScrollBarHeight = 25;
	int _YAxisGap = 15;
	int _ViewPortHeight = 25;

	void RecalLayout();

	// The number of samples per data series used in this demo
	static const int sampleSize = 1500;

	// The initial full range is set to 60 seconds of data.
	static const int initialFullRange = 60;

	// The maximum zoom in is 10 seconds.
	static const int zoomInLimit = 10;

	double m_timeStamps[sampleSize];	// The timestamps for the data series
	double m_dataSeriesA[sampleSize];	// The values for the data series A
	double m_dataSeriesB[sampleSize];	// The values for the data series B
	double m_dataSeriesC[sampleSize];	// The values for the data series C

	// The index of the array position to which new data values are added.
	int m_currentIndex;

	// Used by the random number generator to generate real time data.
	double m_nextDataTime;


	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnViewPortChanged();
	afx_msg void OnMouseMovePlotArea();

	afx_msg LRESULT OnCompDataAdded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCompDataUpdated(WPARAM wParam, LPARAM lParam);

	// Draw chart
	XYChart* drawXYChart(int chartHeight, CChartViewer* viewer, const double* dataSeries, const char* name,
		int color, Axis* xAxisScale, bool hasXAxis);

	XYChart* drawKospiXYChart(int chartHeight, CChartViewer* viewer,  const char* name,
		int color, Axis* xAxisScale, bool hasXAxis);

	XYChart* drawHsXYChart(int chartHeight, CChartViewer* viewer, const char* name,
		int color, Axis* xAxisScale, bool hasXAxis);

	XYChart* drawAbroadXYChart(int chartHeight, CChartViewer* viewer,  const char* name,
		int color, Axis* xAxisScale, bool hasXAxis);

	XYChart* drawAbroadXYChartForHs(int chartHeight, CChartViewer* viewer, const char* name,
		int color, Axis* xAxisScale, bool hasXAxis);

	XYChart* drawDeltaXYChart(int chartHeight, CChartViewer* viewer,  const char* name,
		int color, Axis* xAxisScale, bool hasXAxis);
	void drawMultiChart(CChartViewer* viewer);
	void deleteMultiChart(MultiChart* m);

	// Draw track line
	void drawMultiTrackLine(MultiChart* m, int mouseX);
	void drawXYTrackLine(DrawArea* d, XYChart* c, int mouseX, bool hasXAxis);
	DarkHorse::SmCompData& GetChartData() {
		return _ChartData;
	}
private:
	bool _EnableUpdate = false;
	void drawFullChart(CViewPortControl* vpc);
	std::vector<int> _LineColor;
	void InitSymbolCombo();

	// key : row index, value : smsymbol object
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToMainSymbolMap;
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToCompSymbolMap;

	DarkHorse::SmCompData _ChartData;

	bool _NeedUpdate = false;
	bool _NeedAdd = false;
	bool _NeedRedraw = false;
	int _UniqueId = -1;

	bool _bDataReady = false;

	void LoadChartData();
	void RequestChartData();
	void ProcessAddData();
	void ProcessUpdateData();

	void DrawCurrentValue(XYChart* m, const double& close);
	void DrawCurrentValue(XYChart* m, Axis* axis, const double& close);
	void DrawCurrentValue(XYChart* m, Axis* axis, const int& color, const double& close);

	void DrawCurrentValue(XYChart* m, const int& y_pos, const int& color, const double& close);
	void DrawCurrentValue(XYChart* m, const int& y_pos, const int& color, const double& close, const double& start_val);

	void DrawCurrentValue(XYChart* m, Axis* axis, const int& color, const double& close, const double& start_val);

	void DrawTitleValue(XYChart* m);

	bool _ShowV1 = true;
	bool _ShowV2 = true;
	bool _ShowV3 = true;
	bool _ShowNk = true;
	bool _ShowHs = true;
	bool _ShowNq = true;

	bool _ShowD1 = true;
	bool _ShowD2 = true;
	bool _ShowD3 = true;

	

	double _FirstChartRatio = 0.5;
	double _SecondChartRatio = 0.3;
public:
	void RefreshChart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CBCGPButton _CheckNq;
	CBCGPButton _CheckHs;
	CBCGPButton _CheckV1;
	CBCGPButton _CheckNk;
	CBCGPButton _CheckV2;
	CBCGPButton _CheckV3;
	CBCGPEdit _EditHs;
	CBCGPEdit _EditNk;
	CBCGPEdit _EditNq;
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedCheckNk();
	afx_msg void OnBnClickedCheckHs();
	afx_msg void OnBnClickedCheckV1();
	afx_msg void OnBnClickedCheckV2();
	afx_msg void OnBnClickedCheckV3();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnSet();
	CBCGPButton _CheckD1;
	CBCGPButton _CheckD2;
	CBCGPButton _CheckD3;
	afx_msg void OnBnClickedCheckD1();
	afx_msg void OnBnClickedCheckD2();
	afx_msg void OnBnClickedCheckD3();
	afx_msg void OnBnClickedCheckNq();
	afx_msg void OnCbnSelchangeComboSymbol();
};
