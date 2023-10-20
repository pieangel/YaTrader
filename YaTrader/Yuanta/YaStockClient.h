#pragma once
#include <memory>
#include <map>
#include "../Task/SmTaskConst.h"
#include "../Json/json.hpp"
#include "../Task/SmTaskArg.h"
#include "../Task/SmTaskDefine.h"
namespace DarkHorse {

	typedef std::map<std::string, std::any> task_arg;
	class YaClient;
	class SmOrder;
	struct SmOrderRequest;
	struct OrderRequest;
	using order_request_p = std::shared_ptr<OrderRequest>;
	class YaStockClient : public std::enable_shared_from_this<YaStockClient>
	{
	public:
		void start_timer();
		void stop_timer();
		YaStockClient() {};
		virtual ~YaStockClient();
		void CreateControl();
		int ConnectToServer();
		int Login(task_arg&& login_info);
		void ExecTask(const SmTaskType& task_type, task_arg&& task);
		void ExecTask(DhTaskArg&&);

		// 업무를 제거한다. 실패했을 경우 업무 요청이 누적되는 것을 막는다.
		void RemoveTask(const int& task_id);

		void OnAccountReceived(nlohmann::json&& arg);
		void OnSymbolMaster(nlohmann::json&& arg);
		void OnSymbolQuote(nlohmann::json&& arg);
		void OnSymbolHoga(nlohmann::json&& arg);
		void OnDmSymbolQuote(nlohmann::json&& arg);
		void OnDmSymbolHoga(nlohmann::json&& arg);
		void OnDmSymbolMaster(const std::string& symbol_code);

		void on_dm_expected(nlohmann::json&& arg);
		void on_dm_commodity_future_quote(nlohmann::json&& arg);
		void on_dm_commodity_future_hoga(nlohmann::json&& arg);

		void on_dm_option_quote(nlohmann::json&& arg);
		void on_dm_option_hoga(nlohmann::json&& arg);

		// 해외 실시간 시세
		void OnRealtimeQuote(nlohmann::json&& arg);
		// 해외 실시간 호가
		void OnRealtimeHoga(nlohmann::json&& arg);

		void on_ab_account_asset(nlohmann::json&& arg);
		void on_dm_account_asset(nlohmann::json&& arg);
		void on_ab_symbol_profit_loss(nlohmann::json&& arg);
		void on_dm_symbol_profit_loss(nlohmann::json&& arg);
		void on_symbol_profit_loss(nlohmann::json&& arg);
		void on_ab_symbol_position(nlohmann::json&& arg);
		void OnAcceptedList(nlohmann::json&& arg);
		void on_ab_filled_order_list(nlohmann::json&& arg);
		// 해외 주문 접수
		void OnOrderAccepted(nlohmann::json&& arg);
		// 해외 주문 미체결 
		void OnOrderUnfilled(nlohmann::json&& arg);
		// 해외 주문 체결
		void OnOrderFilledList(nlohmann::json&& arg);
		void OnOrderFilled(nlohmann::json&& arg);
		void RegisterSymbol(const std::string& symbol_code);
		void UnregisterSymbol(const std::string& symbol_code);
		void RegisterAccount(const std::string& account_no);
		void UnregisterAccount(const std::string& account_no);

		void NewOrder(const std::shared_ptr<SmOrderRequest>& order_req);
		void ChangeOrder(const std::shared_ptr<SmOrderRequest>& order_req);
		void CancelOrder(const std::shared_ptr<SmOrderRequest>& order_req);

		void NewOrder(order_request_p order_req);
		void ChangeOrder(order_request_p order_req);
		void CancelOrder(order_request_p order_req);

		void dm_new_order(order_request_p order_req);
		void dm_change_order(order_request_p order_req);
		void dm_cancel_order(order_request_p order_req);

		void SendOrderAcceptedData(const std::shared_ptr<SmOrderRequest>& order_req);
		void SendOrderUnfilledData();
		void SendOrderFilledData();

		bool TestMode() const { return _TestMode; }
		void TestMode(bool val) { _TestMode = val; }
		void get_symbol_list(const int market_type);
		int GetSymbolMaster(task_arg&& arg);
		int GetSymbolQuote(task_arg&& arg);
		int GetSymbolHoga(task_arg&& arg);
		int CheckAccountPassword(task_arg&& arg);
		int CheckDmAccountPassword(task_arg&& arg);
		bool Enable() const { return _Enable; }
		void Enable(bool val);
		void UnRegAll();
		int GetFilledOrderList(const std::string& account_no, const std::string& pwd);

		int dm_symbol_quote(DhTaskArg arg);
		int dm_symbol_hoga(DhTaskArg arg);

		int confirm_account_password(DhTaskArg arg);
		int dm_symbol_master_file_download(DhTaskArg arg);
		int ab_symbol_master_file_download(DhTaskArg arg);
		int ab_symbol_master(DhTaskArg arg);
		int dm_symbol_master(DhTaskArg arg);
		int ab_account_asset(DhTaskArg arg);
		int dm_account_asset(DhTaskArg arg);
		int ab_account_profit_loss(DhTaskArg arg);
		int dm_account_profit_loss(DhTaskArg arg);

		int ab_symbol_profit_loss(DhTaskArg arg);
		int dm_symbol_profit_loss(DhTaskArg arg);

		int dm_symbol_position(DhTaskArg arg);
		int ab_symbol_position(DhTaskArg arg);

		int ab_accepted_order(DhTaskArg arg);
		int dm_accepted_order(DhTaskArg arg);

		int ab_symbol_hoga(DhTaskArg arg);
		int ab_symbol_quote(DhTaskArg arg);
	private:
		int GetDomesticSymbolCode(task_arg&& arg);
		bool _Enable{ true };
		// Set the info for the order that have been sent by the user
		void SetOrderAuxInfo(const std::shared_ptr<DarkHorse::SmOrder>& order);
		void SendOrderEvent(const std::shared_ptr<SmOrder>& order);
		std::shared_ptr<SmOrder> SetOrderAccepted(nlohmann::json&& arg);
		std::shared_ptr<SmOrder> SetOrderUnfilled(nlohmann::json&& arg);
		std::shared_ptr<SmOrder> SetOrderFilled(nlohmann::json&& arg);
		int DownloadFile(task_arg&& arg);
		int DownloadMasterFile(task_arg&& arg);
		int GetAccountAsset(task_arg&& arg);
		int GetDeposit(task_arg&& arg);
		int GetAccountProfitLoss(task_arg&& arg);
		int GetSymbolProfitLoss(task_arg&& arg);
		int GetAcceptedOrderList(task_arg&& arg);
		int GetFilledOrderList(task_arg&& arg);
		int GetChartData(task_arg&& arg);
		void NewOrder(task_arg&& arg);
		void ChangeOrder(task_arg&& arg);
		void CancelOrder(task_arg&& arg);
		void RegisterSymbol(task_arg&& arg);
		void UnregisterSymbol(task_arg&& arg);
		void RegisterAccount(task_arg&& arg);
		void UnregisterAccount(task_arg&& arg);


	private:
		std::shared_ptr<YaClient> _ViCtrol;
		// key : order request id, value : order request object
		std::map<int, std::shared_ptr<DarkHorse::SmOrderRequest>> _OrderReqMap;
		bool _TestMode = false;
	};
}


