#include "stdafx.h"
#include "SmChartFrame.h"
#include "SmChartFrameManager.h"
#include "SmStockSeries.h"
#include "SmExternalStorage.h"


IMPLEMENT_DYNCREATE(SmChartFrame, CBCGPChartVisualObject)
SmChartFrame::SmChartFrame()
{
	_Id = DarkHorse::SmChartFrameManager::GetId();
}

void SmChartFrame::Init()
{
	SmStockSeries* pStockSeries = new SmStockSeries(this, CBCGPBaseChartStockSeries::SST_BAR);
	CBCGPChartSeries* pCloseSeries = pStockSeries->GetChildSeries(CBCGPChartStockSeries::CHART_STOCK_SERIES_CLOSE_IDX);

	pStockSeries->m_strSeriesName = _T("CUR1/CUR2");
	pStockSeries->m_nOpenCloseBarSizePercent = 70;
	pStockSeries->SetExternalStorage(&_Storage);

	AddSeries(pStockSeries);

	CBCGPChartAxis* pAxisX = GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	CBCGPChartAxis* pAxisY = GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);

	ShowAxis(BCGP_CHART_X_PRIMARY_AXIS, TRUE, TRUE);
	ShowAxis(BCGP_CHART_Y_PRIMARY_AXIS, FALSE, FALSE);
	ShowAxis(BCGP_CHART_Y_SECONDARY_AXIS, TRUE, TRUE);
	ShowAxisGridLines(BCGP_CHART_Y_SECONDARY_AXIS, TRUE);
	SetLegendPosition(BCGPChartLayout::LP_NONE);

	pStockSeries->SetRelatedAxis(pAxisY, CBCGPChartSeries::AI_Y);

	pAxisX->m_bFormatAsDate = TRUE;
	pAxisX->m_strDataFormat = _T("%H:%M:%S");
	pAxisX->ShowScrollBar(TRUE);
	pAxisX->SetFixedIntervalWidth(33, 2);

	// set offset of last displayed value in auto-scroll mode (in pixels)
	pAxisX->SetRightOffsetInPixels(33);

	pAxisY->m_strDataFormat = _T("%.4f");
}
