// RealTimeMultiChart.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "afxdialogex.h"
#include "RealTimeMultiChart.h"
#include "chartdir.h"
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

#include "../MainFrm.h"


#include "../Chart/SmChartData.h"
#include "../Util/SmUtil.h"
#include "../resource.h"

#include "../Symbol/SmSymbolTableDialog.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Chart/SmChartDataManager.h"
#include "../Event/SmCallbackManager.h"
#include "../TimeData/SmTimeData.h"
#include "../TimeData/SmTimeDataManager.h"
#include "SmSetSpread.h"
#include <functional>
#include <format>
#include "SetSpreadDialog.h"
#include "SetSpreadHsDialog.h"

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

#define ID_NEXT_DATA_POINT	1
#define REDRAW_TIMER 1
#define REQUEST_TIMER 2

static const int DataRateTimerId = 1;
static const int ChartUpdateTimerId = 2;
static const int DataInterval = 250;

//
// The height of each XYChart. The bottom chart has an extra height for the x-axis labels.
//
//static const int chartHeight = 120;
static const int xAxisHeight = 20;

// RealTimeMultiChart dialog

IMPLEMENT_DYNAMIC(RealTimeMultiChart, CBCGPDialog)

RealTimeMultiChart::RealTimeMultiChart(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_REALTIME_MULTICHART, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	SetWhiteBackground(FALSE);

	// Initialize variables
	for (int i = 0; i < sampleSize; ++i)
		m_timeStamps[i] = m_dataSeriesA[i] = m_dataSeriesB[i] = m_dataSeriesC[i] = Chart::NoValue;
	m_nextDataTime = m_currentIndex = 0;

	_LineColor.push_back(0xc62127);
	_LineColor.push_back(0xf68e2f);
	_LineColor.push_back(0x18aa9d);
	_LineColor.push_back(0x800080);
	_LineColor.push_back(0xc62127);
	_LineColor.push_back(0xf68e2f);
	_LineColor.push_back(0x18aa9d);
	//_LineColor.push_back(0x4169E1);
	//_LineColor.push_back(0x8B4513);
	//_LineColor.push_back(0xFA8072);
	_LineColor.push_back(0xf68e2f);
	_LineColor.push_back(0x18aa9d);
	_LineColor.push_back(0x800080);
	_LineColor.push_back(0xF4A460);
	_LineColor.push_back(0x2E8B57);
	_LineColor.push_back(0xFFF5EE);
	_LineColor.push_back(0xA0522D);
	_LineColor.push_back(0x87CEEB);
}

RealTimeMultiChart::~RealTimeMultiChart()
{
	deleteMultiChart((MultiChart*)m_ChartViewer.getChart());
	delete m_ViewPortControl.getChart();
}

void RealTimeMultiChart::SetEnableUpdate(bool enableUpdate)
{
	if (enableUpdate) _ChartData.Ready(true);
	else _ChartData.Ready(false);
}

void RealTimeMultiChart::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ChartViewer, m_ChartViewer);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, _ComboSymbolMain);
	//DDX_Control(pDX, IDC_HScrollBar, m_HScrollBar);
	DDX_Control(pDX, IDC_ViewPortControl, m_ViewPortControl);
	DDX_Control(pDX, IDC_CHECK_NQ, _CheckNq);
	DDX_Control(pDX, IDC_CHECK_HS, _CheckHs);
	DDX_Control(pDX, IDC_CHECK_V1, _CheckV1);
	DDX_Control(pDX, IDC_CHECK_NK, _CheckNk);
	DDX_Control(pDX, IDC_CHECK_V2, _CheckV2);
	DDX_Control(pDX, IDC_CHECK_V3, _CheckV3);
	//DDX_Control(pDX, IDC_EDIT_HS, _EditHs);
	//DDX_Control(pDX, IDC_EDIT_NK, _EditNk);
	//DDX_Control(pDX, IDC_EDIT_NQ, _EditNq);
	DDX_Control(pDX, IDC_CHECK_D1, _CheckD1);
	DDX_Control(pDX, IDC_CHECK_D2, _CheckD2);
	DDX_Control(pDX, IDC_CHECK_D3, _CheckD3);
}


BEGIN_MESSAGE_MAP(RealTimeMultiChart, CBCGPDialog)
	ON_WM_SIZE()
	ON_CONTROL(CVN_ViewPortChanged, IDC_ChartViewer, OnViewPortChanged)
	ON_CONTROL(CVN_MouseMovePlotArea, IDC_ChartViewer, OnMouseMovePlotArea)
	ON_MESSAGE(UM_COMP_ADDED, &RealTimeMultiChart::OnCompDataAdded)
	ON_MESSAGE(UM_COMP_UPDATED, &RealTimeMultiChart::OnCompDataUpdated)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_NK, &RealTimeMultiChart::OnBnClickedCheckNk)
	ON_BN_CLICKED(IDC_CHECK_HS, &RealTimeMultiChart::OnBnClickedCheckHs)
	ON_BN_CLICKED(IDC_CHECK_V1, &RealTimeMultiChart::OnBnClickedCheckV1)
	ON_BN_CLICKED(IDC_CHECK_V2, &RealTimeMultiChart::OnBnClickedCheckV2)
	ON_BN_CLICKED(IDC_CHECK_V3, &RealTimeMultiChart::OnBnClickedCheckV3)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_SET, &RealTimeMultiChart::OnBnClickedBtnSet)
	ON_BN_CLICKED(IDC_CHECK_D1, &RealTimeMultiChart::OnBnClickedCheckD1)
	ON_BN_CLICKED(IDC_CHECK_D2, &RealTimeMultiChart::OnBnClickedCheckD2)
	ON_BN_CLICKED(IDC_CHECK_D3, &RealTimeMultiChart::OnBnClickedCheckD3)
	ON_BN_CLICKED(IDC_CHECK_NQ, &RealTimeMultiChart::OnBnClickedCheckNq)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &RealTimeMultiChart::OnCbnSelchangeComboSymbol)
END_MESSAGE_MAP()


// RealTimeMultiChart message handlers

LRESULT RealTimeMultiChart::OnCompDataAdded(WPARAM wParam, LPARAM lParam)
{
	//AfxMessageBox("comp data added!");

	_NeedAdd = true;
	return 1;
}

LRESULT RealTimeMultiChart::OnCompDataUpdated(WPARAM wParam, LPARAM lParam)
{
	_NeedUpdate = true;
	return 1;
}
//
// View port changed event
//
void RealTimeMultiChart::OnViewPortChanged()
{
	// In addition to updating the chart, we may also need to update other controls that
	// changes based on the view port.
	//updateControls(&m_ChartViewer);

	// Update the chart if necessary
	if (m_ChartViewer.needUpdateChart())
		drawMultiChart(&m_ChartViewer);

	// Update the full chart
	drawFullChart(&m_ViewPortControl);
}

//
// Draw track cursor when mouse is moving over plotarea
//
void RealTimeMultiChart::OnMouseMovePlotArea()
{
	drawMultiTrackLine((MultiChart*)m_ChartViewer.getChart(), m_ChartViewer.getPlotAreaMouseX());
	m_ChartViewer.updateDisplay();
}

void RealTimeMultiChart::RecalLayout()
{
	CRect rect;
	GetClientRect(rect);

	_EntireChartHeight = rect.Height();
	_EntireChartWidth = rect.Width();

	CRect chartRect;
	chartRect.left = _RightMargin;
	chartRect.top = _ToolBarHeight;
	chartRect.right = _EntireChartWidth;
	chartRect.bottom = rect.bottom - _ToolBarHeight - _ViewPortHeight;
	
	_EntireChartHeight = rect.Height() - _ToolBarHeight - _ViewPortHeight;

	//GetClientRect(rect);

	int temp_height = rect.Height();
	temp_height -= (_ToolBarHeight + _TitleHeight + xAxisHeight + _ViewPortHeight);
	int pure_height = temp_height;
	_FirstChartHeight = (int)(temp_height * _FirstChartRatio);
	_SecondChartHeight = (int)(temp_height * _SecondChartRatio);
	_ThirdChartHeight = pure_height - (_FirstChartHeight + _SecondChartHeight);

	m_ChartViewer.MoveWindow(chartRect);

	chartRect.left = _RightMargin;
	chartRect.top = rect.bottom - _ViewPortHeight;
	chartRect.right = _EntireChartWidth - _LeftMargin - _RightMargin;
	chartRect.bottom = rect.Height();
	m_ViewPortControl.MoveWindow(chartRect);

	m_ChartViewer.updateViewPort(true, false);
}

BOOL RealTimeMultiChart::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// Set m_nextDataTime to the current time. It is used by the real time random number 
	// generator so it knows what timestamp should be used for the next data point.
	SYSTEMTIME st;
	GetLocalTime(&st);
	m_nextDataTime = Chart::chartTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
		st.wSecond);


	m_ChartViewer.setMouseUsage(Chart::MouseUsageScroll);

	// Enable mouse wheel zooming by setting the zoom ratio to 1.1 per wheel event
	m_ChartViewer.setMouseWheelZoomRatio(1.1);

	// Bind the CChartViewer to the CViewPortControl
	m_ViewPortControl.setViewer(&m_ChartViewer);

	// Set up the data acquisition mechanism. In this demo, we just use a timer to get a 
	// sample every 250ms.
	//SetTimer(DataRateTimerId, DataInterval, 0);

	InitSymbolCombo();

	//m_ChartViewer.updateViewPort(true, false);


	SetTimer(REDRAW_TIMER, 10, NULL);
	SetTimer(REQUEST_TIMER, 700, NULL);

	_ChartData.Parent(this);

	if (_ShowNq) _CheckNq.SetCheck(BST_CHECKED);
	else _CheckNq.SetCheck(BST_UNCHECKED);

	if (_ShowHs) _CheckHs.SetCheck(BST_CHECKED);
	else _CheckHs.SetCheck(BST_UNCHECKED);

	if (_ShowNk) _CheckNk.SetCheck(BST_CHECKED);
	else _CheckNk.SetCheck(BST_UNCHECKED);

	if (_ShowV1) _CheckV1.SetCheck(BST_CHECKED);
	else _CheckV1.SetCheck(BST_UNCHECKED);

	if (_ShowV2) _CheckV2.SetCheck(BST_CHECKED);
	else _CheckV2.SetCheck(BST_UNCHECKED);

	if (_ShowV3) _CheckV3.SetCheck(BST_CHECKED);
	else _CheckV3.SetCheck(BST_UNCHECKED);

	if (_ShowD1) _CheckD1.SetCheck(BST_CHECKED);
	else _CheckD1.SetCheck(BST_UNCHECKED);

	if (_ShowD2) _CheckD2.SetCheck(BST_CHECKED);
	else _CheckD2.SetCheck(BST_UNCHECKED);

	if (_ShowD3) _CheckD3.SetCheck(BST_CHECKED);
	else _CheckD3.SetCheck(BST_UNCHECKED);



	_InitDlg = true;

	RecalLayout();
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void RealTimeMultiChart::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if (!_InitDlg) return;

	RecalLayout();
}

XYChart* RealTimeMultiChart::drawXYChart(int chartHeight, CChartViewer* viewer, const double* dataSeries, const char* name, int color, Axis* xAxisScale, bool hasXAxis)
{
	// Get the start date and end date that are visible on the chart.
	double viewPortStartDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft());
	double viewPortEndDate = viewer->getValueAtViewPort("x", viewer->getViewPortRight());


	int startIndex = (int)floor(viewer->getValueAtViewPort("x", viewer->getViewPortLeft()));
	int endIndex = (int)ceil(viewer->getValueAtViewPort("x", viewer->getViewPortLeft() + viewer->getViewPortWidth()));

	hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData.GetDataFrame();
	auto& dt = df.get_column<double>("cv_dt"); // Converted To Chart Director DateTime
	size_t duration = endIndex - startIndex + 1;
	if (startIndex + duration > df.shape().first - 1) {
		size_t delta = startIndex + duration - (df.shape().first - 1);
		duration -= delta;
		duration++;
	}

	// Extract the part of the data arrays that are visible.
	DoubleArray viewPortTimeStamps;
	DoubleArray viewPortDataSeries;

	viewPortTimeStamps = DoubleArray(dt.data() + startIndex, duration);
	viewPortDataSeries = DoubleArray(dataSeries + startIndex, duration);

	//
	// At this stage, we have extracted the visible data. We can use those data to plot the chart.
	//

	//================================================================================
	// Configure overall chart appearance.
	//================================================================================

	// Only the last chart has an x-axis
	int extraHeght = hasXAxis ? xAxisHeight : 0;

	// Create an XYChart object of size 640 x 150 pixels (or 180 pixels for the last chart)
	XYChart* c = new XYChart(_EntireChartWidth, chartHeight + extraHeght);

	// Set the plotarea at (55, 10) with width 80 pixels less than chart width, and height 20 pixels
	// less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
	// as background. Set border to transparent and grid lines to white (ffffff).
	c->setPlotArea(55, 10, c->getWidth() - 85, c->getHeight() - 20 - extraHeght, c->linearGradientColor(0, 10, 0,
		c->getHeight() - 20 - extraHeght, 0xf0f6ff, 0xa0c0ff), -1, Chart::Transparent, 0xffffff, 0xffffff);

	// As the data can lie outside the plotarea in a zoomed chart, we need enable clipping.
	c->setClipping();

	// Add a legend box at (55, 5) using horizontal layout. Use 8pts Arial Bold as font. Set the
	// background and border color to Transparent and use line style legend key.
	LegendBox* b = c->addLegend(55, 5, false, "Arial Bold", 10);
	b->setBackground(Chart::Transparent);
	b->setLineStyleKey();
	b->setFontColor(0xffffff);

	// Set the x and y axis stems to transparent and the label font to 10pt Arial
	c->xAxis()->setColors(Chart::Transparent);
	c->yAxis()->setColors(Chart::Transparent);
	c->xAxis()->setLabelStyle("Arial", 10);
	c->yAxis()->setLabelStyle("Arial", 10);

	// Add axis title using 10pts Arial Bold Italic font
	c->yAxis()->setTitle(name, "Arial Bold", 10);

	//================================================================================
	// Add data to chart
	//================================================================================

	// Add a line layer with the given data, with a line width of 2 pixels.
	LineLayer* layer = c->addLineLayer();
	layer->setLineWidth(2);
	layer->setXData(viewPortTimeStamps);
	layer->addDataSet(viewPortDataSeries, color, name);

	//================================================================================
	// Configure axis scale and labelling
	//================================================================================

	// For the automatic axis labels, set the minimum spacing to 30 pixels for the y axis.
	c->yAxis()->setTickDensity(30);

	if (0 != xAxisScale)
	{
		// If xAxisScale is given, then use it to synchronize with other charts.
		c->xAxis()->copyAxis(xAxisScale);
	}
	else if (m_currentIndex > 0)
	{
		// If xAxisScale is null, this is the first chart, and it needs to set up the axis scale.
		//c->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);

		// For the automatic axis labels, set the minimum spacing to 75 pixels for the x axis.
		c->xAxis()->setTickDensity(75);

		//
		// In this example, the axis range can change from a few seconds to thousands of seconds. 
		// We can need to define the axis label format for the various cases. 
		//

		// If all ticks are minute algined, then we use "hh:nn" as the label format.
		c->xAxis()->setFormatCondition("align", 60);
		c->xAxis()->setLabelFormat("{value|hh:nn}");

		// If all other cases, we use "hh:nn:ss" as the label format.
		c->xAxis()->setFormatCondition("else");
		c->xAxis()->setLabelFormat("{value|hh:nn:ss}");

		// We make sure the tick increment must be at least 1 second.
		c->xAxis()->setMinTickInc(1);
	}

	// Hide the x-axis if it is not visible.
	if (!hasXAxis)
		c->xAxis()->setColors(Chart::Transparent, Chart::Transparent);

	//================================================================================
	// Output the chart
	//================================================================================

	return c;
}

XYChart* RealTimeMultiChart::drawAbroadXYChart(int chartHeight, CChartViewer* viewer, const char* name, int color, Axis* xAxisScale, bool hasXAxis)
{
	// Get the start date and end date that are visible on the chart.
	double viewPortStartDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft());
	double viewPortEndDate = viewer->getValueAtViewPort("x", viewer->getViewPortRight());


	int startIndex = (int)floor(viewer->getValueAtViewPort("x", viewer->getViewPortLeft()));
	int endIndex = (int)ceil(viewer->getValueAtViewPort("x", viewer->getViewPortLeft() + viewer->getViewPortWidth()));

	hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData.GetDataFrame();
	auto& dt = df.get_column<double>("cv_dt"); // Converted To Chart Director DateTime
	size_t duration = endIndex - startIndex + 1;
	if (startIndex + duration > df.shape().first - 1) {
		size_t delta = startIndex + duration - (df.shape().first - 1);
		duration -= delta;
		duration++;
	}

	// Extract the part of the data arrays that are visible.
	DoubleArray viewPortTimeStamps;
	DoubleArray NqSeries;
	DoubleArray NkSeries;
	DoubleArray HsSeries;

	const std::vector<double>& nq = df.get_column<double>("nq"); 
	const std::vector<double>& nk = df.get_column<double>("nk");
	const std::vector<double>& hs = df.get_column<double>("hs");

	const std::vector<double>& nqi = df.get_column<double>("nq_index_value");
	const std::vector<double>& nki = df.get_column<double>("nk_index_value");
	const std::vector<double>& hsi = df.get_column<double>("hs_index_value");

	viewPortTimeStamps = DoubleArray(dt.data() + startIndex, duration);
	NqSeries = DoubleArray(nq.data() + startIndex, duration);
	NkSeries = DoubleArray(nk.data() + startIndex, duration);
	HsSeries = DoubleArray(hs.data() + startIndex, duration);

	//
	// At this stage, we have extracted the visible data. We can use those data to plot the chart.
	//

	//================================================================================
	// Configure overall chart appearance.
	//================================================================================

	// Only the last chart has an x-axis
	int extraHeght = hasXAxis ? xAxisHeight : 0;

	// Create an XYChart object of size 640 x 150 pixels (or 180 pixels for the last chart)
	XYChart* c = new XYChart(_EntireChartWidth, chartHeight + extraHeght);

	

	c->setBackground(0x363636);

	// Set the plotarea at (55, 10) with width 80 pixels less than chart width, and height 20 pixels
	// less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
	// as background. Set border to transparent and grid lines to white (ffffff).
	c->setPlotArea(55, 5, c->getWidth() - 85 - 100, c->getHeight() - 20 - extraHeght + 10, c->linearGradientColor(0, 10, 0,
		c->getHeight() - 20 - extraHeght, 0x393939, 0x393939), -1, Chart::Transparent, 0x6f6f6f, 0x6f6f6f);

	// As the data can lie outside the plotarea in a zoomed chart, we need enable clipping.
	c->setClipping();

	// Add a legend box at (55, 5) using horizontal layout. Use 8pts Arial Bold as font. Set the
	// background and border color to Transparent and use line style legend key.
	LegendBox* b = c->addLegend(55, 5, false, "Arial Bold", 14);
	b->setBackground(Chart::Transparent);
	b->setLineStyleKey();
	b->setFontColor(0x6f6f6f);

	// Set the x and y axis stems to transparent and the label font to 10pt Arial
	c->xAxis()->setColors(Chart::Transparent);
	c->yAxis()->setColors(Chart::Transparent);
	c->xAxis()->setLabelStyle("Arial", 10);
	c->yAxis()->setLabelStyle("Arial", 10);

	// Add axis title using 10pts Arial Bold Italic font
	c->yAxis()->setTitle(name, "Arial Bold", 10, 0x6f6f6f);

	c->yAxis()->setLabelStyle("Arial", 10, 0x6f6f6f);

	if (hasXAxis) c->xAxis()->setLabelStyle("Arial", 10, _ShowNq ? 0x6f6f6f : 0xff6f6f6f);



	// Add the third y-axis at 50 pixels to the left of the plot area
	Axis* leftAxis2 = c->addAxis(Chart::Left, 0);
	// Add a title on top of the third y axis.
	leftAxis2->setTitle("Nk")->setAlignment(Chart::TopLeft2);
	// Set the axis, label and title colors for the third y axis to blue (0000cc) to match the third
	// data set
	leftAxis2->setColors(0xff0000cc, 0xff0000cc, 0x0000cc);

	leftAxis2->setLabelStyle("Arial", 10, (!_ShowNq && !_ShowHs && _ShowNk) ? 0x6f6f6f : 0xff6f6f6f);


	// Add the fouth y-axis at 50 pixels to the right of the plot area
	Axis* leftAxis3 = c->addAxis(Chart::Left, 0);
	// Add a title on top of the fourth y axis.
	leftAxis3->setTitle("Hs")->setAlignment(Chart::TopLeft2);
	// Set the axis, label and title colors for the fourth y axis to purple (880088) to match the
	// fourth data set
	leftAxis3->setColors(0xcc880088, 0xcc880088, 0x880088);

	leftAxis3->setLabelStyle("Arial", 10, (!_ShowNq && !_ShowNk && _ShowHs) ? 0x6f6f6f : 0xff6f6f6f);



	//================================================================================
	// Add data to chart
	//================================================================================

	// Add a line layer with the given data, with a line width of 2 pixels.
	LineLayer* layer = nullptr;
	if (_ShowNq) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(NqSeries, _LineColor[4], TCHARtoUTF8(_T("나스닥100")));
	}

	if (_ShowNk) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(NkSeries, _LineColor[5], TCHARtoUTF8(_T("니케이225")));
		layer->setUseYAxis(leftAxis2);
	}

	if (_ShowHs) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(HsSeries, _LineColor[6], TCHARtoUTF8(_T("항생")));
		layer->setUseYAxis(leftAxis3);
	}

	//================================================================================
	// Configure axis scale and labelling
	//================================================================================

	// For the automatic axis labels, set the minimum spacing to 30 pixels for the y axis.
	c->yAxis()->setTickDensity(30);

	if (0 != xAxisScale)
	{
		// If xAxisScale is given, then use it to synchronize with other charts.
		c->xAxis()->copyAxis(xAxisScale);
	}
	else if (m_currentIndex > 0)
	{
		// If xAxisScale is null, this is the first chart, and it needs to set up the axis scale.
		//c->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);

		// For the automatic axis labels, set the minimum spacing to 75 pixels for the x axis.
		c->xAxis()->setTickDensity(75);

		//
		// In this example, the axis range can change from a few seconds to thousands of seconds. 
		// We can need to define the axis label format for the various cases. 
		//

		// If all ticks are minute algined, then we use "hh:nn" as the label format.
		c->xAxis()->setFormatCondition("align", 60);
		c->xAxis()->setLabelFormat("{value|hh:nn}");

		// If all other cases, we use "hh:nn:ss" as the label format.
		c->xAxis()->setFormatCondition("else");
		c->xAxis()->setLabelFormat("{value|hh:nn:ss}");

		// We make sure the tick increment must be at least 1 second.
		c->xAxis()->setMinTickInc(1);
	}

	// Hide the x-axis if it is not visible.
	if (!hasXAxis) {

		c->setNumberFormat(',');

		// Create custom labels for the x-axis
		std::vector<const char*> xLabels(viewPortTimeStamps.len);
		std::vector<std::string> stringBuffer(viewPortTimeStamps.len);
		const char* lastLabel = nullptr;
		// X축 레이블을 추출해서 적용해 준다.
		for (int i = 0; i < viewPortTimeStamps.len; ++i)
		{
			if (fmod(viewPortTimeStamps[i], 3600) < 60) {
				const char* curLabel = (stringBuffer[i] = c->formatValue(viewPortTimeStamps[i], "{value|hh:nn}")).c_str();
				if (lastLabel != nullptr && strcmp(curLabel, lastLabel) == 0) {
					lastLabel = curLabel;
					continue;
				}
				xLabels[i] = curLabel;
				lastLabel = curLabel;
			}
		}
		c->xAxis()->setLabels(StringArray(&(xLabels[0]), xLabels.size()));

		// We keep the original timestamps in a hidden axis for usage in the track cursor
		c->xAxis2()->setLabels(viewPortTimeStamps);
		//m->xAxis2()->setColors(Chart::Transparent, Chart::Transparent);
		c->xAxis2()->setColors(Chart::Transparent, 0xffff0000);

		c->xAxis()->setColors(Chart::Transparent, Chart::Transparent);
	}


	DrawCurrentValue(c, c->yAxis(), _LineColor[4], nq.back(), nqi.back());
	if (_ShowNk) DrawCurrentValue(c, leftAxis2,  _LineColor[5], nk.back(), nki.back());
	if (_ShowHs) DrawCurrentValue(c, leftAxis3,  _LineColor[6], hs.back(), hsi.back());

	DrawCurrentValue(c, 10, _LineColor[4], nq.back(), nqi.back());
	DrawCurrentValue(c, 30, _LineColor[5], nk.back(), nki.back());
	DrawCurrentValue(c, 50, _LineColor[6], hs.back(), hsi.back());

	//================================================================================
	// Output the chart
	//================================================================================

	return c;
}


XYChart* RealTimeMultiChart::drawAbroadXYChartForHs(int chartHeight, CChartViewer* viewer, const char* name, int color, Axis* xAxisScale, bool hasXAxis)
{
	// Get the start date and end date that are visible on the chart.
	double viewPortStartDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft());
	double viewPortEndDate = viewer->getValueAtViewPort("x", viewer->getViewPortRight());


	int startIndex = (int)floor(viewer->getValueAtViewPort("x", viewer->getViewPortLeft()));
	int endIndex = (int)ceil(viewer->getValueAtViewPort("x", viewer->getViewPortLeft() + viewer->getViewPortWidth()));

	hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData.GetDataFrame();
	auto& dt = df.get_column<double>("cv_dt"); // Converted To Chart Director DateTime
	size_t duration = endIndex - startIndex + 1;
	if (startIndex + duration > df.shape().first - 1) {
		size_t delta = startIndex + duration - (df.shape().first - 1);
		duration -= delta;
		duration++;
	}

	// Extract the part of the data arrays that are visible.
	DoubleArray viewPortTimeStamps;
	DoubleArray NqSeries;
	DoubleArray NkSeries;
	DoubleArray KpSeries;

	const std::vector<double>& nq = df.get_column<double>("nq");
	const std::vector<double>& nk = df.get_column<double>("nk");
	const std::vector<double>& kp = df.get_column<double>("kp");

	const std::vector<double>& nqi = df.get_column<double>("nq_index_value");
	const std::vector<double>& nki = df.get_column<double>("nk_index_value");
	const std::vector<double>& kpi = df.get_column<double>("kospi_index_value");

	viewPortTimeStamps = DoubleArray(dt.data() + startIndex, duration);
	NqSeries = DoubleArray(nq.data() + startIndex, duration);
	NkSeries = DoubleArray(nk.data() + startIndex, duration);
	KpSeries = DoubleArray(kp.data() + startIndex, duration);

	//
	// At this stage, we have extracted the visible data. We can use those data to plot the chart.
	//

	//================================================================================
	// Configure overall chart appearance.
	//================================================================================

	// Only the last chart has an x-axis
	int extraHeght = hasXAxis ? xAxisHeight : 0;

	// Create an XYChart object of size 640 x 150 pixels (or 180 pixels for the last chart)
	XYChart* c = new XYChart(_EntireChartWidth, chartHeight + extraHeght);



	c->setBackground(0x363636);

	// Set the plotarea at (55, 10) with width 80 pixels less than chart width, and height 20 pixels
	// less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
	// as background. Set border to transparent and grid lines to white (ffffff).
	c->setPlotArea(55, 5, c->getWidth() - 85 - 100, c->getHeight() - 20 - extraHeght + 10, c->linearGradientColor(0, 10, 0,
		c->getHeight() - 20 - extraHeght, 0x393939, 0x393939), -1, Chart::Transparent, 0x6f6f6f, 0x6f6f6f);

	// As the data can lie outside the plotarea in a zoomed chart, we need enable clipping.
	c->setClipping();

	// Add a legend box at (55, 5) using horizontal layout. Use 8pts Arial Bold as font. Set the
	// background and border color to Transparent and use line style legend key.
	LegendBox* b = c->addLegend(55, 5, false, "Arial Bold", 14);
	b->setBackground(Chart::Transparent);
	b->setLineStyleKey();
	b->setFontColor(0x6f6f6f);

	// Set the x and y axis stems to transparent and the label font to 10pt Arial
	c->xAxis()->setColors(Chart::Transparent);
	c->yAxis()->setColors(Chart::Transparent);
	c->xAxis()->setLabelStyle("Arial", 10);
	c->yAxis()->setLabelStyle("Arial", 10);

	// Add axis title using 10pts Arial Bold Italic font
	c->yAxis()->setTitle(name, "Arial Bold", 10, 0x6f6f6f);

	c->yAxis()->setLabelStyle("Arial", 10, 0x6f6f6f);

	if (hasXAxis) c->xAxis()->setLabelStyle("Arial", 10, _ShowNq ? 0x6f6f6f : 0xff6f6f6f);



	// Add the third y-axis at 50 pixels to the left of the plot area
	Axis* leftAxis2 = c->addAxis(Chart::Left, 0);
	// Add a title on top of the third y axis.
	leftAxis2->setTitle("Nk")->setAlignment(Chart::TopLeft2);
	// Set the axis, label and title colors for the third y axis to blue (0000cc) to match the third
	// data set
	leftAxis2->setColors(0xff0000cc, 0xff0000cc, 0x0000cc);

	leftAxis2->setLabelStyle("Arial", 10, (!_ShowNq && !_ShowHs && _ShowNk) ? 0x6f6f6f : 0xff6f6f6f);


	// Add the fouth y-axis at 50 pixels to the right of the plot area
	Axis* leftAxis3 = c->addAxis(Chart::Left, 0);
	// Add a title on top of the fourth y axis.
	leftAxis3->setTitle("Hs")->setAlignment(Chart::TopLeft2);
	// Set the axis, label and title colors for the fourth y axis to purple (880088) to match the
	// fourth data set
	leftAxis3->setColors(0xcc880088, 0xcc880088, 0x880088);

	leftAxis3->setLabelStyle("Arial", 10, (!_ShowNq && !_ShowNk && _ShowHs) ? 0x6f6f6f : 0xff6f6f6f);



	//================================================================================
	// Add data to chart
	//================================================================================

	// Add a line layer with the given data, with a line width of 2 pixels.
	LineLayer* layer = nullptr;
	if (_ShowNq) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(NqSeries, _LineColor[4], TCHARtoUTF8(_T("나스닥100")));
	}

	if (_ShowNk) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(NkSeries, _LineColor[5], TCHARtoUTF8(_T("니케이225")));
		layer->setUseYAxis(leftAxis2);
	}

	if (_ShowHs) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(KpSeries, _LineColor[6], TCHARtoUTF8(_T("코스피200")));
		layer->setUseYAxis(leftAxis3);
	}

	//================================================================================
	// Configure axis scale and labelling
	//================================================================================

	// For the automatic axis labels, set the minimum spacing to 30 pixels for the y axis.
	c->yAxis()->setTickDensity(30);

	if (0 != xAxisScale)
	{
		// If xAxisScale is given, then use it to synchronize with other charts.
		c->xAxis()->copyAxis(xAxisScale);
	}
	else if (m_currentIndex > 0)
	{
		// If xAxisScale is null, this is the first chart, and it needs to set up the axis scale.
		//c->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);

		// For the automatic axis labels, set the minimum spacing to 75 pixels for the x axis.
		c->xAxis()->setTickDensity(75);

		//
		// In this example, the axis range can change from a few seconds to thousands of seconds. 
		// We can need to define the axis label format for the various cases. 
		//

		// If all ticks are minute algined, then we use "hh:nn" as the label format.
		c->xAxis()->setFormatCondition("align", 60);
		c->xAxis()->setLabelFormat("{value|hh:nn}");

		// If all other cases, we use "hh:nn:ss" as the label format.
		c->xAxis()->setFormatCondition("else");
		c->xAxis()->setLabelFormat("{value|hh:nn:ss}");

		// We make sure the tick increment must be at least 1 second.
		c->xAxis()->setMinTickInc(1);
	}

	// Hide the x-axis if it is not visible.
	if (!hasXAxis) {

		c->setNumberFormat(',');

		// Create custom labels for the x-axis
		std::vector<const char*> xLabels(viewPortTimeStamps.len);
		std::vector<std::string> stringBuffer(viewPortTimeStamps.len);
		const char* lastLabel = nullptr;
		// X축 레이블을 추출해서 적용해 준다.
		for (int i = 0; i < viewPortTimeStamps.len; ++i)
		{
			if (fmod(viewPortTimeStamps[i], 3600) < 60) {
				const char* curLabel = (stringBuffer[i] = c->formatValue(viewPortTimeStamps[i], "{value|hh:nn}")).c_str();
				if (lastLabel != nullptr && strcmp(curLabel, lastLabel) == 0) {
					lastLabel = curLabel;
					continue;
				}
				xLabels[i] = curLabel;
				lastLabel = curLabel;
			}
		}
		c->xAxis()->setLabels(StringArray(&(xLabels[0]), xLabels.size()));

		// We keep the original timestamps in a hidden axis for usage in the track cursor
		c->xAxis2()->setLabels(viewPortTimeStamps);
		//m->xAxis2()->setColors(Chart::Transparent, Chart::Transparent);
		c->xAxis2()->setColors(Chart::Transparent, 0xffff0000);

		c->xAxis()->setColors(Chart::Transparent, Chart::Transparent);
	}


	DrawCurrentValue(c, c->yAxis(), _LineColor[4], nq.back(), nqi.back());
	if (_ShowNk) DrawCurrentValue(c, leftAxis2, _LineColor[5], nk.back(), nki.back());
	if (_ShowHs) DrawCurrentValue(c, leftAxis3, _LineColor[6], kp.back(), kpi.back());

	DrawCurrentValue(c, 10, _LineColor[4], nq.back(), nqi.back());
	DrawCurrentValue(c, 30, _LineColor[5], nk.back(), nki.back());
	DrawCurrentValue(c, 50, _LineColor[6], kp.back(), kpi.back());

	//================================================================================
	// Output the chart
	//================================================================================

	return c;
}

XYChart* RealTimeMultiChart::drawKospiXYChart(int chartHeight, CChartViewer* viewer, const char* name, int color, Axis* xAxisScale, bool hasXAxis)
{
	// Get the start date and end date that are visible on the chart.
	double viewPortStartDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft());
	double viewPortEndDate = viewer->getValueAtViewPort("x", viewer->getViewPortRight());


	int startIndex = (int)floor(viewer->getValueAtViewPort("x", viewer->getViewPortLeft()));
	int endIndex = (int)ceil(viewer->getValueAtViewPort("x", viewer->getViewPortLeft() + viewer->getViewPortWidth()));

	hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData.GetDataFrame();
	auto& dt = df.get_column<double>("cv_dt"); // Converted To Chart Director DateTime
	const std::vector<double>& kp = df.get_column<double>("kp"); // kospi
	const std::vector<double>& vir1 = df.get_column<double>("vir1");
	const std::vector<double>& vir2 = df.get_column<double>("vir2");
	const std::vector<double>& vir3 = df.get_column<double>("vir3");
	std::vector<double>& kiv = df.get_column<double>("kospi_index_value");


	size_t duration = endIndex - startIndex + 1;
	if (startIndex + duration > df.shape().first - 1) {
		size_t delta = startIndex + duration - (df.shape().first - 1);
		duration -= delta;
		duration++;
	}

	// Extract the part of the data arrays that are visible.
	DoubleArray viewPortTimeStamps;
	DoubleArray KospiSeries;
	DoubleArray Vir1DataSeries;
	DoubleArray Vir2DataSeries;
	DoubleArray Vir3DataSeries;


	viewPortTimeStamps = DoubleArray(dt.data() + startIndex, duration);
	KospiSeries = DoubleArray(kp.data() + startIndex, duration);

	Vir1DataSeries = DoubleArray(vir1.data() + startIndex, duration);
	Vir2DataSeries = DoubleArray(vir2.data() + startIndex, duration);
	Vir3DataSeries = DoubleArray(vir3.data() + startIndex, duration);


	//
	// At this stage, we have extracted the visible data. We can use those data to plot the chart.
	//

	//================================================================================
	// Configure overall chart appearance.
	//================================================================================

	// Only the last chart has an x-axis
	int extraHeght = hasXAxis ? xAxisHeight : 0;

	// Create an XYChart object of size 640 x 150 pixels (or 180 pixels for the last chart)
	XYChart* c = new XYChart(_EntireChartWidth, chartHeight + extraHeght);

	c->setBackground(0x363636);

	//c->setRoundedFrame();

	// Set the plotarea at (55, 10) with width 80 pixels less than chart width, and height 20 pixels
	// less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
	// as background. Set border to transparent and grid lines to white (ffffff).
	c->setPlotArea(55, 5, c->getWidth() - 85 - 100, c->getHeight() - 20 - extraHeght + 10, c->linearGradientColor(0, 10, 0,
		c->getHeight() - 20 - extraHeght, 0x393939, 0x393939), -1, Chart::Transparent, 0x6f6f6f, 0x6f6f6f);

	// As the data can lie outside the plotarea in a zoomed chart, we need enable clipping.
	c->setClipping();

	// Add a legend box at (55, 5) using horizontal layout. Use 8pts Arial Bold as font. Set the
	// background and border color to Transparent and use line style legend key.
	LegendBox* b = c->addLegend(55, 5, false, "Arial Bold", 14);
	b->setBackground(Chart::Transparent);
	b->setLineStyleKey();
	b->setFontColor(0x6f6f6f);

	// Set the x and y axis stems to transparent and the label font to 10pt Arial
	c->xAxis()->setColors(Chart::Transparent);
	c->yAxis()->setColors(Chart::Transparent);
	c->xAxis()->setLabelStyle("Arial", 10);
	c->yAxis()->setLabelStyle("Arial", 10);

	// Add axis title using 10pts Arial Bold Italic font
	c->yAxis()->setTitle(name, "Arial Bold", 10, 0x6f6f6f);

	c->yAxis()->setLabelStyle("Arial", 10, 0x6f6f6f);

	if (hasXAxis) c->xAxis()->setLabelStyle("Arial", 10, 0x6f6f6f);


	//================================================================================
	// Add data to chart
	//================================================================================

	// Add a line layer with the given data, with a line width of 2 pixels.
	LineLayer* layer = c->addLineLayer();
	layer->setLineWidth(2);
	//layer->setXData(viewPortTimeStamps);
	layer->addDataSet(KospiSeries, _LineColor[0], TCHARtoUTF8(_T("코스피200")));

	if (_ShowV1) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(Vir1DataSeries, _LineColor[1], TCHARtoUTF8(_T("예측코스피200A")));
	}

	if (_ShowV2) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(Vir2DataSeries, _LineColor[2], TCHARtoUTF8(_T("예측코스피200B")));
	}

	if (_ShowV3) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(Vir3DataSeries, _LineColor[3], TCHARtoUTF8(_T("예측코스피200C")));
	}

	//================================================================================
	// Configure axis scale and labelling
	//================================================================================

	// For the automatic axis labels, set the minimum spacing to 30 pixels for the y axis.
	c->yAxis()->setTickDensity(30);

	if (0 != xAxisScale)
	{
		// If xAxisScale is given, then use it to synchronize with other charts.
		c->xAxis()->copyAxis(xAxisScale);
	}
	else if (m_currentIndex > 0)
	{
		// If xAxisScale is null, this is the first chart, and it needs to set up the axis scale.
		//c->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);

		// For the automatic axis labels, set the minimum spacing to 75 pixels for the x axis.
		c->xAxis()->setTickDensity(75);

		//
		// In this example, the axis range can change from a few seconds to thousands of seconds. 
		// We can need to define the axis label format for the various cases. 
		//

		// If all ticks are minute algined, then we use "hh:nn" as the label format.
		c->xAxis()->setFormatCondition("align", 60);
		c->xAxis()->setLabelFormat("{value|hh:nn}");

		// If all other cases, we use "hh:nn:ss" as the label format.
		c->xAxis()->setFormatCondition("else");
		c->xAxis()->setLabelFormat("{value|hh:nn:ss}");

		// We make sure the tick increment must be at least 1 second.
		c->xAxis()->setMinTickInc(1);
	}

	// Hide the x-axis if it is not visible.
	if (!hasXAxis) {
		c->setNumberFormat(',');

		// Create custom labels for the x-axis
		std::vector<const char*> xLabels(viewPortTimeStamps.len);
		std::vector<std::string> stringBuffer(viewPortTimeStamps.len);
		const char* lastLabel = nullptr;
		// X축 레이블을 추출해서 적용해 준다.
		for (int i = 0; i < viewPortTimeStamps.len; ++i)
		{
			if (fmod(viewPortTimeStamps[i], 3600) < 60) {
				const char* curLabel = (stringBuffer[i] = c->formatValue(viewPortTimeStamps[i], "{value|hh:nn}")).c_str();
				if (lastLabel != nullptr && strcmp(curLabel, lastLabel) == 0) {
					lastLabel = curLabel;
					continue;
				}
				xLabels[i] = curLabel;
				lastLabel = curLabel;
			}
		}
		c->xAxis()->setLabels(StringArray(&(xLabels[0]), xLabels.size()));

		// We keep the original timestamps in a hidden axis for usage in the track cursor
		c->xAxis2()->setLabels(viewPortTimeStamps);
		//m->xAxis2()->setColors(Chart::Transparent, Chart::Transparent);
		c->xAxis2()->setColors(Chart::Transparent, 0xffff0000);

		c->xAxis()->setColors(Chart::Transparent, Chart::Transparent);
	}

	DrawCurrentValue(c, c->yAxis(), _LineColor[0], kp.back());
	if (_ShowV1) DrawCurrentValue(c, c->yAxis(), _LineColor[1], vir1.back(), kiv.back());
	if (_ShowV2) DrawCurrentValue(c, c->yAxis(), _LineColor[2], vir2.back(), kiv.back());
	if (_ShowV3) DrawCurrentValue(c, c->yAxis(), _LineColor[3], vir3.back(), kiv.back());

	DrawCurrentValue(c, 10, _LineColor[0], kp.back(), kiv.back());
	DrawCurrentValue(c, 30, _LineColor[1], vir1.back(), kiv.back());
	DrawCurrentValue(c, 50, _LineColor[2], vir2.back(), kiv.back());
	DrawCurrentValue(c, 70, _LineColor[3], vir3.back(), kiv.back());

	//DrawCurrentValue(c, c->yAxis(), _LineColor[0], kospi.back());

	//================================================================================
	// Output the chart
	//================================================================================

	return c;
}

XYChart* RealTimeMultiChart::drawHsXYChart(int chartHeight, CChartViewer* viewer, const char* name, int color, Axis* xAxisScale, bool hasXAxis)
{
	// Get the start date and end date that are visible on the chart.
	double viewPortStartDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft());
	double viewPortEndDate = viewer->getValueAtViewPort("x", viewer->getViewPortRight());


	int startIndex = (int)floor(viewer->getValueAtViewPort("x", viewer->getViewPortLeft()));
	int endIndex = (int)ceil(viewer->getValueAtViewPort("x", viewer->getViewPortLeft() + viewer->getViewPortWidth()));

	hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData.GetDataFrame();
	auto& dt = df.get_column<double>("cv_dt"); // Converted To Chart Director DateTime
	const std::vector<double>& hs = df.get_column<double>("hs"); // hs
	const std::vector<double>& vir1 = df.get_column<double>("vir1");
	const std::vector<double>& vir2 = df.get_column<double>("vir2");
	const std::vector<double>& vir3 = df.get_column<double>("vir3");
	std::vector<double>& hiv = df.get_column<double>("hs_index_value");


	size_t duration = endIndex - startIndex + 1;
	if (startIndex + duration > df.shape().first - 1) {
		size_t delta = startIndex + duration - (df.shape().first - 1);
		duration -= delta;
		duration++;
	}

	// Extract the part of the data arrays that are visible.
	DoubleArray viewPortTimeStamps;
	DoubleArray KospiSeries;
	DoubleArray Vir1DataSeries;
	DoubleArray Vir2DataSeries;
	DoubleArray Vir3DataSeries;


	viewPortTimeStamps = DoubleArray(dt.data() + startIndex, duration);
	KospiSeries = DoubleArray(hs.data() + startIndex, duration);

	Vir1DataSeries = DoubleArray(vir1.data() + startIndex, duration);
	Vir2DataSeries = DoubleArray(vir2.data() + startIndex, duration);
	Vir3DataSeries = DoubleArray(vir3.data() + startIndex, duration);


	//
	// At this stage, we have extracted the visible data. We can use those data to plot the chart.
	//

	//================================================================================
	// Configure overall chart appearance.
	//================================================================================

	// Only the last chart has an x-axis
	int extraHeght = hasXAxis ? xAxisHeight : 0;

	// Create an XYChart object of size 640 x 150 pixels (or 180 pixels for the last chart)
	XYChart* c = new XYChart(_EntireChartWidth, chartHeight + extraHeght);

	c->setBackground(0x363636);

	//c->setRoundedFrame();

	// Set the plotarea at (55, 10) with width 80 pixels less than chart width, and height 20 pixels
	// less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
	// as background. Set border to transparent and grid lines to white (ffffff).
	c->setPlotArea(55, 5, c->getWidth() - 85 - 100, c->getHeight() - 20 - extraHeght + 10, c->linearGradientColor(0, 10, 0,
		c->getHeight() - 20 - extraHeght, 0x393939, 0x393939), -1, Chart::Transparent, 0x6f6f6f, 0x6f6f6f);

	// As the data can lie outside the plotarea in a zoomed chart, we need enable clipping.
	c->setClipping();

	// Add a legend box at (55, 5) using horizontal layout. Use 8pts Arial Bold as font. Set the
	// background and border color to Transparent and use line style legend key.
	LegendBox* b = c->addLegend(55, 5, false, "Arial Bold", 14);
	b->setBackground(Chart::Transparent);
	b->setLineStyleKey();
	b->setFontColor(0x6f6f6f);

	// Set the x and y axis stems to transparent and the label font to 10pt Arial
	c->xAxis()->setColors(Chart::Transparent);
	c->yAxis()->setColors(Chart::Transparent);
	c->xAxis()->setLabelStyle("Arial", 10);
	c->yAxis()->setLabelStyle("Arial", 10);

	// Add axis title using 10pts Arial Bold Italic font
	c->yAxis()->setTitle(name, "Arial Bold", 10, 0x6f6f6f);

	c->yAxis()->setLabelStyle("Arial", 10, 0x6f6f6f);

	if (hasXAxis) c->xAxis()->setLabelStyle("Arial", 10, 0x6f6f6f);


	//================================================================================
	// Add data to chart
	//================================================================================

	// Add a line layer with the given data, with a line width of 2 pixels.
	LineLayer* layer = c->addLineLayer();
	layer->setLineWidth(2);
	//layer->setXData(viewPortTimeStamps);
	layer->addDataSet(KospiSeries, _LineColor[0], TCHARtoUTF8(_T("항생")));

	if (_ShowV1) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(Vir1DataSeries, _LineColor[1], TCHARtoUTF8(_T("예측항생A")));
	}

	if (_ShowV2) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(Vir2DataSeries, _LineColor[2], TCHARtoUTF8(_T("예측항생B")));
	}

	if (_ShowV3) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(Vir3DataSeries, _LineColor[3], TCHARtoUTF8(_T("예측항생C")));
	}

	//================================================================================
	// Configure axis scale and labelling
	//================================================================================

	// For the automatic axis labels, set the minimum spacing to 30 pixels for the y axis.
	c->yAxis()->setTickDensity(30);

	if (0 != xAxisScale)
	{
		// If xAxisScale is given, then use it to synchronize with other charts.
		c->xAxis()->copyAxis(xAxisScale);
	}
	else if (m_currentIndex > 0)
	{
		// If xAxisScale is null, this is the first chart, and it needs to set up the axis scale.
		//c->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);

		// For the automatic axis labels, set the minimum spacing to 75 pixels for the x axis.
		c->xAxis()->setTickDensity(75);

		//
		// In this example, the axis range can change from a few seconds to thousands of seconds. 
		// We can need to define the axis label format for the various cases. 
		//

		// If all ticks are minute algined, then we use "hh:nn" as the label format.
		c->xAxis()->setFormatCondition("align", 60);
		c->xAxis()->setLabelFormat("{value|hh:nn}");

		// If all other cases, we use "hh:nn:ss" as the label format.
		c->xAxis()->setFormatCondition("else");
		c->xAxis()->setLabelFormat("{value|hh:nn:ss}");

		// We make sure the tick increment must be at least 1 second.
		c->xAxis()->setMinTickInc(1);

		c->yAxis()->setLabelFormat("{value|2,.}");
	}

	// Hide the x-axis if it is not visible.
	if (!hasXAxis) {
		c->setNumberFormat(',');

		// Create custom labels for the x-axis
		std::vector<const char*> xLabels(viewPortTimeStamps.len);
		std::vector<std::string> stringBuffer(viewPortTimeStamps.len);
		const char* lastLabel = nullptr;
		// X축 레이블을 추출해서 적용해 준다.
		for (int i = 0; i < viewPortTimeStamps.len; ++i)
		{
			if (fmod(viewPortTimeStamps[i], 3600) < 60) {
				const char* curLabel = (stringBuffer[i] = c->formatValue(viewPortTimeStamps[i], "{value|hh:nn}")).c_str();
				if (lastLabel != nullptr && strcmp(curLabel, lastLabel) == 0) {
					lastLabel = curLabel;
					continue;
				}
				xLabels[i] = curLabel;
				lastLabel = curLabel;
			}
		}
		c->xAxis()->setLabels(StringArray(&(xLabels[0]), xLabels.size()));

		// We keep the original timestamps in a hidden axis for usage in the track cursor
		c->xAxis2()->setLabels(viewPortTimeStamps);
		//m->xAxis2()->setColors(Chart::Transparent, Chart::Transparent);
		c->xAxis2()->setColors(Chart::Transparent, 0xffff0000);

		c->xAxis()->setColors(Chart::Transparent, Chart::Transparent);
	}

	DrawCurrentValue(c, c->yAxis(), _LineColor[0], hs.back());
	if (_ShowV1) DrawCurrentValue(c, c->yAxis(), _LineColor[1], vir1.back(), hiv.back());
	if (_ShowV2) DrawCurrentValue(c, c->yAxis(), _LineColor[2], vir2.back(), hiv.back());
	if (_ShowV3) DrawCurrentValue(c, c->yAxis(), _LineColor[3], vir3.back(), hiv.back());

	DrawCurrentValue(c, 10, _LineColor[0], hs.back(), hiv.back());
	DrawCurrentValue(c, 30, _LineColor[1], vir1.back(), hiv.back());
	DrawCurrentValue(c, 50, _LineColor[2], vir2.back(), hiv.back());
	DrawCurrentValue(c, 70, _LineColor[3], vir3.back(), hiv.back());

	//DrawCurrentValue(c, c->yAxis(), _LineColor[0], kospi.back());

	//================================================================================
	// Output the chart
	//================================================================================

	return c;
}

XYChart* RealTimeMultiChart::drawDeltaXYChart(int chartHeight, CChartViewer* viewer, const char* name, int color, Axis* xAxisScale, bool hasXAxis)
{
	// Get the start date and end date that are visible on the chart.
	double viewPortStartDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft());
	double viewPortEndDate = viewer->getValueAtViewPort("x", viewer->getViewPortRight());


	int startIndex = (int)floor(viewer->getValueAtViewPort("x", viewer->getViewPortLeft()));
	int endIndex = (int)ceil(viewer->getValueAtViewPort("x", viewer->getViewPortLeft() + viewer->getViewPortWidth()));

	hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData.GetDataFrame();
	auto& dt = df.get_column<double>("cv_dt"); // Converted To Chart Director DateTime
	size_t duration = endIndex - startIndex + 1;
	if (startIndex + duration > df.shape().first - 1) {
		size_t delta = startIndex + duration - (df.shape().first - 1);
		duration -= delta;
		duration++;
	}

	// Extract the part of the data arrays that are visible.
	DoubleArray viewPortTimeStamps;
	DoubleArray delta1, delta2, delta3;

	const std::vector<double>& price_delta1 = _ChartData.GetDataFrame().get_column<double>("price_delta1");
	const std::vector<double>& price_delta2 = _ChartData.GetDataFrame().get_column<double>("price_delta2");
	const std::vector<double>& price_delta3 = _ChartData.GetDataFrame().get_column<double>("price_delta3");

	viewPortTimeStamps = DoubleArray(dt.data() + startIndex, duration);
	delta1 = DoubleArray(price_delta1.data() + startIndex, duration);
	delta2 = DoubleArray(price_delta2.data() + startIndex, duration);
	delta3 = DoubleArray(price_delta3.data() + startIndex, duration);


	//
	// At this stage, we have extracted the visible data. We can use those data to plot the chart.
	//

	//================================================================================
	// Configure overall chart appearance.
	//================================================================================

	// Only the last chart has an x-axis
	int extraHeght = hasXAxis ? xAxisHeight : 0;

	// Create an XYChart object of size 640 x 150 pixels (or 180 pixels for the last chart)
	XYChart* c = new XYChart(_EntireChartWidth, chartHeight + extraHeght);



	c->setBackground(0x363636);
	// Set the plotarea at (55, 10) with width 80 pixels less than chart width, and height 20 pixels
	// less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
	// as background. Set border to transparent and grid lines to white (ffffff).
	c->setPlotArea(55, 5, c->getWidth() - 85 - 100, c->getHeight() - 20 - extraHeght + 10, c->linearGradientColor(0, 10, 0,
		c->getHeight() - 20 - extraHeght, 0x393939, 0x393939), -1, Chart::Transparent, 0x6f6f6f, 0x6f6f6f);

	// As the data can lie outside the plotarea in a zoomed chart, we need enable clipping.
	c->setClipping();

	// Add a legend box at (55, 5) using horizontal layout. Use 8pts Arial Bold as font. Set the
	// background and border color to Transparent and use line style legend key.
	LegendBox* b = c->addLegend(55, 5, false, "Arial Bold", 14);
	b->setBackground(Chart::Transparent);
	b->setLineStyleKey();
	b->setFontColor(0x6f6f6f);

	// Set the x and y axis stems to transparent and the label font to 10pt Arial
	c->xAxis()->setColors(Chart::Transparent);
	c->yAxis()->setColors(Chart::Transparent);
	c->xAxis()->setLabelStyle("Arial", 10);
	

	// Add axis title using 10pts Arial Bold Italic font
	c->yAxis()->setTitle(name, "Arial Bold", 10, 0x6f6f6f);

	c->yAxis()->setLabelStyle("Arial", 10, 0x6f6f6f);

	if (hasXAxis) c->xAxis()->setLabelStyle("Arial", 10, 0x6f6f6f);


	



	//================================================================================
	// Add data to chart
	//================================================================================

	// Add a line layer with the given data, with a line width of 2 pixels.
	LineLayer* layer = nullptr; 
	if (_ShowD1) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(delta1, _LineColor[7], TCHARtoUTF8(_T("괴리도1")));
	}

	if (_ShowD2) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(delta2, _LineColor[8], TCHARtoUTF8(_T("괴리도2")));
	}

	if (_ShowD3) {
		layer = c->addLineLayer();
		layer->setLineWidth(2);
		//layer->setXData(viewPortTimeStamps);
		layer->addDataSet(delta3, _LineColor[9], TCHARtoUTF8(_T("괴리도3")));
	}


	//================================================================================
	// Configure axis scale and labelling
	//================================================================================

	// For the automatic axis labels, set the minimum spacing to 30 pixels for the y axis.
	c->yAxis()->setTickDensity(30);

	if (0 != xAxisScale)
	{
		// If xAxisScale is given, then use it to synchronize with other charts.
		c->xAxis()->copyAxis(xAxisScale);
	}
	else if (m_currentIndex > 0)
	{
		// If xAxisScale is null, this is the first chart, and it needs to set up the axis scale.
		//c->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);

		// For the automatic axis labels, set the minimum spacing to 75 pixels for the x axis.
		c->xAxis()->setTickDensity(75);

		//
		// In this example, the axis range can change from a few seconds to thousands of seconds. 
		// We can need to define the axis label format for the various cases. 
		//

		// If all ticks are minute algined, then we use "hh:nn" as the label format.
		c->xAxis()->setFormatCondition("align", 60);
		c->xAxis()->setLabelFormat("{value|hh:nn}");

		// If all other cases, we use "hh:nn:ss" as the label format.
		c->xAxis()->setFormatCondition("else");
		c->xAxis()->setLabelFormat("{value|hh:nn:ss}");

		// We make sure the tick increment must be at least 1 second.
		c->xAxis()->setMinTickInc(1);
	}

	// Hide the x-axis if it is not visible.
	if (!hasXAxis) {
		c->xAxis()->setColors(Chart::Transparent, Chart::Transparent);
	}
	else {
		// 천단위 표시를 해준다.
		c->setNumberFormat(',');

		// Create custom labels for the x-axis
		std::vector<const char*> xLabels(viewPortTimeStamps.len);
		std::vector<std::string> stringBuffer(viewPortTimeStamps.len);
		const char* lastLabel = nullptr;
		// X축 레이블을 추출해서 적용해 준다.
		for (int i = 0; i < viewPortTimeStamps.len; ++i)
		{
			if (fmod(viewPortTimeStamps[i], 3600) < 60) {
				const char* curLabel = (stringBuffer[i] = c->formatValue(viewPortTimeStamps[i], "{value|hh:nn}")).c_str();
				if (lastLabel != nullptr && strcmp(curLabel, lastLabel) == 0) {
					lastLabel = curLabel;
					continue;
				}
				xLabels[i] = curLabel;
				lastLabel = curLabel;
			}
		}
		c->xAxis()->setLabels(StringArray(&(xLabels[0]), xLabels.size()));

		// We keep the original timestamps in a hidden axis for usage in the track cursor
		c->xAxis2()->setLabels(viewPortTimeStamps);
		//m->xAxis2()->setColors(Chart::Transparent, Chart::Transparent);
		c->xAxis2()->setColors(Chart::Transparent, 0xffff0000);
	}

	if (_ShowD1) DrawCurrentValue(c, c->yAxis(), _LineColor[7], price_delta1.back());
	if (_ShowD2) DrawCurrentValue(c, c->yAxis(), _LineColor[8], price_delta2.back());
	if (_ShowD3) DrawCurrentValue(c, c->yAxis(), _LineColor[9], price_delta3.back());

	DrawCurrentValue(c, 10, _LineColor[7], price_delta1.back());
	DrawCurrentValue(c, 30, _LineColor[8], price_delta2.back());
	DrawCurrentValue(c, 50, _LineColor[9], price_delta3.back());

	//================================================================================
	// Output the chart
	//================================================================================

	return c;
}

void RealTimeMultiChart::drawMultiChart(CChartViewer* viewer)
{
	if (!_ChartData.Ready()) return;

	const std::vector<hmdf::DateTime>& index = _ChartData.GetDataFrame().get_index();
	const std::vector<double>& dt = _ChartData.GetDataFrame().get_column<double>("cv_dt"); // Converted To Chart Director DateTime
	const std::vector<double>& kp = _ChartData.GetDataFrame().get_column<double>("kp"); // kospi
	const std::vector<double>& nq = _ChartData.GetDataFrame().get_column<double>("nq"); // nq
	const std::vector<double>& nk = _ChartData.GetDataFrame().get_column<double>("nk");
	const std::vector<double>& hs = _ChartData.GetDataFrame().get_column<double>("hs");
	const std::vector<double>& vir1 = _ChartData.GetDataFrame().get_column<double>("vir1");
	const std::vector<double>& vir2 = _ChartData.GetDataFrame().get_column<double>("vir2");
	const std::vector<double>& vir3 = _ChartData.GetDataFrame().get_column<double>("vir3");
	const std::vector<double>& price_delta1 = _ChartData.GetDataFrame().get_column<double>("price_delta1");
	const std::vector<double>& price_delta2 = _ChartData.GetDataFrame().get_column<double>("price_delta2");
	const std::vector<double>& price_delta3 = _ChartData.GetDataFrame().get_column<double>("price_delta3");

	// The MultiChart contains 3 charts. The x-axis is only visible on the last chart, so we only
	// need to reserve space for 1 x-axis.
	//_EntireChartHeight = _TitleHeight + _FirstChartHeight + _SecondChartHeight + _ThirdChartHeight + xAxisHeight;
	MultiChart* m = new MultiChart(_EntireChartWidth, _EntireChartHeight);
	//m->addTitle(TCHARtoUTF8(_T("Index Spread Chart")), "Arial", 16, 0xffffff);
	m->setBackground(0x363636);
	const int chart_gap = 20;
	Axis* xAxisScale = nullptr;
	if (_ChartData.OpOption() == OP_KOSPI) {
		// This first chart is responsible for setting up the x-axis scale.
		m->addChart(0, chart_gap, drawKospiXYChart(_FirstChartHeight, viewer, TCHARtoUTF8(_T("코스피200")), 0xff0000, 0, false));
		xAxisScale = ((XYChart*)m->getChart(0))->xAxis();
	}
	else {
		// This first chart is responsible for setting up the x-axis scale.
		m->addChart(0, chart_gap, drawHsXYChart(_FirstChartHeight, viewer, TCHARtoUTF8(_T("항생")), 0xff0000, 0, false));
		xAxisScale = ((XYChart*)m->getChart(0))->xAxis();
	}

	if (_ChartData.OpOption() == OP_KOSPI) {
		// All other charts synchronize their x-axes with that of the first chart.
		m->addChart(0, chart_gap + _FirstChartHeight, drawAbroadXYChart(_SecondChartHeight, viewer, TCHARtoUTF8(_T("해외선물")), 0x00cc00,
			xAxisScale, false));
	}
	else {
		m->addChart(0, chart_gap + _FirstChartHeight, drawAbroadXYChartForHs(_SecondChartHeight, viewer, TCHARtoUTF8(_T("해외선물")), 0x00cc00,
			xAxisScale, false));
	}

	// The last chart displays the x-axis.
	m->addChart(0, chart_gap + _FirstChartHeight + _SecondChartHeight, drawDeltaXYChart(_ThirdChartHeight, viewer, TCHARtoUTF8(_T(" 괴리도")), 0x0000ff,
		xAxisScale, true));

	// We need to update the track line too. If the mouse is moving on the chart, the track line
	// will be updated in MouseMovePlotArea. Otherwise, we need to update the track line here.
	if (!viewer->isInMouseMoveEvent())
		drawMultiTrackLine(m, (0 == viewer->getChart()) ? m->getWidth() : viewer->getPlotAreaMouseX());

	// Set the combined plot area to be the bounding box of the plot areas of the 3 charts
	m->setMainChart(m);

	deleteMultiChart((MultiChart*)viewer->getChart());
	viewer->setChart(m);
}

void RealTimeMultiChart::deleteMultiChart(MultiChart* m)
{
	if (0 != m) {
		// Delete all the charts inside the MultiChart, then delete the MultiChart itself.
		for (int i = 0; i < m->getChartCount(); ++i)
			delete m->getChart(i);
		delete m;
	}
}

void RealTimeMultiChart::drawMultiTrackLine(MultiChart* m, int mouseX)
{
	// Obtain the dynamic layer of the MultiChart
	DrawArea* d = m->initDynamicLayer();

	// Ask each XYChart to draw the track cursor on the dynamic layer 
	for (int i = 0; i < m->getChartCount(); ++i)
		drawXYTrackLine(d, (XYChart*)m->getChart(i), mouseX, i == m->getChartCount() - 1);
}

void RealTimeMultiChart::drawXYTrackLine(DrawArea* d, XYChart* c, int mouseX, bool hasXAxis)
{
	// In a MultiChart, the XYChart is offsetted from the dynamic layer of the MultiChart
	int offsetY = c->getAbsOffsetY();

	// The plot area object
	PlotArea* plotArea = c->getPlotArea();

	// Get the data x-value that is nearest to the mouse, and find its pixel coordinate.
	double xValue = c->getNearestXValue(mouseX);
	int xCoor = c->getXCoor(xValue);
	if (xCoor < plotArea->getLeftX())
		return;

	// Draw a vertical track line at the x-position
	d->vline(plotArea->getTopY() + offsetY, plotArea->getBottomY() + offsetY, xCoor, 0x888888);

	int plotAreaLeftX = plotArea->getLeftX() + c->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + c->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();

	if (hasXAxis && _ThirdChartHeight > 0) {
		int zeroPos = c->getYCoor(0);

		zeroPos += 20 + _FirstChartHeight + _SecondChartHeight;

		d->line(plotAreaLeftX, zeroPos, plotAreaLeftX + width, zeroPos, 0xffffff, 1);


		zeroPos = c->getYCoor(3);

		zeroPos += 20 + _FirstChartHeight + _SecondChartHeight;

		d->line(plotAreaLeftX, zeroPos, plotAreaLeftX + width, zeroPos, 0x00ccff, 1);


		zeroPos = c->getYCoor(-3);

		zeroPos += 20 + _FirstChartHeight + _SecondChartHeight;

		d->line(plotAreaLeftX, zeroPos, plotAreaLeftX + width, zeroPos, 0xff99ff, 1);
	}

	int startIndex = (int)floor(m_ChartViewer.getValueAtViewPort("x", m_ChartViewer.getViewPortLeft()));
	int endIndex = (int)ceil(m_ChartViewer.getValueAtViewPort("x", m_ChartViewer.getViewPortLeft() + m_ChartViewer.getViewPortWidth()));

	hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData.GetDataFrame();
	auto& dt = df.get_column<double>("cv_dt"); // Converted To Chart Director DateTime
	size_t duration = endIndex - startIndex + 1;
	if (startIndex + duration > df.shape().first - 1) {
		size_t delta = startIndex + duration - (df.shape().first - 1);
		duration -= delta;
		duration++;
	}

	// Extract the part of the data arrays that are visible.
	DoubleArray timeStamps;

	timeStamps = DoubleArray(dt.data() + startIndex, duration);

	const std::set<double>& base_values = _ChartData.GetBaseValues();
	for (auto it = base_values.begin(); it != base_values.end(); it++) {
		double xIndex = floor(Chart::bSearch(timeStamps, *it));
		int xIndexCoor = c->getXCoor(xIndex);
		//if (xIndexCoor < plotArea->getLeftX()) continue;

		//d->vline(plotArea->getTopY() + offsetY, plotArea->getBottomY() + offsetY, xIndexCoor, 0xffff00);

		d->vline(plotAreaTopY, plotAreaTopY + plotArea->getHeight(), c->getXCoor(xIndex) +
			c->getAbsOffsetX(), d->dashLineColor(0xffffff, 0x0101));

		if (hasXAxis) {
			std::ostringstream xlabel;
			xlabel << "<*font,bgColor=000000*> " << c->xAxis2()->getFormattedLabel(xIndex, "hh:nn")
				<< " <*/font*>";
			TTFText* t = d->text(xlabel.str().c_str(), "Arial Bold", 10);

			// Restrict the x-pixel position of the label to make sure it stays inside the chart image.
			int xLabelPos = (std::max)(0, (std::min)(xIndexCoor - t->getWidth() / 2, c->getWidth() - t->getWidth()));
			t->draw(xLabelPos, plotArea->getBottomY() + 6 + offsetY, 0xffffff);
			t->destroy();
		}
	}

	// Draw a label on the x-axis to show the track line position.
	if (hasXAxis) {
		xCoor = c->getXCoor(xValue);
	
		// Draw a label on the x-axis to show the track line position.
		ostringstream xlabel2;

		xlabel2 << "<*block,valign=top,maxWidth=" << (plotArea->getWidth() - 5)
			<< "*><*font=arialbd.ttf,bgColor=0d730d*>" << c->xAxis2()->getFormattedLabel(xValue, "yyyy-mm-dd hh:nn:ss")
			<< "<*/font*>";

		TTFText* t2 = d->text(xlabel2.str().c_str(), "arialbd.ttf", 10);

		// Restrict the x-pixel position of the label to make sure it stays inside the chart image.
		int xLabelPos = max(0, min(xCoor - t2->getWidth() / 2, c->getWidth() - t2->getWidth()));
		if (hasXAxis) t2->draw(xLabelPos, plotArea->getBottomY() + 6 + offsetY, 0xffffff);
		t2->destroy();
	}


	


	// Iterate through all layers to draw the data labels
	for (int i = 0; i < c->getLayerCount(); ++i) {
		Layer* layer = c->getLayerByZ(i);

		// The data array index of the x-value
		int xIndex = layer->getXIndexOf(xValue);

		// Iterate through all the data sets in the layer
		for (int j = 0; j < layer->getDataSetCount(); ++j) {
			DataSet* dataSet = layer->getDataSetByZ(j);
			const char* dataSetName = dataSet->getDataName();

			// Get the color and position of the data label
			int color = dataSet->getDataColor();
			int yCoor = c->getYCoor(dataSet->getPosition(xIndex), dataSet->getUseYAxis());

			// Draw a track dot with a label next to it for visible data points in the plot area
			if ((yCoor >= plotArea->getTopY()) && (yCoor <= plotArea->getBottomY()) && (color !=
				Chart::Transparent) && dataSetName && *dataSetName) {
				d->circle(xCoor, yCoor + offsetY, 4, 4, color, color);

				std::ostringstream label;
				label << "<*font,bgColor=" << std::hex << color << "*> "
					<< c->formatValue(dataSet->getValue(xIndex), "{value|P4}") << " <*font*>";
				TTFText* t = d->text(label.str().c_str(), "Arial Bold", 10);

				// Draw the label on the right side of the dot if the mouse is on the left side the
				// chart, and vice versa. This ensures the label will not go outside the chart image.
				if (xCoor <= (plotArea->getLeftX() + plotArea->getRightX()) / 2)
					t->draw(xCoor + 6, yCoor + offsetY, 0xffffff, Chart::Left);
				else
					t->draw(xCoor - 6, yCoor + offsetY, 0xffffff, Chart::Right);

				t->destroy();
			}
		}
	}
}


void RealTimeMultiChart::drawFullChart(CViewPortControl* vpc)
{
	if (!_ChartData.Ready()) return;

	hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData.GetDataFrame();
	auto& dt = df.get_column<double>("cv_dt"); // Converted To Chart Director DateTime
	const std::vector<double>& kp = df.get_column<double>("kp"); // kospi
	const std::vector<double>& hs = df.get_column<double>("hs"); // kospi

	DoubleArray timeStamps = DoubleArray(dt.data(), dt.size());
	

	// Create an XYChart object of size 640 x 60 pixels   
	XYChart* c = new XYChart(_EntireChartWidth - _LeftMargin - _RightMargin, _ViewPortHeight);

	c->setBackground(0x363636);
	// 55, 10, c->getWidth() - 85
	// Set the plotarea with the same horizontal position as that in the main chart for alignment.
	c->setPlotArea(55, 0, c->getWidth() - 85 - 100, c->getHeight(), 0x393939, 0x444444, 0x393939,
		Chart::Transparent, 0x6f6f6f);


	// Set the x axis stem to transparent and the label font to 10pt Arial
	c->xAxis()->setColors(Chart::Transparent);
	c->xAxis()->setLabelStyle("Arial", 10, 0xffffff);

	// Put the x-axis labels inside the plot area by setting a negative label gap. Use
	// setLabelAlignment to put the label at the right side of the tick.
	c->xAxis()->setLabelGap(-1);
	c->xAxis()->setLabelAlignment(1);

	// Set the y axis stem and labels to transparent (that is, hide the labels)
	c->yAxis()->setColors(Chart::Transparent, Chart::Transparent);

	// Add a line layer for the lines with fast line mode enabled
	//CandleStickLayer* layer = c->addCandleStickLayer(highData, lowData, openData, closeData, 0xff0000, 0x0000ff);
	LineLayer* layer = nullptr;
	if (_ChartData.OpOption() == OP_KOSPI) {
		DoubleArray closeData = DoubleArray(kp.data(), kp.size());
		layer = c->addLineLayer(closeData, 0xff0000);
	}
	else {
		DoubleArray closeData = DoubleArray(hs.data(), hs.size());
		layer = c->addLineLayer(closeData, 0xff0000);
	}
	layer->setLineWidth(3);
	layer->setFastLineMode();

	//c->addSplineLayer(ArrayMath(closeData).lowess(0.02, 2), 0xff0000)->setLineWidth(3);

	// Now we add the 3 data series to a line layer, using the color red (0xff3333), green
	// (0x008800) and blue (0x3333cc)
	//layer->setXData(DoubleArray(m_timeStamps, sampleSize));


	// The x axis scales should reflect the full range of the view port
	// Create custom labels for the x-axis
	std::vector<const char*> xLabels(timeStamps.len);
	std::vector<std::string> stringBuffer(timeStamps.len);
	const char* lastLabel = nullptr;
	// X축 레이블을 추출해서 적용해 준다.
	for (int i = 0; i < timeStamps.len; ++i)
	{
		if (fmod(timeStamps[i], 3600) < 60) {
			const char* curLabel = (stringBuffer[i] = c->formatValue(timeStamps[i], "{value|hh:nn}")).c_str();
			if (lastLabel != nullptr && strcmp(curLabel, lastLabel) == 0) {
				lastLabel = curLabel;
				continue;
			}
			xLabels[i] = curLabel;
			lastLabel = curLabel;
		}
	}
	c->xAxis()->setLabels(StringArray(&(xLabels[0]), xLabels.size()));

	// For the automatic x-axis labels, set the minimum spacing to 75 pixels.
	//c->xAxis()->setTickDensity(75);

	// For the auto-scaled y-axis, as we hide the labels, we can disable axis rounding. This can
	// make the axis scale fit the data tighter.
	c->yAxis()->setRounding(false, false);

	// Output the chart
	delete vpc->getChart();
	vpc->setChart(c);
}

void RealTimeMultiChart::InitSymbolCombo()
{
	// 반드시 실시간 등록을 해줄것
	const std::map<int, std::shared_ptr<DarkHorse::SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	for (auto it = favorite_map.begin(); it != favorite_map.end(); ++it) {
		if (it->second->ProductCode() == "HSI" || it->second->ProductCode() == "101") {
			int index = _ComboSymbolMain.AddString(it->second->SymbolNameKr().c_str());
			_RowToMainSymbolMap[index] = it->second;
		}
	}

	if (_RowToMainSymbolMap.size() >= 2) {
		_ComboSymbolMain.SetCurSel(1);
	}

	_ChartData.SetNqSymbol(mainApp.SymMgr()->GetRecentSymbolCode("NQ"));
	_ChartData.SetKospiSymbol(mainApp.SymMgr()->GetRecentSymbolCode("101"));
	_ChartData.NikkeiCode(mainApp.SymMgr()->GetRecentSymbolCode("NKD"));
	_ChartData.HangSengCode(mainApp.SymMgr()->GetRecentSymbolCode("HSI"));
	mainApp.SymMgr()->RegisterSymbolToServer(mainApp.SymMgr()->GetRecentSymbolCode("NQ"), true);
	mainApp.SymMgr()->RegisterSymbolToServer(mainApp.SymMgr()->GetRecentSymbolCode("101"), true);
	mainApp.SymMgr()->RegisterSymbolToServer(mainApp.SymMgr()->GetRecentSymbolCode("NKD"), true);
	mainApp.SymMgr()->RegisterSymbolToServer(mainApp.SymMgr()->GetRecentSymbolCode("HSI"), true);

	_ComboSymbolMain.SetDroppedWidth(300);
}

void RealTimeMultiChart::LoadChartData()
{
	const std::vector<hmdf::DateTime>& index = _ChartData.GetDataFrame().get_index();
	if (index.size() == 0) return;

	const std::vector<double>& dt = _ChartData.GetDataFrame().get_column<double>("cv_dt"); // Converted To Chart Director DateTime
	const std::vector<double>& kospi = _ChartData.GetDataFrame().get_column<double>("kospi"); // kospi
	const std::vector<double>& nq = _ChartData.GetDataFrame().get_column<double>("nq"); // nq
	const std::vector<double>& nk = _ChartData.GetDataFrame().get_column<double>("nk");
	const std::vector<double>& hs = _ChartData.GetDataFrame().get_column<double>("hs");
	const std::vector<double>& vir1 = _ChartData.GetDataFrame().get_column<double>("vir1");
	const std::vector<double>& vir2 = _ChartData.GetDataFrame().get_column<double>("vir2");
	const std::vector<double>& vir3 = _ChartData.GetDataFrame().get_column<double>("vir3");
	const std::vector<double>& price_delta = _ChartData.GetDataFrame().get_column<double>("price_delta");
	const std::vector<double>& price_delta2 = _ChartData.GetDataFrame().get_column<double>("price_delta2");

	for (int i = 0; i < sampleSize; ++i)
		m_timeStamps[i] = dt[i];
	
	CString strMain;
	double main_divedend = 1, comp_divedend = 1;
	for (size_t i = 0; i < index.size(); i++) {
		auto symbol = mainApp.SymMgr()->FindSymbol(_ChartData.KospiSymbolCode());
		if (symbol) {
			main_divedend = pow(10, symbol->decimal());
			//m_arSeries[0]->AddDataPoint(main[i] / main_divedend, index[i]);
			//m_arSeries[0]->m_strSeriesName = symbol->SymbolNameKr().c_str();
		}
		symbol = mainApp.SymMgr()->FindSymbol(_ChartData.NqSymbolCode());
		if (symbol) {
			comp_divedend = pow(10, symbol->decimal());
			//m_arSeries[1]->m_strSeriesName = symbol->SymbolNameKr().c_str();
			//m_arSeries[1]->AddDataPoint(comp[i] / comp_divedend, index[i]);
			//m_arSeries[2]->AddDataPoint(vir1[i] / comp_divedend, index[i]);
			//m_arSeries[3]->AddDataPoint(vir2[i] / comp_divedend, index[i]);
			//m_arSeries[4]->AddDataPoint(vir3[i] / comp_divedend, index[i]);
			//m_arSeries[5]->AddDataPoint(price_delta[i] / comp_divedend, index[i]);
			//m_arSeries[6]->AddDataPoint(price_delta2[i] / comp_divedend, index[i]);

			// 시가 표시
			/*
			if (index[i].hour() == 9 && index[i].minute() == 1) {
				strMain.Format("O:%.2f", comp[i] / comp_divedend);
				
			}
			// 종가 표시
			if (index[i].hour() == 15 && index[i].minute() == 45) {
				strMain.Format("C:%.2f", comp[i] / comp_divedend);
				
			}
			*/
		}

	}
}

void RealTimeMultiChart::RequestChartData()
{
	if (!_ChartData.RequestChartData(_UniqueId)) {
		KillTimer(REQUEST_TIMER);
		_ChartData.MakeChartData();
		//LoadChartData();
		m_currentIndex = _ChartData.GetDataCount() - 1;

		CChartViewer* viewer = &m_ChartViewer;
		int DataSize = _ChartData.GetDataCount();
		// Enables auto scroll if the viewport is showing the latest data before the update
		bool autoScroll = (0.001 + viewer->getValueAtViewPort("x",
			viewer->getViewPortLeft() + viewer->getViewPortWidth()) >= DataSize - 1);

		// Update the new full data range to include the latest data
		bool axisScaleHasChanged = viewer->updateFullRangeH("x", 0,
			(std::max)(initialFullRange, DataSize), Chart::KeepVisibleRange);

		if (autoScroll) {
			const hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData.GetDataFrame();
			auto& cv_dt = df.get_column<double>("cv_dt"); // Converted To Chart Director DateTime
			// Scroll the viewport if necessary to display the latest data
			double viewPortEndPos = viewer->getViewPortAtValue("x", cv_dt[DataSize - 1]);
			if (viewPortEndPos > viewer->getViewPortLeft() + viewer->getViewPortWidth())
			{
				viewer->setViewPortLeft(viewPortEndPos - viewer->getViewPortWidth());
				axisScaleHasChanged = true;
			}
		}

		// Set the zoom in limit as a ratio to the full range
		viewer->setZoomInWidthLimit(initialFullRange / (viewer->getValueAtViewPort("x", 1) -
			viewer->getValueAtViewPort("x", 0)));

		// Trigger the viewPortChanged event. Updates the chart if the axis scale has changed
		// (scrolling or zooming) or if new data are added to the existing axis scale.
		viewer->updateViewPort(axisScaleHasChanged || (DataSize < initialFullRange), false);


		m_ChartViewer.updateViewPort(true, false);

		_bDataReady = true;
	}
}

void RealTimeMultiChart::ProcessAddData()
{
	if (!_ChartData.Ready()) return;

	m_currentIndex = _ChartData.GetDataCount() - 1;

	CChartViewer* viewer = &m_ChartViewer;
	int DataSize = _ChartData.GetDataCount();
	// Enables auto scroll if the viewport is showing the latest data before the update
	bool autoScroll = (0.001 + viewer->getValueAtViewPort("x",
		viewer->getViewPortLeft() + viewer->getViewPortWidth()) >= DataSize - 1);

	// Update the new full data range to include the latest data
	bool axisScaleHasChanged = viewer->updateFullRangeH("x", 0,
		(std::max)(initialFullRange, DataSize), Chart::KeepVisibleRange);

	if (autoScroll) {
		const hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData.GetDataFrame();
		auto& cv_dt = df.get_column<double>("cv_dt"); // Converted To Chart Director DateTime
		// Scroll the viewport if necessary to display the latest data
		double viewPortEndPos = viewer->getViewPortAtValue("x", cv_dt[DataSize - 1]);
		if (viewPortEndPos > viewer->getViewPortLeft() + viewer->getViewPortWidth())
		{
			viewer->setViewPortLeft(viewPortEndPos - viewer->getViewPortWidth());
			axisScaleHasChanged = true;
		}
	}

	// Set the zoom in limit as a ratio to the full range
	viewer->setZoomInWidthLimit(initialFullRange / (viewer->getValueAtViewPort("x", 1) -
		viewer->getValueAtViewPort("x", 0)));

	// Trigger the viewPortChanged event. Updates the chart if the axis scale has changed
	// (scrolling or zooming) or if new data are added to the existing axis scale.
	viewer->updateViewPort(axisScaleHasChanged || (DataSize < initialFullRange), false);
	m_ChartViewer.updateViewPort(true, false);
}

void RealTimeMultiChart::ProcessUpdateData()
{
	if (!_ChartData.Ready()) return;

	m_ChartViewer.updateViewPort(true, false);
}

void RealTimeMultiChart::DrawCurrentValue(XYChart* chart, const double& close)
{
	DrawArea* drawArea = chart->makeChart();

	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + chart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();

	//DarkHorse::SmStockData last_data;
	//_ChartData->GetLastData(last_data);
	int valuePos = chart->getYCoor(close);
	int rectTop = valuePos - 8;
	int rectLeft = plotAreaLeftX + width + _YAxisGap;
	int rectBottom = rectTop + 16;

	int xarr[3], yarr[3];
	xarr[0] = plotAreaLeftX + width + _YAxisGap - 14;
	xarr[1] = plotAreaLeftX + width + _YAxisGap;
	xarr[2] = plotAreaLeftX + width + _YAxisGap;

	yarr[0] = valuePos;
	yarr[1] = rectTop;
	yarr[2] = rectBottom;

	drawArea->polygon(IntArray(xarr, 3), IntArray(yarr, 3), 0xff0000, 0xff0000);
	drawArea->rect(rectLeft, rectTop, rectLeft + 60, rectBottom, 0xff0000, 0xff0000);

	std::string value = std::format(_T("{0:.2f}"), close);
	DarkHorse::SmUtil::to_thou_sep(value);
	TTFText* t = drawArea->text(value.c_str(), _T("굴림"), 9);
	t->draw(plotAreaLeftX + width + _YAxisGap, valuePos - 6, 0xffffff);
	t->destroy();
}

void RealTimeMultiChart::DrawCurrentValue(XYChart* chart, Axis* axis, const double& close)
{
	DrawArea* drawArea = chart->makeChart();

	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + chart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();

	//DarkHorse::SmStockData last_data;
	//_ChartData->GetLastData(last_data);
	int valuePos = chart->getYCoor(close, axis);
	int rectTop = valuePos - 8;
	int rectLeft = plotAreaLeftX + width + _YAxisGap;
	int rectBottom = rectTop + 16;

	int xarr[3], yarr[3];
	xarr[0] = plotAreaLeftX + width + _YAxisGap - 14;
	xarr[1] = plotAreaLeftX + width + _YAxisGap;
	xarr[2] = plotAreaLeftX + width + _YAxisGap;

	yarr[0] = valuePos;
	yarr[1] = rectTop;
	yarr[2] = rectBottom;

	drawArea->polygon(IntArray(xarr, 3), IntArray(yarr, 3), 0xff0000, 0xff0000);
	drawArea->rect(rectLeft, rectTop, rectLeft + 60, rectBottom, 0xff0000, 0xff0000);

	std::string value = std::format(_T("{0:.2f}"), close);
	DarkHorse::SmUtil::to_thou_sep(value);
	TTFText* t = drawArea->text(value.c_str(), _T("굴림"), 9);
	t->draw(plotAreaLeftX + width + _YAxisGap, valuePos - 6, 0xffffff);
	t->destroy();
}

void RealTimeMultiChart::DrawCurrentValue(XYChart* chart, Axis* axis, const int& color, const double& close)
{
	DrawArea* drawArea = chart->makeChart();

	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + chart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();

	//DarkHorse::SmStockData last_data;
	//_ChartData->GetLastData(last_data);
	int valuePos = chart->getYCoor(close, axis);
	int rectTop = valuePos - 8;
	int rectLeft = plotAreaLeftX + width + _YAxisGap;
	int rectBottom = rectTop + 16;

	int xarr[3], yarr[3];
	xarr[0] = plotAreaLeftX + width + _YAxisGap - 14;
	xarr[1] = plotAreaLeftX + width + _YAxisGap;
	xarr[2] = plotAreaLeftX + width + _YAxisGap;

	yarr[0] = valuePos;
	yarr[1] = rectTop;
	yarr[2] = rectBottom;

	drawArea->polygon(IntArray(xarr, 3), IntArray(yarr, 3), color, color);
	drawArea->rect(rectLeft, rectTop, rectLeft + 60, rectBottom, color, color);

	std::string value = std::format(_T("{0:.2f}"), close);
	DarkHorse::SmUtil::to_thou_sep(value);
	TTFText* t = drawArea->text(value.c_str(), _T("굴림"), 9);
	t->draw(plotAreaLeftX + width + _YAxisGap, valuePos - 6, 0xffffff);
	t->destroy();
}

void RealTimeMultiChart::DrawCurrentValue(XYChart* chart, Axis* axis, const int& color, const double& close, const double& start_val)
{
	DrawArea* drawArea = chart->makeChart();

	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + chart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();

	//DarkHorse::SmStockData last_data;
	//_ChartData->GetLastData(last_data);
	int valuePos = chart->getYCoor(close, axis);
	int rectTop = valuePos - 8;
	int rectLeft = plotAreaLeftX + width + _YAxisGap;
	int rectBottom = rectTop + 16;

	int xarr[3], yarr[3];
	xarr[0] = plotAreaLeftX + width + _YAxisGap - 14;
	xarr[1] = plotAreaLeftX + width + _YAxisGap;
	xarr[2] = plotAreaLeftX + width + _YAxisGap;

	yarr[0] = valuePos;
	yarr[1] = rectTop;
	yarr[2] = rectBottom;

	drawArea->polygon(IntArray(xarr, 3), IntArray(yarr, 3), color, color);
	drawArea->rect(rectLeft, rectTop, rectLeft + 100, rectBottom, color, color);

	double rate = (((close - start_val) / start_val) * 100);

	std::string value = std::format(_T("{0:.2f}"), close);
	std::string ud_rate = std::format(_T("[{0:.2f}]"), rate);
	DarkHorse::SmUtil::to_thou_sep(value);
	value.append(" ");
	value.append(ud_rate);
	TTFText* t = drawArea->text(value.c_str(), _T("굴림"), 9);
	t->draw(plotAreaLeftX + width + _YAxisGap, valuePos - 6, 0xffffff);
	t->destroy();
}

void RealTimeMultiChart::DrawCurrentValue(XYChart* chart, const int& y_pos, const int& color, const double& close)
{
	DrawArea* drawArea = chart->makeChart();

	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + chart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();

	int valuePos = y_pos;
	int rectTop = valuePos - 8;
	int rectLeft = plotAreaLeftX + width + _YAxisGap;
	int rectBottom = rectTop + 16;

	drawArea->rect(rectLeft, rectTop, rectLeft + 100, rectBottom, color, color);


	std::string value = std::format(_T("{0:.2f}"), close);
	DarkHorse::SmUtil::to_thou_sep(value);
	TTFText* t = drawArea->text(value.c_str(), _T("굴림"), 9);
	t->draw(plotAreaLeftX + width + _YAxisGap, valuePos - 6, 0xffffff);
	t->destroy();
}

void RealTimeMultiChart::DrawCurrentValue(XYChart* chart, const int& y_pos, const int& color, const double& close, const double& start_val)
{
	DrawArea* drawArea = chart->makeChart();

	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + chart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();

	int valuePos = y_pos;
	int rectTop = valuePos - 8;
	int rectLeft = plotAreaLeftX + width + _YAxisGap;
	int rectBottom = rectTop + 16;

	drawArea->rect(rectLeft, rectTop, rectLeft + 100, rectBottom, color, color);


	double rate = (((close - start_val) / start_val) * 100);

	std::string value = std::format(_T("{0:.2f}"), close);
	std::string ud_rate = std::format(_T("[{0:.2f}]"), rate);
	DarkHorse::SmUtil::to_thou_sep(value);
	value.append(" ");
	value.append(ud_rate);
	TTFText* t = drawArea->text(value.c_str(), _T("굴림"), 9);
	t->draw(plotAreaLeftX + width + _YAxisGap, valuePos - 6, 0xffffff);
	t->destroy();
}

void RealTimeMultiChart::DrawTitleValue(XYChart* chart)
{
	DrawArea* d = chart->makeChart();

	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + chart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();

	std::string text = _T("<*font = times.ttf, size = 18, color = FF0000*>");
	DarkHorse::SmStockData last_data;
	std::string strValue = std::format("{0:.2f}", 1445.0);
	SmUtil::to_thou_sep(strValue);

	text.append(TCHARtoUTF8(_T(",종목이름:")));

	text.append(TCHARtoUTF8("가상코스피"));

	TTFText* t = d->text(text.c_str(), _T("굴림"), 14);
	t->draw(plotAreaLeftX, 0, 0xff0000);
	t->destroy();
}

void RealTimeMultiChart::RefreshChart()
{
	m_ChartViewer.updateViewPort(true, false);
}

void RealTimeMultiChart::OnTimer(UINT_PTR nIDEvent)
{
	if (!_InitDlg) return;

	if (nIDEvent == REQUEST_TIMER) {
		RequestChartData();
	}
	_NeedRedraw = false;
	if (nIDEvent == REDRAW_TIMER && _bDataReady) {
		if (_NeedAdd) {
			ProcessAddData();
			_NeedAdd = false;
			_NeedRedraw = true;
		}
		if (_NeedUpdate) {
			ProcessUpdateData();
			_NeedUpdate = false;
			_NeedRedraw = true;
		}
		if (_NeedRedraw) {
			m_ChartViewer.updateViewPort(true, false);
			_NeedRedraw = false;
		}
	}

	CBCGPDialog::OnTimer(nIDEvent);
}


void RealTimeMultiChart::OnBnClickedBtnApply()
{
	// TODO: Add your control notification handler code here
	CString value;
	_EditNq.GetWindowText(value);
	_ChartData.ProcessChartData();
}


void RealTimeMultiChart::OnBnClickedCheckNk()
{
	// TODO: Add your control notification handler code here
	if (_CheckNk.GetCheck() == BST_CHECKED) _ShowNk = true;
	else _ShowNk = false;

	RefreshChart();
}


void RealTimeMultiChart::OnBnClickedCheckHs()
{
	// TODO: Add your control notification handler code here
	if (_CheckHs.GetCheck() == BST_CHECKED) _ShowHs = true;
	else _ShowHs = false;

	RefreshChart();
}


void RealTimeMultiChart::OnBnClickedCheckV1()
{
	// TODO: Add your control notification handler code here
	if (_CheckV1.GetCheck() == BST_CHECKED) _ShowV1 = true;
	else _ShowV1 = false;

	RefreshChart();
}


void RealTimeMultiChart::OnBnClickedCheckV2()
{
	// TODO: Add your control notification handler code here
	if (_CheckV2.GetCheck() == BST_CHECKED) _ShowV2 = true;
	else _ShowV2 = false;

	RefreshChart();
}


void RealTimeMultiChart::OnBnClickedCheckV3()
{
	// TODO: Add your control notification handler code here
	if (_CheckV3.GetCheck() == BST_CHECKED) _ShowV3 = true;
	else _ShowV3 = false;

	RefreshChart();
}


void RealTimeMultiChart::OnClose()
{
	_ChartData.RemoveChartRef();
	_ChartData.Ready(false);
	KillTimer(REDRAW_TIMER);

	CBCGPDialog::OnClose();
}


void RealTimeMultiChart::OnBnClickedBtnSet()
{
	SetEnableUpdate(false);
	if (_ChartData.OpOption() == OP_KOSPI) {
		SetSpreadDialog dlg(this);
		dlg.DoModal();
	}
	else {
		SetSpreadHsDialog dlg(this);
		dlg.DoModal();
	}
	SetEnableUpdate(true);
}


void RealTimeMultiChart::OnBnClickedCheckD1()
{
	// TODO: Add your control notification handler code here
	if (_CheckD1.GetCheck() == BST_CHECKED) _ShowD1 = true;
	else _ShowD1 = false;

	RefreshChart();
}


void RealTimeMultiChart::OnBnClickedCheckD2()
{
	if (_CheckD2.GetCheck() == BST_CHECKED) _ShowD2 = true;
	else _ShowD2 = false;

	RefreshChart();
}


void RealTimeMultiChart::OnBnClickedCheckD3()
{
	if (_CheckD3.GetCheck() == BST_CHECKED) _ShowD3 = true;
	else _ShowD3 = false;

	RefreshChart();
}


void RealTimeMultiChart::OnBnClickedCheckNq()
{
	if (_CheckNq.GetCheck() == BST_CHECKED) _ShowNq = true;
	else _ShowNq = false;

	RefreshChart();
}


void RealTimeMultiChart::OnCbnSelchangeComboSymbol()
{
	int index = _ComboSymbolMain.GetCurSel();
	if (index < 0) return;

	auto symbol = _RowToMainSymbolMap[index];
	if (symbol->ProductCode() == "HSI") {
		_ChartData.OpOption(OP_HS);
		// _CheckHs
		_CheckHs.SetWindowText("코스피200");
		_CheckV1.SetWindowText("예측항생A");
		_CheckV2.SetWindowText("예측항생B");
		_CheckV3.SetWindowText("예측항생C");
	}
	else {
		_CheckHs.SetWindowText("항생");
		_CheckV1.SetWindowText("예측코스피200A");
		_CheckV2.SetWindowText("예측코스피200B");
		_CheckV3.SetWindowText("예측코스피200C");
		_ChartData.OpOption(OP_KOSPI);
	}
	_ChartData.Ready(false);
	_ChartData.ProcessChartData();
	_ChartData.Ready(true);
	RefreshChart();
}
