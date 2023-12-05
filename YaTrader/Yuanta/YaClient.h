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
	DM_FUT_MINI_SISE,
	DM_FUT_MINI_HOGA,
	DM_OPTION_MONTH_QUOTE_KOSPI,
	DM_OPTION_MONTH_QUOTE_MINI_KOSPI,
	DM_OPTION_MONTH_QUOTE_WEEKLY,
	DM_OPTION_MONTH_QUOTE_KOSDAQ,
	AB_ACCOUNT_ASSET,
	AB_ACCOUNT_PROFIT_LOSS,
	AB_SYMBOL_PROFIT_LOSS,
	AB_ACCEPTED_ORDER,
	AB_SYMBOL_QUOTE,
	AB_SYMBOL_HOGA,
	AB_SYMBOL_POSITION,
	AB_TRADE_PROFIT_LOSS,
	AB_ORDER_NEW,
	AB_ORDER_MOD,
	AB_ORDER_CANCEL
};

// enum class InfoType {
// 	CODE_INFO_CODE, //                     값(0) : 종목코드
// 	CODE_INFO_STANDARD_CODE, //            값(1) : 표준 종목코드
// 	CODE_INFO_NAME, //                     값(2) : 한글 종목명
// 	CODE_INFO_ENG_NAME, //                 값(3) : 영문 종목명
// 	CODE_INFO_JANG_GUBUN //              값(4) : 장구분
// };
// 
// enum class MarketType {
// 	MAERKET_TYPE_INTERVAL, //              값(0) : 국내(주식, 선물옵션)
// 	MAERKET_TYPE_GLOBAL_STOCK, //          값(1) : 해외주식
// 	MAERKET_TYPE_GLOBAL_DERIVATIVE, //     값(2) : 해외선물옵션
// 	MAERKET_TYPE_INTERNAL_STOCK, //        값(3) : 국내 주식
// 	MAERKET_TYPE_INTERVAL_KOSPIFUTURE, //  값(4) : 국내 코스피선물
// 	MAERKET_TYPE_INTERVAL_KOSPIOPTION, //  값(5) : 국내 코스피옵션
// };



struct YA_REQ_INFO {
	SERVER_REQ req;
	std::string dso_name;
	std::string desc;
	std::string code;
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
		// key : 선물사 요청 번호, value : 요청 정보
		std::map<int, DhTaskArg> request_map_;
		// key : 선물사 요청 번호, value : 요청 정보
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
		int register_dm_symbol(const std::string& symbol_code);
		int unregister_dm_symbol(const std::string& symbol_code);
		int register_dm_account(const std::string& account_no);
		int unregister_dm_account(const std::string& account_no);

		int register_ab_symbol(const std::string& symbol_code);
		int unregister_ab_symbol(const std::string& symbol_code);
		int register_ab_account(const std::string& account_no);
		int unregister_ab_account(const std::string& account_no);

		int dm_symbol_quote(DhTaskArg arg);
		int dm_symbol_hoga(DhTaskArg arg);
		int confirm_account_password(DhTaskArg arg);
		void get_symbol_list(const int market_type);
		int dm_symbol_profit_loss(DhTaskArg arg);
		int dm_order_filled(DhTaskArg arg);
		int dm_order_orderable(DhTaskArg arg);
		int dm_account_asset(DhTaskArg arg);
		int dm_provisional_settlement(DhTaskArg arg);
		int dm_accepted_order(DhTaskArg arg);
		int dm_symbol_position(DhTaskArg arg);
		int dm_option_month_quote(DhTaskArg arg);
		int dm_option_month_quote_kospi_option(DhTaskArg arg);
		int dm_option_month_quote_mini_kospi_option(DhTaskArg arg);
		int dm_option_month_quote_weekly_option(DhTaskArg arg);
		int dm_option_month_quote_kosdaq_option(DhTaskArg arg);
		int dm_daily_profit_loss(DhTaskArg arg);
		int dm_liquidable_qty(DhTaskArg arg);
		int dm_trade_profit_loss(DhTaskArg arg);
		int dm_outstanding_order(DhTaskArg arg);
		int dm_fut_sise(DhTaskArg arg);
		int dm_fut_mini_sise(DhTaskArg arg);
		int dm_fut_mini_hoga(DhTaskArg arg);
		int dm_fut_hoga(DhTaskArg arg);
		int dm_opt_sise(DhTaskArg arg);
		int dm_opt_hoga(DhTaskArg arg);
		int dm_commodity_sise(DhTaskArg arg);
		int dm_commodity_hoga(DhTaskArg arg);
		int dm_chart_n_min(DhTaskArg arg);


		void new_order(const std::shared_ptr<OrderRequest>& order_req);
		void change_order(const std::shared_ptr<OrderRequest>& order_req);
		void cancel_order(const std::shared_ptr<OrderRequest>& order_req);

		void dm_new_order(const std::shared_ptr<OrderRequest>& order_req);
		void dm_change_order(const std::shared_ptr<OrderRequest>& order_req);
		void dm_cancel_order(const std::shared_ptr<OrderRequest>& order_req);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="req_info"></param>
		int ab_account_asset(DhTaskArg arg);
		int ab_account_profit_loss(DhTaskArg arg);
		int ab_symbol_profit_loss(DhTaskArg arg);
		int ab_accepted_order(DhTaskArg arg);
		int ab_symbol_quote(DhTaskArg arg);
		int ab_symbol_hoga(DhTaskArg arg);
		int ab_symbol_position(DhTaskArg arg);
		int ab_trade_profit_loss(DhTaskArg arg);

		void ab_new_order(const std::shared_ptr<OrderRequest>& order_req);
		void ab_change_order(const std::shared_ptr<OrderRequest>& order_req);
		void ab_cancel_order(const std::shared_ptr<OrderRequest>& order_req);


		void on_ab_account_asset(const YA_REQ_INFO& req_info);
		void on_ab_account_profit_loss(const YA_REQ_INFO& req_info);
		void on_ab_accepted_order(const YA_REQ_INFO& req_info);
		void on_ab_symbol_quote(const YA_REQ_INFO& req_info);
		void on_ab_symbol_hoga(const YA_REQ_INFO& req_info);
		void on_ab_symbol_position(const YA_REQ_INFO& req_info);
		void on_ab_symbol_profit_loss(const YA_REQ_INFO& req_info);
		void on_ab_trade_profit_loss(const YA_REQ_INFO& req_info);

		void on_dm_option_month_quote(const YA_REQ_INFO& req_info);
		void on_dm_option_month_quote_kospi_option(const YA_REQ_INFO& req_info);
		void on_dm_option_month_quote_mini_kospi_option(const YA_REQ_INFO& req_info);
		void on_dm_option_month_quote_weekly_option(const YA_REQ_INFO& req_info);
		void on_dm_option_month_quote_kosdaq_option(const YA_REQ_INFO& req_info);
		void on_dm_symbol_profit_loss(const YA_REQ_INFO& req_info);
		void on_dm_order_filled(const YA_REQ_INFO& req_info);
		void on_dm_order_orderable(const YA_REQ_INFO& req_info);
		void on_dm_asset(const YA_REQ_INFO& req_info);
		void on_dm_provisional_settlement(const YA_REQ_INFO& req_info);
		void on_dm_accepted_order(const YA_REQ_INFO& req_info);
		void on_dm_symbol_position(const YA_REQ_INFO& req_info);
		void on_dm_daily_profit_loss(const YA_REQ_INFO& req_info);
		void on_dm_liquidable_qty(const YA_REQ_INFO& req_info);
		void on_dm_trade_profit_loss(const YA_REQ_INFO& req_info);
		void on_dm_outstanding_order(const YA_REQ_INFO& req_info);
		void on_dm_fut_sise(const YA_REQ_INFO& req_info);
		void on_dm_fut_hoga(const YA_REQ_INFO& req_info);
		void on_dm_fut_mini_sise(const YA_REQ_INFO& req_info);
		void on_dm_fut_mini_hoga(const YA_REQ_INFO& req_info);
		void on_dm_opt_sise(const YA_REQ_INFO& req_info);
		void on_dm_opt_hoga(const YA_REQ_INFO& req_info);
		void on_dm_commodity_sise(const YA_REQ_INFO& req_info);
		void on_dm_commodity_hoga(const YA_REQ_INFO& req_info);
		void on_dm_chart_n_min(const YA_REQ_INFO& req_info);
		void on_dm_new_order(const YA_REQ_INFO& req_info);
		void on_dm_change_order(const YA_REQ_INFO& req_info);
		void on_dm_cancel_order(const YA_REQ_INFO& req_info);

		void on_ab_order_new(const YA_REQ_INFO& req_info);
		void on_ab_order_mod(const YA_REQ_INFO& req_info);
		void on_ab_order_cancel(const YA_REQ_INFO& req_info);

		void on_dm_realtime_order();
		void on_dm_realtime_quote();
		void on_dm_realtime_hoga();
		void on_dm_realtime_accepted_count();

		void on_ab_realtime_order();
		void on_ab_realtime_quote();
		void on_ab_realtime_hoga();

		// WPARAM wParam, LPARAM lParam
		virtual BOOL OnInitDialog();
	};
}
