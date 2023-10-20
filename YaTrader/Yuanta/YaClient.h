#pragma once
#include "afxdialogex.h"
#include "IYOA.h"
#include <memory>
#include <map>
#include <set>
#include <string>
#include <any>
#include <windows.h>
#include "afxdialogex.h"
#include "../Json/json.hpp"
#include "../Task/SmTaskArg.h"
#include "../Task/SmTaskDefine.h"
#include "CommDef.h"

using order_event = nlohmann::json;
// YaClient dialog
enum class SERVER_REQ : int { 
	DM_ORDER_FILLED,
	DM_ORDER_ORDERABLE,
	DM_ASSET,
	DM_PROVISIONAL_SETTLEMENT,
	DM_ACCEPTED,
	DM_POSITION_INFO,
	DM_DAILY_PROFIT_LOSS,
	DM_LIQUIDABLE_QTY,
	DM_TRADE_PROFIT_LOSS,
	DM_OUTSTANDING_ORDER,
	DM_FUT_SISE,
	DM_FUT_HOGA,
	DM_OPT_SISE,
	DM_OPT_HOGA,
	DM_COMMODITY_SISE,
	DM_COMMODITY_HOGA,
	DM_CHART_N_MIN,
	DM_ORDER_NEW,
	DM_ORDER_MOD,
	DM_ORDER_CANCEL,
};

// enum class InfoType {
// 	CODE_INFO_CODE, //                     ��(0) : �����ڵ�
// 	CODE_INFO_STANDARD_CODE, //            ��(1) : ǥ�� �����ڵ�
// 	CODE_INFO_NAME, //                     ��(2) : �ѱ� �����
// 	CODE_INFO_ENG_NAME, //                 ��(3) : ���� �����
// 	CODE_INFO_JANG_GUBUN //              ��(4) : �屸��
// };
// 
// enum class MarketType {
// 	MAERKET_TYPE_INTERVAL, //              ��(0) : ����(�ֽ�, �����ɼ�)
// 	MAERKET_TYPE_GLOBAL_STOCK, //          ��(1) : �ؿ��ֽ�
// 	MAERKET_TYPE_GLOBAL_DERIVATIVE, //     ��(2) : �ؿܼ����ɼ�
// 	MAERKET_TYPE_INTERNAL_STOCK, //        ��(3) : ���� �ֽ�
// 	MAERKET_TYPE_INTERVAL_KOSPIFUTURE, //  ��(4) : ���� �ڽ��Ǽ���
// 	MAERKET_TYPE_INTERVAL_KOSPIOPTION, //  ��(5) : ���� �ڽ��ǿɼ�
// };



struct YA_REQ_INFO {
	SERVER_REQ req;
	std::string dso_name;
	std::string desc;
	int request_id;
};
namespace DarkHorse {
	typedef std::map<std::string, std::any> task_arg;
	class YaStockClient;
	struct OrderRequest;
	class YaClient : public CDialog
	{
		DECLARE_DYNAMIC(YaClient)

	public:
		YaClient(CWnd* pParent, YaStockClient& ya_stock_client);
		virtual ~YaClient();

		// Dialog Data
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_YA_CLIENT };
#endif

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()

	private:
		IYOA g_iYuantaAPI;
		BOOL g_bInitial;
		BOOL g_bLogin;
		int g_nStartMsgID;
		YaStockClient& ya_stock_client_;

		std::vector< YA_REQ_INFO> ya_req_info_list_;
		// key : ������ ��û ��ȣ, value : ��û ����
		std::map<int, DhTaskArg> request_map_;
		// key : ������ ��û ��ȣ, value : ��û ����
		std::map<int, YA_REQ_INFO> ya_request_map_;
		void init_ya_req_info_list();
		afx_msg LRESULT OnReceiveError(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnReceiveData(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnReceiveRealData(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnReceiveSystemMessage(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnLoginComplete(WPARAM wParam, LPARAM lParam);

		void on_task_complete(const int& server_request_id);
		void on_task_request_error(const int argument_id);
		void on_task_receive_error(const int server_request_id);
		void log_in(const std::string& id, const std::string& pwd, const std::string& cert);
		int convert_to_int(const std::string& symbol_code, const char* data);
	public:
		void get_account_list();
		int Login(task_arg&& login_info);
		void init();
		BOOL CheckInit();
		BOOL CheckLogin();
		afx_msg void OnDestroy();
		afx_msg void OnTimer(UINT_PTR nIDEvent);
		void start_timer();
		void stop_timer();
	public:
		int register_symbol(const std::string& symbol_code);
		int unregister_symbol(const std::string& symbol_code);
		int register_account(const std::string& account_no);
		int unregister_account(const std::string& account_no);
		int dm_symbol_quote(DhTaskArg arg);
		int dm_symbol_hoga(DhTaskArg arg);
		int confirm_account_password(DhTaskArg arg);
		void get_symbol_list(const int market_type);
		int dm_symbol_profit_loss(DhTaskArg arg);
		int dm_order_filled(DhTaskArg arg);
		int dm_order_orderable(DhTaskArg arg);
		int dm_account_asset(DhTaskArg arg);
		int dm_provisional_settlement(DhTaskArg arg);
		int dm_accepted(DhTaskArg arg);
		int dm_position_info(DhTaskArg arg);
		int dm_daily_profit_loss(DhTaskArg arg);
		int dm_liquidable_qty(DhTaskArg arg);
		int dm_trade_profit_loss(DhTaskArg arg);
		int dm_outstanding_order(DhTaskArg arg);
		int dm_fut_sise(DhTaskArg arg);
		int dm_fut_hoga(DhTaskArg arg);
		int dm_opt_sise(DhTaskArg arg);
		int dm_opt_hoga(DhTaskArg arg);
		int dm_commodity_sise(DhTaskArg arg);
		int dm_commodity_hoga(DhTaskArg arg);
		int dm_chart_n_min(DhTaskArg arg);
		void dm_new_order(const std::shared_ptr<OrderRequest>& order_req);
		void dm_change_order(const std::shared_ptr<OrderRequest>& order_req);
		void dm_cancel_order(const std::shared_ptr<OrderRequest>& order_req);

		void on_req_dm_symbol_profit_loss(const YA_REQ_INFO& req_info);
		void on_req_dm_order_filled(const YA_REQ_INFO& req_info);
		void on_req_dm_order_orderable(const YA_REQ_INFO& req_info);
		void on_req_dm_asset(const YA_REQ_INFO& req_info);
		void on_req_dm_provisional_settlement(const YA_REQ_INFO& req_info);
		void on_req_dm_accepted(const YA_REQ_INFO& req_info);
		void on_req_dm_position_info(const YA_REQ_INFO& req_info);
		void on_req_dm_daily_profit_loss(const YA_REQ_INFO& req_info);
		void on_req_dm_liquidable_qty(const YA_REQ_INFO& req_info);
		void on_req_dm_trade_profit_loss(const YA_REQ_INFO& req_info);
		void on_req_dm_outstanding_order(const YA_REQ_INFO& req_info);
		void on_req_dm_fut_sise(const YA_REQ_INFO& req_info);
		void on_req_dm_fut_hoga(const YA_REQ_INFO& req_info);
		void on_req_dm_opt_sise(const YA_REQ_INFO& req_info);
		void on_req_dm_opt_hoga(const YA_REQ_INFO& req_info);
		void on_req_dm_commodity_sise(const YA_REQ_INFO& req_info);
		void on_req_dm_commodity_hoga(const YA_REQ_INFO& req_info);
		void on_req_dm_chart_n_min(const YA_REQ_INFO& req_info);
		void on_req_dm_new_order(const YA_REQ_INFO& req_info);
		void on_req_dm_change_order(const YA_REQ_INFO& req_info);
		void on_req_dm_cancel_order(const YA_REQ_INFO& req_info);

		void on_realtime_order();
		void on_realtime_quote();
		void on_realtime_hoga();
		void on_realtime_accepted_count();

		// WPARAM wParam, LPARAM lParam
		virtual BOOL OnInitDialog();
	};
}
