#include "stdafx.h"
#include "SmTotalManager.h"
#include "../Client/ViStockClient.h"
#include "../Login/SmLoginManager.h"
#include "../Task/SmServerDataReceiver.h"
#include "../Task/SmTaskRequestMaker.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbolReader.h"
#include "../Account/SmAccountManager.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Quote/SmQuoteManager.h"
#include "../Hoga/SmHogaManager.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Chart/SmChartDataManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Fund/SmFundManager.h"
#include "../Archieve/SmSaveManager.h"
#include "../Task/SmTaskRequestManager.h"
#include "../TimeData/SmTimeDataManager.h"
#include "../Pnf/PnfManager.h"
#include "../System/SmSystemManager.h"
#include "../Order//OrderProcess/TotalOrderManager.h"
#include "../Position/TotalPositionManager.h"
#include "../Order/OrderRequest/OrderRequestManager.h"
#include "../Order/OrderProcess/OrderProcessor.h"
#include "../Event/EventHub.h"
#include "../Task/ViServerDataReceiver.h"
#include "../Task/YaServerDataReceiver.h"
#include "../OutSystem/SmOutSystemManager.h"
#include "../FileWatch/VtFileEventMonitor.h"
#include "../Config/SmConfigManager.h"
#include "../Yuanta/YaStockClient.h"
#include <string>

using namespace DarkHorse;

DarkHorse::SmTotalManager mainApp;

void SmTotalManager::CreateManagers()
{
	use_dark_theme_ = false;
	_Client = std::make_shared<YaStockClient>();
	_LoginMgr = std::make_shared<SmLoginManager>();
	_ReqMkr = std::make_shared<SmTaskRequestMaker>();
	_SvrDataRcvr = std::make_shared<SmServerDataReceiver>();
	_SymMgr = std::make_shared<SmSymbolManager>();
	_SymRdr = std::make_shared<SmSymbolReader>();
	_AcntMgr = std::make_shared<SmAccountManager>();
	_TotalOrderMgr = std::make_shared<SmTotalOrderManager>();
	_TotalPosiMgr = std::make_shared<SmTotalPositionManager>();
	_ChartDataMgr = std::make_shared<SmChartDataManager>();
	_QuoteMgr = std::make_shared<SmQuoteManager>();
	_QuoteMgr->StartProcess();
	_HogaMgr = std::make_shared<SmHogaManager>();
	_HogaMgr->StartProcess();
	_CallbackMgr = std::make_shared<SmCallbackManager>();
	_OrderReqMgr = std::make_shared<SmOrderRequestManager>();
	_FundMgr = std::make_shared<SmFundManager>();
	_SaveMgr = std::make_shared<SmSaveManager>();
	_TaskReqMgr = std::make_shared<SmTaskRequestManager>();
	_TimeDataMgr = std::make_shared<SmTimeDataManager>();
	_PnfMgr = std::make_shared<PnfManager>();
	_SystemMgr = std::make_shared<SmSystemManager>();

	total_order_manager_ = std::make_shared<TotalOrderManager>();
	total_position_manager_ = std::make_shared<TotalPositionManager>();
	event_hub_ = std::make_shared<EventHub>();
	order_processor_ = std::make_shared<OrderProcessor>();
	order_request_manager_ = std::make_shared<OrderRequestManager>();
	vi_server_data_receiver_ = std::make_shared<ViServerDataReceiver>();
	out_system_manager_ = std::make_shared<SmOutSystemManager>();
	file_watch_monitor_ = std::make_shared<VtFileEventMonitor>();
	config_manager_ = std::make_shared<SmConfigManager>();
	ya_server_data_receiver_ = std::make_shared<YaServerDataReceiver>();
}
