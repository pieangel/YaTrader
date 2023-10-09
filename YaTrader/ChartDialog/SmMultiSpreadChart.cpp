// SmMultiSpreadChart.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmMultiSpreadChart.h"
#include "../MainFrm.h"
#include <string>
#include <vector>

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

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

#define ID_NEXT_DATA_POINT	1
#define REDRAW_TIMER 1
#define REQUEST_TIMER 2

static const int RSI_PERIOD = 14;


//
// The DataRateTimerId is for the timer that gets real-time data. In real applications,
// the data can be updated by a timer or other methods. In this example, this timer is 
// set to 250ms.
//
// The ChartUpdateTimerId is for the timer that updates the chart. In this example,
// the user can choose the chart update rate from the user interface.
//
static const int DataRateTimerId = 1;
static const int ChartUpdateTimerId = 2;
static const int DataInterval = 250;

//
// The height of each XYChart. The bottom chart has an extra height for the x-axis labels.
//
static const int chartHeight = 120;
static const int xAxisHeight = 25;


// SmMultiSpreadChart dialog

IMPLEMENT_DYNAMIC(SmMultiSpreadChart, CBCGPDialog)

SmMultiSpreadChart::SmMultiSpreadChart(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_MULTI_SPREAD_CHART, pParent)
{
	_UniqueId = CMainFrame::GetId();

	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	SetWhiteBackground(FALSE);
	m_nChartType = 0;
	m_nOverlayLine = 0;
	m_nOverlayBand = 2;
	//}}AFX_DATA_INIT

	m_nLineType = 0;
	m_bAutoScroll = TRUE;
	m_bDataTable = FALSE;
	//}}AFX_DATA_INIT


	m_bIsDarkTheme = TRUE;


}

SmMultiSpreadChart::~SmMultiSpreadChart()
{
	
}

void SmMultiSpreadChart::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CHART, m_wndChart);
	DDX_Control(pDX, IDC_COMBO_THEME, _ComboTheme);
	DDX_Control(pDX, IDC_CHECK_COMP, _CheckComp);
	DDX_Control(pDX, IDC_CHECK_MAIN, _CheckMain);
	DDX_Control(pDX, IDC_CHECK_VIRTUAL, _CheckVirtual);
	DDX_Control(pDX, IDC_COMBO_SYMBOL_COMP, _ComboSymbolComp);
	DDX_Control(pDX, IDC_COMBO_SYMBOL_MAIN, _ComboSymbolMain);
	DDX_Control(pDX, IDC_COMBO_MUL, _ComboMul);
	DDX_Control(pDX, IDC_EDIT_HS, _EditHs);
	DDX_Control(pDX, IDC_EDIT_NK, _EditNk);
	DDX_Control(pDX, IDC_EDIT_NQ, _EditNq);
	DDX_Control(pDX, IDC_EDIT_AFactor, _EditAFactor);
	DDX_Control(pDX, IDC_EDIT_BFactor, _EditBFactor);
}


BEGIN_MESSAGE_MAP(SmMultiSpreadChart, CBCGPDialog)
	
	ON_BN_CLICKED(IDC_BTN_SET, &SmMultiSpreadChart::OnBnClickedBtnSet)
	ON_BN_CLICKED(IDC_BTN_FIND, &SmMultiSpreadChart::OnBnClickedBtnFind)
	ON_CBN_SELCHANGE(IDC_COMBO_STYLE, &SmMultiSpreadChart::OnCbnSelchangeComboStyle)
	ON_REGISTERED_MESSAGE(BCGM_ON_CHART_AXIS_ZOOMED, OnChartAxisZoomed)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &SmMultiSpreadChart::OnUmSymbolSelected)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_THEME, &SmMultiSpreadChart::OnCbnSelchangeComboTheme)
	ON_WM_SIZE()
	ON_MESSAGE(UM_COMP_ADDED, &SmMultiSpreadChart::OnCompDataAdded)
	ON_MESSAGE(UM_COMP_UPDATED, &SmMultiSpreadChart::OnCompDataUpdated)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_MAIN, &SmMultiSpreadChart::OnBnClickedCheckMain)
	ON_BN_CLICKED(IDC_CHECK_COMP, &SmMultiSpreadChart::OnBnClickedCheckComp)
	ON_BN_CLICKED(IDC_CHECK_VIRTUAL, &SmMultiSpreadChart::OnBnClickedCheckVirtual)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL_MAIN, &SmMultiSpreadChart::OnCbnSelchangeComboSymbolMain)
	ON_CBN_SELCHANGE(IDC_COMBO_MUL, &SmMultiSpreadChart::OnCbnSelchangeComboMul)
	ON_BN_CLICKED(IDC_BTN_ZOOM_OUT, &SmMultiSpreadChart::OnBnClickedBtnZoomOut)
	ON_BN_CLICKED(IDC_BTN_ZOOM_IN, &SmMultiSpreadChart::OnBnClickedBtnZoomIn)
	ON_BN_CLICKED(IDC_BTN_RECAL, &SmMultiSpreadChart::OnBnClickedBtnRecal)
	ON_EN_CHANGE(IDC_EDIT_NQ, &SmMultiSpreadChart::OnEnChangeEditNq)
	ON_EN_CHANGE(IDC_EDIT_NK, &SmMultiSpreadChart::OnEnChangeEditNk)
	ON_EN_CHANGE(IDC_EDIT_HS, &SmMultiSpreadChart::OnEnChangeEditHs)
	ON_EN_CHANGE(IDC_EDIT_AFactor, &SmMultiSpreadChart::OnEnChangeEditAfactor)
	ON_EN_CHANGE(IDC_EDIT_BFactor, &SmMultiSpreadChart::OnEnChangeEditBfactor)
END_MESSAGE_MAP()


// SmMultiSpreadChart message handlers



void SmMultiSpreadChart::ShowSeries(const int& series)
{
	if (series < 0 || series >= MAX_SERIES) return;

	//m_arSeries[series]->;
}

BOOL SmMultiSpreadChart::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	if (!_bDataReady) _bDataReady = true;

	_CheckMain.SetCheck(BST_CHECKED);
	_CheckComp.SetCheck(BST_CHECKED);
	_CheckVirtual.SetCheck(BST_CHECKED);

	m_brText = CBCGPBrush(CBCGPColor::Maroon);
	m_brFill = CBCGPBrush(CBCGPColor::Gold, CBCGPColor::White, CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT, .8);
	m_brBorder = CBCGPBrush(CBCGPColor::Goldenrod);

	m_brFill1 = CBCGPBrush(CBCGPColor::White);
	m_brFill2 = CBCGPBrush(CBCGPColor::White);
	m_brFill3 = CBCGPBrush(CBCGPColor::White);
	m_brFill4 = CBCGPBrush(CBCGPColor::White);
	m_brFill5 = CBCGPBrush(CBCGPColor::White);
	m_brDeltaText = CBCGPBrush(CBCGPColor::Black);

	m_brFillText1 = CBCGPBrush(CBCGPColor(RGB(0xC6, 0x21, 0x27)));
	m_brFillText2 = CBCGPBrush(CBCGPColor(RGB(0xF6, 0x8E, 0x2F)));
	m_brFillText3 = CBCGPBrush(CBCGPColor(RGB(0x18, 0xAA, 0x9D)));
	m_brFillText4 = CBCGPBrush(CBCGPColor::Purple);
	m_brFillText5 = CBCGPBrush(CBCGPColor::HotPink);
	m_brDeltaFill = CBCGPBrush(CBCGPColor::White);

	m_brLine1 = CBCGPBrush(CBCGPColor(RGB(0xC6, 0x21, 0x27)));
	m_brLine2 = CBCGPBrush(CBCGPColor(RGB(0xF6, 0x8E, 0x2F)));
	m_brLine3 = CBCGPBrush(CBCGPColor(RGB(0x18, 0xAA, 0x9D)));
	m_brLine4 = CBCGPBrush(CBCGPColor::Purple);
	m_brLine5 = CBCGPBrush(CBCGPColor::HotPink);
	m_brDeltaOutline = CBCGPBrush(CBCGPColor::White);

	m_brUp = CBCGPBrush(CBCGPColor::Orange);
	m_brDown = CBCGPBrush(CBCGPColor::LimeGreen);

	m_brZeroLine = CBCGPBrush(CBCGPColor::White);;

	InitTheme();

	InitMulCombo();

	_EditNq.SetWindowText("0.75");
	_EditHs.SetWindowText("0.92");
	_EditNk.SetWindowText("1.0");
	_EditAFactor.SetWindowText("0.6");
	_EditBFactor.SetWindowText("0.4");


	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
	if (pLayout != NULL)
	{
		pLayout->AddAnchor(IDC_STATIC_CHART, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);
	}


	UpdateChartColorTheme(m_nColorTheme, m_bIsDarkTheme);



	InitSymbolCombo();

	

	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();

	

	ASSERT_VALID(pChart);
	m_wndChart.EnableTooltip(TRUE);

	pChart->ShowAxisIntervalInterlacing(BCGP_CHART_X_PRIMARY_AXIS, FALSE);
	pChart->ShowAxisIntervalInterlacing(BCGP_CHART_X_PRIMARY_AXIS, FALSE);

	pChart->SetChartType(BCGPChartLine, BCGP_CT_SIMPLE, FALSE);
	pChart->SetLegendPosition(BCGPChartLayout::LP_TOP);

	CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	ASSERT_VALID(pXAxis);

	pXAxis->ShowScrollBar(TRUE);
	pXAxis->m_bFormatAsDate = TRUE;
	pXAxis->m_majorTickMarkType = CBCGPChartAxis::TMT_INSIDE;
	

	CBCGPChartAxis* pYAxis = pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);
	CBCGPChartAxis* pYSecondAxis = pChart->GetChartAxis(BCGP_CHART_Y_SECONDARY_AXIS);
	ASSERT_VALID(pYAxis);
	pYAxis->EnableZoom(FALSE);
	pYSecondAxis->EnableZoom(FALSE);

	//pYAxis->SetFixedDisplayRange(0., MAX_VAL, 20.);

	pYAxis->m_bDisplayAxisName = TRUE;
	pYAxis->m_strAxisName = "지수";

	//pChart->ShowAxisIntervalInterlacing(BCGP_CHART_Y_PRIMARY_AXIS);

	//pChart->SetZoomScrollConfig(BCGPChartMouseConfig::ZSO_NONE);

	pChart->ShowAxis(BCGP_CHART_Y_SECONDARY_AXIS, TRUE, FALSE);
	
	CVirtualSeries* pVirtualSeries = new CVirtualSeries(pChart);
	pVirtualSeries->SetCurveType(BCGPChartFormatSeries::CCT_LINE);
	pVirtualSeries->m_strSeriesName = "main";
	pVirtualSeries->SetSeriesLineColor(m_brLine1);
	m_arSeries[0] = pVirtualSeries;
	pChart->AddSeries(pVirtualSeries);
	
	pVirtualSeries = new CVirtualSeries(pChart);
	pVirtualSeries->SetCurveType(BCGPChartFormatSeries::CCT_LINE);
	m_arSeries[1] = pVirtualSeries;
	pVirtualSeries->m_strSeriesName = "comp";
	pVirtualSeries->SetSeriesLineColor(m_brLine2);
	pChart->AddSeries(pVirtualSeries);

	pVirtualSeries = new CVirtualSeries(pChart);
	pVirtualSeries->SetCurveType(BCGPChartFormatSeries::CCT_LINE);
	m_arSeries[2] = pVirtualSeries;
	pVirtualSeries->m_strSeriesName = "가상1";
	pVirtualSeries->SetSeriesLineColor(m_brLine3);
	pChart->AddSeries(pVirtualSeries);

	pVirtualSeries = new CVirtualSeries(pChart);
	pVirtualSeries->SetCurveType(BCGPChartFormatSeries::CCT_LINE);
	m_arSeries[3] = pVirtualSeries;
	pVirtualSeries->m_strSeriesName = "가상2";
	pVirtualSeries->SetSeriesLineColor(m_brLine4);
	pChart->AddSeries(pVirtualSeries);

	pVirtualSeries = new CVirtualSeries(pChart);
	pVirtualSeries->SetCurveType(BCGPChartFormatSeries::CCT_LINE);
	m_arSeries[4] = pVirtualSeries;
	pVirtualSeries->m_strSeriesName = "가중평균";
	pVirtualSeries->SetSeriesLineColor(m_brLine5);
	pChart->AddSeries(pVirtualSeries);


	pVirtualSeries = new CVirtualSeries(pChart);
	pVirtualSeries->SetCurveType(BCGPChartFormatSeries::CCT_LINE);
	m_arSeries[5] = pVirtualSeries;
	pVirtualSeries->SetSeriesLineWidth(0);
	pVirtualSeries->m_strSeriesName = "괴리율";
	pVirtualSeries->SetSeriesLineColor(m_brDeltaOutline);
	pVirtualSeries->EnableColorEachLine(TRUE, TRUE);
	pChart->AddSeries(pVirtualSeries);

	pVirtualSeries = new CVirtualSeries(pChart);
	pVirtualSeries->SetCurveType(BCGPChartFormatSeries::CCT_LINE);
	m_arSeries[6] = pVirtualSeries;
	pVirtualSeries->SetSeriesLineWidth(0);
	pVirtualSeries->m_strSeriesName = "괴리율2";
	pVirtualSeries->SetSeriesLineColor(m_brDeltaOutline);
	pVirtualSeries->EnableColorEachLine(TRUE, TRUE);
	pChart->AddSeries(pVirtualSeries);

	
	pChart->ShowAxis(BCGP_CHART_X_PRIMARY_AXIS, TRUE, TRUE);
	pChart->ShowAxis(BCGP_CHART_Y_PRIMARY_AXIS, TRUE, TRUE);
	pChart->ShowAxis(BCGP_CHART_Y_SECONDARY_AXIS, TRUE, TRUE);
	//pChart->ShowAxisGridLines(BCGP_CHART_Y_PRIMARY_AXIS, FALSE);
	//pChart->ShowAxisGridLines(BCGP_CHART_Y_SECONDARY_AXIS, FALSE);
	pChart->SetLegendPosition(BCGPChartLayout::LP_TOP);

	pYAxis->ShowMajorGridLines(FALSE);
	pYSecondAxis->ShowMajorGridLines(TRUE);


	m_pAxisY1 = (CBCGPChartAxisY*)pYAxis->Split(60, 6, NULL, FALSE);
	//m_pAxisY1->SetFixedDisplayRange(-5, 5, 1);
	m_pAxisY1->m_strAxisName = _T("괴리율");
	m_pAxisY1->m_bDisplayAxisName = TRUE;
	m_pAxisY1->EnableZoom(FALSE);
	m_pAxisY1->m_crossType = CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE;
	m_pAxisY1->m_majorTickMarkType = CBCGPChartAxis::TMT_INSIDE;

	m_pAxisY2 = (CBCGPChartAxisY*)m_pAxisY1->Split(60, 6, NULL, FALSE);
	//m_pAxisY1->SetFixedDisplayRange(-5, 5, 1);
	m_pAxisY2->m_strAxisName = _T("괴리율2");
	m_pAxisY2->m_bDisplayAxisName = TRUE;
	m_pAxisY2->EnableZoom(FALSE);
	m_pAxisY2->m_crossType = CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE;
	m_pAxisY2->m_majorTickMarkType = CBCGPChartAxis::TMT_INSIDE;

	// 시리즈와 축 연결
	m_arSeries[0]->SetRelatedAxis(pYAxis, CBCGPChartSeries::AI_Y);
	m_arSeries[1]->SetRelatedAxis(pYSecondAxis, CBCGPChartSeries::AI_Y);
	m_arSeries[2]->SetRelatedAxis(pYSecondAxis, CBCGPChartSeries::AI_Y);
	m_arSeries[3]->SetRelatedAxis(pYSecondAxis, CBCGPChartSeries::AI_Y);
	m_arSeries[4]->SetRelatedAxis(pYSecondAxis, CBCGPChartSeries::AI_Y);

	m_arSeries[5]->SetRelatedAxis(m_pAxisY1, CBCGPChartSeries::AI_Y);
	m_arSeries[6]->SetRelatedAxis(m_pAxisY2, CBCGPChartSeries::AI_Y);


	m_arSeries[0]->SetRelatedAxis(pXAxis, CBCGPChartSeries::AI_X);
	m_arSeries[1]->SetRelatedAxis(pXAxis, CBCGPChartSeries::AI_X);
	m_arSeries[2]->SetRelatedAxis(pXAxis, CBCGPChartSeries::AI_X);
	m_arSeries[3]->SetRelatedAxis(pXAxis, CBCGPChartSeries::AI_X);
	m_arSeries[4]->SetRelatedAxis(pXAxis, CBCGPChartSeries::AI_X);

	m_arSeries[5]->SetRelatedAxis(pXAxis, CBCGPChartSeries::AI_X);
	m_arSeries[6]->SetRelatedAxis(pXAxis, CBCGPChartSeries::AI_X);

	// 축 세부 설정
	pXAxis->m_bFormatAsDate = TRUE;
	pXAxis->m_strDataFormat = _T("%F \n %H:%M:%S");
	pXAxis->ShowScrollBar(TRUE);

	// 축과 갭 설정
	// set offset of last displayed value in auto-scroll mode (in pixels)
	pXAxis->SetRightOffsetInPixels(133);

	//pXAxis->SetFixedIntervalWidth(33, 2);

	// X Axis should be always at the diagram bottom along with its labels
	pXAxis->m_crossType = CBCGPChartAxis::CT_FIXED_DEFAULT_POS;
	pXAxis->m_axisLabelType = CBCGPChartAxis::ALT_LOW;

	// 축 포맷 설정
	pYAxis->m_strDataFormat = _T("%.2f");
	pYSecondAxis->m_strDataFormat = _T("%.2f");
	
	pChart->EnableResizeAxes(TRUE);

	SetTimer(REDRAW_TIMER, 10, NULL);
	SetTimer(REQUEST_TIMER, 700, NULL);

	SetDefaultLineWidth();

	_ChartData.Parent(this);
	mainApp.CallbackMgr()->SubscribeChartCallback((long)this, std::bind(&SmMultiSpreadChart::OnChartEvent, this, _1, _2));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmMultiSpreadChart::LoadChartData()
{
	for (int i = 0; i < MAX_SERIES; i++) {
		m_arSeries[i]->Clear();
	}

	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	
	pChart->RemoveAllChartObjects();
	CString strMain;
	pChart->SetLegendPosition(BCGPChartLayout::LP_TOP);
	CBCGPChartAxis* pAxisX = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	CBCGPChartAxis* pAxisY = pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);
	CBCGPChartAxis* pSecondAxisY = pChart->GetChartAxis(BCGP_CHART_Y_SECONDARY_AXIS);

	const std::vector<hmdf::DateTime>& index = _ChartData.GetDataFrame().get_index();
	const std::vector<double>& main = _ChartData.GetDataFrame().get_column<double>("main");
	const std::vector<double>& comp = _ChartData.GetDataFrame().get_column<double>("comp");
	const std::vector<double>& nk = _ChartData.GetDataFrame().get_column<double>("nk");
	const std::vector<double>& hs = _ChartData.GetDataFrame().get_column<double>("hs");
	const std::vector<double>& vir1 = _ChartData.GetDataFrame().get_column<double>("vir1");
	const std::vector<double>& vir2 = _ChartData.GetDataFrame().get_column<double>("vir2");
	const std::vector<double>& vir3 = _ChartData.GetDataFrame().get_column<double>("vir3");
	const std::vector<double>& price_delta = _ChartData.GetDataFrame().get_column<double>("price_delta");
	const std::vector<double>& price_delta2 = _ChartData.GetDataFrame().get_column<double>("price_delta2");
	const std::vector<std::string> date_time_str = _ChartData.GetDataFrame().get_column<std::string>("date_time_str");
	m_arSeries[0]->RemoveAllDataPoints();
	m_arSeries[1]->RemoveAllDataPoints();
	m_arSeries[2]->RemoveAllDataPoints();
	m_arSeries[2]->m_strSeriesName = "가상 코스피200A";
	m_arSeries[3]->RemoveAllDataPoints();
	m_arSeries[3]->m_strSeriesName = "가상 코스피200B";
	m_arSeries[4]->RemoveAllDataPoints();
	m_arSeries[4]->m_strSeriesName = "가중평균";



	m_arSeries[5]->RemoveAllDataPoints();
	m_arSeries[5]->m_strSeriesName = "괴리율";
	m_arSeries[5]->EnableColorEachLine(TRUE, TRUE);

	m_arSeries[6]->RemoveAllDataPoints();
	m_arSeries[6]->m_strSeriesName = "괴리율2";
	m_arSeries[6]->EnableColorEachLine(TRUE, TRUE);

	double main_divedend = 1, comp_divedend = 1;
	/*
	for (size_t i = 0; i < index.size(); i++) {
		auto symbol = mainApp.SymMgr()->FindSymbol(_ChartData.MainSymbolCode());
		if (symbol) {
			main_divedend = pow(10, symbol->Decimal());
			m_arSeries[0]->AddDataPoint(main[i] / main_divedend, index[i]);
			m_arSeries[0]->m_strSeriesName = symbol->SymbolNameKr().c_str();
		}
		symbol = mainApp.SymMgr()->FindSymbol(_ChartData.CompSymbolCode());
		if (symbol) {
			comp_divedend = pow(10, symbol->Decimal());
			m_arSeries[1]->m_strSeriesName = symbol->SymbolNameKr().c_str();
			m_arSeries[1]->AddDataPoint(comp[i] / comp_divedend, index[i]);
			m_arSeries[2]->AddDataPoint(vir1[i] / comp_divedend, index[i]);
			m_arSeries[3]->AddDataPoint(vir2[i] / comp_divedend, index[i]);
			m_arSeries[4]->AddDataPoint(vir3[i] / comp_divedend, index[i]);
			m_arSeries[5]->AddDataPoint(price_delta[i] / comp_divedend, index[i]);
			m_arSeries[6]->AddDataPoint(price_delta2[i] / comp_divedend, index[i]);
			

			if (index[i].GetHour() == 9 && index[i].GetMinute() == 1) {
				strMain.Format("O:%.2f", comp[i] / comp_divedend);
				CBCGPChartTextObject* open_text = new CBCGPChartTextObject(pChart, strMain, index[i], comp[i] / comp_divedend, m_brFill2, m_brFillText2, m_brLine2);

				open_text->m_format.m_textFormat = CBCGPTextFormat(
					_T("Arial"), 15., FW_REGULAR, CBCGPTextFormat::BCGP_FONT_STYLE_NORMAL);

				open_text->SetRelatedAxes(pAxisX, pSecondAxisY);
				pChart->AddChartObject(open_text);
				_OpenTextVec.push_back(open_text);
			}

			if (index[i].GetMonth() == 6 && index[i].GetDay() == 6 && index[i].GetHour() == 15 && index[i].GetMinute() == 45) {
				strMain.Format("C:%.2f", comp[i] / comp_divedend);
				CBCGPChartTextObject* close_text = new CBCGPChartTextObject(pChart, strMain, index[i], comp[i] / comp_divedend, m_brFill2, m_brFillText2, m_brLine2);

				close_text->m_format.m_textFormat = CBCGPTextFormat(
					_T("Arial"), 15., FW_REGULAR, CBCGPTextFormat::BCGP_FONT_STYLE_NORMAL);

				close_text->SetRelatedAxes(pAxisX, pSecondAxisY);
				pChart->AddChartObject(close_text);
				_CloseTextVec.push_back(close_text);

				strMain.Format("C:%.2f", main[i] / main_divedend);
				close_text = new CBCGPChartTextObject(pChart, strMain, index[i], main[i] / main_divedend, m_brFill1, m_brFillText1, m_brLine1);

				close_text->m_format.m_textFormat = CBCGPTextFormat(
					_T("Arial"), 15., FW_REGULAR, CBCGPTextFormat::BCGP_FONT_STYLE_NORMAL);

				close_text->SetRelatedAxes(pAxisX, pAxisY);
				pChart->AddChartObject(close_text);
				_CloseTextVec.push_back(close_text);
			}
		}
		
	}
	
	// (0 - series index, 1 - data point index, LightGreen - data point color).

	
	pChart->RecalcMinMaxValues();


	COleDateTimeSpan dtInterval(0, 0, 200, 0);
	COleDateTimeSpan dtInterval2(0, 0, 300, 0);


	strMain.Format("NQ:%.2f", main.back() / main_divedend);
	m_pMainLabel = new CBCGPChartTextObject(pChart, strMain, index.back(), main.back() / main_divedend, m_brFill1, m_brFillText1, m_brLine1, 60);

	m_pMainLabel->m_format.m_textFormat = CBCGPTextFormat(
		_T("Arial"), 15., FW_REGULAR, CBCGPTextFormat::BCGP_FONT_STYLE_NORMAL);

	m_pMainLabel->SetRelatedAxes(pAxisX, pAxisY);
	pChart->AddChartObject(m_pMainLabel);

	CBCGPChartObject::CoordinateMode mode = m_pMainLabel->GetCoordinateMode();

	CBCGPRect rcText = m_pMainLabel->GetCoordinates();

	strMain.Format("K:%.2f", comp.back() / comp_divedend);
	m_pCompLabel = new CBCGPChartTextObject(pChart, strMain, index.back(), comp.back() / comp_divedend, m_brFill2, m_brFillText2, m_brLine2, 60);


	m_pCompLabel->m_format.m_textFormat = CBCGPTextFormat(
		_T("Arial"), 15., FW_REGULAR, CBCGPTextFormat::BCGP_FONT_STYLE_NORMAL);

	m_pCompLabel->SetRelatedAxes(pAxisX, pSecondAxisY);

	pChart->AddChartObject(m_pCompLabel);

	strMain.Format("%.2f", vir1.back() / comp_divedend);
	m_pVirLabel1 = new CBCGPChartTextObject(pChart, strMain, index.back(), vir1.back() / comp_divedend, m_brFill3, m_brFillText3, m_brLine3, 60);


	m_pVirLabel1->m_format.m_textFormat = CBCGPTextFormat(
		_T("Arial"), 15., FW_REGULAR, CBCGPTextFormat::BCGP_FONT_STYLE_NORMAL);

	m_pVirLabel1->SetRelatedAxes(pAxisX, pSecondAxisY);


	pChart->AddChartObject(m_pVirLabel1);


	strMain.Format("%.2f", vir2.back() / comp_divedend);
	m_pVirLabel2 = new CBCGPChartTextObject(pChart, strMain, index.back(), vir2.back() / comp_divedend, m_brFill4, m_brFillText4, m_brLine4, 60);


	m_pVirLabel2->m_format.m_textFormat = CBCGPTextFormat(
		_T("Arial"), 15., FW_REGULAR, CBCGPTextFormat::BCGP_FONT_STYLE_NORMAL);

	m_pVirLabel2->SetRelatedAxes(pAxisX, pSecondAxisY);


	pChart->AddChartObject(m_pVirLabel2);

	strMain.Format("%.2f", vir3.back() / comp_divedend);
	m_pVirLabel3 = new CBCGPChartTextObject(pChart, strMain, index.back(), vir3.back() / comp_divedend, m_brFill5, m_brFillText5, m_brLine4, 60);


	m_pVirLabel3->m_format.m_textFormat = CBCGPTextFormat(
		_T("Arial"), 15., FW_REGULAR, CBCGPTextFormat::BCGP_FONT_STYLE_NORMAL);

	m_pVirLabel3->SetRelatedAxes(pAxisX, pSecondAxisY);


	pChart->AddChartObject(m_pVirLabel3);

	strMain.Format("%.2f", price_delta.back() / comp_divedend);
	m_pDeltaLabel = new CBCGPChartTextObject(pChart, strMain, index.back(), price_delta.back() / comp_divedend, m_brDeltaText, m_brDeltaFill, m_brDeltaOutline);


	m_pDeltaLabel->m_format.m_textFormat = CBCGPTextFormat(
		_T("Arial"), 15., FW_REGULAR, CBCGPTextFormat::BCGP_FONT_STYLE_NORMAL);

	m_pDeltaLabel->SetRelatedAxes(pAxisX, m_pAxisY1);

	pChart->AddChartObject(m_pDeltaLabel);

	strMain.Format("%.2f", price_delta2.back() / comp_divedend);
	m_pDeltaLabel2 = new CBCGPChartTextObject(pChart, strMain, index.back(), price_delta2.back() / comp_divedend, m_brDeltaText, m_brDeltaFill, m_brDeltaOutline);


	m_pDeltaLabel2->m_format.m_textFormat = CBCGPTextFormat(
		_T("Arial"), 15., FW_REGULAR, CBCGPTextFormat::BCGP_FONT_STYLE_NORMAL);

	m_pDeltaLabel2->SetRelatedAxes(pAxisX, m_pAxisY2);

	pChart->AddChartObject(m_pDeltaLabel2);


	// Horz Line
	m_pLine1 = pChart->AddChartLineObject(0.0, TRUE , m_brZeroLine, 1);
	m_pLine1->SetShadow();
	m_pLine1->SetRelatedAxes(pAxisX, m_pAxisY1);


	m_pLine2 = pChart->AddChartLineObject(0.0, TRUE , m_brZeroLine, 1);
	m_pLine2->SetShadow();
	m_pLine2->SetRelatedAxes(pAxisX, m_pAxisY2);




	//CString strFormat = _T("%s: %.2f\n%s: %.2f\n%s: %.2f\n%s: %.2f");
	CString strFormat = _T("%s: %.2f %s: %.2f %s: %.2f %s: %.2f");
	CString strAnn;
	CBCGPRect diagram_area = pChart->GetDiagramArea();
	double left = diagram_area.left + 70;
	double top = diagram_area.top + 15;
	strAnn.Format("%s: %.2f", m_arSeries[0]->m_strSeriesName, main.back() / main_divedend);
	m_pCloseLabel1 = new CBCGPChartTextObject(pChart, strAnn, left, top, m_brFill1, m_brFillText1, m_brLine1);
	m_pCloseLabel1->SetCoordinateMode(CBCGPChartObject::CM_PIXELS);
	m_pCloseLabel1->m_format.m_textFormat.Create(BCGPChartFormatLabel::m_strDefaultFontFamily, 14);
	m_pCloseLabel1->m_format.SetContentPadding(CBCGPSize(5., 5.));

	pChart->AddChartObject(m_pCloseLabel1);

	m_pCloseLabel1->SetForeground();


	top = top + 5 + 30;

	strAnn.Format("%s: %.2f", m_arSeries[1]->m_strSeriesName, comp.back() / comp_divedend);
	m_pCloseLabel2 = new CBCGPChartTextObject(pChart, strAnn, left, top, m_brFill2, m_brFillText2, m_brLine2);
	m_pCloseLabel2->SetCoordinateMode(CBCGPChartObject::CM_PIXELS);
	m_pCloseLabel2->m_format.m_textFormat.Create(BCGPChartFormatLabel::m_strDefaultFontFamily, 14);
	m_pCloseLabel2->m_format.SetContentPadding(CBCGPSize(5., 5.));
	

	pChart->AddChartObject(m_pCloseLabel2);

	m_pCloseLabel2->SetForeground();

	top = top + 5 + 30;
	strAnn.Format("%s: %.2f", m_arSeries[2]->m_strSeriesName, vir1.back() / comp_divedend);
	m_pCloseLabel3 = new CBCGPChartTextObject(pChart, strAnn, left, top, m_brFill3, m_brFillText3, m_brLine3);
	m_pCloseLabel3->SetCoordinateMode(CBCGPChartObject::CM_PIXELS);
	m_pCloseLabel3->m_format.m_textFormat.Create(BCGPChartFormatLabel::m_strDefaultFontFamily, 14);
	m_pCloseLabel3->m_format.SetContentPadding(CBCGPSize(5., 5.));
	

	pChart->AddChartObject(m_pCloseLabel3);

	m_pCloseLabel3->SetForeground();



	top = top + 5 + 30;
	strAnn.Format("%s: %.2f", m_arSeries[3]->m_strSeriesName, vir2.back() / comp_divedend);
	m_pCloseLabel4 = new CBCGPChartTextObject(pChart, strAnn, left, top, m_brFill4, m_brFillText4, m_brLine4);
	m_pCloseLabel4->SetCoordinateMode(CBCGPChartObject::CM_PIXELS);
	m_pCloseLabel4->m_format.m_textFormat.Create(BCGPChartFormatLabel::m_strDefaultFontFamily, 14);
	m_pCloseLabel4->m_format.SetContentPadding(CBCGPSize(5., 5.));


	pChart->AddChartObject(m_pCloseLabel4);

	m_pCloseLabel4->SetForeground();


	top = top + 5 + 30;
	strAnn.Format("%s: %.2f", m_arSeries[4]->m_strSeriesName, vir3.back() / comp_divedend);
	m_pCloseLabel5 = new CBCGPChartTextObject(pChart, strAnn, left, top, m_brFill5, m_brFillText5, m_brLine5);
	m_pCloseLabel5->SetCoordinateMode(CBCGPChartObject::CM_PIXELS);
	m_pCloseLabel5->m_format.m_textFormat.Create(BCGPChartFormatLabel::m_strDefaultFontFamily, 14);
	m_pCloseLabel5->m_format.SetContentPadding(CBCGPSize(5., 5.));


	pChart->AddChartObject(m_pCloseLabel5);

	m_pCloseLabel5->SetForeground();

	


	pAxisX->SetScrollRange(m_arSeries[0]->GetFirstDate(), m_arSeries[0]->GetLastDate());

	//pAxisX->SetFixedMaximumDisplayValue(index.size() + pAxisX->GetRightOffsetAsNumberOfValues() - 1);

	pChart->SetZoomScrollConfig(BCGPChartMouseConfig::ZSO_WHEEL_PAN);

	SetDefaultLineWidth();
	*/

	pChart->SetDirty();
	pChart->Redraw();
}


LRESULT SmMultiSpreadChart::OnCompDataAdded(WPARAM wParam, LPARAM lParam)
{
	//AfxMessageBox("comp data added!");

	_NeedAdd = true;
	return 1;
}

LRESULT SmMultiSpreadChart::OnCompDataUpdated(WPARAM wParam, LPARAM lParam)
{
	_NeedUpdate = true;
	return 1;
}

void SmMultiSpreadChart::SetDefaultLineWidth()
{
	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	if (pChart == NULL)
	{
		return;
	}

	ASSERT_VALID(pChart);

	for (int i = 0; i < pChart->GetSeriesCount(); i++)
	{
		CBCGPChartSeries* pSeries = pChart->GetSeries(i);
		ASSERT_VALID(pSeries);

		BCGPChartFormatSeries style = pSeries->GetSeriesFormat();
		style.SetSeriesLineWidth(2);

		pSeries->SetSeriesFormat(style);
	}
}

void SmMultiSpreadChart::ProcessAddData()
{
	const std::vector<hmdf::DateTime>& index = _ChartData.GetDataFrame().get_index();
	const std::vector<double>& main = _ChartData.GetDataFrame().get_column<double>("main");
	const std::vector<double>& comp = _ChartData.GetDataFrame().get_column<double>("comp");
	const std::vector<double>& vir1 = _ChartData.GetDataFrame().get_column<double>("vir1");
	const std::vector<double>& vir2 = _ChartData.GetDataFrame().get_column<double>("vir2");
	const std::vector<double>& vir3 = _ChartData.GetDataFrame().get_column<double>("vir3");
	const std::vector<double>& price_delta = _ChartData.GetDataFrame().get_column<double>("price_delta");
	const std::vector<double>& price_delta2 = _ChartData.GetDataFrame().get_column<double>("price_delta2");
	auto symbol = mainApp.SymMgr()->FindSymbol(_ChartData.KospiSymbolCode());
	double main_divedend = 1, comp_divedend = 1;
	/*
	if (symbol) {
		main_divedend = pow(10, symbol->Decimal());
		m_arSeries[0]->AddDataPoint(main.back() / main_divedend, index.back());

		CString strMain;
		strMain.Format("%.2f", main.back() / main_divedend);
		m_pMainLabel->m_strText = strMain;
		m_pMainLabel->SetCoordinates(index.back().m_dt, main.back() / main_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);

	}
	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	symbol = mainApp.SymMgr()->FindSymbol(_ChartData.CompSymbolCode());
	if (symbol) {
		comp_divedend = pow(10, symbol->Decimal());
		m_arSeries[1]->AddDataPoint(comp.back() / comp_divedend, index.back());
		m_arSeries[2]->AddDataPoint(vir1.back() / comp_divedend, index.back());
		m_arSeries[3]->AddDataPoint(vir2.back() / comp_divedend, index.back());
		m_arSeries[4]->AddDataPoint(vir3.back() / comp_divedend, index.back());
		m_arSeries[5]->AddDataPoint(price_delta.back() / comp_divedend, index.back());
		m_arSeries[6]->AddDataPoint(price_delta2.back() / comp_divedend, index.back());
		CString strMain;
		strMain.Format("%.2f", comp.back() / comp_divedend);
		m_pCompLabel->m_strText = strMain;
		m_pCompLabel->SetCoordinates(index.back().m_dt, comp.back() / comp_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);
		
		strMain.Format("%.2f", vir1.back() / comp_divedend);
		m_pVirLabel1->m_strText = strMain;
		m_pVirLabel1->SetCoordinates(index.back().m_dt, vir1.back() / comp_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);

		strMain.Format("%.2f", vir2.back() / comp_divedend);
		m_pVirLabel2->m_strText = strMain;
		m_pVirLabel2->SetCoordinates(index.back().m_dt, vir2.back() / comp_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);


		strMain.Format("%.2f", vir3.back() / comp_divedend);
		m_pVirLabel3->m_strText = strMain;
		m_pVirLabel3->SetCoordinates(index.back().m_dt, vir3.back() / comp_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);



		strMain.Format("%.2f", price_delta.back() / comp_divedend);
		m_pDeltaLabel->m_strText = strMain;
		m_pDeltaLabel->SetCoordinates(index.back().m_dt, price_delta.back() / comp_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);

		strMain.Format("%.2f", price_delta2.back() / comp_divedend);
		m_pDeltaLabel2->m_strText = strMain;
		m_pDeltaLabel2->SetCoordinates(index.back().m_dt, price_delta2.back() / comp_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);

	}

	CString strAnn;
	
	strAnn.Format("%s: %.2f", m_arSeries[0]->m_strSeriesName, main.back() / main_divedend);
	m_pCloseLabel1->m_strText = strAnn;

	strAnn.Format("%s: %.2f", m_arSeries[1]->m_strSeriesName, comp.back() / comp_divedend);
	m_pCloseLabel2->m_strText = strAnn;

	strAnn.Format("%s: %.2f", m_arSeries[2]->m_strSeriesName, vir1.back() / comp_divedend);
	m_pCloseLabel3->m_strText = strAnn;

	strAnn.Format("%s: %.2f", m_arSeries[3]->m_strSeriesName, vir2.back() / comp_divedend);
	m_pCloseLabel4->m_strText = strAnn;

	strAnn.Format("%s: %.2f", m_arSeries[4]->m_strSeriesName, vir3.back() / comp_divedend);
	m_pCloseLabel5->m_strText = strAnn;
	
	pChart->RecalcMinMaxValues();
	*/
}

void SmMultiSpreadChart::ProcessUpdateData()
{
	const std::vector<hmdf::DateTime>& index = _ChartData.GetDataFrame().get_index();
	const std::vector<double>& main = _ChartData.GetDataFrame().get_column<double>("main");
	const std::vector<double>& comp = _ChartData.GetDataFrame().get_column<double>("comp");
	const std::vector<double>& vir1 = _ChartData.GetDataFrame().get_column<double>("vir1");
	const std::vector<double>& vir2 = _ChartData.GetDataFrame().get_column<double>("vir2");
	const std::vector<double>& vir3 = _ChartData.GetDataFrame().get_column<double>("vir3");
	const std::vector<double>& price_delta = _ChartData.GetDataFrame().get_column<double>("price_delta");
	const std::vector<double>& price_delta2 = _ChartData.GetDataFrame().get_column<double>("price_delta2");
	auto symbol = mainApp.SymMgr()->FindSymbol(_ChartData.KospiSymbolCode());
	double main_divedend = 1, comp_divedend = 1;
	/*
	if (symbol) {
		main_divedend = pow(10, symbol->Decimal());
		m_arSeries[0]->SetDataPointValue(m_arSeries[0]->GetDataPointCount() - 1, main.back() / main_divedend);
		CString strMain;
		strMain.Format("%.2f", main.back() / main_divedend);
		m_pMainLabel->m_strText = strMain;

		m_pMainLabel->SetCoordinates(index.back(), main.back() / main_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);

	}
	symbol = mainApp.SymMgr()->FindSymbol(_ChartData.CompSymbolCode());
	if (symbol) {
		comp_divedend = pow(10, symbol->Decimal());
		m_arSeries[1]->SetDataPointValue(m_arSeries[1]->GetDataPointCount() - 1, comp.back() / comp_divedend);
		m_arSeries[2]->SetDataPointValue(m_arSeries[2]->GetDataPointCount() - 1, vir1.back() / comp_divedend);
		m_arSeries[3]->SetDataPointValue(m_arSeries[3]->GetDataPointCount() - 1, vir2.back() / comp_divedend);
		m_arSeries[4]->SetDataPointValue(m_arSeries[4]->GetDataPointCount() - 1, vir3.back() / comp_divedend);
		m_arSeries[5]->SetDataPointValue(m_arSeries[5]->GetDataPointCount() - 1, price_delta.back() / comp_divedend);
		m_arSeries[6]->SetDataPointValue(m_arSeries[6]->GetDataPointCount() - 1, price_delta2.back() / comp_divedend);
		CString strMain;
		strMain.Format("%.2f", comp.back() / comp_divedend);
		m_pCompLabel->m_strText = strMain;
		m_pCompLabel->SetCoordinates(index.back(), comp.back() / comp_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);

		strMain.Format("%.2f", vir1.back() / comp_divedend);
		m_pVirLabel1->m_strText = strMain;
		m_pVirLabel1->SetCoordinates(index.back(), vir1.back() / comp_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);

		strMain.Format("%.2f", vir2.back() / comp_divedend);
		m_pVirLabel2->m_strText = strMain;
		m_pVirLabel2->SetCoordinates(index.back(), vir2.back() / comp_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);

		strMain.Format("%.2f", vir3.back() / comp_divedend);
		m_pVirLabel3->m_strText = strMain;
		m_pVirLabel3->SetCoordinates(index.back(), vir3.back() / comp_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);

		strMain.Format("%.2f", price_delta.back() / comp_divedend);
		m_pDeltaLabel->m_strText = strMain;
		m_pDeltaLabel->SetCoordinates(index.back(), price_delta.back() / comp_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);

		strMain.Format("%.2f", price_delta2.back() / comp_divedend);
		m_pDeltaLabel2->m_strText = strMain;
		m_pDeltaLabel2->SetCoordinates(index.back(), price_delta2.back() / comp_divedend, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::CM_CHART_VALUES);

	}

	CString strAnn;
	strAnn.Format("%s: %.2f", m_arSeries[0]->m_strSeriesName, main.back() / main_divedend);
	m_pCloseLabel1->m_strText = strAnn;

	strAnn.Format("%s: %.2f", m_arSeries[1]->m_strSeriesName, comp.back() / comp_divedend);
	m_pCloseLabel2->m_strText = strAnn;

	strAnn.Format("%s: %.2f", m_arSeries[2]->m_strSeriesName, vir1.back() / comp_divedend);
	m_pCloseLabel3->m_strText = strAnn;

	strAnn.Format("%s: %.2f", m_arSeries[3]->m_strSeriesName, vir2.back() / comp_divedend);
	m_pCloseLabel4->m_strText = strAnn;

	strAnn.Format("%s: %.2f", m_arSeries[4]->m_strSeriesName, vir3.back() / comp_divedend);
	m_pCloseLabel5->m_strText = strAnn;

	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	pChart->RecalcMinMaxValues();
	*/
}

void SmMultiSpreadChart::InitMulCombo()
{
	_ComboMul.AddString("1");
	_ComboMul.AddString("2");
	_ComboMul.AddString("3");
	_ComboMul.AddString("4");
	_ComboMul.AddString("5");
	_ComboMul.AddString("6");
	_ComboMul.AddString("7");
	_ComboMul.AddString("8");
	_ComboMul.AddString("9");
	_ComboMul.AddString("10");
	_ComboMul.AddString("11");
	_ComboMul.SetCurSel(0);
}

void SmMultiSpreadChart::OnBnClickedBtnSet()
{
	_SpreadSet = std::make_shared<SmSetSpread>();
	_SpreadSet->Create(IDD_SPRD_CHART_SET, this);
	_SpreadSet->ShowWindow(SW_SHOW);
}


void SmMultiSpreadChart::OnBnClickedBtnFind()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	_SymbolTableDlg->MultiSpreadChart = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

void SmMultiSpreadChart::OnCbnSelchangeComboStyle()
{
	//m_nLineType = _ComboStyle.GetCurSel();
	//UpdateChart();
}

void SmMultiSpreadChart::OnClear()
{
	
}

LRESULT SmMultiSpreadChart::OnChartAxisZoomed(WPARAM wp, LPARAM lp)
{
	
	return 0;
}

void SmMultiSpreadChart::OnTimer(UINT_PTR nIDEvent)
{
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
			CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
			pChart->SetDirty();
			pChart->Redraw();
			_NeedRedraw = false;
		}
	}
}

LRESULT SmMultiSpreadChart::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = (int)wParam;
	std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbolById(symbol_id);
	int found_index = -1;
	for (auto it = _RowToMainSymbolMap.begin(); it != _RowToMainSymbolMap.end(); ++it) {
		if (it->second->Id() == symbol->Id()) {
			found_index = it->first;
			break;
		}
	}

	if (found_index < 0) {
		found_index = _ComboSymbolMain.AddString(symbol->SymbolCode().c_str());
		_RowToMainSymbolMap[found_index] = symbol;
	}

	_ComboSymbolMain.SetCurSel(found_index);

	if (_ChartData.NqSymbolCode().compare(symbol->SymbolCode()) == 0) return 1;

	_ChartData.KospiSymbolCode(symbol->SymbolCode());

	SetTimer(REQUEST_TIMER, 700, NULL);

	return 1;
}

void SmMultiSpreadChart::UpdateChartColorTheme(int nTheme, BOOL bIsDarkTheme)
{
	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	if (pChart == NULL) { return; }

	if (nTheme < 0)
	{
		if (m_bIsDarkBackground)
		{
			m_bIsDarkBackground = FALSE;
			OnChangeBackgroundDarkness();
		}

		pChart->SetColors(GetCustomTheme(nTheme), bIsDarkTheme);
		m_bIsTexturedTheme = nTheme == -1;
		m_bIsFlatTheme = FALSE;
	}
	else
	{
		CBCGPChartTheme::ChartTheme theme = (CBCGPChartTheme::ChartTheme)nTheme;

		BOOL bIsDarkBackground = FALSE;

		switch (theme)
		{
		case CBCGPChartTheme::CT_BLACK_AND_GOLD:
		case CBCGPChartTheme::CT_BLACK_AND_RED:
		case CBCGPChartTheme::CT_BLACK_AND_GREEN:
		case CBCGPChartTheme::CT_BLACK_AND_BLUE:
			bIsDarkBackground = TRUE;
			break;
		}

		if (m_bIsDarkBackground != bIsDarkBackground)
		{
			m_bIsDarkBackground = bIsDarkBackground;
			OnChangeBackgroundDarkness();
		}

		pChart->SetColors((CBCGPChartTheme::ChartTheme)nTheme, bIsDarkTheme);

		m_bIsTexturedTheme = FALSE;

		m_bIsFlatTheme = theme >= CBCGPChartTheme::CT_FLAT_2014_1 && theme <= CBCGPChartTheme::CT_ICE;
	}
}

void SmMultiSpreadChart::SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& window_id, const int& series_index)
{
	
}

void SmMultiSpreadChart::SetChartDataForMain(std::shared_ptr<DarkHorse::SmChartData> chart_data)
{
	
}

void SmMultiSpreadChart::SetChartDataForSecond(std::shared_ptr<DarkHorse::SmChartData> chart_data)
{
	
}

void SmMultiSpreadChart::SetChartDataForOthers(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& series_index)
{

}

void SmMultiSpreadChart::OnChartEvent(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action)
{
	//if (!_MainChartData || !chart_data || _MainChartData->Id() != chart_data->Id()) return;
	//bool needRedraw = false;
	//if (action == 1) _NeedUpdate = true;
	//if (action == 2) _NeedAdd = true;
}

static void InitSeriesColors(BCGPSeriesColors& sc, const CBCGPColor& color, UINT nTextureResID = 0)
{
	CBCGPColor colorGradient = color;
	colorGradient.MakeLighter(.5);

	if (nTextureResID != 0)
	{
		CBCGPColor colorAlt = color;
		colorAlt.MakeLighter(.2);

		sc.m_brElementFillColor.SetTextureImage(CBCGPImage(nTextureResID), colorAlt);
	}
	else
	{
		sc.m_brElementFillColor.SetColors(color, colorGradient, (CBCGPBrush::BCGP_GRADIENT_TYPE)-1);
	}

	CBCGPColor colorOutline = color;
	colorOutline.MakeDarker(.1);

	sc.m_brElementLineColor.SetColor(colorOutline);
	sc.m_brMarkerFillColor.SetColors(color, colorGradient, CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT);
	sc.m_brMarkerFillColor.MakeLighter(.15);

	sc.m_brMarkerLineColor.SetColor(colorOutline);
	sc.m_brDataLabelLineColor.SetColor(colorOutline);

	sc.m_brDataLabelFillColor.SetColor(color);
	sc.m_brDataLabelFillColor.MakePale();

	CBCGPColor colorText = color;
	colorText.MakeDarker(.2);

	sc.m_brDataLabelTextColor.SetColor(colorText);
}


void SmMultiSpreadChart::InitTheme()
{
	// Setup custom color theme:
	CBCGPChartTheme::InitChartColors(m_CustomColorTheme,
		CBCGPColor::White,	// Fill color
		CBCGPColor::Chocolate,	// Outline color
		CBCGPColor::Maroon,		// Text color
		CBCGPColor(),
		CBCGPColor::SeaShell, .08);

	InitSeriesColors(m_CustomColorTheme.m_seriesColors[0], CBCGPColor::Crimson);
	InitSeriesColors(m_CustomColorTheme.m_seriesColors[1], CBCGPColor::BlueViolet);
	InitSeriesColors(m_CustomColorTheme.m_seriesColors[2], CBCGPColor::CornflowerBlue);
	InitSeriesColors(m_CustomColorTheme.m_seriesColors[3], CBCGPColor::DarkGreen);
	InitSeriesColors(m_CustomColorTheme.m_seriesColors[4], CBCGPColor::Coral);

	// Create custom texture theme:
	CBCGPChartTheme::InitChartColors(m_CustomTexturedTheme,
		CBCGPColor::White,	// Fill color
		CBCGPColor::CadetBlue,	// Outline color
		CBCGPColor::CadetBlue,		// Text color
		CBCGPColor(),
		CBCGPColor::SeaShell, .08);

	CBCGPImage imageBackground(IDR_BACKGROUND);

	m_CustomTexturedTheme.m_brChartFillColor.SetTextureImage(imageBackground, CBCGPColor::AliceBlue, TRUE /* Watermark */);
	m_CustomTexturedTheme.m_brPlotFillColor.SetTextureImage(imageBackground, CBCGPColor::AliceBlue, TRUE /* Watermark */);
	m_CustomTexturedTheme.m_brLegendFillColor.SetTextureImage(imageBackground, CBCGPColor::AliceBlue, TRUE /* Watermark */);
	m_CustomTexturedTheme.m_brLegendEntryFillColor.Empty();

	CBCGPImage imageWall(IDR_WALL);
	CBCGPChartTheme::PrepareWallBrushes(m_CustomTexturedTheme, CBCGPBrush(imageWall, CBCGPColor::LightGray, FALSE));

	InitSeriesColors(m_CustomTexturedTheme.m_seriesColors[0], CBCGPColor::LightSlateGray, IDR_TEXTURE1);
	InitSeriesColors(m_CustomTexturedTheme.m_seriesColors[1], CBCGPColor::LimeGreen, IDR_TEXTURE2);
	InitSeriesColors(m_CustomTexturedTheme.m_seriesColors[2], CBCGPColor::RosyBrown, IDR_TEXTURE3);
	InitSeriesColors(m_CustomTexturedTheme.m_seriesColors[3], CBCGPColor::CadetBlue, IDR_TEXTURE4);
	InitSeriesColors(m_CustomTexturedTheme.m_seriesColors[4], CBCGPColor::Goldenrod, IDR_TEXTURE5);


	_ComboTheme.AddString(_T("Default Theme"));
	_ComboTheme.AddString(_T("Pastel Theme"));
	_ComboTheme.AddString(_T("Spring Theme"));
	_ComboTheme.AddString(_T("Forest Green Theme"));
	_ComboTheme.AddString(_T("Sea Blue Theme"));
	_ComboTheme.AddString(_T("Golden Theme"));
	_ComboTheme.AddString(_T("Dark Rose Theme"));
	_ComboTheme.AddString(_T("Black and Gold Theme"));
	_ComboTheme.AddString(_T("Rainbow Theme"));
	_ComboTheme.AddString(_T("Gray Theme"));
	_ComboTheme.AddString(_T("Arctic Theme"));
	_ComboTheme.AddString(_T("Black and Red Theme"));
	_ComboTheme.AddString(_T("Plum Theme"));
	_ComboTheme.AddString(_T("Sunny Theme"));
	_ComboTheme.AddString(_T("Violet Theme"));
	_ComboTheme.AddString(_T("Flower Theme"));
	_ComboTheme.AddString(_T("Steel Theme"));
	_ComboTheme.AddString(_T("Gray and Green Theme"));
	_ComboTheme.AddString(_T("Olive Theme"));
	_ComboTheme.AddString(_T("Autumn Theme"));
	_ComboTheme.AddString(_T("Black and Green Theme"));
	_ComboTheme.AddString(_T("Black and Blue Theme"));
	_ComboTheme.AddString(_T("Flat 2014 1-st Theme"));
	_ComboTheme.AddString(_T("Flat 2014 2-nd Theme"));
	_ComboTheme.AddString(_T("Flat 2014 3-rd Theme"));
	_ComboTheme.AddString(_T("Flat 2014 4-th Theme"));
	_ComboTheme.AddString(_T("Flat 2015 1-st Theme"));
	_ComboTheme.AddString(_T("Flat 2015 2-nd Theme"));
	_ComboTheme.AddString(_T("Flat 2015 3-rd Theme"));
	_ComboTheme.AddString(_T("Flat 2015 4-th Theme"));
	_ComboTheme.AddString(_T("Flat 2015 5-th Theme"));
	_ComboTheme.AddString(_T("Flat 2016 1-st Theme"));
	_ComboTheme.AddString(_T("Flat 2016 2-nd Theme"));
	_ComboTheme.AddString(_T("Flat 2016 3-rd Theme"));
	_ComboTheme.AddString(_T("Flat 2016 4-th Theme"));
	_ComboTheme.AddString(_T("Flat 2016 5-th Theme"));
	_ComboTheme.AddString(_T("SPB Theme"));
	_ComboTheme.AddString(_T("Palace Theme"));
	_ComboTheme.AddString(_T("Ice Theme"));
	_ComboTheme.AddString(_T("Custom Theme (Color)"));
	_ComboTheme.AddString(_T("Custom Theme (Textures)"));

	_ComboTheme.SetCurSel(39);
}

void SmMultiSpreadChart::InitSymbolCombo()
{
	// 반드시 실시간 등록을 해줄것
	const std::map<int, std::shared_ptr<DarkHorse::SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	for (auto it = favorite_map.begin(); it != favorite_map.end(); ++it) {
		int index = _ComboSymbolMain.AddString(it->second->SymbolNameKr().c_str());
		_RowToMainSymbolMap[index] = it->second;
	}

	if (_RowToMainSymbolMap.size() > 0) {
		_ComboSymbolMain.SetCurSel(2);
		_ChartData.SetKospiSymbol( _RowToMainSymbolMap[2]->SymbolCode());
		_ChartData.NikkeiCode(_RowToMainSymbolMap[1]->SymbolCode());
		_ChartData.HangSengCode(_RowToMainSymbolMap[5]->SymbolCode());
		mainApp.SymMgr()->RegisterSymbolToServer(_RowToMainSymbolMap[2]->SymbolCode(), true);
		mainApp.SymMgr()->RegisterSymbolToServer(_RowToMainSymbolMap[1]->SymbolCode(), true);
		mainApp.SymMgr()->RegisterSymbolToServer(_RowToMainSymbolMap[5]->SymbolCode(), true);
	}

	for (auto it = favorite_map.rbegin(); it != favorite_map.rend(); ++it) {
		int index = _ComboSymbolComp.AddString(it->second->SymbolNameKr().c_str());
		_RowToCompSymbolMap[index] = it->second;
	}

	if (_RowToCompSymbolMap.size() > 0) {
		_ComboSymbolComp.SetCurSel(0);
		_ChartData.SetNqSymbol(_RowToCompSymbolMap.begin()->second->SymbolCode());
		mainApp.SymMgr()->RegisterSymbolToServer(_RowToCompSymbolMap[0]->SymbolCode(), true);
	}

	_ComboSymbolMain.SetDroppedWidth(300);
	_ComboSymbolComp.SetDroppedWidth(300);
}




void SmMultiSpreadChart::CreateChart()
{

}


void SmMultiSpreadChart::RequestChartData()
{
	if (!_ChartData.RequestChartData(_UniqueId)) {
		KillTimer(REQUEST_TIMER);
		_ChartData.MakeChartData();
		LoadChartData();
	}
}



void SmMultiSpreadChart::OnCbnSelchangeComboTheme()
{
	m_nColorTheme = _ComboTheme.GetCurSel();

	if (m_nColorTheme == _ComboTheme.GetCount() - 2)
	{
		m_nColorTheme = -2;	// Custom color theme
	}
	else if (m_nColorTheme == _ComboTheme.GetCount() - 1)
	{
		m_nColorTheme = -1;	// Custom textures theme
	}

	UpdateChartColorTheme(m_nColorTheme, m_bIsDarkTheme);
}



void SmMultiSpreadChart::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if (!_bDataReady) return;
}



void SmMultiSpreadChart::OnClose()
{
	_ChartData.RemoveChartRef();
	KillTimer(REDRAW_TIMER);
	mainApp.CallbackMgr()->UnsubscribeChartCallback((long)this);

	CBCGPDialog::OnClose();
}


void SmMultiSpreadChart::OnBnClickedCheckMain()
{
	if (_CheckMain.GetCheck() == BST_CHECKED) {
		m_arSeries[0]->m_bVisible = TRUE;
	}
	else {
		m_arSeries[0]->m_bVisible = FALSE;
	}

	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);
	pChart->SetDirty();
	pChart->Redraw();
}


void SmMultiSpreadChart::OnBnClickedCheckComp()
{
	if (_CheckComp.GetCheck() == BST_CHECKED) {
		m_arSeries[1]->m_bVisible = TRUE;
	}
	else {
		m_arSeries[1]->m_bVisible = FALSE;
	}

	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);
	pChart->SetDirty();
	pChart->Redraw();
}


void SmMultiSpreadChart::OnBnClickedCheckVirtual()
{
	if (_CheckVirtual.GetCheck() == BST_CHECKED) {
		m_arSeries[2]->m_bVisible = TRUE;
	}
	else {
		m_arSeries[2]->m_bVisible = FALSE;
	}

	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);
	pChart->SetDirty();
	pChart->Redraw();
}


void SmMultiSpreadChart::OnCbnSelchangeComboSymbolMain()
{
	int cur_sel = _ComboSymbolMain.GetCurSel();
	if (cur_sel < 0) return;

	auto found = _RowToMainSymbolMap.find(cur_sel);
	if (found == _RowToMainSymbolMap.end()) return;

	if (_ChartData.NqSymbolCode().compare(found->second->SymbolCode()) == 0) return;

	_ChartData.KospiSymbolCode(found->second->SymbolCode());

	SetTimer(REQUEST_TIMER, 700, NULL);
}


void SmMultiSpreadChart::OnCbnSelchangeComboMul()
{
	int cur_sel = _ComboMul.GetCurSel();
	if (cur_sel < 0) return ;
	_ChartData.Multiply(cur_sel + 1);

	_ChartData.MakeChartData();
	LoadChartData();
}


void SmMultiSpreadChart::OnBnClickedBtnZoomOut()
{
	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);


	CBCGPChartAxis* pAxisX = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	CBCGPChartAxis* pAxisY = pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);
	CBCGPChartAxis* pSecondAxisY = pChart->GetChartAxis(BCGP_CHART_Y_SECONDARY_AXIS);

	double point_x = m_arSeries[0]->GetDataPointValue(m_arSeries[0]->GetDataPointCount() - 1, CBCGPChartData::CI_X);
	double point_y = m_arSeries[0]->GetDataPointValue(m_arSeries[0]->GetDataPointCount() - 1, CBCGPChartData::CI_Y);
	//pAxisX->Zoom(-1, CBCGPPoint(point_x, point_y));
	pChart->Zoom(110);
	pChart->SetDirty();
	pChart->Redraw();
}


void SmMultiSpreadChart::OnBnClickedBtnZoomIn()
{
	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);


	CBCGPChartAxis* pAxisX = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	CBCGPChartAxis* pAxisY = pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);
	CBCGPChartAxis* pSecondAxisY = pChart->GetChartAxis(BCGP_CHART_Y_SECONDARY_AXIS);

	//pChart->Zoom(-1);
	double point_x = m_arSeries[0]->GetDataPointValue(m_arSeries[0]->GetDataPointCount() - 1, CBCGPChartData::CI_X);
	double point_y = m_arSeries[0]->GetDataPointValue(m_arSeries[0]->GetDataPointCount() - 1, CBCGPChartData::CI_Y);
	//pAxisX->Zoom(1, CBCGPPoint(point_x, point_y));
	pChart->Zoom(-110);
	pChart->SetDirty();
	pChart->Redraw();
}


void SmMultiSpreadChart::OnBnClickedBtnRecal()
{
	// TODO: Add your control notification handler code here
	if (!_EditNq.GetSafeHwnd()) return;

	CString nq, nk, hs, fa, fb;
	_EditNq.GetWindowText(nq);
	_EditNk.GetWindowText(nk);
	_EditHs.GetWindowText(hs);

	_EditAFactor.GetWindowText(fa);
	_EditBFactor.GetWindowText(fb);

	_ChartData.ProcessChartData();
	LoadChartData();
}


void SmMultiSpreadChart::OnEnChangeEditNq()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBCGPDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!_EditNq.GetSafeHwnd()) return;
}


void SmMultiSpreadChart::OnEnChangeEditNk()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBCGPDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!_EditNk.GetSafeHwnd()) return;
}


void SmMultiSpreadChart::OnEnChangeEditHs()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBCGPDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!_EditHs.GetSafeHwnd()) return;
}


void SmMultiSpreadChart::OnEnChangeEditAfactor()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBCGPDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!_EditAFactor.GetSafeHwnd()) return;
}


void SmMultiSpreadChart::OnEnChangeEditBfactor()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBCGPDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!_EditBFactor.GetSafeHwnd()) return;
}
