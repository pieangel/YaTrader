#pragma once
#include <memory>
#include <map>
#include <set>
#include <string>
#include <any>
#include <windows.h>
#include "hdfcommagent.h"
#include "afxdialogex.h"
#include "ViStockClient.h"
#include "../Json/json.hpp"
#include "../Task/SmTaskArg.h"
#include "../Task/SmTaskDefine.h"
using order_event = nlohmann::json;
// ViClient dialog
namespace DarkHorse {
	/// <summary>
	/// 데이터 범위를 한정해라. 
	/// 요청 정보가 들어 왔을 때 응답하는 정보가 요청 정보를 포함하지 않을 때
	/// 요청 정보를 임시로 저장했다가 다시 사용한다. 
	/// 절대로 최초로 요청을 내린 곳으로 다시 가서 정보를 찾아 오지 않는다.
	/// 이미 떠난 요청을 다시 찾는 수고를 해서는 절대로 안된다.
	/// 용어는 반드시 통일한다. 
	/// 같은 의미를 지니는 변수는 여러 곳에서 사용될 때 반드시 같은 변수 이름을 사용할 것
	/// 이렇게 하지 않으면 매우 혼란을 일으키므로 반드시 이 규칙을 지킬 것.
	/// 변수 이름에 대하여 표를 만들어라. 그러면 반드시 성공할 것이다.
	/// 이름에 대하여 일관성을 지킬 것
	/// 같은 의미에 대하여 여기서는 이렇게 저기서는 저렇게 짓지 말 것.
	/// </summary>
	class ViStockClient;
	struct SmOrderRequest;
	typedef std::map<std::string, std::any> task_arg;
	struct OrderRequest;
	using order_request_p = std::shared_ptr<OrderRequest>;
	class ViClient : public CDialog
	{

		DECLARE_DYNAMIC(ViClient)

		DECLARE_EVENTSINK_MAP()
		afx_msg void OnDataRecv(LPCTSTR sTrCode, LONG nRqID);
		afx_msg void OnGetBroadData(LPCTSTR strKey, LONG nRealType);
		afx_msg void OnGetMsg(LPCTSTR strCode, LPCTSTR strMsg);
		afx_msg void OnGetMsgWithRqId(int nRqId, LPCTSTR strCode, LPCTSTR strMsg);

	public:
		ViClient(CWnd* pParent = nullptr);   // standard constructor
		virtual ~ViClient();

		// Dialog Data
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_VI_CLIENT };
#endif
	public:
		int Login(const std::string& id, const std::string& pwd, const std::string& cert);
		int Login(task_arg&& arg);
		
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
		int ab_symbol_profit_loss(DhTaskArg arg);
		int dm_symbol_profit_loss(DhTaskArg arg);
		int dm_symbol_position(DhTaskArg arg);
		int ab_symbol_position(DhTaskArg arg);
		void on_task_complete(const int& nRqId);
		void on_task_error(const int& server_request_id, const int request_id);
		int dm_symbol_master_file_download(DhTaskArg arg);
		int ab_symbol_master_file_download(DhTaskArg arg);
		int ab_symbol_master(DhTaskArg arg);
		int ab_account_profit_loss(DhTaskArg arg);
		int dm_account_profit_loss(DhTaskArg arg);

		int ab_accepted_order(DhTaskArg arg);
		int dm_accepted_order(DhTaskArg arg);

		int dm_symbol_code(task_arg&& arg);
		int ab_filled_order_list(const std::string& account_no, const std::string& pwd);
		int check_account_password(task_arg&& arg);
		int dm_check_account_password(task_arg&& arg);
		void remove_task(const int& task_id);
		int download_file(task_arg&& arg);
		int download_symbol_master_file(task_arg&& arg);
		int ab_symbol_master(task_arg&& arg) ;
		int ab_symbol_quote(task_arg&& arg) ;
		int ab_symbol_hoga(task_arg&& arg) ;
		int ab_account_asset(task_arg&& arg) ;
		int ab_account_asset(DhTaskArg arg);
		int dm_account_asset(DhTaskArg arg);
		int ab_account_deposit(task_arg&& arg) ;
		int ab_account_profit_loss(task_arg&& arg) ;
		int ab_symbol_profit_loss(task_arg&& arg) ;
		int ab_accepted_order_list(task_arg&& arg) ;

		//int ab_symbol_quote(DhTaskArg arg);
		//int ab_symbol_hoga(DhTaskArg arg);
		int ab_symbol_quote(DhTaskArg&& arg);
		int ab_symbol_hoga(DhTaskArg&& arg);


		int ab_filled_order_list(task_arg&& arg) ;
		int ab_chart_data(task_arg&& arg) ;
		int ab_chart_data(SmTaskArg&& arg);
		int chart_data(DhTaskArg&& arg);
		//int dm_symbol_master(DhTaskArg&& arg);
		int dm_symbol_master(DhTaskArg arg);
		int account_profit_loss(DhTaskArg&& arg);
		int ab_account_profit_loss(DarkHorse::AccountProfitLossReq arg);
		int dm_account_profit_loss(DarkHorse::AccountProfitLossReq arg);
		void register_symbol(DhTaskArg&& arg);
		void ab_new_order(task_arg&& arg) ;
		void ab_change_order(task_arg&& arg) ;
		void ab_cancel_order(task_arg&& arg) ;
		void register_symbol(task_arg&& arg) ;
		void unregister_symbol(task_arg&& arg) ;
		void register_account(task_arg&& arg) ;
		void unregister_account(task_arg&& arg) ;

		void register_symbol(const std::string& symbol_code);
		void register_account(const std::string& account_no);
		void unregister_symbol(const std::string& symbol_code);
		void unregister_account(const std::string& account_no);

		void ab_new_order(const std::shared_ptr<SmOrderRequest>& order_req);
		void ab_change_order(const std::shared_ptr<SmOrderRequest>& order_req);
		void ab_cancel_order(const std::shared_ptr<SmOrderRequest>& order_req);

		void new_order(order_request_p order_req);
		void change_order(order_request_p order_req);
		void cancel_order(order_request_p order_req);

		static void make_custom_order_info(const order_request_p& order_req, std::string& custom_order_info);

		int connect_to_server();
		void Client(std::weak_ptr<ViStockClient> wp)
		{
			_Client = wp;
		}

		int convert_to_int(CString& strSymbolCode, CString& strValue);
		void unregister_all_symbols();
		void unregister_all_accounts();
		bool Enable() const { return _Enable; }
		void Enable(bool val) { _Enable = val; }
	private:
		void handle_order_event(order_event&& order_info_item);
		void dm_new_order(order_request_p order_req);
		void dm_change_order(order_request_p order_req);
		void dm_cancel_order(order_request_p order_req);

		void ab_new_order(order_request_p order_req);
		void ab_change_order(order_request_p order_req);
		void ab_cancel_order(order_request_p order_req);

		int ab_accepted_order_list(task_arg arg);
		int dm_accepted_order_list(task_arg arg);

		int dm_check_account_password(const std::string& account_no, const std::string& pwd);
		int ab_check_account_password(const std::string& account_no, const std::string& pwd);

		bool _Enable{ true };

		int _CheckPwdReqId{ -1 };
		int dm_chart_data(task_arg&& arg);
		int ab_chart_data_long(task_arg&& arg);
		int ab_chart_data_short(task_arg&& arg);

		int dm_chart_data(SmTaskArg&& arg);
		int ab_chart_data_long(SmTaskArg&& arg);
		int ab_chart_data_short(SmTaskArg&& arg);
	
		/// <summary>
		/// 클라이언트 객체
		/// </summary>
		std::weak_ptr<ViStockClient> _Client;
		/// <summary>
		/// 증권사 객체
		/// </summary>
		CHDFCommAgent m_CommAgent;
		void on_dm_symbol_code(const CString& sTrCode, const LONG& nRqID);
		void on_account_list(const CString& sTrCode, const LONG& nRqID) ;
		void on_ab_symbol_master(const CString& server_trade_code, const LONG& server_request_id);
		void on_dm_symbol_master(const CString& sTrCode, const LONG& nRqID);
		void on_dm_symbol_master_file(const CString& server_trade_code, const LONG& server_request_id);

		void on_dm_account_profit_loss(const CString& server_trade_code, const LONG& server_request_id);
		void on_ab_account_profit_loss(const CString& server_trade_code, const LONG& server_request_id);

		void on_ab_accepted_order(const CString& server_trade_code, const LONG& server_request_id);
		void on_dm_accepted_order(const CString& server_trade_code, const LONG& server_request_id);

		void on_ab_symbol_quote(const CString& sTrCode, const LONG& nRqID);
		void on_ab_symbol_hoga(const CString& sTrCode, const LONG& nRqID);
		void on_ab_account_asset(const CString& sTrCode, const LONG& nRqID);
		void on_dm_account_asset(const CString& sTrCode, const LONG& nRqID);
		void on_ab_symbol_profit_loss(const CString& sTrCode, const LONG& nRqID);
		void on_dm_symbol_profit_loss(const CString& sTrCode, const LONG& nRqID);
		void on_ab_symbol_position(const CString& sTrCode, const LONG& nRqID);
		void on_dm_symbol_position(const CString& sTrCode, const LONG& nRqID);
		void on_ab_filled_order_list(const CString& sTrCode, const LONG& nRqID);
		void on_ab_chart_data_long(const CString& sTrCode, const LONG& nRqID);
		void on_ab_chart_data_short(const CString& sTrCode, const LONG& nRqID);
		void on_dm_chart_data(const CString& sTrCode, const LONG& nRqID);
		/// <summary>
		/// 초기에 해외 데이터를 받을 때 처리하는 함수 
		/// 초기에는 요청 아이디를 검사해서 있다면 작업 완료 통보를 해준다. 
		/// </summary>
		/// <param name="sTrCode"></param>
		/// <param name="nRqID"></param>
		void OnChartDataLong_Init(const CString& sTrCode, const LONG& nRqID);
		/// <summary>
		/// 초기에 해외 데이터를 받을 때 처리하는 함수 
		/// 초기에는 요청 아이디를 검사해서 있다면 작업 완료 통보를 해준다. 
		/// </summary>
		/// <param name="sTrCode"></param>
		/// <param name="nRqID"></param>
		void OnChartDataShort_Init(const CString& sTrCode, const LONG& nRqID);
		/// <summary>
		/// 초기에 국내 데이터를 받을 때 처리하는 함수 
		/// 초기에는 요청 아이디를 검사해서 있다면 작업 완료 통보를 해준다. 
		/// </summary>
		/// <param name="sTrCode"></param>
		/// <param name="nRqID"></param>
		void OnDomesticChartData_Init(const CString& sTrCode, const LONG& nRqID);

		// 해외 주문 접수확인
		void on_ab_order_accepted(const CString& strKey, const LONG& nRealType);
		// 해외 주문 미체결 
		void on_ab_order_unfilled(const CString& strKey, const LONG& nRealType);
		// 해외 주문 체결
		void on_ab_order_filled(const CString& strKey, const LONG& nRealType);

		// 국내 주문 접수확인
		void on_dm_order_accepted(const CString& strKey, const LONG& nRealType);
		// 국내 주문 미체결 
		void on_dm_order_unfilled(const CString& strKey, const LONG& nRealType);
		// 국내 주문 체결
		void on_dm_order_filled(const CString& strKey, const LONG& nRealType);

		// 해외 실시간 시세
		void on_ab_future_quote(const CString& strKey, const LONG& nRealType);
		// 해외 실시간 호가
		void on_ab_future_hoga(const CString& strKey, const LONG& nRealType);

		// 국내 실시간 시세
		void on_dm_future_quote(const CString& strKey, const LONG& nRealType);
		// 국내 실시간 호가
		void on_dm_future_hoga(const CString& strKey, const LONG& nRealType);

		void on_dm_expected(const CString& strKey, const LONG& nRealType);

		void on_dm_commodity_future_quote(const CString& strKey, const LONG& nRealType);
		void on_dm_commodity_future_hoga(const CString& strKey, const LONG& nRealType);

		void on_dm_option_quote(const CString& strKey, const LONG& nRealType);
		void on_dm_option_hoga(const CString& strKey, const LONG& nRealType);

		void on_dm_order_position(const CString& strKey, const LONG& nRealType);
		std::map<int, DhTaskArg> request_map_;
		// Key : 선물사 요청 번호, value : argument map.
		std::map<int, task_arg> _ReqMap;
		// Key : Order Request Id, value : order request object
		std::map<int, std::shared_ptr<SmOrderRequest>> _OrderReqMap;
		// Key : req id, value : chart argument.
		std::map<int, SmChartDataReq> _ChartReqMap;
		// Key : 선물사 요청 번호, value : order request object.
		std::map<int, order_request_p> order_request_map;
		std::set<std::string> _RegSymbolSet;
		std::set<std::string> _RegAccountSet;
	};

}