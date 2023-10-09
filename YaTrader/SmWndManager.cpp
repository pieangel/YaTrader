#include "stdafx.h"
#include "SmWndManager.h"
#include "SmMainOrderDialog.h"
using namespace  DarkHorse;
void DarkHorse::SmWndManager::AddOrderWnd(std::shared_ptr<SmMainOrderDialog> order_wnd)
{
	if (!order_wnd) return;

	_OrderWndMap[order_wnd->GetSafeHwnd()] = order_wnd;
}
