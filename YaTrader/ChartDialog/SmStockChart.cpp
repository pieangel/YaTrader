// SmStockChart.cpp: 구현 파일
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmStockChart.h"
#include "../Chart/SmChartData.h"
#include <string>
#include "../Global/SmTotalManager.h"
#include "../Chart/SmChartDataManager.h"
#include "afxdialogex.h"
#include "../Pnf/PnfManager.h"
#include "../Pnf/PnfMaker.h"
#include "../Drawing/VtLine.h"
#include "../Util/SmUtil.h"
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../System/SmSystemManager.h"
#include "../System/SmSystem.h"
#include "../System/SmSystemFactor.h"
#include "../System/SmSystemData.h"
#include "../resource.h"

using namespace DarkHorse;
using namespace std;


// SmStockChart 대화 상자

IMPLEMENT_DYNAMIC(SmStockChart, CBCGPDialog)

SmStockChart::SmStockChart(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_SYSTEM_CHART, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	SetWhiteBackground(FALSE);
}

SmStockChart::~SmStockChart()
{
	delete m_ChartViewer.getChart();
	delete m_ViewPortControl.getChart();
}

void SmStockChart::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, _ComboSymbol);
	DDX_Control(pDX, IDC_COMBO_CYCLE, _ComboCycle);
	DDX_Control(pDX, IDC_COMBO_TICK, _ComboTick);

	DDX_Control(pDX, IDC_ChartViewer, m_ChartViewer);
	DDX_Control(pDX, IDC_ViewPortControl, m_ViewPortControl);
	DDX_Control(pDX, IDC_COMBO_STYLE, _ComboStyle);
	DDX_Control(pDX, IDC_COMBO_SYSTEM, _ComboSystem);
	DDX_Control(pDX, IDC_COMBO_DATA, _ComboData);
	DDX_Control(pDX, IDC_COMBO_JOIN_INDEX, _ComboJoin);
}


BEGIN_MESSAGE_MAP(SmStockChart, CBCGPDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_CONTROL(CVN_ViewPortChanged, IDC_ChartViewer, OnViewPortChanged)
	ON_CONTROL(CVN_MouseMovePlotArea, IDC_ChartViewer, OnMouseMovePlotArea)
	ON_CONTROL(CVN_MouseMoveChart, IDC_ChartViewer, OnMouseMoveOverChart)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &SmStockChart::OnCbnSelchangeComboSymbol)
	ON_CBN_SELCHANGE(IDC_COMBO_STYLE, &SmStockChart::OnCbnSelchangeComboStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_DATA, &SmStockChart::OnCbnSelchangeComboData)
	ON_BN_CLICKED(IDC_BTN_JOIN, &SmStockChart::OnBnClickedBtnJoin)
	ON_CBN_SELCHANGE(IDC_COMBO_JOIN_INDEX, &SmStockChart::OnCbnSelchangeComboJoinIndex)
END_MESSAGE_MAP()


// SmStockChart 메시지 처리기


BOOL SmStockChart::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	_EditMode = EditStatus::None;
	_PenColor.A = 0;
	_PenColor.R = 0;
	_PenColor.G = 0;
	_PenColor.B = 0;

	_FillColor.A = 120;
	_FillColor.R = 21;
	_FillColor.G = 225;
	_FillColor.B = 103;

 	m_ChartViewer.setFullRange("x", 0, DefaultChartDataSize - 1);
	m_ChartViewer.setZoomInWidthLimit(30.0 / (DefaultChartDataSize - 1));
	m_ChartViewer.setViewPortWidth(m_ChartViewer.getZoomInWidthLimit());
	//m_ChartViewer.setMouseUsage(Chart::MouseUsageScroll);
	m_ChartViewer.setMouseUsage(Chart::MouseUsageDefault);
	// Enable mouse wheel zooming by setting the zoom ratio to 1.1 per wheel event
	m_ChartViewer.setMouseWheelZoomRatio(1.1);
	// Bind the CChartViewer to the CViewPortControl
	m_ViewPortControl.setViewer(&m_ChartViewer);

	InitChartDataCombo();

	std::vector<std::string> style_vec;
	style_vec.push_back("CandleStick");
	style_vec.push_back("OHLC");
	style_vec.push_back("ClosePrice");
	style_vec.push_back("TypicalPrice");
	style_vec.push_back("WeightedClose");
	style_vec.push_back("MedianPrice");

	for (size_t i = 0; i < style_vec.size(); ++i) {
		_ComboStyle.AddString(style_vec[i].c_str());
	}
	_ComboStyle.SetCurSel(1);


	if (_ChartDataMap.size() > 0) {
		_ComboSymbol.SetCurSel(0);
		SetChartData(_ChartDataMap.begin()->second);
	}

	_ComboJoin.SetCurSel(0);
	InitSystemCombo();


	if (_System) {
		_System->ProcessData(_ChartData);
	}


	InitTypeCombo();

	RecalLayout();

	SetTimer(1, 200, NULL);

	_InitDlg = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void SmStockChart::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	_Enable = false;
	CBCGPDialog::OnClose();
}

void SmStockChart::drawChart(CChartViewer* viewer)
{
	if (!_Enable || !_ChartData) return;

	// After determining the extra points, we can obtain the data
	int startIndex = (int)floor(viewer->getValueAtViewPort("x", viewer->getViewPortLeft()));
	int endIndex = (int)ceil(viewer->getValueAtViewPort("x", viewer->getViewPortLeft() + viewer->getViewPortWidth()));

	hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData->GetDataFrame();

	size_t duration = endIndex - startIndex + 1;
	if (startIndex + duration > df.shape().first - 1) {
		size_t delta = startIndex + duration - (df.shape().first - 1);
		duration -= delta;
		duration++;
	}

	std::string prefix = _ChartData->MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	auto& dt = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	auto& high = df.get_column<double>(col_high.c_str());
	auto& low = df.get_column<double>(col_low.c_str());
	auto& open = df.get_column<double>(col_open.c_str());
	auto& close = df.get_column<double>(col_close.c_str());
	auto& volume = df.get_column<double>(col_volume.c_str());

	std::string col_ha_high(prefix), col_ha_close(prefix), col_ha_low(prefix), col_ha_open(prefix);
	col_ha_high.append("ha_high");
	col_ha_close.append("ha_close");
	col_ha_low.append("ha_low");
	col_ha_open.append("ha_open");
	auto& ha_high = df.get_column<double>(col_ha_high.c_str());
	auto& ha_low = df.get_column<double>(col_ha_low.c_str());
	auto& ha_open = df.get_column<double>(col_ha_open.c_str());
	auto& ha_close = df.get_column<double>(col_ha_close.c_str());

	std::string col_ha2_high(prefix), col_ha2_close(prefix), col_ha2_low(prefix), col_ha2_open(prefix);
	col_ha2_high.append("ha2_high");
	col_ha2_close.append("ha2_close");
	col_ha2_low.append("ha2_low");
	col_ha2_open.append("ha2_open");

	auto& ha2_high = df.get_column<double>(col_ha2_high.c_str());
	auto& ha2_low = df.get_column<double>(col_ha2_low.c_str());
	auto& ha2_open = df.get_column<double>(col_ha2_open.c_str());
	auto& ha2_close = df.get_column<double>(col_ha2_close.c_str());
	

	DoubleArray timeStamps = DoubleArray(dt.data() + startIndex, duration);
	DoubleArray volData = DoubleArray(volume.data() + startIndex, duration);
	DoubleArray highData;
	DoubleArray lowData;
	DoubleArray openData;
	DoubleArray closeData;
	if (_MainDataType == 0) {
		highData = DoubleArray(high.data() + startIndex, duration);
		lowData = DoubleArray(low.data() + startIndex, duration);
		openData = DoubleArray(open.data() + startIndex, duration);
		closeData = DoubleArray(close.data() + startIndex, duration);
	}
	
	else if (_MainDataType == 1) {
		highData = DoubleArray(ha_high.data() + startIndex, duration);
		lowData = DoubleArray(ha_low.data() + startIndex, duration);
		openData = DoubleArray(ha_open.data() + startIndex, duration);
		closeData = DoubleArray(ha_close.data() + startIndex, duration);
	}
	else {
		highData = DoubleArray(ha2_high.data() + startIndex, duration);
		lowData = DoubleArray(ha2_low.data() + startIndex, duration);
		openData = DoubleArray(ha2_open.data() + startIndex, duration);
		closeData = DoubleArray(ha2_close.data() + startIndex, duration);
	}
	
	//
	// At this stage, we have extracted the visible data. We can use those data to plot the chart.
	//

	//================================================================================
	// Configure overall chart appearance.
	//================================================================================

	// Create a FinanceChart object of width _ChartWidth pixels
	FinanceChart* f_chart = new FinanceChart(_ChartWidth);
	f_chart->setBackground(0x363636);
	f_chart->setMargins(0, 0, 0, 0);

	// Disable default legend box, as we are using dynamic legend
	f_chart->setLegendStyle("normal", 8, Chart::Transparent, Chart::Transparent);

	// Add a title to the chart
	//c->addTitle("Finance Chart Demonstration");

	// Set the data into the finance chart object
	f_chart->setData(timeStamps, highData, lowData, openData, closeData, volData, 0);
	// 메인 차트의 높이는 차트 윈도우 전체 크기에서 툴바와 차트포트컨트롤을 제외한 높이에서 지표들의 높이를 뺀 값이 된다.
	int mainChartHeight = _ChartHeight - _PlotAreaGapHeight;
	// Add the main chart with _ChartHeight pixels in height
	XYChart* main_chart = f_chart->addMainChart(mainChartHeight);

	main_chart->setBackground(0x363636);

	int curWidth = main_chart->getWidth();
	int curHeight = main_chart->getHeight();
	curHeight = curWidth + curHeight;
	// 메인 차트의 차트 영역을 지정해 준다.
	//main_chart->setPlotArea(0, 35, _ChartWidth - _LeftMargin - _RightMargin, mainChartHeight - 80, 0x393939, 0x393939, 0x393939, main_chart->dashLineColor(0x6f6f6f, Chart::DotLine), -1);
	PlotArea* plot_area = main_chart->setPlotArea(0, 35, _ChartWidth - _LeftMargin - _RightMargin, mainChartHeight - 80, 0x393939, 0x393939, 0x393939, 0x6f6f6f, 0x6f6f6f);
	//plot_area->setAltBgColor(false, 0x393939, 0x444444, Chart::Transparent);
	// Y축의 오프셋을 지정해 준다.
	main_chart->yAxis()->setOffset(_YAxisGap, 0);
	// Y축의 색상을 지정해 준다.
	main_chart->yAxis()->setColors(0x6f6f6f);
	// X축의 색상을 지정해 준다.
	main_chart->xAxis()->setColors(0x6f6f6f);
	// 천단위 표시를 해준다.
	//main_chart->setNumberFormat(',');
	main_chart->yAxis()->setLabelFormat("{value|2,.}");
	

	// Create custom labels for the x-axis
	std::vector<const char*> xLabels(timeStamps.len);
	std::vector<std::string> stringBuffer(timeStamps.len);
	const char* lastLabel = nullptr;
	// X축 레이블을 추출해서 적용해 준다.
	for (int i = 0; i < timeStamps.len; ++i)
	{
		if (fmod(timeStamps[i], 3600) < 60) {
			const char* curLabel = (stringBuffer[i] = main_chart->formatValue(timeStamps[i], "{value|hh:nn}")).c_str();
			if (lastLabel != nullptr && strcmp(curLabel, lastLabel) == 0) {
				lastLabel = curLabel;
				continue;
			}
			xLabels[i] = curLabel;
			lastLabel = curLabel;
		}
	}
	main_chart->xAxis()->setLabels(StringArray(&(xLabels[0]), xLabels.size()));

	// We keep the original timestamps in a hidden axis for usage in the track cursor
	main_chart->xAxis2()->setLabels(timeStamps);
	//m->xAxis2()->setColors(Chart::Transparent, Chart::Transparent);
	main_chart->xAxis2()->setColors(Chart::Transparent, 0xffff0000);

	main_chart->yAxis()->setLabelStyle("Arial", 10, 0x6f6f6f);

	main_chart->xAxis()->setLabelStyle("Arial", 10, 0x6f6f6f);


	if (_MainChartType == SmChartStyle::ClosePrice)
	{
		f_chart->addCloseLine(0x1EA7F1);
	}
	else if (_MainChartType == SmChartStyle::TypicalPrice)
	{
		f_chart->addTypicalPrice(0x1EA7F1);
	}
	else if (_MainChartType == SmChartStyle::WeightedClose)
	{
		f_chart->addWeightedClose(0x1EA7F1);
	}
	else if (_MainChartType == SmChartStyle::MedianPrice)
	{
		f_chart->addMedianPrice(0x1EA7F1);
	}
	else if (_MainChartType == SmChartStyle::OHLC)
	{
		f_chart->addHLOC(0xdc143c, 0x87cefa);
	}
	else
	{
		// Add candlestick symbols to the main chart, using green/red for up/down days
		CandleStickLayer* candle = f_chart->addCandleStick(0xdc143c, 0x87cefa);
		candle->setColors(0xdc143c, 0xdc143c, 0x87cefa, 0x87cefa);
	}


	

	if (_System && _System->DataProcessed()) {
		const SmSystemFactor& fact = _System->Factor();
		std::shared_ptr<SmSystemData> sys_data = _System->Data();
		std::string col_hl(prefix), col_ll(prefix), col_ml(prefix);
		col_hl.append("high_lowess");
		col_ll.append("low_lowess");
		col_ml.append("med_lowess");
		if (fact.ShowLowess) {
			auto& high_lowess_v = sys_data->GetDataFrame().get_column<double>(col_hl.c_str());
			auto& low_lowess_v = sys_data->GetDataFrame().get_column<double>(col_ll.c_str());

			DoubleArray high_lowess = DoubleArray(high_lowess_v.data() + startIndex, duration);
			DoubleArray low_lowess = DoubleArray(low_lowess_v.data() + startIndex, duration);

			main_chart->addSplineLayer(ArrayMath(high_lowess), 0x00ccff)->setLineWidth(3);
			main_chart->addSplineLayer(ArrayMath(low_lowess), 0xff99ff)->setLineWidth(3);
		}

		if (fact.ShowMidLowess) {
			auto& med_lowess_v = sys_data->GetDataFrame().get_column<double>(col_ml.c_str());
			DoubleArray close_lowess = DoubleArray(med_lowess_v.data() + startIndex, duration);

			main_chart->addSplineLayer(ArrayMath(close_lowess), 0x66ff66)->setLineWidth(3);
		}

		if (fact.ShowChannel) {
			std::string col_hcu(prefix), col_hcl(prefix), col_lcu(prefix), col_lcl(prefix), col_mc(prefix), col_mp(prefix);
			col_hcu.append("high_channel_upper");
			col_hcl.append("high_channel_lower");
			col_lcu.append("low_channel_upper");
			col_lcl.append("low_channel_lower");
			col_mc.append("mid_channel");

			auto& high_channel_upper = sys_data->GetDataFrame().get_column<double>(col_hcu.c_str());
			auto& high_channel_lower = sys_data->GetDataFrame().get_column<double>(col_hcl.c_str());
			auto& low_channel_upper = sys_data->GetDataFrame().get_column<double>(col_lcu.c_str());
			auto& low_channel_lower = sys_data->GetDataFrame().get_column<double>(col_lcl.c_str());
			auto& mid_channel = sys_data->GetDataFrame().get_column<double>(col_mc.c_str());

			DoubleArray hcu = DoubleArray(high_channel_upper.data() + startIndex, duration);
			DoubleArray hcl = DoubleArray(high_channel_lower.data() + startIndex, duration);
			DoubleArray lcl = DoubleArray(low_channel_lower.data() + startIndex, duration);
			DoubleArray lcu = DoubleArray(low_channel_upper.data() + startIndex, duration);
			DoubleArray mc = DoubleArray(mid_channel.data() + startIndex, duration);

			f_chart->addBand(hcu, hcl, 0x8668a6, 0x8c045f, "HighChannel");
			f_chart->addBand(lcu, lcl, 0x8668a6, 0x0e7c29, "LowChannel");
			main_chart->addLineLayer(ArrayMath(mc), 0x8668a6)->setLineWidth(3);

		}
	}


	std::shared_ptr<PnfMaker> pnf_maker = mainApp.PnfMgr()->FindPnfMaker(_ChartData->GetChartDataKey());
	if (pnf_maker) DrawPnf(duration, pnf_maker->GetPnfMap(), timeStamps, main_chart);

	if (!viewer->isInMouseMoveEvent()){
		DrawTitleValue(f_chart);
		DrawCurrentValue(f_chart);
		trackFinance(f_chart, (0 == viewer->getChart()) ? ((XYChart*)f_chart->getChart(0))->getPlotArea()->getRightX() : viewer->getPlotAreaMouseX());
	}



	// Set the chart to the viewer
	delete viewer->getChart();
	viewer->setChart(f_chart);
}

void SmStockChart::trackFinance(MultiChart* m, int mouseX)
{
	// Clear the current dynamic layer and get the DrawArea object to draw on it.
	DrawArea* d = m->initDynamicLayer();

	// It is possible for a FinanceChart to be empty, so we need to check for it.
	if (m->getChartCount() == 0)
		return;

	// Get the data x-value that is nearest to the mouse
	XYChart* mainChart = (XYChart*)m->getChart(0);
	int xValue = (int)(mainChart->getNearestXValue(mouseX));

	PlotArea* plot = mainChart->getPlotArea();
	int bottom = plot->getBottomY();
	int mouseY = m_ChartViewer.getChartMouseY();
	// 	if (mouseY <= bottom) {
	// 		::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
	// 	}


		// Iterate the XY charts (main price chart and indicator charts) in the FinanceChart
	XYChart* c = 0;
	for (int i = 0; i < m->getChartCount(); ++i) {
		c = (XYChart*)m->getChart(i);

		// Variables to hold the legend entries
		ostringstream ohlcLegend;
		vector<string> legendEntries;

		// Iterate through all layers to find the highest data point
		for (int j = 0; j < c->getLayerCount(); ++j) {
			Layer* layer = c->getLayerByZ(j);
			int xIndex = layer->getXIndexOf(xValue);
			int dataSetCount = layer->getDataSetCount();

			// In a FinanceChart, only layers showing OHLC data can have 4 data sets
			if (dataSetCount == 4) {
				double highValue = layer->getDataSet(0)->getValue(xIndex);
				double lowValue = layer->getDataSet(1)->getValue(xIndex);
				double openValue = layer->getDataSet(2)->getValue(xIndex);
				double closeValue = layer->getDataSet(3)->getValue(xIndex);

				if (closeValue != Chart::NoValue) {
					// Build the OHLC legend
					ohlcLegend << "      <*block*>";
					ohlcLegend << "Open: " << c->formatValue(openValue, "{value|P4}");
					ohlcLegend << ", High: " << c->formatValue(highValue, "{value|P4}");
					ohlcLegend << ", Low: " << c->formatValue(lowValue, "{value|P4}");
					ohlcLegend << ", Close: " << c->formatValue(closeValue, "{value|P4}");

					// We also draw an upward or downward triangle for up and down days and the %
					// change
					double lastCloseValue = layer->getDataSet(3)->getValue(xIndex - 1);
					if (lastCloseValue != Chart::NoValue) {
						double change = closeValue - lastCloseValue;
						double percent = change * 100 / closeValue;
						string symbol = (change >= 0) ?
							"<*font,color=008800*><*img=@triangle,width=8,color=008800*>" :
							"<*font,color=CC0000*><*img=@invertedtriangle,width=8,color=CC0000*>";

						ohlcLegend << "  " << symbol << " " << c->formatValue(change, "{value|P4}");
						ohlcLegend << " (" << c->formatValue(percent, "{value|2}") << "%)<*/font*>";
					}

					ohlcLegend << "<*/*>";
				}
			}
			else {
				// Iterate through all the data sets in the layer
				for (int k = 0; k < layer->getDataSetCount(); ++k) {
					DataSet* dataSet = layer->getDataSetByZ(k);

					string name = dataSet->getDataName();
					double value = dataSet->getValue(xIndex);
					if ((0 != name.size()) && (value != Chart::NoValue)) {

						// In a FinanceChart, the data set name consists of the indicator name and its
						// latest value. It is like "Vol: 123M" or "RSI (14): 55.34". As we are
						// generating the values dynamically, we need to extract the indictor name
						// out, and also the volume unit (if any).

						// The volume unit
						string unitChar;

						// The indicator name is the part of the name up to the colon character.
						int delimiterPosition = (int)name.find(':');
						if (name.npos != delimiterPosition) {

							// The unit, if any, is the trailing non-digit character(s).
							int lastDigitPos = (int)name.find_last_of("0123456789");
							if ((name.npos != lastDigitPos) && (lastDigitPos + 1 < (int)name.size()) &&
								(lastDigitPos > delimiterPosition))
								unitChar = name.substr(lastDigitPos + 1);

							name.resize(delimiterPosition);
						}

						// In a FinanceChart, if there are two data sets, it must be representing a
						// range.
						if (dataSetCount == 2) {
							// We show both values in the range in a single legend entry
							value = layer->getDataSet(0)->getValue(xIndex);
							double value2 = layer->getDataSet(1)->getValue(xIndex);
							name = name + ": " + c->formatValue(min(value, value2), "{value|P3}");
							name = name + " - " + c->formatValue(max(value, value2), "{value|P3}");
						}
						else {
							// In a FinanceChart, only the layer for volume bars has 3 data sets for
							// up/down/flat days
							if (dataSetCount == 3) {
								// The actual volume is the sum of the 3 data sets.
								value = layer->getDataSet(0)->getValue(xIndex) + layer->getDataSet(1
								)->getValue(xIndex) + layer->getDataSet(2)->getValue(xIndex);
							}

							// Create the legend entry
							name = name + ": " + c->formatValue(value, "{value|P3}") + unitChar;
						}

						// Build the legend entry, consist of a colored square box and the name (with
						// the data value in it).
						ostringstream legendEntry;
						legendEntry << "<*block*><*img=@square,width=8,edgeColor=000000,color="
							<< hex << dataSet->getDataColor() << "*> " << name << "<*/*>";
						legendEntries.push_back(legendEntry.str());
					}
				}
			}
		}

		// Get the plot area position relative to the entire FinanceChart
		PlotArea* plotArea = c->getPlotArea();
		int plotAreaLeftX = plotArea->getLeftX() + c->getAbsOffsetX();
		int plotAreaTopY = plotArea->getTopY() + c->getAbsOffsetY();

		// The legend begins with the date label, then the ohlcLegend (if any), and then the
		// entries for the indicators.
		ostringstream legendText;
		legendText << "<*block,valign=top,maxWidth=" << (plotArea->getWidth() - 5)
			<< "*><*font=arialbd.ttf*>[" << mainChart->xAxis2()->getFormattedLabel(xValue, "yyyy-mm-dd hh:nn:ss")
			<< "]<*/font*>" << ohlcLegend.str();
		for (int i = ((int)legendEntries.size()) - 1; i >= 0; --i) {
			legendText << "      " << legendEntries[i];
		}
		legendText << "<*/*>";

		// Draw a vertical track line at the x-position
		d->vline(plotAreaTopY, plotAreaTopY + plotArea->getHeight(), c->getXCoor(xValue) +
			c->getAbsOffsetX(), d->dashLineColor(0x6f6f6f, 0x0101));

		// Display the legend on the top of the plot area
		TTFText* t = d->text(legendText.str().c_str(), "arial.ttf", 8);
		t->draw(plotAreaLeftX + 5, plotAreaTopY + 3, 0x6f6f6f, Chart::TopLeft);
		t->destroy();


		int xCoor = c->getXCoor(xValue);
		if (xCoor < plotArea->getLeftX())
			return;

		// Draw a label on the x-axis to show the track line position.
		ostringstream xlabel2;

		xlabel2 << "<*block,valign=top,maxWidth=" << (plotArea->getWidth() - 5)
			<< "*><*font=arialbd.ttf,bgColor=0d730d*>" << mainChart->xAxis2()->getFormattedLabel(xValue, "yyyy-mm-dd hh:nn:ss")
			<< "<*/font*>";

		TTFText* t2 = d->text(xlabel2.str().c_str(), "arialbd.ttf", 10);

		// Restrict the x-pixel position of the label to make sure it stays inside the chart image.
		int xLabelPos = max(0, min(xCoor - t2->getWidth() / 2, c->getWidth() - t2->getWidth()));
		if (i == m->getChartCount() - 1)
			t2->draw(xLabelPos, plotAreaTopY + plotArea->getBottomY() - 15, 0xffffff);
		t2->destroy();

		if (i == 0) {
			double yValue = c->getYValue(_MouseY);
			int plotAreaLeftX = plotArea->getLeftX() + c->getAbsOffsetX();
			int plotAreaTopY = plotArea->getTopY() + c->getAbsOffsetY();
			int height = plotArea->getHeight();
			int width = plotArea->getWidth();

			int valuePos = c->getYCoor(yValue);
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

			d->polygon(IntArray(xarr, 3), IntArray(yarr, 3), 0x0d730d, 0x0d730d);
			d->rect(rectLeft, rectTop, rectLeft + 60, rectBottom, 0x0d730d, 0x0d730d);
			std::ostringstream ss;
			ss.imbue(std::locale("en_US.UTF-8"));
			ss << c->formatValue(yValue, "{value|2,.}");
			std::string value = ss.str();
			TTFText* t3 = d->text(value.c_str(), _T("굴림"), 9);
			t3->draw(plotAreaLeftX + width + _YAxisGap, valuePos - 6, 0xffffff);
			t3->destroy();

			d->hline(plotAreaLeftX, plotAreaLeftX + width, valuePos, d->dashLineColor(0x6f6f6f, 0x0101));
		}
		else {
			int plotAreaLeftX = plotArea->getLeftX() + c->getAbsOffsetX();
			int plotAreaTopY = plotArea->getTopY() + c->getAbsOffsetY();
			int height = plotArea->getHeight();
			int width = plotArea->getWidth();

			double yValue = c->getYValue(_MouseY - plotAreaTopY);

			yValue = yValue;

		}
	}
}

void SmStockChart::drawFullChart(CViewPortControl* vpc)
{
	hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData->GetDataFrame();

	std::string prefix = _ChartData->MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	auto& dt = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	auto& high = df.get_column<double>(col_high.c_str());
	auto& low = df.get_column<double>(col_low.c_str());
	auto& open = df.get_column<double>(col_open.c_str());
	auto& close = df.get_column<double>(col_close.c_str());
	auto& volume = df.get_column<double>(col_volume.c_str());

	DoubleArray timeStamps = DoubleArray(dt.data(), dt.size());
	DoubleArray highData = DoubleArray(high.data(), high.size());
	DoubleArray lowData = DoubleArray(low.data(), low.size());
	DoubleArray openData = DoubleArray(open.data(), open.size());
	DoubleArray closeData = DoubleArray(close.data(), close.size());
	DoubleArray volData = DoubleArray(volume.data(), volume.size());

	// Create an XYChart object of size 640 x 60 pixels   
	XYChart* c = new XYChart(_ChartWidth - _LeftMargin - _RightMargin, _ViewPortHeight);

	//c->setBackground(0x363636);
	// Set the plotarea with the same horizontal position as that in the main chart for alignment.
	c->setPlotArea(0, 0, c->getWidth(), c->getHeight(), 0x393939, 0x444444, 0x393939,
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
	LineLayer* layer = c->addLineLayer(closeData, 0xff0000);
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

int SmStockChart::CalcChartHeight()
{
	CRect rect;

	GetClientRect(rect);

	int height = rect.Height();

	int totalIndexHeight = 0;
	if (_ShowIndicator) {
		totalIndexHeight = _IndicatorHeight1 + _IndicatorHeight2;
	}
	height -= totalIndexHeight;
	height -= _ViewPortHeight;
	height -= _ToolBarHeight;

	return height;
}

void SmStockChart::InitChartDataCombo()
{
	const std::map<std::string, std::shared_ptr<DarkHorse::SmChartData>>& chart_data_map = mainApp.ChartDataMgr()->GetChartDataMap();
	for (auto it = chart_data_map.begin(); it != chart_data_map.end(); ++it) {
		int index = _ComboSymbol.AddString(it->first.c_str());
		_ChartDataMap[index] = it->second;

		CString msg;
		msg.Format("%d", index + 1);
		_ComboJoin.AddString(msg);
	}
	_ComboSymbol.SetDroppedWidth(300);
}


void SmStockChart::RecalLayout()
{
	CRect rect;
	GetClientRect(rect);

	_ChartHeight = CalcChartHeight();
	_ChartWidth = rect.Width();

	CRect chartRect;
	chartRect.left = _RightMargin;
	chartRect.top = _ToolBarHeight;
	chartRect.right = _ChartWidth;
	chartRect.bottom = rect.bottom - _ViewPortHeight;
	m_ChartViewer.MoveWindow(chartRect);

	chartRect.left = _RightMargin;
	chartRect.top = rect.bottom - _ViewPortHeight;
	chartRect.right = _ChartWidth - _LeftMargin - _RightMargin;
	chartRect.bottom = rect.Height();
	m_ViewPortControl.MoveWindow(chartRect);

	CRect btnRect;
	btnRect.left = rect.right - 22;
	btnRect.top = rect.bottom - 22;
	btnRect.right = rect.right;
	btnRect.bottom = rect.bottom;
	//_BtnFull.MoveWindow(btnRect);

	m_ChartViewer.updateViewPort(true, false);
}

void SmStockChart::SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data)
{
	if (!chart_data)
		return;

	InitChart(chart_data);

	_ChartData = chart_data;
	m_ChartViewer.setViewPortWidth(0.1);
	m_ChartViewer.setViewPortLeft(0.9);
	m_ChartViewer.updateViewPort(true, false);
}

void SmStockChart::ChangeChartData(std::shared_ptr<DarkHorse::SmChartData> new_chart_data)
{
	if (!new_chart_data) return;

	InitChart(new_chart_data);

	hmdf::StdDataFrame<hmdf::DateTime>& df = new_chart_data->GetDataFrame();

	std::string prefix = new_chart_data->MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	auto& dt = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	auto& high = df.get_column<double>(col_high.c_str());
	auto& low = df.get_column<double>(col_low.c_str());
	auto& open = df.get_column<double>(col_open.c_str());
	auto& close = df.get_column<double>(col_close.c_str());
	auto& volume = df.get_column<double>(col_volume.c_str());

	new_chart_data->ProcessDataPerBar();
	_ChartData = new_chart_data;
	if (_System) _System->ProcessData(new_chart_data);
	m_ChartViewer.updateViewPort(true, false);
}

void SmStockChart::DrawPnf(const int NbPoints, const std::map<double, std::shared_ptr<PnfItem>>& wave_map, const DoubleArray& timeStamp, XYChart* const chart)
{
	if (!chart || wave_map.size() == 0 || !_ChartData) return;

	for (auto i = wave_map.begin(); i != wave_map.end(); ++i) {
		const auto pnf = i->second;
		
		int clrPen = VtElement::ColorToInt(_PenColor);
		int penWidth = 3;
		VtPoint start, end, line_end, left_start, left_end, right_start, right_end;
		double xIndex = floor(Chart::bSearch(timeStamp, pnf->time));

		if (xIndex > 0 && xIndex < (NbPoints - 1)) {

			xIndex = xIndex + 0.5;
			start.x = chart->getXCoor(xIndex);
			start.y = chart->getYCoor(pnf->open_value);
			//int xIndex2 = (int)floor(Chart::bSearch(timeStamp, pnf.time));
			end.x = chart->getXCoor(xIndex);
			if (pnf->direction == 1) {
				end.y = chart->getYCoor(pnf->close_value);
				if (pnf->start_point) penWidth = 5;
				VtLine::DrawLine(std::to_string(pnf->acc_count), start, end, chart, 0xff0000, penWidth, _ChartData->SymbolCode());

				left_start.x = end.x;
				left_start.y = end.y;
				left_end.x = left_start.x - 5;
				left_end.y = left_start.y + 5;

				VtLine::DrawLine(false, left_start, left_end, chart, 0xff0000, penWidth, _ChartData->SymbolCode());

				right_start.x = end.x;
				right_start.y = end.y;

				right_end.x = right_start.x + 5;
				right_end.y = right_start.y + 5;

				VtLine::DrawLine(false, right_start, right_end, chart, 0xff0000, penWidth, _ChartData->SymbolCode());
			}
			else {
				end.y = chart->getYCoor(pnf->close_value);
				if (pnf->start_point) penWidth = 5;
				VtLine::DrawLine(std::to_string(pnf->acc_count), start, end, chart, 0x0000ff, penWidth, _ChartData->SymbolCode());

				left_start.x = end.x;
				left_start.y = end.y;
				left_end.x = left_start.x - 5;
				left_end.y = left_start.y - 5;

				VtLine::DrawLine(false, left_start, left_end, chart, 0x0000ff, penWidth, _ChartData->SymbolCode());

				right_start.x = end.x;
				right_start.y = end.y;
				right_end.x = right_start.x + 5;
				right_end.y = right_start.y - 5;

				VtLine::DrawLine(false, right_start, right_end, chart, 0x0000ff, penWidth, _ChartData->SymbolCode());
			}
		}
		
	}
}

void SmStockChart::DrawTitleValue(FinanceChart* m)
{
	if (m->getChartCount() == 0) return;

	XYChart* chart = (XYChart*)m->getChart(0);
	DrawArea* d = chart->makeChart();

	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + chart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();
	std::shared_ptr<DarkHorse::SmChartData> selData = _ChartData;

	std::string text = _T("<*font = times.ttf, size = 18, color = FF0000*>");
	DarkHorse::SmStockData last_data;
	_ChartData->GetLastData(last_data);
	std::string strValue = std::format("{0:.2f}", (double)last_data.close);
	SmUtil::to_thou_sep(strValue);

	text.append(strValue);
	text.append(_T("<*font = gulim.ttc, size = 10, color = ffffff*>"));
	text.append(selData->Currency());

	text.append(_T("<*font = gulim.ttc, size = 10, color = ffffff*>"));
	text.append(TCHARtoUTF8(_T(" 거래소:")));
	text.append(selData->Exchange());
	text.append(TCHARtoUTF8(_T(", 종목이름:")));

	std::shared_ptr<DarkHorse::SmSymbol> sym = mainApp.SymMgr()->FindSymbol(selData->SymbolCode());
	if (sym)
		text.append(TCHARtoUTF8(sym->SymbolNameKr().c_str()));
	int cycle = selData->Cycle();
	CString strCycle;
	strCycle.Format(_T("(%d"), cycle);
	std::string temp;
	temp = strCycle;
	text.append(temp);
	switch (selData->ChartType())
	{
	case SmChartType::DAY:
		text.append(TCHARtoUTF8(_T("일")));
		break;
	case SmChartType::MON:
		text.append(TCHARtoUTF8(_T("월")));
		break;
	case SmChartType::WEEK:
		text.append(TCHARtoUTF8(_T("주")));
		break;
	case SmChartType::MIN:
		text.append(TCHARtoUTF8(_T("분")));
		break;
	case SmChartType::TICK:
		text.append(TCHARtoUTF8(_T("틱")));
		break;
	default:
		break;
	}

	text.append(_T(")"));
	TTFText* t = d->text(text.c_str(), _T("굴림"), 14);
	t->draw(plotAreaLeftX, 0, 0xff0000);
	t->destroy();
}

void SmStockChart::DrawCurrentValue(FinanceChart* m)
{
	if (m->getChartCount() == 0 || !_ChartData)
		return;

	XYChart* chart = (XYChart*)m->getChart(0);
	DrawArea* drawArea = chart->makeChart();

	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + chart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();

	DarkHorse::SmStockData last_data;
	_ChartData->GetLastData(last_data);
	int valuePos = chart->getYCoor(last_data.close);
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

	std::string value = std::format(_T("{0:.2f}"), (double)last_data.close);
	DarkHorse::SmUtil::to_thou_sep(value);
	TTFText* t = drawArea->text(value.c_str(), _T("굴림"), 9);
	t->draw(plotAreaLeftX + width + _YAxisGap, valuePos - 6, 0xffffff);
	t->destroy();
}

void SmStockChart::OnChartUpdateTimer()
{
	if (!_InitDlg) return;

	CChartViewer* viewer = &m_ChartViewer;
	int DataSize = (int)_ChartData->GetChartDataCount();
	// Enables auto scroll if the viewport is showing the latest data before the update
	bool autoScroll = (0.001 + viewer->getValueAtViewPort("x",
		viewer->getViewPortLeft() + viewer->getViewPortWidth()) >= DataSize - 1);

	// Update the new full data range to include the latest data
	bool axisScaleHasChanged = viewer->updateFullRangeH("x", 0,
		(std::max)(initialFullRange, DataSize), Chart::KeepVisibleRange);

	if (autoScroll) {
		hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData->GetDataFrame();

		std::string prefix = _ChartData->MakePrefix();
		std::string col_dt(prefix);
		col_dt.append("dt");

		auto& cv_dt = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
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
}

void SmStockChart::InitChart(std::shared_ptr<DarkHorse::SmChartData> chart_data)
{
	if (!chart_data) return;

	const hmdf::StdDataFrame<hmdf::DateTime>& df = chart_data->GetDataFrame();

	m_ChartViewer.setFullRange("x", 0, df.shape().first - 1);
	m_ChartViewer.setZoomInWidthLimit(30.0 / (df.shape().first - 1));
	m_ChartViewer.setViewPortWidth(m_ChartViewer.getZoomInWidthLimit());
	//m_ChartViewer.setMouseUsage(Chart::MouseUsageScroll);
	m_ChartViewer.setMouseUsage(Chart::MouseUsageDefault);
	// Enable mouse wheel zooming by setting the zoom ratio to 1.1 per wheel event
	m_ChartViewer.setMouseWheelZoomRatio(1.1);
	// Bind the CChartViewer to the CViewPortControl
	m_ViewPortControl.setViewer(&m_ChartViewer);
}

void SmStockChart::InitSystemCombo()
{
	const std::map<std::string, std::shared_ptr<DarkHorse::SmSystem>>& system_map = mainApp.SystemMgr()->GetSystemMap();
	for (auto it = system_map.begin(); it != system_map.end(); ++it) {
		int index = _ComboSystem.AddString(it->second->Name().c_str());
		_SystemMap[index] = it->second;
	}

	if (_SystemMap.size() > 0) {
		_ComboSystem.SetCurSel(0);
		_System = _SystemMap[0];
	}
}

void SmStockChart::InitTypeCombo()
{
	_ComboData.AddString("일반");
	_ComboData.AddString("HA");
	_ComboData.AddString("HA2");

	_ComboData.SetCurSel(0);
}

void SmStockChart::OnViewPortChanged()
{
	// Update the chart if necessary
	if (m_ChartViewer.needUpdateChart())
		drawChart(&m_ChartViewer);

	// Update the full chart
	drawFullChart(&m_ViewPortControl);
}

void SmStockChart::OnMouseMovePlotArea()
{
	// 	_MouseX = m_ChartViewer.getPlotAreaMouseX();
	// 	_MouseY = m_ChartViewer.getPlotAreaMouseY();
	// 	CString msg;
	// 	msg.Format("mouse: x = %d, y = %d \n", _MouseX, _MouseY);
	// 	TRACE(msg);

		//trackFinance((FinanceChart*)m_ChartViewer.getChart(), m_ChartViewer.getPlotAreaMouseX());
		//m_ChartViewer.updateDisplay();
}

void SmStockChart::OnMouseMoveOverChart()
{
	_MouseX = m_ChartViewer.getChartMouseX();
	_MouseY = m_ChartViewer.getChartMouseY();

	if (_EditMode != EditStatus::None) {
		_MovingX = m_ChartViewer.getChartMouseX();
		_MovingY = m_ChartViewer.getChartMouseY();
	}

	CString msg;
	msg.Format("mouse: x = %d, y = %d \n", _MouseX, _MouseY);
	//TRACE(msg);

	trackFinance((FinanceChart*)m_ChartViewer.getChart(), m_ChartViewer.getPlotAreaMouseX());
	if (_EditMode != EditStatus::None) {
		//if (_Drawing)
		//	DrawInstanceObject((FinanceChart*)m_ChartViewer.getChart());
	}



	m_ChartViewer.updateDisplay();
}

void SmStockChart::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if (!_InitDlg) return;

	RecalLayout();
}



void SmStockChart::OnCbnSelchangeComboSymbol()
{
	int cur_sel = _ComboSymbol.GetCurSel();
	if (cur_sel < 0 || _ChartDataMap.size() == 0) return;

	auto it = _ChartDataMap.find(cur_sel);
	if (it == _ChartDataMap.end()) return;

	std::shared_ptr<DarkHorse::SmChartData> new_chart_data = it->second;

	ChangeChartData(new_chart_data);

	if (_System) {
		_System->ProcessData(_ChartData);
	}
}

void SmStockChart::OnTimer(UINT_PTR nIDEvent)
{
	//UpdateClose();
	//m_ChartViewer.updateViewPort(true, false);
	if (!_ChartData) return;

	OnChartUpdateTimer();
	CBCGPDialog::OnTimer(nIDEvent);
}


void SmStockChart::OnCbnSelchangeComboStyle()
{
	int curSel = _ComboStyle.GetCurSel();
	if (curSel < 0) return;

	SmChartStyle style = SmChartStyle::CandleStick;
	switch (curSel)
	{
	case 0:
		style = SmChartStyle::CandleStick;
		break;
	case 1:
		style = SmChartStyle::OHLC;
		break;
	case 2:
		style = SmChartStyle::ClosePrice;
		break;
	case 3:
		style = SmChartStyle::TypicalPrice;
		break;
	case 4:
		style = SmChartStyle::WeightedClose;
		break;
	case 5:
		style = SmChartStyle::MedianPrice;
		break;
	default:
		break;
	}

	_MainChartType = style;

	m_ChartViewer.updateViewPort(true, false);
}


void SmStockChart::OnCbnSelchangeComboData()
{
	_MainDataType = _ComboData.GetCurSel();
	m_ChartViewer.updateViewPort(true, false);
}


void SmStockChart::OnBnClickedBtnJoin()
{
	if (_System) {
		_System->Data()->JoinAllChartData(_ChartData->SymbolCode());
	}
}


void SmStockChart::OnCbnSelchangeComboJoinIndex()
{
	// TODO: Add your control notification handler code here
}
