#pragma once
#include <map>
#include <memory>
#include <windows.h>
class SmMainOrderDialog;

namespace DarkHorse {	
	class SmWndManager
	{
	public:
		SmWndManager() {};
		~SmWndManager() {};
		void AddOrderWnd(std::shared_ptr<SmMainOrderDialog> order_wnd);
	private:
		std::map<HWND, std::shared_ptr<SmMainOrderDialog>> _OrderWndMap;
	};
}

