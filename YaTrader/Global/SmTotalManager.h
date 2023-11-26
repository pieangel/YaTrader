#pragma once
#include <memory>
#include <string>
#include "../Order/SmTotalOrderManager.h"
namespace DarkHorse {
	class ViStockClient;
	class YaStockClient;
	class SmLoginManager;
	class SmTaskRequestMaker;
	class SmServerDataReceiver;
	class SmSymbolManager;
	class SmSymbolReader;
	class SmAccountManager;
	class SmQuoteManager;
	class SmHogaManager;
	class SmTotalPositionManager;
	class SmChartDataManager;
	class SmCallbackManager;
	class SmOrderRequestManager;
	class SmFundManager;
	class SmSaveManager;
	class SmTaskRequestManager;
	class SmTimeDataManager;
	class PnfManager;
	class SmSystemManager;
	class TotalOrderManager;
	class TotalPositionManager;
	class OrderRequestManager;
	class OrderProcessor;
	class EventHub;
	class ViServerDataReceiver;
	class SmOutSystemManager;
	class VtFileEventMonitor;
	class SmConfigManager;
	class YaServerDataReceiver;
	class SmTotalManager
	{
	public:
		// 0 : domestic server, 1 : abroad server
		int mode = 0;
		void CreateManagers();
		std::shared_ptr<SmTaskRequestMaker> ReqMkr() const { return _ReqMkr; }
		std::shared_ptr<SmServerDataReceiver> SvrDataRcvr() const { return _SvrDataRcvr; }
		std::shared_ptr<YaStockClient> Client() const { return _Client; }
		std::shared_ptr<SmLoginManager> LoginMgr() const { return _LoginMgr; }
		std::shared_ptr<SmSymbolManager> SymMgr() const { return _SymMgr; }
		std::shared_ptr<SmSymbolReader> SymRdr() const { return _SymRdr; }
		std::shared_ptr<SmAccountManager> AcntMgr() const { return _AcntMgr; }
		std::shared_ptr<SmTotalOrderManager> TotalOrderMgr() const { return _TotalOrderMgr; }
		std::shared_ptr<SmQuoteManager> QuoteMgr() const { return _QuoteMgr; }
		std::shared_ptr<SmHogaManager> HogaMgr() const { return _HogaMgr; }
		std::shared_ptr<SmTotalPositionManager> TotalPosiMgr() { return _TotalPosiMgr; }
		std::shared_ptr<SmChartDataManager> ChartDataMgr() { return _ChartDataMgr; }
		std::shared_ptr<SmCallbackManager> CallbackMgr() { return _CallbackMgr; }
		std::shared_ptr<SmOrderRequestManager> OrderReqMgr() { return _OrderReqMgr; }
		std::shared_ptr<SmFundManager> FundMgr() { return _FundMgr; }
		std::shared_ptr<SmSaveManager> SaveMgr() { return _SaveMgr; }
		std::shared_ptr<SmTaskRequestManager> TaskReqMgr() { return _TaskReqMgr; }
		std::shared_ptr<SmTimeDataManager> TimeDataMgr() { return _TimeDataMgr; }
		std::shared_ptr<PnfManager> PnfMgr() { return _PnfMgr; }
		std::shared_ptr<SmSystemManager> SystemMgr() { return _SystemMgr; }

		std::shared_ptr<TotalOrderManager> total_order_manager() { return total_order_manager_; }
		std::shared_ptr<TotalPositionManager> total_position_manager() { return total_position_manager_; }
		std::shared_ptr<OrderRequestManager> order_request_manager() { return order_request_manager_; }
		std::shared_ptr<OrderProcessor> order_processor() { return order_processor_; }
		std::shared_ptr<EventHub> event_hub() { return event_hub_; }
		std::shared_ptr<ViServerDataReceiver> vi_server_data_receiver() { return vi_server_data_receiver_; }
		std::shared_ptr<SmOutSystemManager> out_system_manager() { return out_system_manager_; }
		std::shared_ptr<VtFileEventMonitor> file_watch_monitor() { return file_watch_monitor_; }
		std::shared_ptr<SmConfigManager> config_manager() { return config_manager_; }
		std::shared_ptr<YaServerDataReceiver> ya_server_data_receiver() { return ya_server_data_receiver_; }

		bool use_dark_theme() const { return use_dark_theme_; }
		void use_dark_theme(bool val) { use_dark_theme_ = val; }
	private:
		bool use_dark_theme_ = false;
		std::shared_ptr<SmCallbackManager> _CallbackMgr = nullptr;
		std::shared_ptr<YaStockClient> _Client = nullptr;
		std::shared_ptr<SmLoginManager> _LoginMgr = nullptr;
		std::shared_ptr<SmTaskRequestMaker> _ReqMkr = nullptr;
		std::shared_ptr<SmServerDataReceiver> _SvrDataRcvr = nullptr;
		std::shared_ptr<SmSymbolManager> _SymMgr = nullptr;
		std::shared_ptr<SmSymbolReader> _SymRdr = nullptr;
		std::shared_ptr<SmAccountManager> _AcntMgr = nullptr;
		std::shared_ptr<SmTotalOrderManager> _TotalOrderMgr = nullptr;
		std::shared_ptr<SmQuoteManager> _QuoteMgr = nullptr;
		std::shared_ptr<SmHogaManager> _HogaMgr = nullptr;
		std::shared_ptr<SmTotalPositionManager> _TotalPosiMgr = nullptr;
		std::shared_ptr<SmChartDataManager> _ChartDataMgr = nullptr;
		std::shared_ptr<SmOrderRequestManager> _OrderReqMgr = nullptr;
		std::shared_ptr<SmFundManager> _FundMgr = nullptr;
		std::shared_ptr<SmSaveManager> _SaveMgr = nullptr;
		std::shared_ptr<SmTaskRequestManager> _TaskReqMgr = nullptr;
		std::shared_ptr<SmTimeDataManager> _TimeDataMgr = nullptr;
		std::shared_ptr<PnfManager> _PnfMgr = nullptr;
		std::shared_ptr<SmSystemManager> _SystemMgr = nullptr;

		std::shared_ptr<TotalOrderManager> total_order_manager_ = nullptr;
		std::shared_ptr<TotalPositionManager> total_position_manager_ = nullptr;
		std::shared_ptr<OrderRequestManager> order_request_manager_ = nullptr;
		std::shared_ptr<EventHub> event_hub_ = nullptr;
		std::shared_ptr<OrderProcessor> order_processor_ = nullptr;
		std::shared_ptr<ViServerDataReceiver> vi_server_data_receiver_ = nullptr;
		std::shared_ptr< SmOutSystemManager> out_system_manager_ = nullptr;
		std::shared_ptr< VtFileEventMonitor> file_watch_monitor_ = nullptr;
		std::shared_ptr<SmConfigManager> config_manager_ = nullptr;
		std::shared_ptr<YaServerDataReceiver> ya_server_data_receiver_ = nullptr;
	};
}

extern DarkHorse::SmTotalManager mainApp;

