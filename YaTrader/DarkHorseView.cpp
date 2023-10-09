// DarkHorseView.cpp : implementation of the CDarkHorseView class
//

#include "stdafx.h"
#include "DarkHorse.h"

#include "DarkHorseDoc.h"
#include "DarkHorseView.h"

#include "MainFrm.h"
#include "Chart/SmChartData.h"
#include "Util/SmUtil.h"
#include <vector>
#include <BCGCBProInc.h>
#include "ChartDialog/SmChartStorage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace DarkHorse;

#define ID_NEXT_DATA_POINT	1

static const int RSI_PERIOD = 14;
static const CString RSI_INFO_FORMAT = _T("RSI(%d), Value: %.4f");
static const CString OHLC_INFO_FORMAT = _T("O: %.4f, H: %.4f, L: %.4f, C: %.4f");

CExternalStorage storage;

// CDarkHorseView

IMPLEMENT_DYNCREATE(CDarkHorseView, CBCGPFormView)

// CDarkHorseView construction/destruction

CBCGPChartValue CALLBACK ChartStockCallbackFunc(int nDataPointIndex, CBCGPChartStockSeries* pSeries)
{
	CBCGPChartStockData data = pSeries->GetStockDataAt(nDataPointIndex);
	if (data.IsEmpty())
	{
		return CBCGPChartValue();
	}
	return CBCGPChartValue((data.m_dblHigh + data.m_dblLow) / 2);
}

CDarkHorseView::CDarkHorseView()
	: CBCGPFormView(IDD_DARKHORSE_FORM)
{

	EnableVisualManagerStyle();
	EnableLayout();
	SetWhiteBackground(FALSE);
	m_nChartType = 0;
	m_nOverlayLine = 0;
	m_nOverlayBand = 2;
	//}}AFX_DATA_INIT

	m_pOverlaySeriesLine = NULL;
	m_pOverlaySeriesBand = NULL;

	m_nLineType = 1;
	m_bAutoScroll = FALSE;
	m_bDataTable = FALSE;
	//}}AFX_DATA_INIT

	m_dblUpTrendLimit = 0.5;
	m_pRSISeries = NULL;
	m_pRSIInfoObject = NULL;
	m_pOHLCInfo = NULL;

}

CDarkHorseView::~CDarkHorseView()
{
}

void CDarkHorseView::DoDataExchange(CDataExchange* pDX)
{
	CBCGPFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHART, m_wndChart);
	DDX_CBIndex(pDX, IDC_CHART_TYPE, m_nChartType);
	DDX_CBIndex(pDX, IDC_LINE_TYPE, m_nLineType);
	DDX_CBIndex(pDX, IDC_OVERLAY_LINE, m_nOverlayLine);
	DDX_CBIndex(pDX, IDC_OVERLAY_BAND, m_nOverlayBand);

	DDX_Control(pDX, IDC_BUTTON_START, m_wndStart);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_wndCancel);
	DDX_Check(pDX, IDC_CHECK_AUTO_SCROLL, m_bAutoScroll);
	DDX_Check(pDX, IDC_CHECK_DATA_LABEL, m_bDataTable);
}


BEGIN_MESSAGE_MAP(CDarkHorseView, CBCGPFormView)

	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CChartStockView)
	ON_CBN_SELENDOK(IDC_CHART_TYPE, OnUpdateChartType)
	ON_CBN_SELENDOK(IDC_LINE_TYPE, OnUpdateChart)
	ON_CBN_SELENDOK(IDC_OVERLAY_LINE, OnUpdateChart)
	ON_CBN_SELENDOK(IDC_OVERLAY_BAND, OnUpdateChart)
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)

	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START, OnStart)

	ON_REGISTERED_MESSAGE(BCGM_ON_CHART_AXIS_ZOOMED, OnChartAxisZoomed)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_DATA, OnClear)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnCancel)
	ON_BN_CLICKED(IDC_CHECK_AUTO_SCROLL, OnUpdateChart)
	ON_BN_CLICKED(IDC_CHECK_DATA_LABEL, OnUpdateChart)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CDarkHorseView::OnBnClickedButton1)
END_MESSAGE_MAP()

BOOL CDarkHorseView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CBCGPFormView::PreCreateWindow(cs);
}

void CDarkHorseView::OnInitialUpdate()
{
	CBCGPFormView::OnInitialUpdate();


// 	if (m_bIsReady)
// 	{
// 		return;
// 	}

	srand((unsigned)time(NULL));

// 	if (m_arYears[0].IsEmpty())
// 	{
// 		COleDateTime now = COleDateTime::GetCurrentTime();
// 
// 		for (int i = 0; i < YEARS_NUM; i++)
// 		{
// 			m_arYears[i].Format(_T("%d"), now.GetYear() - i - 1);
// 		}
// 	}

	GetParentFrame()->RecalcLayout();

	if (AfxGetMainWnd() == NULL)
	{
		return;
	}

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	GetTitle(m_strTitle);

	UpdateChartColorTheme(22, TRUE);

	CBCGPChartVisualObject* pChart = GetChart();
	if (pChart != NULL)
	{
		pChart->ShowAxisIntervalInterlacing(BCGP_CHART_X_PRIMARY_AXIS);
	}

	if (GetCtrl() != NULL)
	{
		//		GetCtrl()->SetGraphicsManager(CBCGPGraphicsManager::BCGP_GRAPHICS_MANAGER_GDI);

	}

// 	m_bIsReady = TRUE;
// 	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
// 	UpdateChartColorTheme(pMainFrame->GetColorTheme(), pMainFrame->IsDarkTheme());

	//CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	

	CCustomStockSeries* pStockSeries = new CCustomStockSeries(pChart, CBCGPBaseChartStockSeries::SST_BAR);
	CBCGPChartSeries* pCloseSeries = pStockSeries->GetChildSeries(CBCGPChartStockSeries::CHART_STOCK_SERIES_CLOSE_IDX);

	pStockSeries->m_strSeriesName = _T("CUR1/CUR2");
	pStockSeries->m_nOpenCloseBarSizePercent = 70;
	pStockSeries->SetExternalStorage(&storage);

	pChart->AddSeries(pStockSeries);

	CBCGPChartAxis* pAxisX = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	CBCGPChartAxis* pAxisY = pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);

	pChart->ShowAxis(BCGP_CHART_X_PRIMARY_AXIS, TRUE, TRUE);
	pChart->ShowAxis(BCGP_CHART_Y_PRIMARY_AXIS, FALSE, FALSE);
	pChart->ShowAxis(BCGP_CHART_Y_SECONDARY_AXIS, TRUE, TRUE);
	pChart->ShowAxisGridLines(BCGP_CHART_Y_SECONDARY_AXIS, TRUE);
	pChart->SetLegendPosition(BCGPChartLayout::LP_NONE);

	pStockSeries->SetRelatedAxis(pAxisY, CBCGPChartSeries::AI_Y);

	pAxisX->m_bFormatAsDate = TRUE;
	pAxisX->m_strDataFormat = _T("%H:%M:%S");
	pAxisX->ShowScrollBar(TRUE);
	pAxisX->SetFixedIntervalWidth(33, 2);

	// set offset of last displayed value in auto-scroll mode (in pixels)
	pAxisX->SetRightOffsetInPixels(33);

	pAxisY->m_strDataFormat = _T("%.4f");

	// Setup last OHLC info
	m_pOHLCInfo = SetupIndicatorObjects(DBL_MAX, pAxisY, _T(" ")); // DBL_MAX means that we don't need overbought line

	// Setup Custom Axes for indicators:
	// RSI
	CBCGPChartAxis* pAxisRSI = (CBCGPChartAxisY*)pAxisY->Split(80, 6, NULL, FALSE);
	pAxisRSI->SetFixedDisplayRange(0, 100, 50);
	pAxisRSI->m_strAxisName = _T("RSI Axis");
	pAxisRSI->EnableZoom(FALSE);
	pAxisRSI->m_crossType = CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE;
	pAxisRSI->m_majorTickMarkType = CBCGPChartAxis::TMT_INSIDE;

	// Create indicators
	// RSI
	static const int nRSIPeriod = 14;
	CBCGPChartRSIFormula rsi(CBCGPChartRSIFormula::RSI_SMOOTHED, nRSIPeriod);
	m_pRSISeries = rsi.Create(pChart, _T("RSI Smooth"), pCloseSeries, pAxisRSI);
	m_pRSISeries->m_bIncludeSeriesToLegend = FALSE;

	// Setup overbought and oversold indicator lines and indicator info
	static double dblRSIOversoldValue = 20;
	static double dblRSIOverboughtValue = 80;

	CString strRSIInfo;
	strRSIInfo.Format(RSI_INFO_FORMAT, RSI_PERIOD, 0.);

	SetupIndicatorObjects(dblRSIOversoldValue, pAxisRSI);
	m_pRSIInfoObject = SetupIndicatorObjects(dblRSIOverboughtValue, pAxisRSI, strRSIInfo);

	// X Axis should be always at the diagram bottom along with its labels
	pAxisX->m_crossType = CBCGPChartAxis::CT_FIXED_DEFAULT_POS;
	pAxisX->m_axisLabelType = CBCGPChartAxis::ALT_LOW;

	// allow interlacing for indicator area as well
	pAxisX->UsePerpendicularAxisForZoning(FALSE);

	pChart->SetZoomScrollConfig(BCGPChartMouseConfig::ZSO_WHEEL_PAN);
	//pChart->SetZoomScrollConfig(BCGPChartMouseConfig::ZSO_WHEEL_PAN);
	m_wndChart.EnableTooltip();

	m_wndCancel.EnableWindow(FALSE);

	OnUpdateChart();
}


void CDarkHorseView::OnChartAnimation()
{
	BOOL bIsReady = FALSE;

	CBCGPChartVisualObject* pChart = GetChart();
	if (pChart != NULL)
	{
// 		if (IsAnimationAvailable())
// 		{
// 			bIsReady = pChart->StartAnimation(theApp.m_dblChartAnimationTime, theApp.m_animationType, theApp.m_animationStyle);
// 		}

		if (!bIsReady)
		{
			pChart->SetDirty(TRUE, TRUE);
		}
	}
}


void CDarkHorseView::OnTimer(UINT_PTR nIDEvent)
{
	CBCGPFormView::OnTimer(nIDEvent);

	if (nIDEvent == ID_NEXT_DATA_POINT)
	{
		double dblOpen, dblHigh, dblLow, dblClose, dblTime;

		GenerateDataPoint(dblOpen, dblHigh, dblLow, dblClose, dblTime);
		storage.AddData(dblOpen, dblHigh, dblLow, dblClose, dblTime);

		int nStorageCount = storage.GetCount();

		// limit the storage to ten millions data points
		if (nStorageCount > 10000000)
		{
			OnClear();
			return;
		}

		CBCGPChartVisualObject* pChart = GetChart();
		CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);

		// scroll range must be updated to show all data points in the storage
		if (m_bAutoScroll && !pChart->IsThumbTrackMode() && !pChart->IsPanMode())
		{
			// always scroll to the last added value if not paused;
			// otherwise scroll the chart while data points are still being added
			// add some number of values returned by GetRightOffsetAsNumberOfValues for extra 
			// offset from the right side (see SetRightOffsetInPixels in OnInitialUpdate)
			pXAxis->SetFixedMaximumDisplayValue(nStorageCount + pXAxis->GetRightOffsetAsNumberOfValues() - 1);
		}

		if (m_pRSISeries != NULL)
		{
			CBCGPChartRSIFormula* pRSIFormula = DYNAMIC_DOWNCAST(CBCGPChartRSIFormula, m_pRSISeries->GetFormula());

			if (pRSIFormula != NULL)
			{
				pRSIFormula->UpdateLastDataPoints();
			}

			if (m_pRSIInfoObject != NULL)
			{
				double dblVal = m_pRSISeries->GetDataPointValue(nStorageCount - 1);
				m_pRSIInfoObject->m_strText.Format(RSI_INFO_FORMAT, RSI_PERIOD, dblVal);
			}
		}

		if (m_pOHLCInfo != NULL)
		{
			m_pOHLCInfo->m_strText.Format(OHLC_INFO_FORMAT, dblOpen, dblHigh, dblLow, dblClose);
		}

		pChart->SetDirty(TRUE, TRUE);
	}
}

void CDarkHorseView::OnStart()
{
	m_wndStart.EnableWindow(FALSE);
	m_wndCancel.EnableWindow();
	m_wndCancel.SetFocus();

	SetTimer(ID_NEXT_DATA_POINT, 500, NULL);
}

void CDarkHorseView::OnCancel()
{
	m_wndStart.EnableWindow();
	m_wndCancel.EnableWindow(FALSE);
	m_wndStart.SetFocus();

	KillTimer(ID_NEXT_DATA_POINT);
}

void CDarkHorseView::SetupLayout()
{
	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
	if (pLayout != NULL && GetCtrl() != NULL)
	{
		pLayout->AddAnchor(GetCtrl()->GetDlgCtrlID(), CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);
	}

	//CBCGPChartExampleView::SetupLayout();

// 	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
// 	if (pLayout != NULL)
// 	{
// 		pLayout->AddAnchor(IDC_BUTTON_CLEAR_DATA, CBCGPStaticLayout::e_MoveTypeVert, CBCGPStaticLayout::e_SizeTypeNone);
// 	}
}

void CDarkHorseView::GetTitle(CString& strTitle)
{

}

void CDarkHorseView::UpdateChartColorTheme(int nTheme, BOOL bIsDarkTheme)
{
	CBCGPChartVisualObject* pChart = GetChart();
	if (pChart == NULL)
	{
		return;
	}

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

// CDarkHorseView diagnostics

#ifdef _DEBUG
void CDarkHorseView::AssertValid() const
{
	CBCGPFormView::AssertValid();
}

void CDarkHorseView::Dump(CDumpContext& dc) const
{
	CBCGPFormView::Dump(dc);
}

void CDarkHorseView::UpdateChartData()
{
	//UpdateData();
}

CDarkHorseDoc* CDarkHorseView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDarkHorseDoc)));
	return (CDarkHorseDoc*)m_pDocument;
}
#endif //_DEBUG


// CDarkHorseView message handlers

void CDarkHorseView::OnContextMenu(CWnd*, CPoint point)
{
	if (CBCGPPopupMenu::GetSafeActivePopupMenu() != NULL)
	{
		return;
	}

	theApp.ShowPopupMenu(IDR_CONTEXT_MENU, point, this);
}

void CDarkHorseView::OnUpdateChartType()
{
	UpdateData();

	CreateChart();
}

LRESULT CDarkHorseView::HandleInitDialog(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = CBCGPFormView::HandleInitDialog(wParam, lParam);

	if (GetLayout() != NULL)
	{
		UpdateData(FALSE);
		SetupLayout();
	}

	return lRes;
}

void CDarkHorseView::OnUpdateChart()
{
	UpdateData();

	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	CBCGPChartStockSeries* pStockSeries = DYNAMIC_DOWNCAST(CBCGPChartStockSeries, pChart->GetSeries(0));

	if (pStockSeries != NULL)
	{
		pStockSeries->SetStockSeriesType((CBCGPBaseChartStockSeries::StockSeriesType)m_nLineType, FALSE);
	}

	SetBarLineWidth();

	BCGPChartFormatDataTable formatDataTable;
	formatDataTable.m_bShowLegendKeys = TRUE;
	formatDataTable.m_bDrawVerticalGridLines = FALSE;
	formatDataTable.m_bDrawOutline = FALSE;
	formatDataTable.SetContentPadding(CBCGPSize(1., 5.));

	pChart->ShowDataTable(m_bDataTable, &formatDataTable);

	pChart->SetDirty(TRUE, TRUE);
	m_wndChart.SetFocus();
}

LRESULT CDarkHorseView::OnChartAxisZoomed(WPARAM, LPARAM lp)
{
	// show "thick" OHLC bars on X zoom in
	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	CBCGPChartAxis* pXAxis = (CBCGPChartAxis*)lp;

	if (pXAxis == NULL || pXAxis->m_nAxisID != BCGP_CHART_X_PRIMARY_AXIS)
	{
		return 0;
	}

	SetBarLineWidth();

	return 0;
}


void CDarkHorseView::OnClear()
{
	OnCancel();
	storage.Clear();

	if (m_pRSISeries != NULL)
	{
		m_pRSISeries->RemoveAllDataPoints();
	}

	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	pXAxis->SetScrollRange(); // clear scroll range

	if (m_pOHLCInfo != NULL)
	{
		m_pOHLCInfo->m_strText.Empty();
	}

	if (m_pRSIInfoObject != NULL)
	{
		CString strRSIInfo;
		strRSIInfo.Format(RSI_INFO_FORMAT, RSI_PERIOD, 0.);

		m_pRSIInfoObject->m_strText = strRSIInfo;
	}

	pChart->SetDirty(TRUE, TRUE);
	m_wndChart.SetFocus();
}

void CDarkHorseView::CreateChart()
{
	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	pChart->EnableRedraw(FALSE);

	pChart->CleanUpChartData();

	int i = 0;

	for (i = 0; i < pChart->GetAxisCount(); i++)
	{
		CBCGPChartAxis* pAxis = pChart->GetChartAxis(i);
		if (pAxis == NULL)
		{
			continue;
		}

		ASSERT_VALID(pAxis);
		pChart->RemoveCustomAxis(pAxis, FALSE);
	}

	m_wndChart.EnableTooltip(TRUE);

	static const CString strCurrencyNames = _T("CUR1/CUR2");

	CBCGPChartStockSeries* pStockSeries = new CBCGPChartStockSeries(pChart, (CBCGPBaseChartStockSeries::StockSeriesType)m_nLineType, (CBCGPChartStockSeries::StockChartType)m_nChartType);
	ASSERT_VALID(pStockSeries);

	pChart->AddSeries(pStockSeries);

	//CBCGPChartStockSeries* pStockSeries = pChart->GetSeries(0);
	//pStockSeries->AddData()

	pStockSeries->m_strSeriesName = strCurrencyNames;
	CBCGPChartAxis* pYAxis = pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);
	ASSERT_VALID(pYAxis);

	pYAxis->m_strAxisName = strCurrencyNames;
	pYAxis->m_bDisplayAxisName = TRUE;
	pYAxis->m_strDataFormat = _T("%.4f");
	pYAxis->SetFixedDisplayRange(2.181, 2.187);

	CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	ASSERT_VALID(pXAxis);

	pXAxis->m_strAxisName = _T("Date/Time");
	pXAxis->m_bFormatAsDate = TRUE;
	pXAxis->m_strDataFormat = _T("%H:%M");
	pXAxis->EnableMajorUnitIntervalInterlacing(FALSE);
	pXAxis->ShowScrollBar();

	COleDateTime dtStartRange;
	COleDateTime dtEndRange;

	LoadStockData(pStockSeries, dtStartRange, dtEndRange);

	// Set callback function to calculate custom values for line representation
	pStockSeries->SetCustomStockValueCallback(ChartStockCallbackFunc);

	CBCGPChartHistogramSeries* pVolumeSeries = new CBCGPChartHistogramSeries(pChart, _T("Volume"));

	// generate sample data for volumes
	for (i = 0; i < pStockSeries->GetDataPointCount(); i++)
	{
		pVolumeSeries->AddDataPoint(Rand(700000, 90000000),
			pStockSeries->GetDataPointValue(i, CBCGPChartData::CI_X));
	}

	pChart->AddSeries(pVolumeSeries);
	pVolumeSeries->m_bIncludeSeriesToLegend = FALSE;

	const int nMAPeriod = 14;
	const int nRSIPeriod = 9;

	CBCGPChartSeries* pCloseSeries = pStockSeries->GetChildSeries(CBCGPChartStockSeries::CHART_STOCK_SERIES_CLOSE_IDX);

	// setup moving averages
	CBCGPChartMAFormula ma(CBCGPChartMAFormula::MA_SIMPLE, nMAPeriod);
	m_pOverlaySeriesLine = ma.Create(pChart, _T("SMA"), pCloseSeries);

	// setup Bollinger Bands
	CBCGPChartBollingerBandsFormula bb(2, 20);
	m_pOverlaySeriesBand = bb.Create(pChart, _T("BB (20, 2)"), pCloseSeries);
	m_pOverlaySeriesBand->SetSeriesLineDashStyle(CBCGPStrokeStyle::BCGP_DASH_STYLE_DASH_DOT);

	// setup secondary main Y axis
	pChart->ShowAxis(BCGP_CHART_Y_SECONDARY_AXIS, TRUE, TRUE);
	CBCGPChartAxis* pYSecondaryAxis = pChart->GetChartAxis(BCGP_CHART_Y_SECONDARY_AXIS);
	pYSecondaryAxis->m_strAxisName = strCurrencyNames;

	// Setup custom axis for volumes:
	CBCGPChartAxis* pAxisVolume = pYAxis->Split(60, 6, NULL, FALSE);
	pAxisVolume->SetDisplayUnits(1000000); // displays volume in millions
	pAxisVolume->m_strDataFormat = _T("%.fM"); // ... with postfix M
	pAxisVolume->m_crossType = CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE; // at right side
	pAxisVolume->m_majorTickMarkType = CBCGPChartAxis::TMT_INSIDE; // in this case "inside" points tick marks outside
	pAxisVolume->m_strAxisName = _T("Volume Axis");

	pVolumeSeries->SetRelatedAxis(pAxisVolume, CBCGPChartSeries::AI_Y);

	// Setup Custom Axes for indicators:
	// RSI
	CBCGPChartAxis* pAxisRSI = (CBCGPChartAxisY*)pYAxis->Split(20, 6, NULL, TRUE);
	pAxisRSI->SetFixedDisplayRange(0, 100, 50);
	pAxisRSI->m_strAxisName = _T("RSI Axis");

	// Stochastic
	CBCGPChartAxis* pAxisStochastic = (CBCGPChartAxisY*)pAxisVolume->Split(30, 6, NULL, FALSE);
	pAxisStochastic->SetFixedDisplayRange(0, 100, 50);
	pAxisStochastic->m_strAxisName = _T("Stochastic Axis");

	// MACD
	CBCGPChartAxis* pAxisMACD = pAxisStochastic->Split(50, 6, NULL, FALSE);
	pAxisMACD->m_strAxisName = _T("MACD Axis");

	// Create indicators
	// RSI
	CBCGPChartRSIFormula rsi(CBCGPChartRSIFormula::RSI_SMOOTHED, nRSIPeriod);
	CBCGPChartSeries* pRSISeries = rsi.Create(pChart, _T("RSI Smooth"), pCloseSeries, pAxisRSI);
	pRSISeries->m_bIncludeSeriesToLegend = FALSE;

	//  Uncomment to see exponential and simple types of RSI
	// 	CBCGPChartRSIFormula rsi1(CBCGPChartRSIFormula::RSI_EXPONENTIAL, nRSIPeriod);
	// 	CBCGPChartSeries* pRSISeries1 = rsi1.Create(pChart, _T("RSI Exp"), pCloseSeries, pAxisRSI);
	// 	pRSISeries1->m_bIncludeSeriesToLegend = FALSE;
	// 
	// 	CBCGPChartRSIFormula rsi2(CBCGPChartRSIFormula::RSI_SIMPLE, nRSIPeriod);
	// 	CBCGPChartSeries* pRSISeries2 = rsi2.Create(pChart, _T("RSI Simple"), pCloseSeries, pAxisRSI);
	// 	pRSISeries2->m_bIncludeSeriesToLegend = FALSE;

		// Stochastic
		// %K
	CBCGPChartStochasticFormula stochastic(5, 3);
	CBCGPChartSeries* pStochSeries = stochastic.Create(pChart, _T("Stochastic"), pStockSeries, pAxisStochastic);
	pStochSeries->m_bIncludeSeriesToLegend = FALSE;

	// %D
	CBCGPChartMAFormula signalStoch(CBCGPChartMAFormula::MA_SIMPLE, 3);
	CBCGPChartSeries* pSignalStochSeries = signalStoch.Create(pChart, _T("Stochastic Signal"), pStochSeries, pAxisStochastic);
	pSignalStochSeries->m_bIncludeSeriesToLegend = FALSE;
	pSignalStochSeries->SetSeriesLineDashStyle(CBCGPStrokeStyle::BCGP_DASH_STYLE_DASH);
	pSignalStochSeries->SetLongDataOffset(9);

	// MACD
	CBCGPChartMACDFormula macd(12, 26);
	macd.SetOutputSeriesCategory(BCGPChartLine);
	CBCGPChartSeries* pMACDSeries = macd.Create(pChart, _T("MACD"), pCloseSeries, pAxisMACD);
	pMACDSeries->m_bIncludeSeriesToLegend = FALSE;

	// MACD Signal
	CBCGPChartMAFormula signalMACD(CBCGPChartMAFormula::MA_SIMPLE, 9);
	CBCGPChartSeries* pSignalMACDSeries = signalMACD.Create(pChart, _T("MACD Signal"), pMACDSeries, pAxisMACD);
	pSignalMACDSeries->m_bIncludeSeriesToLegend = FALSE;
	pSignalMACDSeries->SetSeriesLineDashStyle(CBCGPStrokeStyle::BCGP_DASH_STYLE_DASH);

	// MACD Histogram
	CBCGPChartTransitionFormula histMACD(CBCGPChartTransitionFormula::TT_DIFF);
	histMACD.AddInputSeries(pMACDSeries);
	histMACD.AddInputSeries(pSignalMACDSeries);
	CBCGPChartSeries* pHistMACDSeries = pChart->CreateSeries(_T("MACD Histogram"), CBCGPColor(), BCGP_CT_SIMPLE, BCGPChartHistogram);
	pHistMACDSeries->SetRelatedAxis(pAxisMACD, CBCGPChartSeries::AI_Y);
	pHistMACDSeries->m_bIncludeSeriesToLegend = FALSE;
	pHistMACDSeries->SetFormula(histMACD);

	// X Axis should be always at the diagram bottom along with its labels
	pXAxis->m_crossType = CBCGPChartAxis::CT_FIXED_DEFAULT_POS;
	pXAxis->m_axisLabelType = CBCGPChartAxis::ALT_LOW;
	pChart->EnableResizeAxes(TRUE);

	// Setup overbought and oversold indicator lines and axis markers
	static double dblRSIOversoldValue = 30;
	static double dblRSIOverboughtValue = 70;

	SetupIndicatorObjects(dblRSIOversoldValue, pAxisRSI);
	SetupIndicatorObjects(dblRSIOverboughtValue, pAxisRSI, _T("RSI(9)"));

	static double dblStochOversoldValue = 20;
	static double dblStochOverboughtValue = 80;
	SetupIndicatorObjects(dblStochOversoldValue, pAxisStochastic);
	SetupIndicatorObjects(dblStochOverboughtValue, pAxisStochastic, _T("Stochastic(5, 3, 3)"));

	// Setup volume chart
	SetupIndicatorObjects(DBL_MAX, pAxisVolume, _T("Volume"));

	// Setup MACD chart
	SetupIndicatorObjects(DBL_MAX, pAxisMACD, _T("MACD (12, 26, 9)"));

	pChart->EnableRedraw(TRUE);

	OnUpdateChart();
}
// 
// CBCGPChartObject* CDarkHorseView::SetupIndicatorObjects(double dblValue, CBCGPChartAxis* pIndicatorAxis, const CString& strIndicatorName)
// {
// 	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
// 	ASSERT_VALID(pChart);
// 
// 	CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
// 	ASSERT_VALID(pXAxis);
// 
// 	// set overbought/oversold line and its axis mark
// 	if (dblValue != DBL_MAX)
// 	{
// 		const CBCGPChartTheme& colors = pChart->GetColors();
// 
// 		CArray<float, float> dashes;
// 		dashes.Add(4.0);
// 		dashes.Add(4.0);
// 
// 		CBCGPStrokeStyle style(dashes);
// 
// 		CBCGPChartLineObject* pLine =
// 			pChart->AddChartLineObject(dblValue, TRUE, colors.m_brAxisMajorGridLineColor, 1.0, &style);
// 		pLine->SetRelatedAxes(pXAxis, pIndicatorAxis);
// 		pLine->SetForeground(FALSE);
// 
// 		CString strText;
// 		strText.Format(_T("%.0f"), dblValue);
// 
// 		CBCGPChartAxisMarkObject* pMark = pChart->AddChartAxisMarkObject(dblValue, strText, TRUE, FALSE);
// 		pMark->SetRelatedAxes(pXAxis, pIndicatorAxis);
// 	}
// 
// 	// set indicator name; the name will be used to display last value
// 	if (!strIndicatorName.IsEmpty())
// 	{
// 		CBCGPRect rectLabel(5, -3, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate);
// 		CBCGPChartObject* pTextObject = pChart->AddChartTextObject(rectLabel, strIndicatorName);
// 
// 		pTextObject->SetRelatedAxes(pXAxis, pIndicatorAxis);
// 		return pTextObject;
// 	}
// 
// 	return NULL;
// }


void CDarkHorseView::GenerateDataPoint(double& dblOpen, double& dblHigh, double& dblLow, double& dblClose, double& dblTime)
{
	static const double MIN_RANGE = 1.25;
	static const double MAX_RANGE = 2.40;

	BOOL bUpTrend = Rand(0.1, 1.) >= m_dblUpTrendLimit;
	double dblDeltaOpen = Rand(0., 0.0004);
	double dblDeltaHigh = bUpTrend ? Rand(0., 0.0010) : Rand(0., 0.0005);
	double dblDeltaLow = Rand(0., 0.0015);
	double dblDeltaClose = bUpTrend ? Rand(0., 0.0008) : Rand(0., 0.0004);

	double dblBaseValue = 0;

	int nStorageCount = storage.GetCount();

	if (nStorageCount == 0)
	{
		dblBaseValue = Rand(MIN_RANGE, MAX_RANGE);
	}
	else
	{
		dblBaseValue = storage.GetData(nStorageCount - 1).m_dblClose;
	}

	dblOpen = bUpTrend ? dblBaseValue + dblDeltaOpen : dblBaseValue - dblDeltaOpen;
	dblHigh = dblOpen + dblDeltaHigh;
	dblLow = dblHigh - dblDeltaLow;
	dblClose = dblLow + dblDeltaClose;
	dblTime = COleDateTime::GetCurrentTime();

	dblHigh = max(max(dblOpen, dblHigh), dblClose);
	dblLow = min(min(dblOpen, dblLow), dblClose);

	if (dblLow < MIN_RANGE * 1.2)
	{
		m_dblUpTrendLimit = 0.3;
	}
	else if (dblHigh > MAX_RANGE * 0.8)
	{
		m_dblUpTrendLimit = 0.7;
	}
	else
	{
		m_dblUpTrendLimit = 0.5;
	}
}

void CDarkHorseView::LoadStockData(CBCGPChartStockSeries* pStockSeries, COleDateTime& dtStartRange, COleDateTime& dtEndRange)
{
	COleDateTime dtCurr = COleDateTime::GetCurrentTime();
	COleDateTime dtStart(dtCurr.GetYear(), dtCurr.GetMonth(), max(1, dtCurr.GetDay() - 1), dtCurr.GetHour(), 0, 0);

	dtStartRange = dtStart;

	COleDateTimeSpan span(0, 0, 5, 0);

	pStockSeries->AddData(1.1825, 1.1827, 1.1816, 1.1823, dtStart += span);
	pStockSeries->AddData(1.1824, 1.1829, 1.1823, 1.1826, dtStart += span);
	pStockSeries->AddData(1.1826, 1.1835, 1.1824, 1.1827, dtStart += span);
	pStockSeries->AddData(1.1829, 1.1837, 1.1828, 1.1834, dtStart += span);
	pStockSeries->AddData(1.1836, 1.1840, 1.1835, 1.1838, dtStart += span);
	pStockSeries->AddData(1.1838, 1.1841, 1.1837, 1.1839, dtStart += span);
	pStockSeries->AddData(1.1839, 1.1840, 1.1838, 1.1839, dtStart += span);
	pStockSeries->AddData(1.1839, 1.1840, 1.1836, 1.1837, dtStart += span);
	pStockSeries->AddData(1.1835, 1.1836, 1.1831, 1.1832, dtStart += span);
	pStockSeries->AddData(1.1833, 1.1837, 1.1831, 1.1832, dtStart += span);
	pStockSeries->AddData(1.1832, 1.1835, 1.1831, 1.1833, dtStart += span);
	pStockSeries->AddData(1.1833, 1.1838, 1.1832, 1.1836, dtStart += span);
	pStockSeries->AddData(1.1837, 1.1840, 1.1836, 1.1839, dtStart += span);
	pStockSeries->AddData(1.1839, 1.1844, 1.1839, 1.1842, dtStart += span);
	pStockSeries->AddData(1.1843, 1.1848, 1.1843, 1.1847, dtStart += span);
	pStockSeries->AddData(1.1847, 1.1851, 1.1845, 1.1849, dtStart += span);
	pStockSeries->AddData(1.1849, 1.1858, 1.1847, 1.1855, dtStart += span);
	pStockSeries->AddData(1.1855, 1.1856, 1.1854, 1.1855, dtStart += span);
	pStockSeries->AddData(1.1854, 1.1855, 1.1850, 1.1851, dtStart += span);
	pStockSeries->AddData(1.1849, 1.1850, 1.1842, 1.1843, dtStart += span);
	pStockSeries->AddData(1.1843, 1.1844, 1.1835, 1.1837, dtStart += span);
	pStockSeries->AddData(1.1837, 1.1838, 1.1836, 1.1837, dtStart += span);
	pStockSeries->AddData(1.1838, 1.1843, 1.1836, 1.1840, dtStart += span);
	pStockSeries->AddData(1.1841, 1.1845, 1.1839, 1.1842, dtStart += span);
	pStockSeries->AddData(1.1844, 1.1850, 1.1844, 1.1849, dtStart += span);
	pStockSeries->AddData(1.1848, 1.1853, 1.1847, 1.1851, dtStart += span);
	pStockSeries->AddData(1.1851, 1.1854, 1.1850, 1.1852, dtStart += span);
	pStockSeries->AddData(1.1852, 1.1860, 1.1851, 1.1858, dtStart += span);
	pStockSeries->AddData(1.1858, 1.1866, 1.1857, 1.1865, dtStart += span);
	pStockSeries->AddData(1.1865, 1.1866, 1.1862, 1.1863, dtStart += span);
	pStockSeries->AddData(1.1863, 1.1864, 1.1852, 1.1853, dtStart += span);
	pStockSeries->AddData(1.1853, 1.1855, 1.1849, 1.1850, dtStart += span);
	pStockSeries->AddData(1.1850, 1.1852, 1.1839, 1.1841, dtStart += span);
	pStockSeries->AddData(1.1836, 1.1841, 1.1827, 1.1829, dtStart += span);
	pStockSeries->AddData(1.1825, 1.1827, 1.1819, 1.1820, dtStart += span);
	pStockSeries->AddData(1.1820, 1.1822, 1.1818, 1.1819, dtStart += span);
	pStockSeries->AddData(1.1820, 1.1824, 1.1819, 1.1823, dtStart += span);
	pStockSeries->AddData(1.1823, 1.1825, 1.1822, 1.1824, dtStart += span);
	pStockSeries->AddData(1.1824, 1.1827, 1.1823, 1.1824, dtStart += span);
	pStockSeries->AddData(1.1823, 1.1824, 1.1812, 1.1814, dtStart += span);

	dtEndRange = dtStart + span;
}


void CDarkHorseView::SetBarLineWidth()
{
	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	CBCGPChartStockSeries* pStockSeries = DYNAMIC_DOWNCAST(CBCGPChartStockSeries, pChart->GetSeries(0));

	if (pXAxis == NULL || pStockSeries == NULL)
	{
		return;
	}

	CBCGPBaseChartStockSeries::StockSeriesType currStockType = pStockSeries->GetStockSeriesType();

	if (currStockType == CBCGPBaseChartStockSeries::SST_BAR)
	{
		if (pXAxis->GetValuesPerInterval() < 4)
		{
			pStockSeries->SetSeriesLineWidth(2.);
			pStockSeries->m_downBarStyle.m_dblWidth = 2.;
		}
		else
		{
			pStockSeries->SetSeriesLineWidth(1.);
			pStockSeries->m_downBarStyle.m_dblWidth = 1.;
		}
	}
	else if (currStockType == CBCGPBaseChartStockSeries::SST_CANDLE)
	{
		pStockSeries->SetSeriesLineWidth(1.);
		pStockSeries->m_downBarStyle.m_dblWidth = 1.;
	}
	else
	{
		pStockSeries->SetSeriesLineWidth(2.);
		pStockSeries->m_downBarStyle.m_dblWidth = 2.;
	}
}

void CDarkHorseView::OnBnClickedButton1()
{
	OnUpdateChart();
}

void CDarkHorseView::SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data)
{
	
}


CBCGPChartObject* CDarkHorseView::SetupIndicatorObjects(double dblValue, CBCGPChartAxis* pIndicatorAxis, const CString& strIndicatorName)
{
	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	ASSERT_VALID(pXAxis);

	// set overbought/oversold line and its axis mark
	if (dblValue != DBL_MAX)
	{
		const CBCGPChartTheme& colors = pChart->GetColors();

		CArray<float, float> dashes;
		dashes.Add(4.0);
		dashes.Add(4.0);

		CBCGPStrokeStyle style(dashes);

		CBCGPChartLineObject* pLine =
			pChart->AddChartLineObject(dblValue, TRUE, colors.m_brAxisMajorGridLineColor, 1.0, &style);
		pLine->SetRelatedAxes(pXAxis, pIndicatorAxis);
		pLine->SetForeground(FALSE);

		CString strText;
		strText.Format(_T("%.0f"), dblValue);

		CBCGPChartAxisMarkObject* pMark = pChart->AddChartAxisMarkObject(dblValue, strText, TRUE, FALSE);
		pMark->SetRelatedAxes(pXAxis, pIndicatorAxis);
	}

	// set indicator name; the name will be used to display last value
	if (!strIndicatorName.IsEmpty())
	{
		CBCGPRect rectLabel(5, -3, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate);
		CBCGPChartObject* pTextObject = pChart->AddChartTextObject(rectLabel, strIndicatorName);

		pTextObject->SetRelatedAxes(pXAxis, pIndicatorAxis);
		return pTextObject;
	}

	return NULL;
}
