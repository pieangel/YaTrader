#pragma once
#include <map>
#include <string>
#include <memory>
#include <mutex>
#include "../Util/Signal.h"
#include "SmOrderConst.h"
namespace DarkHorse {
	class SmOrder;
	struct SmPosition;
	class SmOrderViewer;
	class SmStopOrderManager;
	struct SmOrderRequest;
	class SmSymbol;
	class SmSymbolOrderManager
	{
	public:
		SmSymbolOrderManager(const std::string& account_no, const std::string& symbol_code);
		~SmSymbolOrderManager() {};
		void ClearStopOrders();
		void ClearBuyStopOrders();
		void ClearSellStopOrders();
		void RemoveStopOrder(const SmPositionType& position, const int& req_id);
		void RemoveStopOrder(const int& req_id);
		void AddBuyStopOrder(const std::shared_ptr<SmOrderRequest>& order_req);
		void AddSellStopOrder(const std::shared_ptr<SmOrderRequest>& order_req);
		void AddCutStopOrder(const std::shared_ptr<SmOrder>& filled_order);
		void AddAcceptedOrder(std::shared_ptr<SmOrder> order);
		void RemoveAcceptedOrder(const std::string& order_no);
		void AddFilledOrder(std::shared_ptr<SmOrder> order);
		void RemoveFilledOrder(const std::string& order_no);
		void AddSettledOrder(std::shared_ptr<SmOrder> order);
		void RemoveSettledOrder(const std::string& order_no);
		void OnFilledOrderByEach(const std::shared_ptr<SmOrder>& new_filled_order);
		void OnAcceptedOrder(const std::shared_ptr<SmOrder>& accepted_order);
		void OnUnfilledOrder(const std::shared_ptr<SmOrder>& unfilled_order);
		void AddHistoryOrder(const std::shared_ptr<SmOrder>& order);
		//std::shared_ptr<SmPosition> CalcPosition();
		void OnFilledOrder(const std::shared_ptr<SmOrder>& new_filled_order);
		std::string AccountNo() const { return _AccountNo; }
		void AccountNo(std::string val) { _AccountNo = val; }
		std::string SymbolCode() const { return _SymbolCode; }
		void SymbolCode(std::string val) { _SymbolCode = val; }
		const std::map<std::string, std::shared_ptr<SmOrder>>& GetAcceptedOrders();
		//{
		//	return _AcceptedOrderMap;
		//}

		const std::map<std::string, std::shared_ptr<SmOrder>>& GetFilledOrders() {
			return _FilledOrderMap;
		}
		std::shared_ptr<DarkHorse::SmOrderViewer> BuyOrderViewer() const { return _BuyOrderViewer; }
		std::shared_ptr<DarkHorse::SmOrderViewer> SellOrderViewer() const { return _SellOrderViewer; }
		std::shared_ptr<DarkHorse::SmStopOrderManager> SellStopOrderMgr() const { return _SellStopOrderMgr; }
		std::shared_ptr<DarkHorse::SmStopOrderManager> BuyStopOrderMgr() const { return _BuyStopOrderMgr; }

		Gallant::Signal1<const std::string&> order_update;

		void UpdateOrder(std::shared_ptr<DarkHorse::SmOrderViewer>& buy_order_viewer, std::shared_ptr<DarkHorse::SmOrderViewer>& sell_order_viewer);
		void UpdateOrder();
		int GetUnsettledCount();
		void CheckStopOrder(const int& close);
		void CheckStopOrder(std::shared_ptr<SmSymbol> symbol);
	private:
		int get_order_request_id();
		void RemoveFilledOrder(int remove_count);
		void ClearFilledOrder();
		std::map<std::string, std::shared_ptr<SmOrder>> _HistoryOrderMap;
		std::map<std::string, std::shared_ptr<SmOrder>> _AcceptedOrderMap;
		std::map<std::string, std::shared_ptr<SmOrder>> _FilledOrderMap;
		std::map<std::string, std::shared_ptr<SmOrder>> _SettledOrderMap;

		std::string _AccountNo;
		std::string _SymbolCode;

		std::shared_ptr<SmOrderViewer> _BuyOrderViewer = nullptr;
		std::shared_ptr<SmOrderViewer> _SellOrderViewer = nullptr;

		std::mutex _m;
		std::mutex _m_filled;

		std::shared_ptr<DarkHorse::SmStopOrderManager> _SellStopOrderMgr{ nullptr };

		std::shared_ptr<DarkHorse::SmStopOrderManager> _BuyStopOrderMgr{ nullptr };
		
	};
}

