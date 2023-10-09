#pragma once
#include <memory>
#include <map>
#include <vector>
namespace DarkHorse {
	/// <summary>
	/// Stop 주문을 관리한다.
	/// 매도, 매수 스탑 주문을 분리하여 관리한다.
	/// </summary>
	struct SmOrderRequest;
	class SmSymbol;
	class SmStopOrderManager
	{
	public:
		void RemoveOrderRequest(const int& request_id);
		void AddOrderRequest(std::shared_ptr<SmOrderRequest> order_req);
		size_t FindCount(const int& price);
		// Key : price , Value : Order Request Object
		const std::map<int, std::map< int, std::shared_ptr<SmOrderRequest>>>& GetRequestMap() {
			return _OrderMap;
		}
		void Clear() {
			_OrderReqMap.clear();
		}
		const std::map<int, std::shared_ptr<SmOrderRequest>>& GetOrderReqMap() {
			return _OrderReqMap;
		}
		const size_t GetOrderReqCount() {
			return _OrderReqMap.size();
		}
		void CheckStopOrder(const int& close);
		void CheckStopOrder(std::shared_ptr<SmSymbol> symbol);
	private:
		bool _ProcessStop = false;
		/// <summary>
		/// Key : order price, order request object map
		/// Internal Map => Key : request id, value : order request object
		/// </summary>
		std::map<int, std::map<int,  std::shared_ptr<SmOrderRequest>>> _OrderMap;
		// key : order request id, value : order request object
		std::map<int, std::shared_ptr<SmOrderRequest>> _OrderReqMap;
	};
}

