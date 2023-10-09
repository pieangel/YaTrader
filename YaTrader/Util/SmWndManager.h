#pragma once
#include <map>
#include <memory>
#include <windows.h>
class AbAccountOrderWindow;
class SmFundOrderDialog;
class GaSpreadChart;
namespace DarkHorse {	
	class SmWndManager
	{
	public:
		SmWndManager() {};
		~SmWndManager() {};
		void AddOrderWnd(std::shared_ptr<AbAccountOrderWindow> order_wnd);
		void AddFundOrderWnd(std::shared_ptr<SmFundOrderDialog> order_wnd);
		void AddSpreadChart(std::shared_ptr<GaSpreadChart> chart);

	private:
		std::map<HWND, std::shared_ptr<AbAccountOrderWindow>> _OrderWndMap;
		std::map<HWND, std::shared_ptr<SmFundOrderDialog>> _FundOrderWndMap;
		std::map<HWND, std::shared_ptr<GaSpreadChart>> _SpreadChartMap;
	};
}

