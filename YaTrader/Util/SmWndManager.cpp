#include "stdafx.h"
#include "SmWndManager.h"
#include "../Order/AbAccountOrderWindow.h"
#include "../Fund/SmFundOrderDialog.h"
#include "../ChartDialog/GaSpreadChart.h"
using namespace  DarkHorse;
void DarkHorse::SmWndManager::AddOrderWnd(std::shared_ptr<AbAccountOrderWindow> order_wnd)
{
	if (!order_wnd) return;

	_OrderWndMap[order_wnd->GetSafeHwnd()] = order_wnd;
}

void DarkHorse::SmWndManager::AddFundOrderWnd(std::shared_ptr<SmFundOrderDialog> order_wnd)
{
	if (!order_wnd) return;

	_FundOrderWndMap[order_wnd->GetSafeHwnd()] = order_wnd;
}

void DarkHorse::SmWndManager::AddSpreadChart(std::shared_ptr<GaSpreadChart> chart)
{
	if (!chart) return;

	_SpreadChartMap[chart->GetSafeHwnd()] = chart;
}
