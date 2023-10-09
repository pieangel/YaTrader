#pragma once
#include <map>
#include <set>
#include <functional>
#include <memory>
#include <windows.h>
#include <string>
#include <mutex>
#include "../MessageDefine.h"
//#include "../Global/TemplateSingleton.h"
// WM_USER + 3

// 뮤텍스를 추가하고 실행하기 전에 복사해서 실행하는 루틴 추가할 필요가 있다. 
// 쓰레드 동기화를 반드시 적용할 것. 
namespace DarkHorse {

	class SmSymbol;
	class SmOrder;
	class SmChartData;
	class SmAccount;
	struct Position;
	class SmCallbackManager // : public TemplateSingleton<SmCallbackManager>
	{
	public:
		SmCallbackManager();
		~SmCallbackManager();

		void SubscribeQuoteCallback(long id, std::function <void(const std::string& symbol_code )> callback) {
			_QuoteMap[id] = callback;
		}
		void UnsubscribeQuoteCallback(long id);
		void OnQuoteEvent(const std::string& symbol_code);
		void OnHogaEvent(const std::string& symbol_code);

		void SubscribeHogaCallback(long id, std::function <void(const std::string& symbol_code)> callback) {
			_HogaMap[id] = callback;
		}
		void UnsubscribeHogaCallback(long id);

		void SubscribeOrderCallback(long id, std::function <void(const std::string& account_no, const std::string& symbol_code)> callback) {
			_OrderMap[id] = callback;
		}
		void UnsubscribeOrderCallback(long id);

		void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);

		void SubscribeMasterCallback(long id, std::function <void(const std::string& symbol_code)> callback) {
			_MasterMap[id] = callback;
		}
		void UnsubscribeMasterCallback(long id);
		void OnMasterEvent(const std::string& symbol_code);

		void SubscribeChartCallback(long id, std::function <void(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action)> callback) {
			_ChartMap[id] = callback;
		}
		void UnsubscribeChartCallback(long id);
		void OnChartEvent(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action);

		void SubscribeAccountCallback(long id, std::function <void(const int& account_id)> callback) {
			_AccountMap[id] = callback;
		}
		void UnsubscribeAccountCallback(long id);
		void OnAccountEvent(const int& account_id);

		void SubscribeQuoteWndCallback(HWND id) {
			_WndQuoteSet.insert(id);
		}
		void UnsubscribeQuoteWndCallback(HWND id) {
			auto it = _WndQuoteSet.find(id);
			if (it != _WndQuoteSet.end()) {
				_WndQuoteSet.erase(it);
			}
		}

		void SubscribeHogaWndCallback(HWND id) {
			_WndHogaSet.insert(id);
		}
		void UnsubscribeHogaWndCallback(HWND id) {
			auto it = _WndHogaSet.find(id);
			if (it != _WndHogaSet.end()) {
				_WndHogaSet.erase(it);
			}
		}

		void OnWndQuoteEvent(const int& symbol_id);


		void OnWndHogaEvent(const int& symbol_id);

		void SubscribeOrderWndCallback(HWND id) {
			_WndOrderSet.insert(id);
		}
		void UnsubscribeOrderWndCallback(HWND id) {
			auto it = _WndOrderSet.find(id);
			if (it != _WndOrderSet.end()) {
				_WndOrderSet.erase(it);
			}
		}

		void OnWndOrderEvent(const std::string& order_no);

		void OnWndOrderEvent(const int& account_id, const int& symbol_id);

		void SubscribeAccountWndCallback(HWND id) {
			_WndAccountSet.insert(id);
		}
		void UnsubscribeAccountWndCallback(HWND id) {
			auto it = _WndAccountSet.find(id);
			if (it != _WndAccountSet.end()) {
				_WndAccountSet.erase(it);
			}
		}

		void OnWndAccountEvent(const int& account_id);

		void OnWndChartEvent(UINT Msg, SmChartData* chartData);

		void SubscribeChartWndCallback(HWND id) {
			_WndChartSet.insert(id);
		}
		void UnsubscribeChartWndCallback(HWND id) {
			auto it = _WndChartSet.find(id);
			if (it != _WndChartSet.end()) {
				_WndChartSet.erase(it);
			}
		}

		void OnSymbolListReceived();

		void ClearAllEventMap();

		void SubscribeSymbolMasterWndCallback(HWND id) {
			_WndSymbolMasterSet.insert(id);
		}
		void UnsubscribeSymbolMasterWndCallback(HWND id) {
			auto it = _WndSymbolMasterSet.find(id);
			if (it != _WndSymbolMasterSet.end()) {
				_WndSymbolMasterSet.erase(it);
			}
		}

		void OnSymbolMasterEvent(const int& symbol_id);

		void SubscribeOrderUpdateCallback(HWND id) {
			_OrderUpdateSet.insert(id);
		}
		void UnsubscribeOrderUpdateCallback(HWND id) {
			auto it = _OrderUpdateSet.find(id);
			if (it != _OrderUpdateSet.end()) {
				_OrderUpdateSet.erase(it);
			}
		}

		void OnOrderUpdateEvent(const int& account_id, const int& symbol_id);

		void SubscribePasswordCallback(HWND id) {
			_WndPasswordSet.insert(id);
		}
		void UnsubscribePasswordCallback(HWND id) {
			auto it = _WndPasswordSet.find(id);
			if (it != _WndPasswordSet.end()) {
				_WndPasswordSet.erase(it);
			}
		}

		void OnPasswordConfirmed(const int& account_id, const int& result);

		

		void SubscribeServerMsgCallback(HWND id) {
			_WndServerMsgSet.insert(id);
		}
		void UnsubscribeServerMsgCallback(HWND id) {
			auto it = _WndServerMsgSet.find(id);
			if (it != _WndServerMsgSet.end()) {
				_WndServerMsgSet.erase(it);
			}
		}

		void OnServerMsg(const int& msg_id);

		void SubscribeFundMsgCallback(HWND id) {
			_FundWndSet.insert(id);
		}
		void UnsubscribeFundMsgCallback(HWND id) {
			auto it = _FundWndSet.find(id);
			if (it != _FundWndSet.end()) {
				_FundWndSet.erase(it);
			}
		}

		void OnFundChanged();

		void subscribe_position_event_handler(const int position_control_id, std::function<void(std::shared_ptr<Position> position)>&& handler)
		{
			std::lock_guard<std::mutex> lock(symbol_position_mutex_); // Lock the mutex
			position_cb_handle_map_[position_control_id] = handler;
		}
		void unsubscribe_position_event_handler(const int position_control_id)
		{
			std::lock_guard<std::mutex> lock(symbol_position_mutex_); // Lock the mutex
			auto found = position_cb_handle_map_.find(position_control_id);
			if (found == position_cb_handle_map_.end()) return;
			position_cb_handle_map_.erase(found);
		}

		void process_position_event(std::shared_ptr<Position> position)
		{
			std::lock_guard<std::mutex> lock(symbol_position_mutex_); // Lock the mutex
			for (auto& var : position_cb_handle_map_) { 
				var.second(position); 
			}
		}

	private:
		// key : control id , value : callback function
		std::map<int, std::function<void(std::shared_ptr<Position> position)>> position_cb_handle_map_;
		std::mutex symbol_position_mutex_; // Mutex for thread synchronization
		std::map<long, std::function<void(const std::string& symbol_code)>> _QuoteMap;
		std::map<long, std::function<void(const std::string& symbol_code)>> _HogaMap;
		std::map<int, std::function<void(const std::string& account_no, const std::string& symbol_code)>> _OrderMap;
		std::map<int, std::function<void(const std::string& symbol_code)>> _MasterMap;
		std::map<int, std::function<void(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action)>> _ChartMap;
		std::map<int, std::function<void(const int& account_id)>> _AccountMap;
		std::set<HWND> _WndQuoteSet;
		std::set<HWND> _WndHogaSet;

		std::set<HWND> _OrderUpdateSet;

		// 주문 이벤트를 받을 창들의 핸들 집합
		std::set<HWND> _WndOrderSet;
		// 계좌 이벤트를 받을 창들의 핸들 집합
		std::set<HWND> _WndAccountSet;
		/// <summary>
		/// 차트 윈도우 집합 - 차트 데이터 업데이트 메시지가 전달되는 창들의 모임
		/// </summary>
		std::set<HWND> _WndChartSet;
		// Symbol Master 
		std::set<HWND> _WndSymbolMasterSet;

		std::set<HWND> _WndPasswordSet;

		std::set<HWND> _WndServerMsgSet;

		std::set<HWND> _FundWndSet;
	};
}

