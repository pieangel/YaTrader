// YaClient.cpp : implementation file
//


#include "stdafx.h"
#include "../resource.h"
#include "afxdialogex.h"
#include "YaClient.h"
#include "MessageDef.h"
#include "magic_enum/magic_enum.hpp"
#include "../Log/MyLogger.h"
#include "../Global/SmTotalManager.h"
#include "../Task/YaServerDataReceiver.h"
#include "../Login/SmLoginManager.h"
#include "../MainFrm.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Config/SmConfigManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Order/OrderRequest/OrderRequest.h"
#include "YaStockClient.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/OrderProcess/TotalOrderManager.h"
#include "../Position/TotalPositionManager.h"
#include <random>
#include "../Util/VtStringUtil.h"

class CMainFrame;
using namespace DarkHorse;
using namespace nlohmann;
// YaClient dialog

IMPLEMENT_DYNAMIC(YaClient, CDialog)

YaClient::YaClient(CWnd* pParent, YaStockClient& ya_stock_client)
	: CDialog(IDD_YA_CLIENT, pParent), ya_stock_client_(ya_stock_client)
{
	init_ya_req_info_list();
}

YaClient::~YaClient()
{
	g_iYuantaAPI.YOA_UnInitial();
}

void YaClient::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(YaClient, CDialog)
	ON_WM_TIMER()
	ON_MESSAGE(WMU_RECEIVE_ERROR, OnReceiveError)
	ON_MESSAGE(WMU_RECEIVE_DATA, OnReceiveData)
	ON_MESSAGE(WMU_RECEIVE_REAL_DATA, OnReceiveRealData)
	ON_MESSAGE(WMU_RECEIVE_SYSTEM_MESSAGE, OnReceiveSystemMessage)
	ON_MESSAGE(WMU_LOGIN, OnLoginComplete)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// YaClient message handlers
LRESULT YaClient::OnReceiveError(WPARAM wParam, LPARAM lParam)
{
	const int request_id = wParam;
	auto found = ya_request_map_.find(request_id);
	if (found == ya_request_map_.end()) return 1;
	const std::string& trade_code = found->second.dso_name.substr(3);
	const std::string& desc = found->second.desc;
	long error_code = 0;
	CString strMsg;
	switch (lParam)
	{
	case ERROR_TIMEOUT_DATA:			// 설정된 시간 안에 서버로부터 응답이 없는 경우, 타임아웃이 발생합니다. (기본 10초)
	{
		strMsg.Format(_T("Trade Code[%s] [%s] Request id[%d] :: Timeout %s 요청의 응답이 없습니다."), trade_code.c_str(), desc.c_str(), request_id);
	}
	break;
	case ERROR_REQUEST_FAIL:			// 서버에서 조회TR(DSO) 처리중 오류가 있는 경우 발생합니다.
	{
		TCHAR msg[2048] = { 0, };
		error_code = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(request_id, msg, sizeof(msg));	// 처리 중 오류에 대한 메시지를 얻을 수 있습니다.
		strMsg.Format(_T("Trade code[%s], desc[%s], error code[%d], Request id[%d], msg [%s]"), trade_code.c_str(), desc.c_str(), error_code, request_id, msg);
		strMsg.TrimRight();
		
		auto found = request_map_.find(request_id);
		if (found != request_map_.end())
		{
			on_task_request_error(found->second.argument_id);
		}
	}
	break;
	}

	mainApp.TotalOrderMgr()->ServerMsg = strMsg;
	mainApp.CallbackMgr()->OnServerMsg(error_code);

	LOGINFO(CMyLogger::getInstance(), strMsg.GetBuffer(0));
	return 0;
}

LRESULT YaClient::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	LPRECV_DATA pRecvData = (LPRECV_DATA)lParam;

	const int trade_code = (const int)wParam;
	auto found = ya_request_map_.find(trade_code);
	if (found == ya_request_map_.end()) return 1;

	if (found->second.req == SERVER_REQ::DM_ORDER_FILLED)
		on_dm_order_filled(found->second);
	else if (found->second.req == SERVER_REQ::DM_ORDER_ORDERABLE)
		on_dm_order_orderable(found->second);
	else if (found->second.req == SERVER_REQ::DM_ASSET)
		on_dm_asset(found->second);
	else if (found->second.req == SERVER_REQ::DM_PROVISIONAL_SETTLEMENT)
		on_dm_provisional_settlement(found->second);
	else if (found->second.req == SERVER_REQ::DM_ACCEPTED)
		on_dm_accepted_order(found->second);
	else if (found->second.req == SERVER_REQ::DM_POSITION_INFO)
		on_dm_symbol_position(found->second);
	else if (found->second.req == SERVER_REQ::DM_DAILY_PROFIT_LOSS)
		on_dm_daily_profit_loss(found->second);
	else if (found->second.req == SERVER_REQ::DM_LIQUIDABLE_QTY)
		on_dm_liquidable_qty(found->second);
	else if (found->second.req == SERVER_REQ::DM_TRADE_PROFIT_LOSS)
		on_dm_trade_profit_loss(found->second);
	else if (found->second.req == SERVER_REQ::DM_OUTSTANDING_ORDER)
		on_dm_outstanding_order(found->second);
	else if (found->second.req == SERVER_REQ::DM_FUT_SISE)
		on_dm_fut_sise(found->second);
	else if (found->second.req == SERVER_REQ::DM_FUT_HOGA)
		on_dm_fut_hoga(found->second);
	else if (found->second.req == SERVER_REQ::DM_FUT_MINI_SISE)
		on_dm_fut_mini_sise(found->second);
	else if (found->second.req == SERVER_REQ::DM_FUT_MINI_HOGA)
		on_dm_fut_mini_hoga(found->second);
	else if (found->second.req == SERVER_REQ::DM_OPT_SISE)
		on_dm_opt_sise(found->second);
	else if (found->second.req == SERVER_REQ::DM_OPT_HOGA)
		on_dm_opt_hoga(found->second);
	else if (found->second.req == SERVER_REQ::DM_COMMODITY_SISE)
		on_dm_commodity_sise(found->second);
	else if (found->second.req == SERVER_REQ::DM_COMMODITY_HOGA)
		on_dm_commodity_hoga(found->second);
	else if (found->second.req == SERVER_REQ::DM_CHART_N_MIN)
		on_dm_chart_n_min(found->second);
	else if (found->second.req == SERVER_REQ::DM_ORDER_NEW)
		on_dm_new_order(found->second);
	else if (found->second.req == SERVER_REQ::DM_ORDER_MOD)
		on_dm_change_order(found->second);
	else if (found->second.req == SERVER_REQ::DM_ORDER_CANCEL)
		on_dm_cancel_order(found->second);
	else if (found->second.req == SERVER_REQ::DM_OPTION_MONTH_QUOTE_KOSPI)
		on_dm_option_month_quote_kospi_option(found->second);
	else if (found->second.req == SERVER_REQ::DM_OPTION_MONTH_QUOTE_MINI_KOSPI)
		on_dm_option_month_quote_mini_kospi_option(found->second);
	else if (found->second.req == SERVER_REQ::DM_OPTION_MONTH_QUOTE_WEEKLY)
		on_dm_option_month_quote_weekly_option(found->second);
	else if (found->second.req == SERVER_REQ::DM_OPTION_MONTH_QUOTE_KOSDAQ)
		on_dm_option_month_quote_kosdaq_option(found->second);
	else if (found->second.req == SERVER_REQ::AB_ACCOUNT_ASSET)
		on_ab_account_asset(found->second);
	else if (found->second.req == SERVER_REQ::AB_ACCOUNT_PROFIT_LOSS)
		on_ab_account_profit_loss(found->second);
	else if (found->second.req == SERVER_REQ::AB_SYMBOL_PROFIT_LOSS)
		on_ab_symbol_profit_loss(found->second);
	else if (found->second.req == SERVER_REQ::AB_ACCEPTED_ORDER)
		on_ab_accepted_order(found->second);
	else if (found->second.req == SERVER_REQ::AB_SYMBOL_QUOTE)
		on_ab_symbol_quote(found->second);
	else if (found->second.req == SERVER_REQ::AB_SYMBOL_HOGA)
		on_ab_symbol_hoga(found->second);
	else if (found->second.req == SERVER_REQ::AB_SYMBOL_POSITION)
		on_ab_symbol_position(found->second);
	else if (found->second.req == SERVER_REQ::AB_TRADE_PROFIT_LOSS)
		on_ab_trade_profit_loss(found->second);
	else if (found->second.req == SERVER_REQ::AB_ORDER_NEW)
		on_ab_order_new(found->second);
	else if (found->second.req == SERVER_REQ::AB_ORDER_MOD)
		on_ab_order_mod(found->second);
	else if (found->second.req == SERVER_REQ::AB_ORDER_CANCEL)
		on_ab_order_cancel(found->second);
	return 0;
}

LRESULT YaClient::OnReceiveRealData(WPARAM wParam, LPARAM lParam)
{
	LPRECV_DATA pRecvData = (LPRECV_DATA)lParam;
	CString strAutoID(pRecvData->szTrCode);

	if (0 == strAutoID.Compare(_T("41")))
		on_dm_realtime_quote();
	else if (0 == strAutoID.Compare(_T("71")))
		on_dm_realtime_order();
	else if (0 == strAutoID.Compare(_T("61")))
		on_ab_realtime_quote();
	else if (0 == strAutoID.Compare(_T("62")))
		on_ab_realtime_hoga();
	else if (0 == strAutoID.Compare(_T("81")))
		on_ab_realtime_order();

	return 0;
}

LRESULT YaClient::OnReceiveSystemMessage(WPARAM wParam, LPARAM lParam)
{
	LRESULT ret = 0;

	int nMsgID = wParam;

	TCHAR msg[2048] = { 0, };

	g_iYuantaAPI.YOA_GetErrorMessage(nMsgID, msg, sizeof(msg));
	LOGINFO(CMyLogger::getInstance(), msg);

	if (NOTIFY_SYSTEM_NEED_TO_RESTART == nMsgID)
	{
		g_iYuantaAPI.YOA_ReStart();
	}

	return ret;
}

LRESULT YaClient::OnLoginComplete(WPARAM wParam, LPARAM lParam)
{
	LRESULT ret = 0;

	if (RESPONSE_LOGIN_SUCCESS == wParam)
	{
		mainApp.LoginMgr()->IsLoggedIn(true);

		LOGINFO(CMyLogger::getInstance(), "로그인 성공 사용자 저장 user id = %s", mainApp.LoginMgr()->id().c_str());
		mainApp.SymMgr()->MakeDomesticMarket();

		get_account_list();

		get_symbol_list(5);

		((CMainFrame*)AfxGetMainWnd())->SetAccountInfo();

		g_bLogin = TRUE;

	}
	else if (RESPONSE_LOGIN_FAIL == wParam)
	{
		CString message;
		if (ERROR_TIMEOUT_DATA == lParam)
		{
			message =_T("로그인이 실패하였습니다.\n");
			message.Append(_T("서버로부터 로그인 응답이 없습니다. 다시 시도하여 주십시오."));
		}
		else
		{
			message = _T("로그인이 실패하였습니다.\n");

			TCHAR msg[2048] = { 0, };

			g_iYuantaAPI.YOA_GetErrorMessage(lParam, msg, sizeof(msg));
			message.Append(msg);
		}

		AfxMessageBox(message);
	}

	
	return 1;
}

void YaClient::init()
{
	const int nCurSel = mainApp.LoginMgr()->ya_server_index();
	CString strServer = _T("simul.tradar.api.com");
	if (1 == nCurSel)
		strServer = _T("real.tradar.api.com");
	else if (2 == nCurSel)
		strServer = _T("simul.tradarglobal.api.com");
	else if (3 == nCurSel)
		strServer = _T("real.tradarglobal.api.com");

	const std::string app_path = mainApp.config_manager()->GetApplicationPath();
	//if ( RESULT_SUCCESS == g_iYuantaAPI.YOA_Initial( strServer, GetSafeHwnd(), NULL, WMU_STARTMSGID ) )
	if (RESULT_SUCCESS == g_iYuantaAPI.YOA_Initial(strServer, GetSafeHwnd(), app_path.c_str(), WMU_STARTMSGID, FALSE))
	{
		if (0 == nCurSel || 2 == nCurSel)
			AfxMessageBox(_T("모의투자로 접속합니다.\n모의투자의 계좌비밀번호는 0000입니다."));

		LOGINFO(CMyLogger::getInstance(), _T("유안타 Open API가 초기화되었습니다."));

		g_bInitial = TRUE;
	}
	else
	{
		LOGINFO(CMyLogger::getInstance(), _T("유안타 Open API가 초기화에 실패하였습니다."), 1);

		return;
	}
}

BOOL YaClient::CheckInit()
{
	if (FALSE == g_bInitial)
	{
		AfxMessageBox("유안타 Open API 초기화가 필요합니다.", 1);
	}

	return g_bInitial;
}

BOOL YaClient::CheckLogin()
{
	if (FALSE == g_bLogin)
	{
		AfxMessageBox("로그인이 필요합니다.", 1);
	}

	return g_bLogin;
}

void YaClient::log_in(const std::string& id, const std::string& pwd, const std::string& cert)
{
	if (CheckInit())
	{
		long nResult = g_iYuantaAPI.YOA_Login(GetSafeHwnd(), id.c_str(), pwd.c_str(), cert.c_str());
		if (RESULT_SUCCESS == nResult)
		{
			LOGINFO(CMyLogger::getInstance(), _T("로그인 요청이 되었습니다."));
		}
		else
		{
			LOGINFO(CMyLogger::getInstance(), _T("로그인 요청이 실패하였습니다."));
			TCHAR msg[2048] = { 0, };
			g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));
			LOGINFO(CMyLogger::getInstance(), msg);
		}
	}
}


void YaClient::OnDestroy()
{
	CDialog::OnDestroy();

	g_iYuantaAPI.YOA_UnInitial();
}

int YaClient::dm_symbol_profit_loss(DhTaskArg arg)
{
	return 1;
}

void YaClient::dm_new_order(const std::shared_ptr<OrderRequest>& order_req)
{
	ya_request_map_.clear();
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_ORDER_NEW)];
	const std::string trade_code = req_info.dso_name.substr(3);
	
	CString format_price;
	format_price.Format("%.2f", static_cast<double>(static_cast<double>(order_req->order_price) / 100.0));
	std::string order_price = std::string(static_cast<const char*>(format_price));
	

	g_iYuantaAPI.YOA_SetTRFieldString(_T("160001"), _T("InBlock1"), _T("acnt_aid"), order_req->account_no.c_str(), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160001"), _T("InBlock1"), _T("passwd"), order_req->password.c_str(), 0);		// 비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160001"), _T("InBlock1"), _T("jumun_gubun"), order_req->position_type == SmPositionType::Sell ? "1" : "2", 0);		// 주문구분1매도2매수 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160001"), _T("InBlock1"), _T("meme_gubun"), order_req->price_type == SmPriceType::Price ? "L" : "M", 0);		// 매매구분L지정M시장C조건부B최유 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160001"), _T("InBlock1"), _T("jong_code"), order_req->symbol_code.c_str(), 0);		// 종목코드 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldLong(_T("160001"), _T("InBlock1"), _T("order_cnt"), order_req->order_amount, 0);		// 주문수량 값을 설정합니다.

	g_iYuantaAPI.YOA_SetTRFieldString(_T("160001"), _T("InBlock1"), _T("order_price"), order_price.c_str(), 0);		// 주문가격 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160001"), _T("InBlock1"), _T("jang_gubun"), std::to_string(order_req->future_or_option).c_str(), 0);		// 선물옵션구분0선물1옵션2개별3코 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160001"), _T("InBlock1"), _T("futu_ord_if"), _T("S"), 0);		// 주문조건S일반I일부충족F전량충족 값을 설정합니다.

	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: 시그널이름[%s]"), order_req->order_context.signal_name.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: 계좌번호[%s]"), order_req->account_no.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: 비밀번호[%s]"), order_req->password.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: 종목코드[%s]"), order_req->symbol_code.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: 매수/매도[%s]"), std::to_string((int)order_req->position_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: 주문가격[%s]"), order_price.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: 주문수량[%s]"), std::to_string((int)order_req->order_amount).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: 주문타입[%s]"), std::to_string((int)order_req->order_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: 가격타입[%s]"), std::to_string((int)order_req->price_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: 선물/옵션[%s]"), std::to_string((int)order_req->future_or_option).c_str());

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 국내선물 신규주문 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		//request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]국내선물 신규주문 요청중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		//on_task_request_error(arg.argument_id);
		//return -1;
	}

	//return 1;
}

void YaClient::dm_change_order(const std::shared_ptr<OrderRequest>& order_req)
{
	ya_request_map_.clear();
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_ORDER_MOD)];
	const std::string trade_code = req_info.dso_name.substr(3);
	//g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160003"), _T("InBlock1"), _T("acnt_aid"), order_req->account_no.c_str(), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160003"), _T("InBlock1"), _T("passwd"), order_req->password.c_str(), 0);		// 비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160003"), _T("InBlock1"), _T("meme_gubun"), order_req->price_type == SmPriceType::Price ? "L" : "M", 0);		// 매매구분L지정M시장C조건부B최유 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160003"), _T("InBlock1"), _T("org_order_no"), order_req->original_order_no.c_str(), 0);		// 원주문번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldLong(_T("160003"), _T("InBlock1"), _T("order_cnt"), order_req->order_amount, 0);		// 정정주문수량 값을 설정합니다.

	CString format_price;
	format_price.Format("%.2f", static_cast<double>(order_req->order_price / 100.0));
	std::string order_price = std::string(CT2CA(format_price));

	g_iYuantaAPI.YOA_SetTRFieldString(_T("160003"), _T("InBlock1"), _T("order_price"), order_price.c_str(), 0);		// 주문가격 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160003"), _T("InBlock1"), _T("jang_gubun"), std::to_string(order_req->future_or_option).c_str(), 0);		// 선물옵션구분0선물1옵션2개별3코 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160003"), _T("InBlock1"), _T("futu_ord_if"), _T("S"), 0);		// 주문조건S일반I일부충족F전량충족 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 국내선물 정정주문 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		//request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]국내선물 정정주문 요청중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		//on_task_request_error(arg.argument_id);
		//return -1;
	}
}

void YaClient::dm_cancel_order(const std::shared_ptr<OrderRequest>& order_req)
{
	ya_request_map_.clear();

	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 시그널이름[%s]"), order_req->order_context.signal_name.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 해외/국내[%s]"), std::to_string((int)order_req->request_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 계좌번호[%s]"), order_req->account_no.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 비밀번호[%s]"), order_req->password.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 원주문번호[%s]"), order_req->original_order_no.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 종목코드[%s]"), order_req->symbol_code.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 매수/매도[%s]"), std::to_string((int)order_req->position_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 주문가격[%s]"), std::to_string((int)order_req->order_price).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 주문수량[%s]"), std::to_string((int)order_req->order_amount).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 주문타입[%s]"), std::to_string((int)order_req->order_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 가격타입[%s]"), std::to_string((int)order_req->price_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 선물/옵션[%s]"), std::to_string((int)order_req->future_or_option).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 주문조건[%s]"), std::to_string((int)order_req->fill_condition).c_str());


	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_ORDER_CANCEL)];
	const std::string trade_code = req_info.dso_name.substr(3);
	//g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160004"), _T("InBlock1"), _T("acnt_aid"), order_req->account_no.c_str(), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160004"), _T("InBlock1"), _T("passwd"), order_req->password.c_str(), 0);		// 비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("160004"), _T("InBlock1"), _T("org_order_no"), order_req->original_order_no.c_str(), 0);		// 원주문번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldLong(_T("160004"), _T("InBlock1"), _T("order_cnt"), order_req->order_amount, 0);		// 취소주문수량 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 국내선물 취소주문 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		//request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]국내선물 취소주문 요청중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		//on_task_request_error(arg.argument_id);
		//return -1;
	}
}

int YaClient::dm_order_filled(DhTaskArg arg)
{
	const std::string& account_no = arg.parameter_map["account_no"];
	const std::string& password = arg.parameter_map["password"];
	const std::string& start_date = arg.parameter_map["start_date"];
	const int& order_type = arg.parameter_map["order_type"];
	const int& qry_type = arg.parameter_map["qry_type"];
	const int& work_type = arg.parameter_map["work_type"];

	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_ORDER_FILLED)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	g_iYuantaAPI.YOA_SetFieldString( _T("kyejwa"), account_no.c_str());
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), password.c_str());
	g_iYuantaAPI.YOA_SetFieldString( _T("date"), start_date.c_str());
	g_iYuantaAPI.YOA_SetFieldByte(_T("ord_tp"), order_type);
	g_iYuantaAPI.YOA_SetFieldByte(_T("qry_tp"), qry_type);
	g_iYuantaAPI.YOA_SetFieldByte(_T("work_tp"), work_type);

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 선물옵션주문체결 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]선물옵션주문체결 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_order_orderable(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("250012"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("입력값"), 0);		// 계좌비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("jang_gubun"), _T("입력값"), 0);		// 계좌상품구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("jong_code"), _T("입력값"), 0);		// 종목코드 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("buy_sell_tp"), _T("입력값"), 0);		// 매도매수구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("ord_tp"), _T("입력값"), 0);		// 주문유형구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("qry_tp"), _T("입력값"), 0);		// 조회구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("order_qty"), _T("입력값"), 0);		// 주문수량 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("order_idx"), _T("입력값"), 0);		// 주문지수 값을 설정합니다.

	return 1;
}

int YaClient::dm_account_asset(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_ASSET)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), password.c_str(), 0);		// 계좌비밀번호 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 자산정보 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], account_no[%s], Request : %s", trade_code.c_str(), account_no.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]자산정보 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		auto account = mainApp.AcntMgr()->FindAccount(account_no);
		if (account) {
			const int account_id = account->id();
			account->Confirm(0);
			mainApp.CallbackMgr()->OnPasswordConfirmed(account_id, 0);
			return 1;
		}

		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_provisional_settlement(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("입력값"), 0);		// 계좌비밀번호 값을 설정합니다.
	return 1;
}

int YaClient::dm_accepted_order(DhTaskArg arg)
{
	/*
	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("입력값"), 0);		// 계좌비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("date"), _T("입력값"), 0);		// 조회시작일 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("ord_tp"), _T("입력값"), 0);		// 순서구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("work_tp"), _T("입력값"), 0);		// 업무구분 값을 설정합니다.
	*/
	//return 1;


	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_ACCEPTED)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), password.c_str(), 0);		// 계좌비밀번호 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 미체결 주문 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], account_no[%s], Request : %s", trade_code.c_str(), account_no.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]미체결 주문 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		auto account = mainApp.AcntMgr()->FindAccount(account_no);
		if (account) {
			const int account_id = account->id();
			account->Confirm(0);
			mainApp.CallbackMgr()->OnPasswordConfirmed(account_id, 0);
			return 1;
		}

		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_symbol_position(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_POSITION_INFO)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldByte(_T("work_tp"), 1);		// 업무구분 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 계좌별 포지션 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]계좌별 포지션 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		

		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_daily_profit_loss(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_DAILY_PROFIT_LOSS)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), password.c_str(), 0);		// 비밀번호 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 계좌별 일일 손익 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]계좌별 일일 손익조회 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		

		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_liquidable_qty(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("251007"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("입력값"), 0);		// 비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("qry_tp"), _T("입력값"), 0);		// 조회구분 값을 설정합니다.
	return 1;
}

int YaClient::dm_trade_profit_loss(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_TRADE_PROFIT_LOSS)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// 계좌 값을 설정합니다.
	//g_iYuantaAPI.YOA_SetFieldByte(_T("work_tp"), 1);		// 업무구분 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 계좌별 실현손익과수수료 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]계좌별 계좌별 실현손익과수수료 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		

		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_outstanding_order(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_OUTSTANDING_ORDER)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldByte(_T("work_tp"), 1);		// 업무구분 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 계좌별 미체결 주문 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]계좌별 미체결 주문 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		
		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_fut_sise(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_FUT_SISE)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// 계좌 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 국내선물시세 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]국내선물시세 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		

		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_fut_hoga(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_FUT_HOGA)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// 계좌 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 국내선물 호가 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]국내선물 호가 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_opt_sise(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_OPT_SISE)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// 계좌 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 국내 옵션시세 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]국내 옵션시세  조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_opt_hoga(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_OPT_HOGA)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// 계좌 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 국내 옵션 호가 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]국내 옵션 호가 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_commodity_sise(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_COMMODITY_SISE)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// 계좌 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 상품선물 시세 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]상품선물 시세 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_commodity_hoga(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_COMMODITY_HOGA)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// 계좌 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 상품선물 호가 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]상품선물 호가 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_chart_n_min(DhTaskArg arg)
{
	return 1;
}

void YaClient::init_ya_req_info_list()
{
	YA_REQ_INFO req_info;
	req_info.desc = "선물옵션주문체결";
	req_info.dso_name = "DSO250009";
	req_info.req = SERVER_REQ::DM_ORDER_FILLED;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵션주문가능수량조회_수량";
	req_info.dso_name = "DSO250012";
	req_info.req = SERVER_REQ::DM_ORDER_ORDERABLE;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵션증거금상세내역";
	req_info.dso_name = "DSO250013";
	req_info.req = SERVER_REQ::DM_ASSET;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵션가정산예수현황";
	req_info.dso_name = "DSO250014";
	req_info.req = SERVER_REQ::DM_PROVISIONAL_SETTLEMENT;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵션 미체결 조회";
	req_info.dso_name = "DSO250031";
	req_info.req = SERVER_REQ::DM_ACCEPTED;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵션 잔고 조회";
	req_info.dso_name = "DSO250032";
	req_info.req = SERVER_REQ::DM_POSITION_INFO;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵션당일평가";
	req_info.dso_name = "DSO251002";
	req_info.req = SERVER_REQ::DM_DAILY_PROFIT_LOSS;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵션청산가능수량";
	req_info.dso_name = "DSO251007";
	req_info.req = SERVER_REQ::DM_LIQUIDABLE_QTY;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵션실현손익과수수료조회";
	req_info.dso_name = "DSO251009";
	req_info.req = SERVER_REQ::DM_TRADE_PROFIT_LOSS;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "미결제약정정보조회";
	req_info.dso_name = "DSO280002";
	req_info.req = SERVER_REQ::DM_OUTSTANDING_ORDER;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물현재가";
	req_info.dso_name = "DSO350001";
	req_info.req = SERVER_REQ::DM_FUT_SISE;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물현재가_호가체결";
	req_info.dso_name = "DSO350002";
	req_info.req = SERVER_REQ::DM_FUT_HOGA;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "옵션현재가";
	req_info.dso_name = "DSO360001";
	req_info.req = SERVER_REQ::DM_OPT_SISE;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "옵션현재가_호가체결";
	req_info.dso_name = "DSO360002";
	req_info.req = SERVER_REQ::DM_OPT_HOGA;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "상품선물현재가";
	req_info.dso_name = "DSO391001";
	req_info.req = SERVER_REQ::DM_COMMODITY_SISE;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "상품선물현재가_호가체결";
	req_info.dso_name = "DSO391002";
	req_info.req = SERVER_REQ::DM_COMMODITY_HOGA;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "챠트n분봉";
	req_info.dso_name = "DSO402001";
	req_info.req = SERVER_REQ::DM_CHART_N_MIN;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵션신규주문";
	req_info.dso_name = "DSO160001";
	req_info.req = SERVER_REQ::DM_ORDER_NEW;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵셥정정주문";
	req_info.dso_name = "DSO160003";
	req_info.req = SERVER_REQ::DM_ORDER_MOD;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵션취소주문";
	req_info.dso_name = "DSO160004";
	req_info.req = SERVER_REQ::DM_ORDER_CANCEL;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "미니선물현재가";
	req_info.dso_name = "DSO368001";
	req_info.req = SERVER_REQ::DM_FUT_MINI_SISE;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "미니선물현재가_호가체결";
	req_info.dso_name = "DSO368002";
	req_info.req = SERVER_REQ::DM_FUT_MINI_HOGA;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "코스피옵션_월물시세";
	req_info.dso_name = "DSO361007";
	req_info.code = "201";
	req_info.req = SERVER_REQ::DM_OPTION_MONTH_QUOTE_KOSPI;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "미니코스피옵션_월물시세";
	req_info.dso_name = "DSO389004";
	req_info.code = "205";
	req_info.req = SERVER_REQ::DM_OPTION_MONTH_QUOTE_MINI_KOSPI;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "위클리옵션_월물시세";
	req_info.dso_name = "DSO389104";
	req_info.code = "209";
	req_info.req = SERVER_REQ::DM_OPTION_MONTH_QUOTE_WEEKLY;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "코스닥옵션_월물시세";
	req_info.dso_name = "DSO384007";
	req_info.code = "206";
	req_info.req = SERVER_REQ::DM_OPTION_MONTH_QUOTE_KOSDAQ;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "해외선물_자산정보";
	req_info.dso_name = "DSO863007";
	req_info.code = "";
	req_info.req = SERVER_REQ::AB_ACCOUNT_ASSET;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "해외선물_계좌별손익";
	req_info.dso_name = "DSO861001";
	req_info.code = "";
	req_info.req = SERVER_REQ::AB_ACCOUNT_PROFIT_LOSS;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "해외선물_종목별손익";
	req_info.dso_name = "DSO861001";
	req_info.code = "";
	req_info.req = SERVER_REQ::AB_SYMBOL_PROFIT_LOSS;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "해외선물_미체결목록";
	req_info.dso_name = "DSO860005";
	req_info.code = "";
	req_info.req = SERVER_REQ::AB_ACCEPTED_ORDER;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "해외선물_종목시세";
	req_info.dso_name = "DSO810001";
	req_info.code = "";
	req_info.req = SERVER_REQ::AB_SYMBOL_QUOTE;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "해외선물_종목호가";
	req_info.dso_name = "DSO810002";
	req_info.code = "";
	req_info.req = SERVER_REQ::AB_SYMBOL_HOGA;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "해외선물_종목별잔고";
	req_info.dso_name = "DSO861001";
	req_info.code = "";
	req_info.req = SERVER_REQ::AB_SYMBOL_POSITION;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "해외선물_종목별거래손익";
	req_info.dso_name = "DSO862011";
	req_info.code = "";
	req_info.req = SERVER_REQ::AB_TRADE_PROFIT_LOSS;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "해외선물_신규주문";
	req_info.dso_name = "DSO850001";
	req_info.code = "";
	req_info.req = SERVER_REQ::AB_ORDER_NEW;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "해외선물_정정주문";
	req_info.dso_name = "DSO850002";
	req_info.code = "";
	req_info.req = SERVER_REQ::AB_ORDER_MOD;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "해외선물_취소주문";
	req_info.dso_name = "DSO850003";
	req_info.code = "";
	req_info.req = SERVER_REQ::AB_ORDER_CANCEL;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);
}

void YaClient::on_dm_symbol_profit_loss(const YA_REQ_INFO& req_info)
{
	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_order_filled(const YA_REQ_INFO& req_info)
{
	const std::string& trade_code = req_info.desc.substr(3);
	long list_cnt = 0;
	g_iYuantaAPI.YOA_GetTRFieldLong(trade_code.c_str(), _T("OutBlock1"), _T("list_cnt"), &list_cnt);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("OutBlock2"));
	TCHAR data[1024] = { 0, };
	for (long i = 0; i < list_cnt; i++) {
		g_iYuantaAPI.YOA_GetFieldString(_T("org_ord_no"), data, sizeof(data));
		const std::string org_ord_no = data;
		g_iYuantaAPI.YOA_GetFieldString(_T("ord_no"), data, sizeof(data));
		const std::string ord_no = data;
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_code"), data, sizeof(data));
		const std::string jong_code = data;
		g_iYuantaAPI.YOA_GetFieldString(_T("buy_sell_tp"), data, sizeof(data));
		const std::string buy_sell_tp = data;
		g_iYuantaAPI.YOA_GetFieldString(_T("ord_tp"), data, sizeof(data));
		const std::string ord_tp = data;
		g_iYuantaAPI.YOA_GetFieldString(_T("price_tp"), data, sizeof(data));
		const std::string price_tp = data;
		g_iYuantaAPI.YOA_GetFieldString(_T("order_idx"), data, sizeof(data));
		const std::string order_idx = data;
		g_iYuantaAPI.YOA_GetFieldString(_T("ord_qty"), data, sizeof(data));
		const std::string order_qty = data;
		g_iYuantaAPI.YOA_GetFieldString(_T("che_code"), data, sizeof(data));
		const std::string che_code = data;
		g_iYuantaAPI.YOA_GetFieldString(_T("che_buy_sell_tp"), data, sizeof(data));
		const std::string che_buy_sell_tp = data;
		g_iYuantaAPI.YOA_GetFieldString(_T("che_idx"), data, sizeof(data));
		const std::string che_idx = data;
		double che_qty = 0;
		g_iYuantaAPI.YOA_GetFieldDouble(_T("che_qty"), &che_qty);
		double che_remn_qty = 0;
		g_iYuantaAPI.YOA_GetFieldDouble(_T("che_remn_qty"), &che_remn_qty);
		g_iYuantaAPI.YOA_GetFieldString(_T("rcv_tp"), data, sizeof(data));
		const std::string rcv_tp = data;
		g_iYuantaAPI.YOA_GetFieldString(_T("rcv_time"), data, sizeof(data));
		const std::string rcv_time = data;
		g_iYuantaAPI.YOA_GetFieldString(_T("mdm_tp"), data, sizeof(data));
		const std::string mdm_tp = data;
	}

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_order_orderable(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("250012"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("ksq_brk_prof"), data, sizeof(data), 0);		// 위탁증거금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("ksq_brk_prof_cash"), data, sizeof(data), 0);		// 현금증거금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("new_qty"), data, sizeof(data), 0);		// 신규수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("clear_qty"), data, sizeof(data), 0);		// 청산수량 값을 가져옵니다.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_asset(const YA_REQ_INFO& req_info)
{
	if (mainApp.LoginMgr()->IsLoggedIn()) {
		auto found = request_map_.find(req_info.request_id);
		if (found != request_map_.end()) {
			const std::string account_no = found->second.parameter_map["account_no"];
			auto account = mainApp.AcntMgr()->FindAccount(account_no);
			if (account) {
				const int account_id = account->id();
				account->Confirm(1);
				mainApp.CallbackMgr()->OnPasswordConfirmed(account_id, 1);
			}
		}
	}

	nlohmann::json account_asset;

	TCHAR data[1024] = { 0, };
	g_iYuantaAPI.YOA_SetTRInfo(_T("250013"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// 계좌 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: 계좌[%s]"), data);
	account_asset["account_no"] = std::string(data);
	g_iYuantaAPI.YOA_SetTRInfo(_T("250013"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kyejwa"), data, sizeof(data), 0);		// 계좌 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: kyejwa[%s]"), data);
	

	memset(data, 0x00, sizeof(data));
	BYTE jang_gubun = 0;
	g_iYuantaAPI.YOA_GetFieldByte(_T("jang_gubun"), &jang_gubun, 0);		// 계좌상품구분 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250013"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tot"), data, sizeof(data), 0);// 예탁총액 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: 예탁총액[%s]"), data);
	account_asset["entrust_total"] = _ttof(data);
	account_asset["balance"] = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("deposittot"), data, sizeof(data), 0);// 위탁증거금총액 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: 위탁증거금총액[%s]"), data);
	
	account_asset["open_trust_total"] = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("orddeposit"), data, sizeof(data), 0);// 주문가능총증거금 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: 주문가능총증거금[%s]"), data);
	account_asset["order_deposit"] = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("drawtot"), data, sizeof(data), 0);// 인출가능총액 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: 인출가능총액[%s]"), data);
	account_asset["outstanding_deposit"] = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("keepdeposit"), data, sizeof(data), 0);// 유지증거금총액 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: 유지증거금총액[%s]"), data);
	account_asset["entrust_deposit"] = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("addtot"), data, sizeof(data), 0);// 추가증거금총액 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: 추가증거금총액[%s]"), data);
	account_asset["additional_margin"] = _ttof(data);
	account_asset["currency"] = "KRW";
	
	
	
	
	account_asset["maintenance_margin"] = 0;
	account_asset["settled_profit_loss"] = 0;
	account_asset["fee"] = 0;
	account_asset["open_profit_loss"] = 0;
	
	account_asset["order_margin"] = 0;
	

	mainApp.AcntMgr()->on_account_asset(std::move(account_asset));


	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_provisional_settlement(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("yesu_yetak_tot"), data, sizeof(data), 0);		// 예수금예탁총액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("yesu_cash"), data, sizeof(data), 0);		// 예수금현금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("yesu_daeyoung"), data, sizeof(data), 0);		// 예수금대용 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("futjungsan_yt"), data, sizeof(data), 0);		// 선물정산차금_예탁총액용 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("optmaesu_yt"), data, sizeof(data), 0);		// 옵션매수대금_예탁총액용 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("optmaedo_yt"), data, sizeof(data), 0);		// 옵션매도대금_예탁총액용 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("b_n11_1"), data, sizeof(data), 0);		// 행사배정차금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("b_n12_1"), data, sizeof(data), 0);		// 행사배정대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("susuryo_yt"), data, sizeof(data), 0);		// 수수료_예탁총액용 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("futjungsan_cash"), data, sizeof(data), 0);		// 선물정산차금_현금용 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("optmaesu_cash"), data, sizeof(data), 0);		// 옵션매수대금_현금용 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("optmaedo_cash"), data, sizeof(data), 0);		// 옵션매도대금_현금용 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("b_n11_2"), data, sizeof(data), 0);		// 행사배정차금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("b_n12_2"), data, sizeof(data), 0);		// 행사배정대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("susuryo_cash"), data, sizeof(data), 0);		// 수수료_현금용 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("OutBlock4"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("yesterdaeyoung_yt"), data, sizeof(data), 0);		// 전일대용매도금액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("todaydaeyoung_yt"), data, sizeof(data), 0);		// 당일대용매도금액 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("OutBlock5"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("yesterdaeyoung_cash"), data, sizeof(data), 0);		// 전일대용매도금액_현금용 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("todaydaeyoung_cash"), data, sizeof(data), 0);		// 당일대용매도금액_현금용 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("OutBlock6"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("gajung_yetak_tot"), data, sizeof(data), 0);		// 가정산예수금_예탁총액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("gajung_cash"), data, sizeof(data), 0);		// 가정산예수금_현금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("gajung_daeyoung"), data, sizeof(data), 0);		// 가정산예수금_대용 값을 가져옵니다.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_accepted_order(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// 계좌번호 값을 가져옵니다.
	const std::string account_no = data;
	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// 총조회건수 값을 가져옵니다.
	const int list_cnt = _ttoi(data);
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: 총조회건수[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jang_gubun"), data, sizeof(data), 0);		// 계좌구분 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: 계좌구분[%s]"), data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < list_cnt; i++) {
		nlohmann::json order_info;
		order_info["account_no"] = account_no;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: 계좌번호[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("ord_no"), data, sizeof(data), i);		// 주문번호 값을 가져옵니다.
		std::string order_no = data;
		VtStringUtil::trim(order_no);
		order_info["order_no"] = order_no;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: 주문번호[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_code"), data, sizeof(data), i);		// 종목코드 값을 가져옵니다.
		order_info["symbol_code"] = data;
		const std::string symbol_code = data;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: 종목코드[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_name"), data, sizeof(data), i);		// 종목명 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: 종목명[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("buy_sell_tp"), data, sizeof(data), i);		// 매도매수구분 값을 가져옵니다.
		std::string buy_sell_tp = data;
		auto npos = buy_sell_tp.find("매도");
		if (npos != std::string::npos) {
			buy_sell_tp = "2";
		}
		else {
			buy_sell_tp = "1";
		}
		order_info["position_type"] = buy_sell_tp;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: 매도매수구분[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("order_qty"), data, sizeof(data), i);		// 주문수량 값을 가져옵니다.
		const int order_amount = _ttoi(data);
		order_info["order_amount"] = order_amount;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: 주문수량[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("order_idx"), data, sizeof(data), i);		// 주문지수 값을 가져옵니다.
		order_info["order_price"] = convert_to_int(symbol_code, data);
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: 주문지수[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("che_remn_qty"), data, sizeof(data), i);		// 미체결잔량 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: 미체결잔량[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("ord_tp"), data, sizeof(data), i);		// 주문유형 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: 주문유형[%s]"), data);


		order_info["order_event"] = OrderEvent::OE_Accepted;
		order_info["order_time"] = "00:00:00";
		order_info["order_date"] = "20231020";
		order_info["order_type"] = "1";
		//order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
		order_info["original_order_no"] = "";
		order_info["first_order_no"] = "";
		//order_info["order_type"] = static_cast<const char*>(strMan.Trim());
		order_info["remain_count"] = order_amount;
		order_info["cancelled_count"] = 0;
		order_info["modified_count"] = 0;
		order_info["filled_count"] = 0;
		order_info["order_sequence"] = 1;
		order_info["custom_info"] = "";

		mainApp.total_order_manager()->on_order_event(std::move(order_info));
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("next"), data, sizeof(data), 0);		// 다음버튼 값을 가져옵니다.
	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_symbol_position(const YA_REQ_INFO& req_info)
{

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("250032"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// 계좌번호 값을 가져옵니다.
	const std::string account_no = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 계좌번호[%s]"), data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("250032"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// 총조회건수 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 총조회건수[%s]"), data);

	const int list_cnt = _ttoi(data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("250032"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < list_cnt; i++) {
		nlohmann::json symbol_position;
		symbol_position["account_no"] = account_no;
		symbol_position["account_name"] = "";
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
		const std::string symbol_code = data;
		symbol_position["symbol_code"] = symbol_code;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 종목코드[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_name"), data, sizeof(data), 0);		// 종목명 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 종목명[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("buy_sell_tp"), data, sizeof(data), 0);		// 매매구분 값을 가져옵니다.
		const std::string buy_sell_tp = data;
		symbol_position["symbol_position"] = buy_sell_tp == "매수" ? 1 : -1;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 매매구분[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("avg_price"), data, sizeof(data), 0);		// 평균가 값을 가져옵니다.
		symbol_position["symbol_avg_price"] = convert_to_int(symbol_code, data);
		symbol_position["symbol_unit_price"] = convert_to_int(symbol_code, data);
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 평균가[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("cur_price"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 현재가[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("cnt"), data, sizeof(data), 0);		// 보유수량 값을 가져옵니다.
		symbol_position["symbol_open_qty"] = _ttoi(data);
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 보유수량[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("cntclear"), data, sizeof(data), 0);		// 청산가능수량 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 청산가능수량[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("eval_amt"), data, sizeof(data), 0);		// 평가금액 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 평가금액[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("eval_pl"), data, sizeof(data), 0);		// 평가손익 값을 가져옵니다.
		symbol_position["symbol_open_profit_loss"] = _ttof(data);
		symbol_position["symbol_pre_open_qty"] = 0;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 평가손익[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("rate"), data, sizeof(data), 0);		// 수익률 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 수익률[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("unit"), data, sizeof(data), 0);		// 종목별단위금액 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 종목별단위금액[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("autocode"), data, sizeof(data), 0);		// 오토시세용종목코드 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 오토시세용종목코드[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("meaip"), data, sizeof(data), 0);		// 매입가액 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 매입가액[%s]"), data);

		mainApp.total_position_manager()->on_symbol_position(std::move(symbol_position));
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("250032"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("next"), data, sizeof(data), 0);		// 다음여부 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("fut_eval_pl"), data, sizeof(data), 0);		// 선물평가손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opt_eval_pl"), data, sizeof(data), 0);		// 옵션평가손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tot_eval_pl"), data, sizeof(data), 0);		// 총평가손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tot_rate"), data, sizeof(data), 0);		// 총평가손익 값을 가져옵니다.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);	
}

void YaClient::on_dm_daily_profit_loss(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("251002"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// 계좌번호 값을 가져옵니다.
	const std::string account_no = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: 계좌번호[%s]"), data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("251002"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// 총조회건수 값을 가져옵니다.
	const int list_cnt = _ttoi(data);

	LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 총조회건수[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jang_gubun"), data, sizeof(data), 0);		// 시장구분 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 시장구분[%s]"), data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("251002"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < list_cnt; i++) {

		nlohmann::json symbol_profit_loss;
		symbol_profit_loss["account_no"] = account_no;
		symbol_profit_loss["account_name"] = "";
		symbol_profit_loss["currency"] = "KRW";
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
		const std::string symbol_code = data;
		symbol_profit_loss["symbol_code"] = symbol_code;
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 종목코드[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_name"), data, sizeof(data), 0);		// 종목명 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 종목명[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("buy_sell_tp"), data, sizeof(data), 0);		// 매수/매도구분 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 매수/매도구분[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("now_amount"), data, sizeof(data), 0);		// 보유잔량 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 보유잔량[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("buy_price"), data, sizeof(data), 0);		// 매입가 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 매입가[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("total_buy_price"), data, sizeof(data), 0);		// 매입가액 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 매입가액[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("now_price"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 현재가[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("total_now_price"), data, sizeof(data), 0);		// 현재가액 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 현재가액[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 매도호가[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// 매도잔량 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 매도잔량[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 매수호가[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// 매수잔량 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 매수잔량[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// 약정수량 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 약정수량[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("openvol"), data, sizeof(data), 0);		// 미결제약정수량 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 미결제약정수량[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("maemae_son"), data, sizeof(data), 0);		// 매매손익 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 매매손익[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("clear_son"), data, sizeof(data), 0);		// 청산손익 값을 가져옵니다.
		symbol_profit_loss["trade_profit_loss"] = _ttof(data);
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 청산손익[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("total_son"), data, sizeof(data), 0);		// 총손익 값을 가져옵니다.
		symbol_profit_loss["pure_trade_profit_loss"] = _ttof(data);
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 총손익[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("unit"), data, sizeof(data), 0);		// 종목별단위금액 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: 종목별단위금액[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("autocode"), data, sizeof(data), 0);		// 오토시세용종목코드 값을 가져옵니다.

		symbol_profit_loss["trade_fee"] = 0.0f;
		
		symbol_profit_loss["open_profit_loss"] = 0.0f;

		mainApp.total_position_manager()->on_symbol_profit_loss(std::move(symbol_profit_loss));
	}

	mainApp.total_position_manager()->update_account_profit_loss(account_no);

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_liquidable_qty(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("251007"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// 총조회건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("251007"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jong_code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jong_name"), data, sizeof(data), 0);		// 종목명 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("result_tp"), data, sizeof(data), 0);		// 결과구분 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("qty"), data, sizeof(data), 0);		// 보유수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("clear_qty"), data, sizeof(data), 0);		// 청산가능수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("avg_price"), data, sizeof(data), 0);		// 평균가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("miche_qty"), data, sizeof(data), 0);		// 미체결수량 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("251007"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("next"), data, sizeof(data), 0);		// 다음여부 값을 가져옵니다.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_trade_profit_loss(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	nlohmann::json trade_profit_loss;

	g_iYuantaAPI.YOA_SetTRInfo(_T("251009"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// 계좌번호 값을 가져옵니다.
	const std::string account_no = data;
	trade_profit_loss["account_no"] = account_no;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_trade_profit_loss:: 계좌번호[%s]"), data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("251009"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sonik"), data, sizeof(data), 0);		// 실현손익 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_trade_profit_loss:: 실현손익[%s]"), data);
	trade_profit_loss["trade_profit_loss"] = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cmsn"), data, sizeof(data), 0);		// 수수료 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_trade_profit_loss:: 수수료[%s]"), data);
	trade_profit_loss["trade_fee"] = _ttof(data);


	mainApp.total_position_manager()->on_trade_profit_loss(std::move(trade_profit_loss));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_outstanding_order(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("280002"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongcode"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medojango"), data, sizeof(data), 0);		// 매도미결제잔고 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesujango"), data, sizeof(data), 0);		// 매수미결제잔고 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medomoney"), data, sizeof(data), 0);		// 매도약정금액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesumoney"), data, sizeof(data), 0);		// 매수약정금액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoopen"), data, sizeof(data), 0);		// 매도미체결잔고 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuopen"), data, sizeof(data), 0);		// 매수미체결잔고 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("curprice"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("profit"), data, sizeof(data), 0);		// 평가손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jangbuga"), data, sizeof(data), 0);		// 장부가 값을 가져옵니다.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_fut_sise(const YA_REQ_INFO& req_info)
{
	auto found = request_map_.find(req_info.request_id);
	if (found == request_map_.end()) {
		return;
	}
	const std::string symbol_code_origin = found->second.parameter_map["symbol_code"];
	std::string temp = symbol_code_origin;

	LOGINFO(CMyLogger::getInstance(), _T("on_req_dm_fut_sise:: symbol code[%s]"), symbol_code_origin.c_str());

	TCHAR data[1024] = { 0, };
	nlohmann::json quote;

	g_iYuantaAPI.YOA_SetTRInfo(_T("350001"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	quote["symbol_code"] = data;
	const std::string symbol_code = data;
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	g_iYuantaAPI.YOA_SetTRInfo(_T("350001"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongname"), data, sizeof(data), 0);		// 종목코드설명 값을 가져옵니다.
	quote["symbol_name_kr"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data), 0);		// 등락율 값을 가져옵니다.
	quote["updown_rate"] = data;
	double updown_rate = _ttof(data);
	if (updown_rate > 0) {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		quote["delta_day_sign"] = "-";
		quote["up_down"] = -1;
	}
	else {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 0;
	}
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("startdebi"), data, sizeof(data), 0);		// 시가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highdebi"), data, sizeof(data), 0);		// 고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// 고가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	quote["time"] = data;
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
	quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowdebi"), data, sizeof(data), 0);		// 저가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowtime"), data, sizeof(data), 0);		// 저가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice"), data, sizeof(data), 0);		// 상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice"), data, sizeof(data), 0);		// 하한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxcb"), data, sizeof(data), 0);		// cb상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mincb"), data, sizeof(data), 0);		// cb하한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospi"), data, sizeof(data), 0);		// 코스피 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospidebi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospirate"), data, sizeof(data), 0);		// 코스피등락 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theoreticalprice"), data, sizeof(data), 0);		// 이론가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rate"), data, sizeof(data), 0);		// 괴리율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basist"), data, sizeof(data), 0);		// 이론베이시스 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basism"), data, sizeof(data), 0);		// 시장베이시스 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// 약정수량 값을 가져옵니다.
	quote["volume"] = _ttoi(data);
	quote["cumulative_amount"] = _ttoi(data);
	// 한글 테스트 
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("svolume"), data, sizeof(data), 0);		// 스프레드수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("money"), data, sizeof(data), 0);		// 약정대금(백만) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("smoney"), data, sizeof(data), 0);		// 스프레드대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// 미결제약정 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevvolume"), data, sizeof(data), 0);		// 전일약정수량 값을 가져옵니다.
	quote["preday_volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sprevvolume"), data, sizeof(data), 0);		// 전일스프레드수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevmoney"), data, sizeof(data), 0);		// 잔일약정대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sprevmoney"), data, sizeof(data), 0);		// 전일스프레드대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// 전일미결제약정대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopeninterest"), data, sizeof(data), 0);		// 순미결제약정 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopendebi"), data, sizeof(data), 0);		// 전일순미결제약정대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendate"), data, sizeof(data), 0);		// 거래개시일 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lastdate"), data, sizeof(data), 0);		// 최종거래일 값을 가져옵니다.
	if (symbol) symbol->LastTradeDay(data);
	if (symbol) symbol->ExpireDate(data);

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("remainday"), data, sizeof(data), 0);		// 잔존일수 값을 가져옵니다.
	if (symbol) symbol->RemainDays(_ttoi(data));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highprice"), data, sizeof(data), 0);		// 최고가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowprice"), data, sizeof(data), 0);		// 최저가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebi"), data, sizeof(data), 0);		// 최고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebi"), data, sizeof(data), 0);		// 최저가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebirate"), data, sizeof(data), 0);		// 최고가대비율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebirate"), data, sizeof(data), 0);		// 최저가대비율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedate"), data, sizeof(data), 0);		// 최고일자 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedate"), data, sizeof(data), 0);		// 최저일자 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("weightedavg"), data, sizeof(data), 0);		// 가중평균가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cdinterest"), data, sizeof(data), 0);		// CD금리 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mememulti"), data, sizeof(data), 0);		// 거래승수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("restriction_gbn"), data, sizeof(data), 0);		// 실시간상한가적용여부 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice2"), data, sizeof(data), 0);		// 일중상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice2"), data, sizeof(data), 0);		// 일중하한가 값을 가져옵니다.


	ya_stock_client_.OnSymbolQuote(std::move(quote));
	
	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_fut_hoga(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };
	nlohmann::json hoga;
	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	const std::string symbol_code = data;
	hoga["symbol_code"] = symbol_code;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_fut_hoga:: symbol code[%s]"), symbol_code.c_str());
	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < 5; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), i);		// 매도호가 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), i);		// 매도호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), i);		// 매도호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_cnt"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), i);		// 매수호가 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), i);		// 매수호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), i);		// 매수호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_cnt"] = _ttoi(data);
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// 호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// 호가건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// 호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// 호가건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock4"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// 약정시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 약정가격 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("nowvol"), data, sizeof(data), 0);		// 약정수량 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock5"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime"), data, sizeof(data), 0);		// 호가시간 값을 가져옵니다.
	hoga["hoga_time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 총매도잔량 값을 가져옵니다.
	hoga["tot_sell_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 총매수잔량 값을 가져옵니다.
	hoga["tot_buy_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// 총매도건수 값을 가져옵니다.
	hoga["tot_sell_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// 총매수건수 값을 가져옵니다.
	hoga["tot_buy_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// 호가시간2 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dongsi"), data, sizeof(data), 0);		// 동시구분 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectjuka"), data, sizeof(data), 0);		// 예상체결가 값을 가져옵니다.


	ya_stock_client_.OnDmSymbolHoga(std::move(hoga));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_opt_sise(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[360001] 옵션현재가 - 출력블록
	nlohmann::json quote;
	TCHAR data[1024] = { 0, };
	g_iYuantaAPI.YOA_SetTRInfo(_T("360001"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	quote["symbol_code"] = data;
	const std::string symbol_code = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_opt_sise:: symbol code[%s]"), symbol_code.c_str());
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	g_iYuantaAPI.YOA_SetTRInfo(_T("360001"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongname"), data, sizeof(data), 0);		// 종목명(설명) 값을 가져옵니다.
	quote["symbol_name_kr"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data), 0);		// 등락율 값을 가져옵니다.
	quote["updown_rate"] = data;
	double updown_rate = _ttof(data);
	if (updown_rate > 0) {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		quote["delta_day_sign"] = "-";
		quote["up_down"] = -1;
	}
	else {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 0;
	}
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("standardprice"), data, sizeof(data), 0);		// 기준가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("startdebi"), data, sizeof(data), 0);		// 시가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highdebi"), data, sizeof(data), 0);		// 고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// 고가시간 값을 가져옵니다.
	quote["time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
	quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowdebi"), data, sizeof(data), 0);		// 저가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowtime"), data, sizeof(data), 0);		// 저가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice"), data, sizeof(data), 0);		// 상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice"), data, sizeof(data), 0);		// 하한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospi"), data, sizeof(data), 0);		// 코스피 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospidebi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospirate"), data, sizeof(data), 0);		// 코스피등락 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theoreticalprice"), data, sizeof(data), 0);		// 이론가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volatility"), data, sizeof(data), 0);		// 내재변동성 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("introvalue"), data, sizeof(data), 0);		// 내재가치 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("delta"), data, sizeof(data), 0);		// 델타 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("gamma"), data, sizeof(data), 0);		// 감마 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theta"), data, sizeof(data), 0);		// 쎄타 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vega"), data, sizeof(data), 0);		// 베가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rho"), data, sizeof(data), 0);		// 로 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// 약정수량 값을 가져옵니다.
	quote["volume"] = _ttoi(data);
	quote["cumulative_amount"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("money"), data, sizeof(data), 0);		// 약정대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// 미결제약정 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevvolume"), data, sizeof(data), 0);		// 전일약정수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevmoney"), data, sizeof(data), 0);		// 잔일거래대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// 전일미결제약정대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevopen"), data, sizeof(data), 0);		// 전일미결제약정 값을 가져옵니다.
	quote["preday_volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highprice"), data, sizeof(data), 0);		// 최고가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebi"), data, sizeof(data), 0);		// 최고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebirate"), data, sizeof(data), 0);		// 최고가대비율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedate"), data, sizeof(data), 0);		// 최고일자 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowprice"), data, sizeof(data), 0);		// 최저가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebi"), data, sizeof(data), 0);		// 최저가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebirate"), data, sizeof(data), 0);		// 최저가대비율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedate"), data, sizeof(data), 0);		// 최저일자 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendate"), data, sizeof(data), 0);		// 거래개시일 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lastdate"), data, sizeof(data), 0);		// 최종거래일 값을 가져옵니다.
	if (symbol) symbol->LastTradeDay(data);
	if (symbol) symbol->ExpireDate(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("remainday"), data, sizeof(data), 0);		// 잔존일수 값을 가져옵니다.
	if (symbol) symbol->RemainDays(_ttoi(data));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("weightedavg"), data, sizeof(data), 0);		// 가중평균가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mememulti"), data, sizeof(data), 0);		// 거래승수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("restriction_gbn"), data, sizeof(data), 0);		// 실시간상한가적용여부 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice2"), data, sizeof(data), 0);		// 일중상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice2"), data, sizeof(data), 0);		// 일중하한가 값을 가져옵니다.

	ya_stock_client_.OnSymbolQuote(std::move(quote));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_opt_hoga(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[360002] 옵션현재가_호가체결 - 출력블록

	TCHAR data[1024] = { 0, };
	nlohmann::json hoga;
	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	const std::string symbol_code = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_opt_hoga:: symbol code[%s]"), symbol_code.c_str());
	hoga["symbol_code"] = symbol_code;
	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < 5; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), i);		// 매도호가 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), i);		// 매도호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), i);		// 매도호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_cnt"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), i);		// 매수호가 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), i);		// 매수호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), i);		// 매수호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_cnt"] = _ttoi(data);
	}
	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// 호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// 호가건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// 호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// 호가건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("OutBlock4"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// 약정시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 약정가격 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("nowvol"), data, sizeof(data), 0);		// 약정수량 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("OutBlock5"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime"), data, sizeof(data), 0);		// 호가시간 값을 가져옵니다.
	hoga["hoga_time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 총매도잔량 값을 가져옵니다.
	hoga["tot_sell_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 총매수잔량 값을 가져옵니다.
	hoga["tot_buy_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// 총매도건수 값을 가져옵니다.
	hoga["tot_sell_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// 총매수건수 값을 가져옵니다.
	hoga["tot_buy_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// 호가시간2 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dongsi"), data, sizeof(data), 0);		// 동시구분 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectjuka"), data, sizeof(data), 0);		// 예상체결가 값을 가져옵니다.

	ya_stock_client_.OnDmSymbolHoga(std::move(hoga));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_commodity_sise(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[391001] 상품선물현재가 - 출력블록

	TCHAR data[1024] = { 0, };
	nlohmann::json quote;
	g_iYuantaAPI.YOA_SetTRInfo(_T("391001"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	quote["symbol_code"] = data;
	const std::string symbol_code = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_commodity_sise:: symbol code[%s]"), symbol_code.c_str());
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_SetTRInfo(_T("391001"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongname"), data, sizeof(data), 0);		// 종목코드설명 값을 가져옵니다.
	quote["symbol_name_kr"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data), 0);		// 등락율 값을 가져옵니다.
	quote["updown_rate"] = data;
	double updown_rate = _ttof(data);
	if (updown_rate > 0) {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		quote["delta_day_sign"] = "-";
		quote["up_down"] = -1;
	}
	else {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 0;
	}
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("standardprice"), data, sizeof(data), 0);		// 기준가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("startdebi"), data, sizeof(data), 0);		// 시가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highdebi"), data, sizeof(data), 0);		// 고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// 고가시간 값을 가져옵니다.
	quote["time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
	quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowdebi"), data, sizeof(data), 0);		// 저가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowtime"), data, sizeof(data), 0);		// 저가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice"), data, sizeof(data), 0);		// 상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice"), data, sizeof(data), 0);		// 하한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxcb"), data, sizeof(data), 0);		// cb상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mincb"), data, sizeof(data), 0);		// cb하한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("baseid"), data, sizeof(data), 0);		// 현물코드 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basename"), data, sizeof(data), 0);		// 현물종목명 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("baseprice"), data, sizeof(data), 0);		// 현물현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basedebi"), data, sizeof(data), 0);		// 현물전일대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("baserate"), data, sizeof(data), 0);		// 현물등락율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theoreticalprice"), data, sizeof(data), 0);		// 이론가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rate"), data, sizeof(data), 0);		// 괴리율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basist"), data, sizeof(data), 0);		// 이론베이시스 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basism"), data, sizeof(data), 0);		// 시장베이시스 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// 약정수량 값을 가져옵니다.
	quote["volume"] = _ttoi(data);
	quote["cumulative_amount"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("svolume"), data, sizeof(data), 0);		// 스프레드수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("money"), data, sizeof(data), 0);		// 약정대금(백만) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("smoney"), data, sizeof(data), 0);		// 스프레드대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// 미결제약정 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevvolume"), data, sizeof(data), 0);		// 전일약정수량 값을 가져옵니다.
	quote["preday_volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sprevvolume"), data, sizeof(data), 0);		// 전일스프레드수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevmoney"), data, sizeof(data), 0);		// 잔일약정대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sprevmoney"), data, sizeof(data), 0);		// 전일스프레드대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// 전일미결제약정대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopeninterest"), data, sizeof(data), 0);		// 순미결제약정 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopendebi"), data, sizeof(data), 0);		// 전일순미결제약정대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendate"), data, sizeof(data), 0);		// 거래개시일 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lastdate"), data, sizeof(data), 0);		// 최종거래일 값을 가져옵니다.
	if (symbol) symbol->LastTradeDay(data);
	if (symbol) symbol->ExpireDate(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("remainday"), data, sizeof(data), 0);		// 잔존일수 값을 가져옵니다.
	if (symbol) symbol->RemainDays(_ttoi(data));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highprice"), data, sizeof(data), 0);		// 최고가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowprice"), data, sizeof(data), 0);		// 최저가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebi"), data, sizeof(data), 0);		// 최고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebi"), data, sizeof(data), 0);		// 최저가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebirate"), data, sizeof(data), 0);		// 최고가대비율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebirate"), data, sizeof(data), 0);		// 최저가대비율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedate"), data, sizeof(data), 0);		// 최고일자 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedate"), data, sizeof(data), 0);		// 최저일자 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("weightedavg"), data, sizeof(data), 0);		// 가중평균가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("restriction_gbn"), data, sizeof(data), 0);		// 실시간상한가적용여부 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice2"), data, sizeof(data), 0);		// 일중상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice2"), data, sizeof(data), 0);		// 일중하한가 값을 가져옵니다.

	ya_stock_client_.OnSymbolQuote(std::move(quote));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_commodity_hoga(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[391002] 상품선물현재가_호가체결 - 출력블록

	TCHAR data[1024] = { 0, };
	nlohmann::json hoga;
	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	const std::string symbol_code = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_commodity_hoga:: symbol code[%s]"), symbol_code.c_str());
	hoga["symbol_code"] = symbol_code;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < 5; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), i);		// 매도호가 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), i);		// 매도호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), i);		// 매도호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_cnt"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), i);		// 매수호가 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), i);		// 매수호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), i);		// 매수호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_cnt"] = _ttoi(data);
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// 호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// 호가건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// 호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// 호가건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("OutBlock4"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// 약정시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 약정가격 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("nowvol"), data, sizeof(data), 0);		// 약정수량 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("OutBlock5"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime"), data, sizeof(data), 0);		// 호가시간 값을 가져옵니다.
	hoga["hoga_time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 총매도잔량 값을 가져옵니다.
	hoga["tot_sell_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 총매수잔량 값을 가져옵니다.
	hoga["tot_buy_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// 총매도건수 값을 가져옵니다.
	hoga["tot_sell_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// 총매수건수 값을 가져옵니다.
	hoga["tot_buy_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// 호가시간2 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.

	ya_stock_client_.OnDmSymbolHoga(std::move(hoga));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_chart_n_min(const YA_REQ_INFO& req_info)
{
	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_new_order(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("160001"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("msg"), data, sizeof(data), 0);		// 메세지 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("org_no"), data, sizeof(data), 0);		// 원주문번호 값을 가져옵니다.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_change_order(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("160003"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("msg"), data, sizeof(data), 0);		// 메세지 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("org_no"), data, sizeof(data), 0);		// 원주문번호 값을 가져옵니다.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_cancel_order(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("160004"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("msg"), data, sizeof(data), 0);		// 메세지 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("org_no"), data, sizeof(data), 0);		// 원주문번호 값을 가져옵니다.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_task_complete(const int& server_request_id)
{
	auto it = request_map_.find(server_request_id);
	if (it == request_map_.end()) return;
	const int argument_id = it->second.argument_id;
	request_map_.erase(it);
	
	mainApp.ya_server_data_receiver()->on_task_complete(argument_id);
}

void YaClient::on_task_request_error(const int argument_id)
{
	mainApp.ya_server_data_receiver()->on_task_error(argument_id);
}

void YaClient::on_task_receive_error(const int server_request_id)
{
	auto found = request_map_.find(server_request_id);
	if (found == request_map_.end()) return;
	mainApp.ya_server_data_receiver()->on_task_error(found->second.argument_id);
}

int YaClient::Login(task_arg&& login_info)
{
	const std::string id = std::any_cast<const std::string>(login_info["id"]);
	const std::string pwd = std::any_cast<const std::string>(login_info["pwd"]);
	const std::string cert = std::any_cast<const std::string>(login_info["cert"]);

	log_in(id, pwd, cert);

	mainApp.LoginMgr()->SaveUserInfo(id, pwd, cert);

	return 1;
}

void YaClient::get_account_list()
{
	TCHAR account[64];
	TCHAR acctInfo[1024];
	TCHAR acct_gubun[1024];

	int nCnt = g_iYuantaAPI.YOA_GetAccountCount();
	for (int i = 0; i < nCnt; i++)
	{
		memset(account, 0x00, sizeof(account));

		if (RESULT_SUCCESS == g_iYuantaAPI.YOA_GetAccount(i, account, sizeof(account)))
		{
			std::string account_type = "9";
			if (g_iYuantaAPI.YOA_GetAccountInfo(ACCOUNT_INFO_TYPE, account, acct_gubun, sizeof(acct_gubun)))
			{
				std::string gubun_code = acct_gubun;
				if (gubun_code == "A05010001")
					account_type = "9";
				else if (gubun_code == "A05020001")
					account_type = "1";
				else
					account_type = "4";
			}

			std::shared_ptr<DarkHorse::SmAccount> account_t = std::make_shared<DarkHorse::SmAccount>();
			account_t->Type(account_type);
			account_t->No(account);
			memset(acctInfo, 0x00, sizeof(acctInfo));
			memset(acct_gubun, 0x00, sizeof(acct_gubun));
			if (g_iYuantaAPI.YOA_GetAccountInfo(ACCOUNT_INFO_NAME, account, acctInfo, sizeof(acctInfo)))
			{
				account_t->Name(acctInfo);
			}
			

			LOGINFO(CMyLogger::getInstance(), _T("get_account_list:: account_no[%s], account_name[%s], account_type[%s]"), account_t->No().c_str(), account_t->Name().c_str(), account_t->Type().c_str());
			account_t->is_server_side(true);
			mainApp.AcntMgr()->AddAccount(account_t);
		}
	}

	if (0 == nCnt)
	{
		AfxMessageBox(_T("계좌정보가 없습니다."));
	}
}

#define CODETYPE_FUTURE  4
#define CODETYPE_OPTION  5
#define INFOTYPE_CODE     0

void YaClient::get_symbol_list(const int market_type)
{
	mainApp.SymMgr()->read_domestic_productfile();
	mainApp.SymMgr()->read_dm_masterfile_ya();
	mainApp.SymMgr()->MakeAbFavorite();
}


void YaClient::on_dm_realtime_order()
{
	LOGINFO(CMyLogger::getInstance(), _T("////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////)"));
//	유안타증권 Open API 출력코드 예제입니다.
//	[71] REAL_PA - 출력블록

	TCHAR data[1024] = { 0, };
	nlohmann::json order_info;

	g_iYuantaAPI.YOA_SetTRInfo(_T("71"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tr_code"), data, sizeof(data), 0);		// TR CODE(1) 0:OR01 1:CH01 2:HO01 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: TR CODE(1) 0:OR01 1:CH01 2:HO01[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("ordno"), data, sizeof(data), 0);		// 주문접수번호(거래소) 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 주문접수번호(거래소)[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("autocancel"), data, sizeof(data), 0);		// 자동취소여부 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 자동취소여부[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mdmtp"), data, sizeof(data), 0);		// 매체구분 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 매체구분[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), 0);		// filler 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: filler[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cheno"), data, sizeof(data), 0);		// 체결번호(c.yak_num) 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 체결번호[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("origno"), data, sizeof(data), 0);		// 원주문번호(0) 값을 가져옵니다.
	const std::string ori_order_no = data;
	order_info["original_order_no"] = data;
	order_info["first_order_no"] = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 원주문번호[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jumunno"), data, sizeof(data), 0);		// 내부주문번호 값을 가져옵니다.
	const std::string intra_order_no = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 내부주문번호[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("bpjumunno"), data, sizeof(data), 0);		// 지점별 주문번호 값을 가져옵니다.
	// 이번호가 내부 처리용 주문 번호가 됩니다. 그리고 정정, 취소 주문시 이 주문 번호를 사용해야 합니다. 
	order_info["order_no"] = data;
	const std::string order_no = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 지점별 주문번호[%s]"), data);

	intra_order_no_map_[intra_order_no] = order_no;

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jumunuv"), data, sizeof(data), 0);		// 주문단가(jumun_su) 값을 가져옵니다.
	order_info["order_price"] = _ttoi(data);
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 주문단가[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jumunqty"), data, sizeof(data), 0);		// 주문수량(jumun_price) 값을 가져옵니다.
	order_info["order_amount"] = _ttoi(data);
	order_info["remain_count"] = _ttoi(data);
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 주문수량[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("corqty"), data, sizeof(data), 0);		// 정정/취소 수량 값을 가져옵니다.
	const int modified_count = _ttoi(data);
	
	order_info["cancelled_count"] = modified_count;
	order_info["modified_count"] = modified_count;

	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order::  정정/취소 수량 [%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cheuv"), data, sizeof(data), 0);		// 체결단가(c.yak_price) 값을 가져옵니다.
	order_info["filled_price"] = _ttoi(data);
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 체결단가[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cheqty"), data, sizeof(data), 0);		// 체결수량(c.yak_su) 값을 가져옵니다.
	const int filled_count = _ttoi(data);
	order_info["filled_count"] = filled_count;

	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 체결수량[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("chesum"), data, sizeof(data), 0);		// 체결수량합 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 체결수량합[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("shyak"), data, sizeof(data), 0);		// S 근월물 체결가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lnyak"), data, sizeof(data), 0);		// S 원월물 체결가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("bp_time"), data, sizeof(data), 0);		// data 수신 발생시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tr_time"), data, sizeof(data), 0);		// 체결시간(c.yak_time) 값을 가져옵니다.
	order_info["filled_date"] = data;
	order_info["filled_time"] = data;

	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 체결시간[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("op_id"), data, sizeof(data), 0);		// 고객 Login ID(user_id) 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 고객 Login ID([%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("custseq"), data, sizeof(data), 0);		// 계좌번호(cust_no) 값을 가져옵니다.
	order_info["account_no"] = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 계좌번호[%s]"), data);
	memset(data, 0x00, sizeof(data));
	BYTE buy_or_sell;
	g_iYuantaAPI.YOA_GetFieldByte(_T("gubun48"), &buy_or_sell);		// 매수(4) 매도(8) 값을 가져옵니다.
	
	int order_type = 0;
	switch (buy_or_sell)
	{
	case '4': // 메수
	case '8': // 매도
	case '1': // 매수주문
	case '2': // 매도주문
		order_info["order_type"] = "1";
		order_type = 1;
		break;
	case 'N': // 정정주문
	case 'J': // 매수정정주문
	case 'j': // 매도정정주문
	case 'M': // 매수정정주문
	case 'm': // 매도정정주문
		order_info["order_type"] = "2";
		order_type = 2;
		break;
	case 'D': // 취소주문
	case 'Z': // 매수취소주문
	case 'z': // 매도최수주문
	case 'C': // 매수취소주문
	case 'c': // 매도취소주문
		order_info["order_type"] = "3";
		order_type = 3;
		break;
	case 'R': // 매수주문거부
	case 'r': // 매도주문거부
	case 'S': // 매수거부확인
	case 's': // 매도거부확인
	case 'T': // 정정거부
	case 't': // 취소거부
		order_info["order_type"] = "1";
		order_type = 1;
		break;
	case 'U': // 알수없음.
		order_info["order_type"] = "1";
		order_type = 1;
		break;
	default:
		order_info["order_type"] = "1";
		order_type = 1;
		break;
	}


	switch (buy_or_sell)
	{
		case '1':
		case '4':
		case 'J':
		case 'M':
		case 'Z':
		case 'C':
		case 'R':
		case 'S':
			order_info["position_type"] = "1";
			break;
		case '2':
		case '8':
		case 'j':
		case 'm':
		case 'z':
		case 'c':
		case 'r':
		case 's':
			order_info["position_type"] = "2";
			break;
		default:
			order_info["position_type"] = "0";
			break;
	}
	//order_info["position_type"] = ((buy_or_sell == '4') || (buy_or_sell == '1')) ? "1" : "2";
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 매수/매도gubun48[%c]"), buy_or_sell);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("stkcode"), data, sizeof(data), 0);		// 종목코드(c.jongcode) 값을 가져옵니다.

	std::string symbol_code = data;
	if (symbol_code.substr(0, 1).at(0) == '1' && symbol_code.length() > 5)
		symbol_code = symbol_code.substr(0, 5);

	order_info["symbol_code"] = symbol_code;
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 종목코드[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("stkname"), data, sizeof(data), 0);		// 종목명(' ... ') 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 종목명[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jununjong"), data, sizeof(data), 0);		// L:지정가 M:시장가 C:조건부지정 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: L:지정가 M:시장가 C:조건부지정[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("reject"), data, sizeof(data), 0);		// 거부사유 코드 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 거부사유 Code[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatp"), data, sizeof(data), 0);		// 호가유형 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 호가유형[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("futu_ord_if"), data, sizeof(data), 0);		// 주문조건 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 주문조건[%s]"), data);

	
	

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("trtime2"), data, sizeof(data), 0);		// 시간 HH:MM:SS 값을 가져옵니다.
	order_info["order_time"] = data;
	order_info["order_date"] = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 시간[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 체결가 또는 주문가 값을 가져옵니다.
	//order_info["order_price"] = convert_to_int(symbol_code, data);
	//order_info["filled_price"] = convert_to_int(symbol_code, data);

	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 체결가 또는 주문가[%s]"), data);

	
	
	order_info["order_sequence"] = 1;
	//order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
	
	
	//order_info["order_type"] = static_cast<const char*>(strMan.Trim());
	
	
	
	
	//order_info["order_date"] = static_cast<const char*>(strOrderDate.Trim());
	//order_info["order_time"] = static_cast<const char*>(strOrderTime.Trim());

	//order_info["filled_price"] = static_cast<const char*>(strFilledPrice.Trim());
	//order_info["filled_amount"] = static_cast<const char*>(strFilledAmount.Trim());

	//order_info["filled_date"] = static_cast<const char*>(strFilledDate.Trim());
	//order_info["filled_time"] = static_cast<const char*>(strFilledTime.Trim());

	order_info["custom_info"] = "";


	switch (buy_or_sell)
	{
	case '4': // 메수
	case '8': // 매도
	case '1': // 매수주문
	case '2': // 매도주문
		//mainApp.total_order_manager()->on_order_event(std::move(order_info));
		order_info["order_event"] = OrderEvent::OE_Accepted;
		break;
	case 'N': // 정정주문
	case 'J': // 매수정정주문
	case 'j': // 매도정정주문
	case 'M': // 매수정정주문
	case 'm': // 매도정정주문
		//mainApp.total_order_manager()->on_order_event(std::move(order_info));
		//order_info["remain_count"] = 0;
		order_info["order_event"] = OrderEvent::OE_Accepted;
		break;
	case 'D': // 취소주문
	case 'Z': // 매수취소주문
	case 'z': // 매도최수주문
	case 'C': // 매수취소주문
	case 'c': // 매도취소주문
		order_info["order_type"] = "3";
		//order_info["remain_count"] = 0;
		order_info["order_event"] = OrderEvent::OE_Accepted;
		break;
	case 'R': // 매수주문거부
	case 'r': // 매도주문거부
	case 'S': // 매수거부확인
	case 's': // 매도거부확인
	case 'T': // 정정거부
	case 't': // 취소거부
		order_info["order_event"] = OrderEvent::OE_Accepted;
		order_info["order_type"] = "1";
		//mainApp.total_order_manager()->on_order_event(std::move(order_info));
		break;
	case 'U': // 알수없음.
		order_info["order_event"] = OrderEvent::OE_Accepted;
		order_info["order_type"] = "1";
		//mainApp.total_order_manager()->on_order_event(std::move(order_info));
		break;
	default:
		order_info["order_type"] = "1";
		order_info["order_event"] = OrderEvent::OE_Accepted;
		
		break;
	}

	if (order_type == 2 || order_type == 3) {
		auto found = intra_order_no_map_.find(ori_order_no);
		if (found != intra_order_no_map_.end()) {
			order_info["original_order_no"] = found->second;
		}
	}

	if (filled_count > 0) {
		order_info["order_event"] = OrderEvent::OE_Filled;
		order_info["remain_count"] = 0;
	}
	//else
	//	order_info["order_event"] = OrderEvent::OE_Accepted;

	mainApp.total_order_manager()->on_order_event(std::move(order_info));
}

void YaClient::on_dm_realtime_quote()
{
	TCHAR data[1024] = { 0, };
	nlohmann::json quote;
	nlohmann::json hoga;
	g_iYuantaAPI.YOA_SetTRInfo(_T("41"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongcode"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	std::string symbol_code = data;
	if (symbol_code.substr(0, 1).at(0) == '1')
		symbol_code = symbol_code.substr(0, 5);
	quote["symbol_code"] = symbol_code;
	hoga["symbol_code"] = symbol_code;
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
	quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// 미결제약정수량 값을 가져옵니다.
	quote["volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// 미결제약정전일대비수량 값을 가져옵니다.
	quote["preday_volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopeninterest"), data, sizeof(data), 0);		// 순미결제약정수량_KOFEX 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopendebi"), data, sizeof(data), 0);		// 순미결제약정전일대비수량_KOFEX 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// 누적체결수량 값을 가져옵니다.
	quote["cumulative_amount"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("svolume"), data, sizeof(data), 0);		// 스프레드누적체결수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("money"), data, sizeof(data), 0);		// 누적거래대금_백만 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("smoney"), data, sizeof(data), 0);		// 스프레드누적거래대금_백만 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler1"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("changerate"), data, sizeof(data), 0);		// 등락률 값을 가져옵니다.
	quote["updown_rate"] = data;
	double updown_rate = _ttof(data);
	if (updown_rate > 0) {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		quote["delta_day_sign"] = "-";
		quote["up_down"] = -1;
	}
	else {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 0;
	}
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mbasis"), data, sizeof(data), 0);		// 시장BASIS 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tbasis"), data, sizeof(data), 0);		// 이론BASIS 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theory"), data, sizeof(data), 0);		// 이론가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volatility"), data, sizeof(data), 0);		// 내재변동성 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("delta"), data, sizeof(data), 0);		// 델타 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rate"), data, sizeof(data), 0);		// 괴리율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time2"), data, sizeof(data), 0);		// 시간_HH:MM:SS 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// 시간_HH:MM 값을 가져옵니다.
	quote["time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// 고가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowtime"), data, sizeof(data), 0);		// 저가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("change"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("nowvol"), data, sizeof(data), 0);		// 직전체결량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highchange"), data, sizeof(data), 0);		// 고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowchange"), data, sizeof(data), 0);		// 저가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler2"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// 최우선매도호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), 0);		// 최우선매도호가건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// 최우선매수호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), 0);		// 최우선매수호가건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("41"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < 5; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), i);		// 매도호가 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), i);		// 매수호가 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), i);		// 4bytes맞추기위해 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), i);		// 매도호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), i);		// 매수호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), i);		// 매도호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_cnt"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), i);		// 매수호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_cnt"] = _ttoi(data);
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("41"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 매도호가수량 값을 가져옵니다.
	hoga["tot_sell_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 매수호가수량 값을 가져옵니다.
	hoga["tot_buy_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// 매도총호가건수 값을 가져옵니다.
	hoga["tot_sell_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// 매수총호가건수 값을 가져옵니다.
	hoga["tot_buy_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// 호가시간 값을 가져옵니다.
	hoga["hoga_time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("diffmesumedo"), data, sizeof(data), 0);		// 매수잔량-매도잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("weightedavg"), data, sizeof(data), 0);		// 가중평균가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("gamma"), data, sizeof(data), 0);		// 감마 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theta"), data, sizeof(data), 0);		// 쎄타 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vega"), data, sizeof(data), 0);		// 베가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rho"), data, sizeof(data), 0);		// 로 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler3"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("restriction_gbn"), data, sizeof(data), 0);		// 실시간상하한가구분 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice"), data, sizeof(data), 0);		// 상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice"), data, sizeof(data), 0);		// 하한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler4"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vilevel"), data, sizeof(data), 0);		// 가격제한폭단계 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vimaxprice"), data, sizeof(data), 0);		// 가격제한폭상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("viminprice"), data, sizeof(data), 0);		// 가격제한폭하한가 값을 가져옵니다.

	ya_stock_client_.OnSymbolQuote(std::move(quote));
	ya_stock_client_.OnDmSymbolHoga(std::move(hoga));
}

void YaClient::on_dm_realtime_hoga()
{
	TCHAR data[1024] = { 0, };
	nlohmann::json hoga;
	g_iYuantaAPI.YOA_SetTRInfo(_T("42"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongcode"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	std::string symbol_code = data;
	if (symbol_code.substr(0, 1).at(0) == '1' && symbol_code.length() > 5)
		symbol_code = symbol_code.substr(0, 5);

	hoga["symbol_code"] = symbol_code;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// 시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 최우선매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 최우선매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler1"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// 최우선매도호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), 0);		// 최우선매도호가건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// 최우선매수호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), 0);		// 최우선매수호가건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("42"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// 매도호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// 매수호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), 0);		// 매도호가건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), 0);		// 매수호가건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("42"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 매도호가수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 매수호가수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// 매도총호가건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// 매수총호가건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time2"), data, sizeof(data), 0);		// 시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("diffmesumedo"), data, sizeof(data), 0);		// 매수잔량-매도잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectjuka"), data, sizeof(data), 0);		// 예상체결가격 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectvol"), data, sizeof(data), 0);		// 예상체결수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectrate"), data, sizeof(data), 0);		// 예상체결등락율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectdebi"), data, sizeof(data), 0);		// 예상체결대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dongsigubun"), data, sizeof(data), 0);		// 동시구분 값을 가져옵니다.
}

void YaClient::on_dm_realtime_accepted_count()
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("44"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongcode"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// 미결제약정수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// 미결제약정전일대비수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopeninterest"), data, sizeof(data), 0);		// 순미결제약정수량_KOFEX 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopendebi"), data, sizeof(data), 0);		// 순미결제약정전일대비수량_KOFEX 값을 가져옵니다.
}

int YaClient::confirm_account_password(DhTaskArg arg)
{
	const std::string account_type = arg.parameter_map["account_type"];
	if (account_type == "9")
		return dm_account_asset(arg);
	else
		return ab_account_asset(arg);
}

int YaClient::dm_symbol_quote(DhTaskArg arg)
{
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	if (arg.parameter_map["gubun_code"] == "21")
		dm_fut_sise(arg);
	else if (arg.parameter_map["gubun_code"] == "61")
		dm_fut_mini_sise(arg);
	else if (arg.parameter_map["gubun_code"] == "84")
		dm_fut_sise(arg);
	else if (arg.parameter_map["gubun_code"] == "45")
		dm_commodity_sise(arg);
	else
		dm_opt_sise(arg);
	return 1;
}

int YaClient::dm_symbol_hoga(DhTaskArg arg)
{
	if (arg.parameter_map["gubun_code"] == "21")
		dm_fut_hoga(arg);
	else if (arg.parameter_map["gubun_code"] == "61")
		dm_fut_mini_hoga(arg);
	else if (arg.parameter_map["gubun_code"] == "84")
		dm_fut_hoga(arg);
	else if (arg.parameter_map["gubun_code"] == "45")
		dm_commodity_hoga(arg);
	else
		dm_opt_hoga(arg);
	return 1;
}

int YaClient::convert_to_int(const std::string& symbol_code, const char* data)
{
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (!symbol) return -1;
	double converted_value = _ttof(data);
	double multiplier = std::pow(10.0, symbol->decimal());
	int result = static_cast<int>(std::round(converted_value * multiplier));
	return result;
}

int YaClient::register_dm_symbol(const std::string& symbol_code)
{
	std::string symbol_code_to_send = symbol_code;
	if (symbol_code.substr(0, 1).at(0) == '1')
		symbol_code_to_send.append("000");
	g_iYuantaAPI.YOA_SetTRFieldString(_T("41"), _T("InBlock1"), _T("jongcode"), symbol_code_to_send.c_str());
	int nResult = g_iYuantaAPI.YOA_RegistAuto(GetSafeHwnd(), _T("41"));

	if (ERROR_MAX_CODE < nResult)
	{
		//LOGINFO(CMyLogger::getInstance(), _T("[41]국내 선물 옵션 실시간체결이 등록 되었습니다."));
		return 1;
	}
	else
	{
		TCHAR msg[2048] = { 0, };
		g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

		LOGINFO(CMyLogger::getInstance(), msg);
	}

	return 1;
}

int YaClient::unregister_dm_symbol(const std::string& symbol_code)
{
	int nResult = g_iYuantaAPI.YOA_UnregistAuto(GetSafeHwnd(), _T("41"));

	if (RESULT_SUCCESS == nResult)
	{
		LOGINFO(CMyLogger::getInstance(), _T("[41]국내 선물 옵션 실시간체결이 해제 되었습니다."));
	}
	else
	{
		TCHAR msg[2048] = { 0, };
		g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

		LOGINFO(CMyLogger::getInstance(), msg);
	}

	return 1;

}

int YaClient::register_dm_account(const std::string& account_no)
{
	int nResult = g_iYuantaAPI.YOA_RegistAuto(GetSafeHwnd(), _T("71"));

	if (ERROR_MAX_CODE < nResult)
	{
		//LOGINFO(CMyLogger::getInstance(), _T("[71]국내 선물옵션 주문확인_체결 실시간체결이 등록 되었습니다."));
		return 1;
	}
	else
	{
		TCHAR msg[2048] = { 0, };
		g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

		LOGINFO(CMyLogger::getInstance(), msg);
	}

	return 1;
}

int YaClient::unregister_dm_account(const std::string& account_no)
{
	int nResult = g_iYuantaAPI.YOA_UnregistAuto(GetSafeHwnd(), _T("71"));

	if (RESULT_SUCCESS == nResult)
	{
		LOGINFO(CMyLogger::getInstance(), _T("[71]국내 선물 옵션 실시간체결이 해제 되었습니다."));
	}
	else
	{
		TCHAR msg[2048] = { 0, };
		g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

		LOGINFO(CMyLogger::getInstance(), msg);
	}

	return 1;
}

void YaClient::OnTimer(UINT_PTR nIDEvent)
{
	{
		//LOGINFO(CMyLogger::getInstance(), _T("OnTimer:: "));

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> dist(32295 / 5, 32450 / 5);

		int random_number = dist(mt) * 5;

		nlohmann::json quote;

		quote["symbol_code"] = "101TC";
		quote["symbol_name_kr"] = "101TC";
		quote["delta_day"] = 5;
		//quote["delta_day_sign"] = static_cast<const char*>(strDeltaDaySign.Trim());
		quote["updown_rate"] = "-0.1";
		quote["time"] = "20230907";
		quote["close"] = random_number;
		quote["open"] = 32355;
		quote["high"] = 32355;
		quote["low"] = 32355;
		quote["pre_day_close"] = 32360;
		quote["cumulative_amount"] = 0;
		quote["volume"] = 0;
		quote["up_down"] = 1;
		quote["preday_volume"] = 0;


		ya_stock_client_.OnSymbolQuote(std::move(quote));


		nlohmann::json hoga;
		hoga["symbol_code"] = "101TC";
		hoga["hoga_time"] = "10:24:35";
		hoga["tot_buy_qty"] = 100;
		hoga["tot_sell_qty"] = 200;
		hoga["tot_buy_cnt"] = 300;
		hoga["tot_sell_cnt"] = 400;

		hoga["hoga_items"][0]["sell_price"] = random_number;
		hoga["hoga_items"][0]["buy_price"] = random_number;
		hoga["hoga_items"][0]["sell_qty"] = 1;
		hoga["hoga_items"][0]["buy_qty"] = 2;
		hoga["hoga_items"][0]["sell_cnt"] = 3;
		hoga["hoga_items"][0]["buy_cnt"] = 4;

		hoga["hoga_items"][1]["sell_price"] = random_number - 5*1;
		hoga["hoga_items"][1]["buy_price"] = random_number + 5*1;
		hoga["hoga_items"][1]["sell_qty"] = 1;
		hoga["hoga_items"][1]["buy_qty"] = 2;
		hoga["hoga_items"][1]["sell_cnt"] = 3;
		hoga["hoga_items"][1]["buy_cnt"] = 4;

		hoga["hoga_items"][2]["sell_price"] = random_number - 5 * 2;
		hoga["hoga_items"][2]["buy_price"] = random_number + 5 * 2;
		hoga["hoga_items"][2]["sell_qty"] = 16;
		hoga["hoga_items"][2]["buy_qty"] =25;
		hoga["hoga_items"][2]["sell_cnt"] = 45;
		hoga["hoga_items"][2]["buy_cnt"] = 34;

		hoga["hoga_items"][3]["sell_price"] = random_number - 5 * 3;
		hoga["hoga_items"][3]["buy_price"] = random_number + 5 * 3;
		hoga["hoga_items"][3]["sell_qty"] = 34;
		hoga["hoga_items"][3]["buy_qty"] = 45;
		hoga["hoga_items"][3]["sell_cnt"] = 56;
		hoga["hoga_items"][3]["buy_cnt"] = 67;

		hoga["hoga_items"][4]["sell_price"] = random_number - 5 * 4;
		hoga["hoga_items"][4]["buy_price"] = random_number + 5 * 4;
		hoga["hoga_items"][4]["sell_qty"] = 23;
		hoga["hoga_items"][4]["buy_qty"] = 90;
		hoga["hoga_items"][4]["sell_cnt"] = 34;
		hoga["hoga_items"][4]["buy_cnt"] = 45;

		ya_stock_client_.OnDmSymbolHoga(std::move(hoga));
	}
}

void YaClient::start_timer()
{
	SetTimer(1, 10, NULL);
}

void YaClient::stop_timer()
{
	KillTimer(1);
}

int YaClient::dm_fut_mini_sise(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_FUT_MINI_SISE)];
	const std::string trade_code = req_info.dso_name.substr(3);
	//g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	//g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		

	//g_iYuantaAPI.YOA_SetTRInfo(_T("368001"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("368001"), _T("InBlock1"), _T("code"), symbol_code.c_str(), 0);		// 선물코드 값을 설정합니다.


	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 국내미니 선물시세 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]국내선물시세 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_fut_mini_hoga(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_FUT_MINI_HOGA)];
	const std::string trade_code = req_info.dso_name.substr(3);
	//g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	g_iYuantaAPI.YOA_SetTRFieldString(_T("368001"), _T("InBlock1"), _T("code"), symbol_code.c_str(), 0);		// 계좌 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 국내미니선물 호가 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]국내미니선물 호가 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

void YaClient::on_dm_fut_mini_sise(const YA_REQ_INFO& req_info)
{
	auto found = request_map_.find(req_info.request_id);
	if (found == request_map_.end()) {
		return;
	}
	const std::string symbol_code_origin = found->second.parameter_map["symbol_code"];
	std::string temp = symbol_code_origin;

	LOGINFO(CMyLogger::getInstance(), _T("on_dm_fut_mini_sise:: symbol code[%s]"), symbol_code_origin.c_str());

	TCHAR data[1024] = { 0, };
	nlohmann::json quote;

	g_iYuantaAPI.YOA_SetTRInfo(_T("368001"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	quote["symbol_code"] = data;
	const std::string symbol_code = data;
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	g_iYuantaAPI.YOA_SetTRInfo(_T("368001"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongname"), data, sizeof(data), 0);		// 종목코드설명 값을 가져옵니다.
	quote["symbol_name_kr"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data), 0);		// 등락율 값을 가져옵니다.
	quote["updown_rate"] = data;
	double updown_rate = _ttof(data);
	if (updown_rate > 0) {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		quote["delta_day_sign"] = "-";
		quote["up_down"] = -1;
	}
	else {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 0;
	}
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("standardprice"), data, sizeof(data), 0);		// 기준가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("startdebi"), data, sizeof(data), 0);		// 시가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highdebi"), data, sizeof(data), 0);		// 고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// 고가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	quote["time"] = data;
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
	quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowdebi"), data, sizeof(data), 0);		// 저가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowtime"), data, sizeof(data), 0);		// 저가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice"), data, sizeof(data), 0);		// 상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice"), data, sizeof(data), 0);		// 하한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxcb"), data, sizeof(data), 0);		// cb상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mincb"), data, sizeof(data), 0);		// cb하한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospi"), data, sizeof(data), 0);		// 코스피 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospidebi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospirate"), data, sizeof(data), 0);		// 코스피등락 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theoreticalprice"), data, sizeof(data), 0);		// 이론가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rate"), data, sizeof(data), 0);		// 괴리율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basist"), data, sizeof(data), 0);		// 이론베이시스 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basism"), data, sizeof(data), 0);		// 시장베이시스 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// 약정수량 값을 가져옵니다.
	quote["volume"] = _ttoi(data);
	quote["cumulative_amount"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("svolume"), data, sizeof(data), 0);		// 스프레드수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("money"), data, sizeof(data), 0);		// 약정대금(백만) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("smoney"), data, sizeof(data), 0);		// 스프레드대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// 미결제약정 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevvolume"), data, sizeof(data), 0);		// 전일약정수량 값을 가져옵니다.
	quote["preday_volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sprevvolume"), data, sizeof(data), 0);		// 전일스프레드수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevmoney"), data, sizeof(data), 0);		// 잔일약정대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sprevmoney"), data, sizeof(data), 0);		// 전일스프레드대금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// 전일미결제약정대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopeninterest"), data, sizeof(data), 0);		// 순미결제약정 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopendebi"), data, sizeof(data), 0);		// 전일순미결제약정대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendate"), data, sizeof(data), 0);		// 거래개시일 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lastdate"), data, sizeof(data), 0);		// 최종거래일 값을 가져옵니다.
	if (symbol) symbol->LastTradeDay(data);
	if (symbol) symbol->ExpireDate(data);

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("remainday"), data, sizeof(data), 0);		// 잔존일수 값을 가져옵니다.
	if (symbol) symbol->RemainDays(_ttoi(data));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highprice"), data, sizeof(data), 0);		// 최고가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowprice"), data, sizeof(data), 0);		// 최저가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebi"), data, sizeof(data), 0);		// 최고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebi"), data, sizeof(data), 0);		// 최저가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebirate"), data, sizeof(data), 0);		// 최고가대비율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebirate"), data, sizeof(data), 0);		// 최저가대비율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedate"), data, sizeof(data), 0);		// 최고일자 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedate"), data, sizeof(data), 0);		// 최저일자 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("weightedavg"), data, sizeof(data), 0);		// 가중평균가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cdinterest"), data, sizeof(data), 0);		// CD금리 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mememulti"), data, sizeof(data), 0);		// 거래승수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("restriction_gbn"), data, sizeof(data), 0);		// 실시간상한가적용여부 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice2"), data, sizeof(data), 0);		// 일중상한가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice2"), data, sizeof(data), 0);		// 일중하한가 값을 가져옵니다.


	ya_stock_client_.OnSymbolQuote(std::move(quote));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_fut_mini_hoga(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };
	nlohmann::json hoga;
	g_iYuantaAPI.YOA_SetTRInfo(_T("368002"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	const std::string symbol_code = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_fut_mini_hoga:: symbol code[%s]"), symbol_code.c_str());
	hoga["symbol_code"] = symbol_code;
	g_iYuantaAPI.YOA_SetTRInfo(_T("368002"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < 5; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), i);		// 매도호가 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), i);		// 매도호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), i);		// 매도호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_cnt"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), i);		// 매수호가 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), i);		// 매수호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), i);		// 매수호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_cnt"] = _ttoi(data);
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// 호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// 호가건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// 호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// 호가건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock4"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// 약정시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 약정가격 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("nowvol"), data, sizeof(data), 0);		// 약정수량 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock5"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime"), data, sizeof(data), 0);		// 호가시간 값을 가져옵니다.
	hoga["hoga_time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 총매도잔량 값을 가져옵니다.
	hoga["tot_sell_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 총매수잔량 값을 가져옵니다.
	hoga["tot_buy_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// 총매도건수 값을 가져옵니다.
	hoga["tot_sell_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// 총매수건수 값을 가져옵니다.
	hoga["tot_buy_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// 호가시간2 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dongsi"), data, sizeof(data), 0);		// 동시구분 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectjuka"), data, sizeof(data), 0);		// 예상체결가 값을 가져옵니다.


	ya_stock_client_.OnDmSymbolHoga(std::move(hoga));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

int YaClient::dm_option_month_quote(DhTaskArg arg)
{
	const std::string product_code = arg.parameter_map["product_code"];
	if (product_code == "201")
		dm_option_month_quote_kospi_option(arg);
	else if (product_code == "205" )
		dm_option_month_quote_mini_kospi_option(arg);
	else if (product_code == "209" || product_code == "2AF")
		dm_option_month_quote_weekly_option(arg);
	else if (product_code == "206")
		dm_option_month_quote_kosdaq_option(arg);

	return 1;
}

int YaClient::dm_option_month_quote_kospi_option(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_OPTION_MONTH_QUOTE_KOSPI)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRFieldString(_T("361007"), _T("InBlock1"), _T("janggubun"), _T("0"), 0);		// 구분 값을 설정합니다.
	const std::string year_month = arg.parameter_map["year_month"];
	g_iYuantaAPI.YOA_SetTRFieldString(_T("361007"), _T("InBlock1"), _T("month"), year_month.c_str(), 0);		// 월 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), _T("361007"));
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 코스피 옵션 월별 시세조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", _T("361007"), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]코스피 옵션 월별 시세 조회중 오류가 발생하였습니다.Error Message[%s]"), _T("361007"), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_option_month_quote_mini_kospi_option(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_OPTION_MONTH_QUOTE_MINI_KOSPI)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(_T("389004"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("389004"), _T("InBlock1"), _T("janggubun"), _T("0"), 0);		// 구분 값을 설정합니다.
	const std::string year_month = arg.parameter_map["year_month"];
	g_iYuantaAPI.YOA_SetTRFieldString(_T("389004"), _T("InBlock1"), _T("month"), year_month.c_str(), 0);		// 월 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), _T("389004"));
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 미니옵션복합시세 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", _T("389004"), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]미니옵션복합시세 조회중 오류가 발생하였습니다.Error Message[%s]"), _T("389004"), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_option_month_quote_weekly_option(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_OPTION_MONTH_QUOTE_WEEKLY)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(_T("389104"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("389104"), _T("InBlock1"), _T("janggubun"), _T("0"), 0);		// 구분 값을 설정합니다.
	const std::string year_month = arg.parameter_map["year_month"];
	g_iYuantaAPI.YOA_SetTRFieldString(_T("389104"), _T("InBlock1"), _T("month"), year_month.c_str(), 0);		// 월 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), _T("389104"));
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 코스피위클리옵션복합시세 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", _T("389104"), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]코스피위클리옵션복합시세 조회중 오류가 발생하였습니다.Error Message[%s]"), _T("389104"), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_option_month_quote_kosdaq_option(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_OPTION_MONTH_QUOTE_KOSDAQ)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(_T("384007"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("384007"), _T("InBlock1"), _T("janggubun"), _T("0"), 0);		// 구분 값을 설정합니다.
	const std::string year_month = arg.parameter_map["year_month"];
	g_iYuantaAPI.YOA_SetTRFieldString(_T("384007"), _T("InBlock1"), _T("month"), year_month.c_str(), 0);		// 월 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), _T("384007"));
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 코스닥150옵션복합시세 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", _T("384007"), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]코스닥150옵션복합시세 조회중 오류가 발생하였습니다.Error Message[%s]"), _T("384007"), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

void YaClient::on_dm_option_month_quote_kospi_option(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	//g_iYuantaAPI.YOA_SetTRInfo(_T("361007"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock1"), _T("last"), data, sizeof(data), 0);		// 지수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock1"), _T("debi"), data, sizeof(data), 0);		// 대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock1"), _T("debirate"), data, sizeof(data), 0);		// 등락율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock1"), _T("volume"), data, sizeof(data), 0);		// 거래량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock1"), _T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock1"), _T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock1"), _T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock1"), _T("remainday"), data, sizeof(data), 0);		// 잔존일수 값을 가져옵니다.

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("NextBlock1"), _T("scrkey"), data, sizeof(data), 0);

	nlohmann::json call_quote;
	//g_iYuantaAPI.YOA_SetTRInfo(_T("361007"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("ccode"), data, sizeof(data), 0);		// C코드 값을 가져옵니다.

	call_quote["symbol_code"] = data;
	std::string symbol_code = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_opt_sise:: symbol code[%s]"), symbol_code.c_str());

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("clowprice"), data, sizeof(data), 0);		// C저가 값을 가져옵니다.
	call_quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("chighprice"), data, sizeof(data), 0);		// C고가 값을 가져옵니다.
	call_quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("cstartprice"), data, sizeof(data), 0);		// C시가 값을 가져옵니다.
	call_quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("cvolume"), data, sizeof(data), 0);		// C약정 값을 가져옵니다.
	call_quote["volume"] = _ttoi(data);
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("cdebirate"), data, sizeof(data), 0);		// C등락률 값을 가져옵니다.
	call_quote["updown_rate"] = data;
	double updown_rate = _ttof(data);
	if (updown_rate > 0) {
		call_quote["delta_day_sign"] = "+";
		call_quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		call_quote["delta_day_sign"] = "-";
		call_quote["up_down"] = -1;
	}
	else {
		call_quote["delta_day_sign"] = "+";
		call_quote["up_down"] = 0;
	}
	call_quote["time"] = "00:00:00";
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("cdebi"), data, sizeof(data), 0);		// C대비 값을 가져옵니다.
	call_quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("clast"), data, sizeof(data), 0);		// C현재가 값을 가져옵니다.
	call_quote["close"] = convert_to_int(symbol_code, data);

	ya_stock_client_.OnSymbolQuote(std::move(call_quote));


	nlohmann::json put_quote;

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("pcode"), data, sizeof(data), 0);		// P코드 값을 가져옵니다.
	symbol_code = data;
	put_quote["symbol_code"] = symbol_code;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("plast"), data, sizeof(data), 0);		// P현재가 값을 가져옵니다.
	put_quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("pdebi"), data, sizeof(data), 0);		// P대비 값을 가져옵니다.
	put_quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("pdebirate"), data, sizeof(data), 0);		// P등락률 값을 가져옵니다.
	put_quote["updown_rate"] = data;
	updown_rate = _ttof(data);
	if (updown_rate > 0) {
		put_quote["delta_day_sign"] = "+";
		put_quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		put_quote["delta_day_sign"] = "-";
		put_quote["up_down"] = -1;
	}
	else {
		put_quote["delta_day_sign"] = "+";
		put_quote["up_down"] = 0;
	}
	put_quote["time"] = "00:00:00";

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("pvolume"), data, sizeof(data), 0);		// P약정 값을 가져옵니다.
	put_quote["volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("pstartprice"), data, sizeof(data), 0);		// P시가 값을 가져옵니다.
	put_quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("phighprice"), data, sizeof(data), 0);		// P고가 값을 가져옵니다.
	put_quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("361007"), _T("OutBlock2"), _T("plowprice"), data, sizeof(data), 0);		// P저가 값을 가져옵니다.
	put_quote["low"] = convert_to_int(symbol_code, data);

	ya_stock_client_.OnSymbolQuote(std::move(put_quote));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_option_month_quote_mini_kospi_option(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	//g_iYuantaAPI.YOA_SetTRInfo(_T("389004"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock1"), _T("last"), data, sizeof(data), 0);		// 지수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock1"), _T("debi"), data, sizeof(data), 0);		// 대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock1"), _T("debirate"), data, sizeof(data), 0);		// 등락율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock1"), _T("volume"), data, sizeof(data), 0);		// 거래량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock1"), _T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock1"), _T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock1"), _T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock1"), _T("remainday"), data, sizeof(data), 0);		// 잔존일수 값을 가져옵니다.

	//g_iYuantaAPI.YOA_SetTRInfo(_T("389004"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("ccode"), data, sizeof(data), 0);		// C코드 값을 가져옵니다.
	nlohmann::json call_quote;
	call_quote["symbol_code"] = data;
	std::string symbol_code = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_opt_sise:: symbol code[%s]"), symbol_code.c_str());
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("cvolume"), data, sizeof(data), 0);		// C약정 값을 가져옵니다.
	call_quote["volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("copeninterest"), data, sizeof(data), 0);		// C미결제 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("cmesuvol"), data, sizeof(data), 0);		// C매수잔 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("cmesuprice"), data, sizeof(data), 0);		// C매수호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("cmedovol"), data, sizeof(data), 0);		// C매도잔 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("cmedoprice"), data, sizeof(data), 0);		// C매도호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("cdebirate"), data, sizeof(data), 0);		// C등락률 값을 가져옵니다.
	call_quote["updown_rate"] = data;
	double updown_rate = _ttof(data);
	if (updown_rate > 0) {
		call_quote["delta_day_sign"] = "+";
		call_quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		call_quote["delta_day_sign"] = "-";
		call_quote["up_down"] = -1;
	}
	else {
		call_quote["delta_day_sign"] = "+";
		call_quote["up_down"] = 0;
	}
	call_quote["time"] = "00:00:00";
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("cdebi"), data, sizeof(data), 0);		// C대비 값을 가져옵니다.
	call_quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("cexpect"), data, sizeof(data), 0);		// C예상가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("clast"), data, sizeof(data), 0);		// C현재가 값을 가져옵니다.
	call_quote["close"] = convert_to_int(symbol_code, data);


	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("cstart"), data, sizeof(data), 0);		// C시가 값을 가져옵니다.
	call_quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("chigh"), data, sizeof(data), 0);		// C고가 값을 가져옵니다.
	call_quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("clow"), data, sizeof(data), 0);		// C저가 값을 가져옵니다.
	call_quote["low"] = convert_to_int(symbol_code, data);

	ya_stock_client_.OnSymbolQuote(std::move(call_quote));


	nlohmann::json put_quote;

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("pcode"), data, sizeof(data), 0);		// P코드 값을 가져옵니다.
	symbol_code = data;
	put_quote["symbol_code"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("jongprice"), data, sizeof(data), 0);		// 행사가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("jongsisu"), data, sizeof(data), 0);		// 지수환산 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("plast"), data, sizeof(data), 0);		// P현재가 값을 가져옵니다.
	put_quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("pexpect"), data, sizeof(data), 0);		// P예상가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("pstart"), data, sizeof(data), 0);		// P시가 값을 가져옵니다.
	put_quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("phigh"), data, sizeof(data), 0);		// P고가 값을 가져옵니다.
	put_quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("plow"), data, sizeof(data), 0);		// P저가 값을 가져옵니다.
	put_quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("pdebi"), data, sizeof(data), 0);		// P대비 값을 가져옵니다.
	put_quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("pdebirate"), data, sizeof(data), 0);		// P등락률 값을 가져옵니다.
	put_quote["updown_rate"] = data;
	updown_rate = _ttof(data);
	if (updown_rate > 0) {
		put_quote["delta_day_sign"] = "+";
		put_quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		put_quote["delta_day_sign"] = "-";
		put_quote["up_down"] = -1;
	}
	else {
		put_quote["delta_day_sign"] = "+";
		put_quote["up_down"] = 0;
	}
	put_quote["time"] = "00:00:00";
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("pmedoprice"), data, sizeof(data), 0);		// P매도호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("pmedovol"), data, sizeof(data), 0);		// P매도잔 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("pmesuprice"), data, sizeof(data), 0);		// P매수호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("pmesuvol"), data, sizeof(data), 0);		// P매수잔 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("popeninterest"), data, sizeof(data), 0);		// P미결제 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("pvolume"), data, sizeof(data), 0);		// P약정 값을 가져옵니다.
	put_quote["volume"] = _ttoi(data);
	
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389004"), _T("OutBlock2"), _T("atmflag"), data, sizeof(data), 0);		// ATM여부 값을 가져옵니다.

	ya_stock_client_.OnSymbolQuote(std::move(put_quote));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_option_month_quote_weekly_option(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	//g_iYuantaAPI.YOA_SetTRInfo(_T("389104"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock1"), _T("last"), data, sizeof(data), 0);		// 지수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock1"), _T("debi"), data, sizeof(data), 0);		// 대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock1"), _T("debirate"), data, sizeof(data), 0);		// 등락율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock1"), _T("volume"), data, sizeof(data), 0);		// 거래량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock1"), _T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock1"), _T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock1"), _T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock1"), _T("remainday"), data, sizeof(data), 0);		// 잔존일수 값을 가져옵니다.

	//g_iYuantaAPI.YOA_SetTRInfo(_T("389104"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("ccode"), data, sizeof(data), 0);		// C코드 값을 가져옵니다.
	nlohmann::json call_quote;
	call_quote["symbol_code"] = data;
	std::string symbol_code = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_opt_sise:: symbol code[%s]"), symbol_code.c_str());
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("cvolume"), data, sizeof(data), 0);		// C약정 값을 가져옵니다.
	call_quote["volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("copeninterest"), data, sizeof(data), 0);		// C미결제 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("cmesuvol"), data, sizeof(data), 0);		// C매수잔 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("cmesuprice"), data, sizeof(data), 0);		// C매수호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("cmedovol"), data, sizeof(data), 0);		// C매도잔 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("cmedoprice"), data, sizeof(data), 0);		// C매도호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("cdebirate"), data, sizeof(data), 0);		// C등락률 값을 가져옵니다.
	call_quote["updown_rate"] = data;
	double updown_rate = _ttof(data);
	if (updown_rate > 0) {
		call_quote["delta_day_sign"] = "+";
		call_quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		call_quote["delta_day_sign"] = "-";
		call_quote["up_down"] = -1;
	}
	else {
		call_quote["delta_day_sign"] = "+";
		call_quote["up_down"] = 0;
	}
	call_quote["time"] = "00:00:00";
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("cdebi"), data, sizeof(data), 0);		// C대비 값을 가져옵니다.
	call_quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("cexpect"), data, sizeof(data), 0);		// C예상가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("clast"), data, sizeof(data), 0);		// C현재가 값을 가져옵니다.
	call_quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("cstart"), data, sizeof(data), 0);		// C시가 값을 가져옵니다.
	call_quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("chigh"), data, sizeof(data), 0);		// C고가 값을 가져옵니다.
	call_quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("clow"), data, sizeof(data), 0);		// C저가 값을 가져옵니다.
	call_quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("jongprice"), data, sizeof(data), 0);		// 행사가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("jongsisu"), data, sizeof(data), 0);		// 지수환산 값을 가져옵니다.

	ya_stock_client_.OnSymbolQuote(std::move(call_quote));


	nlohmann::json put_quote;

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("pcode"), data, sizeof(data), 0);		// P코드 값을 가져옵니다.
	symbol_code = data;
	put_quote["symbol_code"] = data;

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("plast"), data, sizeof(data), 0);		// P현재가 값을 가져옵니다.
	put_quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("pexpect"), data, sizeof(data), 0);		// P예상가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("pstart"), data, sizeof(data), 0);		// P시가 값을 가져옵니다.
	put_quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("phigh"), data, sizeof(data), 0);		// P고가 값을 가져옵니다.
	put_quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("plow"), data, sizeof(data), 0);		// P저가 값을 가져옵니다.
	put_quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("pdebi"), data, sizeof(data), 0);		// P대비 값을 가져옵니다.
	put_quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("pdebirate"), data, sizeof(data), 0);		// P등락률 값을 가져옵니다.
	put_quote["updown_rate"] = data;
	updown_rate = _ttof(data);
	if (updown_rate > 0) {
		put_quote["delta_day_sign"] = "+";
		put_quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		put_quote["delta_day_sign"] = "-";
		put_quote["up_down"] = -1;
	}
	else {
		put_quote["delta_day_sign"] = "+";
		put_quote["up_down"] = 0;
	}
	put_quote["time"] = "00:00:00";
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("pmedoprice"), data, sizeof(data), 0);		// P매도호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("pmedovol"), data, sizeof(data), 0);		// P매도잔 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("pmesuprice"), data, sizeof(data), 0);		// P매수호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("pmesuvol"), data, sizeof(data), 0);		// P매수잔 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("popeninterest"), data, sizeof(data), 0);		// P미결제 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("pvolume"), data, sizeof(data), 0);		// P약정 값을 가져옵니다.
	put_quote["volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("389104"), _T("OutBlock2"), _T("atmflag"), data, sizeof(data), 0);		// ATM여부 값을 가져옵니다.

	ya_stock_client_.OnSymbolQuote(std::move(put_quote));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_option_month_quote_kosdaq_option(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	//g_iYuantaAPI.YOA_SetTRInfo(_T("384007"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock1"), _T("last"), data, sizeof(data), 0);		// 지수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock1"), _T("debi"), data, sizeof(data), 0);		// 대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock1"), _T("debirate"), data, sizeof(data), 0);		// 등락율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock1"), _T("volume"), data, sizeof(data), 0);		// 거래량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock1"), _T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock1"), _T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock1"), _T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock1"), _T("remainday"), data, sizeof(data), 0);		// 잔존일수 값을 가져옵니다.

	//g_iYuantaAPI.YOA_SetTRInfo(_T("384007"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("ccode"), data, sizeof(data), 0);		// C코드 값을 가져옵니다.
	nlohmann::json call_quote;
	call_quote["symbol_code"] = data;
	std::string symbol_code = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_opt_sise:: symbol code[%s]"), symbol_code.c_str());
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("ctv"), data, sizeof(data), 0);		// C시간가치 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("civ"), data, sizeof(data), 0);		// C내재가치 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("copendiff"), data, sizeof(data), 0);		// C미결제증감 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("crho"), data, sizeof(data), 0);		// C로 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("cvega"), data, sizeof(data), 0);		// C배가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("ctheta"), data, sizeof(data), 0);		// C쎄타 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("cgamma"), data, sizeof(data), 0);		// C감마 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("cdelta"), data, sizeof(data), 0);		// C델타 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("civdiff"), data, sizeof(data), 0);		// C내제변동성 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("ctheoryprice"), data, sizeof(data), 0);		// C이론가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("clowprice"), data, sizeof(data), 0);		// C저가 값을 가져옵니다.
	call_quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("chighprice"), data, sizeof(data), 0);		// C고가 값을 가져옵니다.
	call_quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("cstartprice"), data, sizeof(data), 0);		// C시가 값을 가져옵니다.
	call_quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("cvolume"), data, sizeof(data), 0);		// C약정 값을 가져옵니다.
	call_quote["volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("copeninterest"), data, sizeof(data), 0);		// C미결제 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("cmesuvol"), data, sizeof(data), 0);		// C매수잔 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("cmesuprice"), data, sizeof(data), 0);		// C매수호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("cmedovol"), data, sizeof(data), 0);		// C매도잔 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("cmedoprice"), data, sizeof(data), 0);		// C매도호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("cdebirate"), data, sizeof(data), 0);		// C등락률 값을 가져옵니다.
	call_quote["updown_rate"] = data;
	double updown_rate = _ttof(data);
	if (updown_rate > 0) {
		call_quote["delta_day_sign"] = "+";
		call_quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		call_quote["delta_day_sign"] = "-";
		call_quote["up_down"] = -1;
	}
	else {
		call_quote["delta_day_sign"] = "+";
		call_quote["up_down"] = 0;
	}
	call_quote["time"] = "00:00:00";
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("cdebi"), data, sizeof(data), 0);		// C대비 값을 가져옵니다.
	call_quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("clast"), data, sizeof(data), 0);		// C현재가 값을 가져옵니다.
	call_quote["close"] = convert_to_int(symbol_code, data);

	ya_stock_client_.OnSymbolQuote(std::move(call_quote));


	nlohmann::json put_quote;

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pcode"), data, sizeof(data), 0);		// P코드 값을 가져옵니다.
	symbol_code = data;
	put_quote["symbol_code"] = data;

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("jongprice"), data, sizeof(data), 0);		// 행사가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("jongsisu"), data, sizeof(data), 0);		// 지수환산 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("plast"), data, sizeof(data), 0);		// P현재가 값을 가져옵니다.
	put_quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pdebi"), data, sizeof(data), 0);		// P대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pdebirate"), data, sizeof(data), 0);		// P등락률 값을 가져옵니다.
	put_quote["updown_rate"] = data;
	updown_rate = _ttof(data);
	if (updown_rate > 0) {
		put_quote["delta_day_sign"] = "+";
		put_quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		put_quote["delta_day_sign"] = "-";
		put_quote["up_down"] = -1;
	}
	else {
		put_quote["delta_day_sign"] = "+";
		put_quote["up_down"] = 0;
	}
	put_quote["time"] = "00:00:00";
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pmedoprice"), data, sizeof(data), 0);		// P매도호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pmedovol"), data, sizeof(data), 0);		// P매도잔 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pmesuprice"), data, sizeof(data), 0);		// P매수호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pmesuvol"), data, sizeof(data), 0);		// P매수잔 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("popeninterest"), data, sizeof(data), 0);		// P미결제 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pvolume"), data, sizeof(data), 0);		// P약정 값을 가져옵니다.
	put_quote["volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pstartprice"), data, sizeof(data), 0);		// P시가 값을 가져옵니다.
	put_quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("phighprice"), data, sizeof(data), 0);		// P고가 값을 가져옵니다.
	put_quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("plowprice"), data, sizeof(data), 0);		// P저가 값을 가져옵니다.
	put_quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("ptheoryprice"), data, sizeof(data), 0);		// P이론가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pivdiff"), data, sizeof(data), 0);		// P내제변동성 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pdelta"), data, sizeof(data), 0);		// P델타 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pgamma"), data, sizeof(data), 0);		// P감마 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("ptheta"), data, sizeof(data), 0);		// P쎄타 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("pvega"), data, sizeof(data), 0);		// P배가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("prho"), data, sizeof(data), 0);		// P로 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("popendiff"), data, sizeof(data), 0);		// P미결제증감 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("piv"), data, sizeof(data), 0);		// P내재가치 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("ptv"), data, sizeof(data), 0);		// P시간가치 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("atmflag"), data, sizeof(data), 0);		// ATM여부 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("384007"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("384007"), _T("OutBlock2"), _T("next"), data, sizeof(data), 0);		// 다음 값을 가져옵니다.

	ya_stock_client_.OnSymbolQuote(std::move(put_quote));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_option_month_quote(const YA_REQ_INFO& req_info)
{
	if (req_info.code == "201")
		on_dm_option_month_quote_kospi_option(req_info);
	else if (req_info.code == "206")
		on_dm_option_month_quote_mini_kospi_option(req_info);
	else if (req_info.code == "209")
		on_dm_option_month_quote_weekly_option(req_info);
	else if (req_info.code == "205")
		on_dm_option_month_quote_kosdaq_option(req_info);
	else
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_option_month_quote:: unknown dso name[%s]"), req_info.dso_name.c_str());
}

int YaClient::ab_account_asset(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::AB_ACCOUNT_ASSET)];
	const std::string trade_code = req_info.dso_name.substr(3);
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	const std::string crc_cd = arg.parameter_map["crc_cd"];
	const std::string qry_tp = arg.parameter_map["qry_tp"];
	g_iYuantaAPI.YOA_SetTRInfo(_T("863007"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_pswd"), password.c_str(), 0);		// 계좌비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("crc_cd"), crc_cd.c_str(), 0);		// 통화코드 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("qry_tp"), qry_tp.c_str(), 0);		// 구분%:전체F:선물O:옵션 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 해외 자산정보 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], account_no[%s], Request : %s", trade_code.c_str(), account_no.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s] 해외 자산정보 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		auto account = mainApp.AcntMgr()->FindAccount(account_no);
		if (account) {
			const int account_id = account->id();
			account->Confirm(0);
			mainApp.CallbackMgr()->OnPasswordConfirmed(account_id, 0);
			return 1;
		}

		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::ab_account_profit_loss(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::AB_ACCOUNT_PROFIT_LOSS)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), password.c_str(), 0);		// 비밀번호 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 해외계좌별 일일 손익 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]해외계좌별 일일 손익조회 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::ab_accepted_order(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::AB_ACCEPTED_ORDER)];
	const std::string trade_code = req_info.dso_name.substr(3);
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetTRInfo(_T("860005"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("qry_tp"), _T("0"), 0);		// 조회구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("anct_pswd"), password.c_str(), 0);		// 계좌비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("sb_tp"), _T("%"), 0);		// 매매구분(B:매수 S:매도 %:전체) 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_grp_nm"), _T(""), 0);		// 그룹 값을 설정합니다.
	const std::string user_id = mainApp.LoginMgr()->id();
	g_iYuantaAPI.YOA_SetFieldString(_T("usr_id"), user_id.c_str(), 0);		// 사용자ID 값을 설정합니다.
	const std::string start_date = arg.parameter_map["start_date"];
	const std::string end_date = arg.parameter_map["end_date"];
	g_iYuantaAPI.YOA_SetFieldString(_T("str_dt"), start_date.c_str(), 0);		// 조회시작일자 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("end_dt"), end_date.c_str(), 0);		// 조회종료일자 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 해외 미체결 주문 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], account_no[%s], Request : %s", trade_code.c_str(), account_no.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]해외 미체결 주문 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		auto account = mainApp.AcntMgr()->FindAccount(account_no);
		if (account) {
			const int account_id = account->id();
			account->Confirm(0);
			mainApp.CallbackMgr()->OnPasswordConfirmed(account_id, 0);
			return 1;
		}

		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::ab_symbol_quote(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::AB_SYMBOL_QUOTE)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// 계좌 값을 설정합니다.

	//g_iYuantaAPI.YOA_SetTRInfo(_T("810001"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("810001"), _T("InBlock1"), _T("code"), symbol_code.c_str(), 0);		// 해외선물코드 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 해외선물시세 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]해외선물시세 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::ab_symbol_hoga(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::AB_SYMBOL_HOGA)];
	const std::string trade_code = req_info.dso_name.substr(3);
	//g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	g_iYuantaAPI.YOA_SetTRFieldString(_T("810002"), _T("InBlock1"), _T("code"), symbol_code.c_str(), 0);		// 해외선물코드 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 해외선물 호가 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]해외선물 호가 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::ab_symbol_position(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::AB_SYMBOL_POSITION)];
	const std::string trade_code = req_info.dso_name.substr(3);
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetTRInfo(_T("861001"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_pswd"), password.c_str(), 0);		// 계좌비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("qry_tp"), _T("0"), 0);		// 조회구분0:계좌별1:그룹별조회 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_grp_nm"), _T(""), 0);		// 그룹(그룹별조회인경우사용) 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("ftop_tp"), _T("%"), 0);		// 선물옵션구분%:전체F:선물O:옵션 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("sb_tp"), _T("%"), 0);		// 매매구분%:전체S:매도B:매수 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("qry_dt"), _T(""), 0);		// 조회일(없으면당일) 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("usr_id"), _T(""), 0);		// 사용자ID그룹별조회인경우사용 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("stk_cd"), _T("%"), 0);		// 종목코드%:전체 값을 설정합니다.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 해외 계좌별 포지션 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]해외 계좌별 포지션 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::ab_symbol_profit_loss(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::AB_SYMBOL_PROFIT_LOSS)];
	const std::string trade_code = req_info.dso_name.substr(3);
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetTRInfo(_T("861001"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_pswd"), password.c_str(), 0);		// 계좌비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("qry_tp"), _T("0"), 0);		// 조회구분0:계좌별1:그룹별조회 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_grp_nm"), _T(""), 0);		// 그룹(그룹별조회인경우사용) 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("ftop_tp"), _T("%"), 0);		// 선물옵션구분%:전체F:선물O:옵션 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("sb_tp"), _T("%"), 0);		// 매매구분%:전체S:매도B:매수 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("qry_dt"), _T(""), 0);		// 조회일(없으면당일) 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("usr_id"), _T(""), 0);		// 사용자ID그룹별조회인경우사용 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("stk_cd"), _T("%"), 0);		// 종목코드%:전체 값을 설정합니다.


	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 해외 종목별 일일 손익 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]해외 종목별 일일 손익조회 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

void YaClient::ab_new_order(const std::shared_ptr<OrderRequest>& order_req)
{
	ya_request_map_.clear();
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::AB_ORDER_NEW)];
	const std::string trade_code = req_info.dso_name.substr(3);

	CString format_price;
	format_price.Format("%.0f", static_cast<double>(static_cast<double>(order_req->order_price)));
	std::string order_price = std::string(static_cast<const char*>(format_price));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	유안타증권 Open API 입력코드 예제입니다.
	//	[850001] 해외선물_신규주문 - 입력블록

	//g_iYuantaAPI.YOA_SetTRInfo(_T("850001"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("usr_tp"), _T("0"), 0);		// 사용자구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("acnt_aid"), order_req->account_no.c_str(), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("acnt_pwsd"), order_req->password.c_str(), 0);		// 계좌비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("stk_cd"), order_req->symbol_code.c_str(), 0);		// 종목코드 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("ord_prc"), order_price.c_str(), 0);		// 주문가격 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldDouble(_T("850001"), _T("InBlock1"), _T("ord_qty"), order_req->order_amount, 0);		// 주문수량 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("stop_prc"), _T(""), 0);		// STOP가격 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("sb_tp_cd"), order_req->position_type == SmPositionType::Sell ? "S" : "B", 0);		// 매수/매도(B:매수 S:매도) 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("prc_cond_tp_cd"), order_req->price_type == SmPriceType::Price ? "2" : "1", 0);		// 가격조건 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("akprc_cond_cd"), _T("0"), 0);		// 체결조건(0:FAS 3:FAK 4:FOK) 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("ord_tp_cd"), _T("N"), 0);		// 주문전략구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("ord_tp_day_cd"), _T("1"), 0);		// 주문기간구분(1:DAY 2:GTD) 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("key"), _T(""), 0);		// 주문키(주문번호 Returb 필요시사 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("oco_ogn_ord_no"), _T(""), 0);		// OCO원주문번호(OFDone주문인 경우 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("oco_clrn_tp_cd"), _T(""), 0);		// OCO청산구분코드 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("exci_rsrv_yn"), _T("N"), 0);		// 행사예약여부(Y/N) 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850001"), _T("InBlock1"), _T("cth_prc"), _T(""), 0);		// 포착가격 값을 설정합니다.

	LOGINFO(CMyLogger::getInstance(), _T("ab_new_order:: 시그널이름[%s]"), order_req->order_context.signal_name.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("ab_new_order:: 계좌번호[%s]"), order_req->account_no.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("ab_new_order:: 비밀번호[%s]"), order_req->password.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("ab_new_order:: 종목코드[%s]"), order_req->symbol_code.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("ab_new_order:: 매수/매도[%s]"), std::to_string((int)order_req->position_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("ab_new_order:: 주문가격[%s]"), order_price.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("ab_new_order:: 주문수량[%s]"), std::to_string((int)order_req->order_amount).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("ab_new_order:: 주문타입[%s]"), std::to_string((int)order_req->order_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("ab_new_order:: 가격타입[%s]"), std::to_string((int)order_req->price_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("ab_new_order:: 선물/옵션[%s]"), std::to_string((int)order_req->future_or_option).c_str());

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 해외선물 신규주문 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], reg_id[%d], Request : %s", trade_code.c_str(), req_id, strMsg);
		//request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]해외선물 신규주문 요청중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		//on_task_request_error(arg.argument_id);
		//return -1;
	}

}

void YaClient::ab_change_order(const std::shared_ptr<OrderRequest>& order_req)
{
	ya_request_map_.clear();
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::AB_ORDER_MOD)];
	const std::string trade_code = req_info.dso_name.substr(3);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	유안타증권 Open API 입력코드 예제입니다.
	//	[850002] 해외선물_정정주문 - 입력블록

	g_iYuantaAPI.YOA_SetTRInfo(_T("850002"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850002"), _T("InBlock1"), _T("usr_tp"), _T("0"), 0);		// 사용자구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850002"), _T("InBlock1"), _T("acnt_aid"), order_req->account_no.c_str(), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850002"), _T("InBlock1"), _T("acnt_pswd"), order_req->password.c_str(), 0);		// 계좌비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850002"), _T("InBlock1"), _T("stk_cd"), order_req->symbol_code.c_str(), 0);		// 종목코드 값을 설정합니다.
	std::string order_date = VtStringUtil::getCurentDate();
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850002"), _T("InBlock1"), _T("mdy_ord_dt"), order_date.c_str(), 0);		// 주문일자 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850002"), _T("InBlock1"), _T("mdy_ord_no"), order_req->original_order_no.c_str(), 0);		// 정정주문번호 값을 설정합니다.
	CString format_price;
	format_price.Format("%.0f", static_cast<double>(order_req->order_price ));
	std::string order_price = std::string(CT2CA(format_price));
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850002"), _T("InBlock1"), _T("mdy_ord_prc"), order_price.c_str(), 0);		// 정정가격 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldLong(_T("850002"), _T("InBlock1"), _T("mdy_ord_qty"), order_req->order_amount, 0);		// 정정수량 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850002"), _T("InBlock1"), _T("mdy_stop_prc"), _T(""), 0);		// STOP가격 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850002"), _T("InBlock1"), _T("ord_tp_cd"), _T("N"), 0);		// 주문전략구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850002"), _T("InBlock1"), _T("ord_tp_day_cd"), _T("1"), 0);		// 주문기간구분(1:DAY 2:GTD) 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850002"), _T("InBlock1"), _T("key"), _T(""), 0);		// 주문키(주문번호 Return 필요시사 값을 설정합니다.


	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 해외선물 정정주문 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		//request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]해외선물 정정주문 요청중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		//on_task_request_error(arg.argument_id);
		//return -1;
	}
}

void YaClient::ab_cancel_order(const std::shared_ptr<OrderRequest>& order_req)
{
	ya_request_map_.clear();

	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 시그널이름[%s]"), order_req->order_context.signal_name.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 해외/국내[%s]"), std::to_string((int)order_req->request_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 계좌번호[%s]"), order_req->account_no.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 비밀번호[%s]"), order_req->password.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 원주문번호[%s]"), order_req->original_order_no.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 종목코드[%s]"), order_req->symbol_code.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 매수/매도[%s]"), std::to_string((int)order_req->position_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 주문가격[%s]"), std::to_string((int)order_req->order_price).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 주문수량[%s]"), std::to_string((int)order_req->order_amount).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 주문타입[%s]"), std::to_string((int)order_req->order_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 가격타입[%s]"), std::to_string((int)order_req->price_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 선물/옵션[%s]"), std::to_string((int)order_req->future_or_option).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 주문조건[%s]"), std::to_string((int)order_req->fill_condition).c_str());

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	유안타증권 Open API 입력코드 예제입니다.
	//	[850003] 해외선물_취소주문 - 입력블록

	g_iYuantaAPI.YOA_SetTRInfo(_T("850003"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850003"), _T("InBlock1"), _T("usr_tp"), _T("0"), 0);		// 사용자구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850003"), _T("InBlock1"), _T("acnt_aid"), order_req->account_no.c_str(), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850003"), _T("InBlock1"), _T("acnt_pswd"), order_req->password.c_str(), 0);		// 계좌비밀번호 값을 설정합니다.
	std::string order_date = VtStringUtil::getCurentDate();
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850003"), _T("InBlock1"), _T("cncl_ord_dt"), order_date.c_str(), 0);		// 주문일자 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850003"), _T("InBlock1"), _T("cncl_ord_no"), order_req->original_order_no.c_str(), 0);		// 취소주문번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850003"), _T("InBlock1"), _T("stk_cd"), order_req->symbol_code.c_str(), 0);		// 종목코드 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850003"), _T("InBlock1"), _T("ord_tp_cd"), _T("N"), 0);		// 주문전략구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850003"), _T("InBlock1"), _T("ord_tp_day_cd"), _T("1"), 0);		// 주문기간구분(1:DAY 2:GTD) 값을 설정합니다.
	g_iYuantaAPI.YOA_SetTRFieldString(_T("850003"), _T("InBlock1"), _T("key"), _T(""), 0);		// 주문키(주문번호 Return 필요시사 값을 설정합니다.


	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::AB_ORDER_CANCEL)];
	const std::string trade_code = req_info.dso_name.substr(3);
	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 해외선물 취소주문 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		//request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]해외선물 취소주문 요청중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		//on_task_request_error(arg.argument_id);
		//return -1;
	}
}

void YaClient::on_ab_account_asset(const YA_REQ_INFO& req_info)
{
	//	유안타증권 Open API 출력코드 예제입니다.
//	[860003] 해외선물 계좌별예수금조회 - 출력블록

	if (mainApp.LoginMgr()->IsLoggedIn()) {
		auto found = request_map_.find(req_info.request_id);
		if (found != request_map_.end()) {
			const std::string account_no = found->second.parameter_map["account_no"];
			auto account = mainApp.AcntMgr()->FindAccount(account_no);
			if (account) {
				const int account_id = account->id();
				account->Confirm(1);
				mainApp.CallbackMgr()->OnPasswordConfirmed(account_id, 1);
			}
		}
	}


	TCHAR data[1024] = { 0, };

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetTRFieldString(_T("863007"), _T("InBlock1"), _T("acnt_aid"), data, sizeof(data), 0);		// 계좌번호 값을 설정합니다.
	std::string account_no(data);

	nlohmann::json account_asset;
	g_iYuantaAPI.YOA_SetTRInfo(_T("863007"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// 계좌식별번호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("crc_cd"), data, sizeof(data), 0);		// 통화코드 값을 가져옵니다.
	std::string currency = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tdy_cash_amt"), data, sizeof(data), 0);		// 예탁금잔액 값을 가져옵니다.
	double entrust_total = _ttof(data);
	double balance = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("eval_pl_amt"), data, sizeof(data), 0);		// 평가손익 값을 가져옵니다.
	double outstanding_deposit = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("nmth_ord_opn_prf_amt"), data, sizeof(data), 0);		// 주문증거금 값을 가져옵니다.
	double order_deposit = _ttof(data);

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opn_prf_amt"), data, sizeof(data), 0);		// 위탁증거금 값을 가져옵니다.
	double entrust_deposit = _ttof(data);

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("psv_prf_amt"), data, sizeof(data), 0);		// 유지증거금 값을 가져옵니다.
	double maintenance_margin = _ttof(data);

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("clrn_pl_amt"), data, sizeof(data), 0);		// 확정손익 값을 가져옵니다.
	double settled_profit_loss = _ttof(data);
	
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cmsn_amt"), data, sizeof(data), 0);		// 수수료 값을 가져옵니다.
	double fee = _ttof(data);
	
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("eval_pl_amt"), data, sizeof(data), 0);		// 평가손익 값을 가져옵니다.
	double open_profit_loss = _ttof(data);
	
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("pres_dptam"), data, sizeof(data), 0);		// 예탁자산평가액 값을 가져옵니다.
	double open_trust_total = _ttof(data);


	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("adtn_prf_amt"), data, sizeof(data), 0);		// 추가증거금 값을 가져옵니다.
	double additional_margin = _ttof(data);

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("ord_psb_amt"), data, sizeof(data), 0);		// 주문가능금액 값을 가져옵니다.
	double order_margin = _ttof(data);
	
	account_asset["account_no"] = account_no;
	account_asset["currency"] = currency;
	account_asset["entrust_total"] = entrust_total;
	account_asset["balance"] = balance;
	account_asset["outstanding_deposit"] = outstanding_deposit;
	account_asset["order_deposit"] = order_deposit;
	account_asset["entrust_deposit"] = entrust_deposit;
	account_asset["maintenance_margin"] = maintenance_margin;
	account_asset["settled_profit_loss"] = settled_profit_loss;
	account_asset["fee"] = fee;
	account_asset["open_profit_loss"] = open_profit_loss;
	account_asset["open_trust_total"] = open_trust_total;
	account_asset["additional_margin"] = additional_margin;
	account_asset["order_margin"] = order_margin;

	mainApp.AcntMgr()->on_account_asset(std::move(account_asset));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_ab_account_profit_loss(const YA_REQ_INFO& req_info)
{
	
}

void YaClient::on_ab_accepted_order(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[860005] 해외선물_계좌별미체결내역조회 - 출력블록

	TCHAR data[1024] = { 0, };
	nlohmann::json order_info;

	g_iYuantaAPI.YOA_SetTRInfo(_T("860005"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// 계좌번호 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_accepted_order:: 계좌번호[%s]"), data);
	std::string account_no = data;

	g_iYuantaAPI.YOA_SetTRInfo(_T("860005"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("next"), data, sizeof(data), 0);		// 다음버튼 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// 총갯수 값을 가져옵니다.
	const int list_cnt = _ttoi(data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("860005"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < list_cnt; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("ord_no"), data, sizeof(data), 0);		// 주문번호 값을 가져옵니다.
		std::string order_no = data;
		VtStringUtil::trim(order_no);
		order_info["order_no"] = order_no;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// 계좌번호 값을 가져옵니다.
		
		order_info["account_no"] = account_no;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("stk_cd"), data, sizeof(data), 0);		// 종목 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_ab_accepted_order:: 종목[%s]"), data);
		order_info["symbol_code"] = data;
		std::string symbol_code = data;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("frntrd_sb_tp"), data, sizeof(data), 0);		// 매매구분 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_ab_accepted_order:: 매매구분[%s]"), data);
		std::string buy_sell_tp = data;
		auto npos = buy_sell_tp.find("매도");
		if (npos != std::string::npos) {
			buy_sell_tp = "2";
		}
		else {
			buy_sell_tp = "1";
		}
		order_info["position_type"] = buy_sell_tp;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("ord_prc"), data, sizeof(data), 0);		// 주문가격 값을 가져옵니다.
		order_info["order_price"] = _ttoi(data);
		LOGINFO(CMyLogger::getInstance(), _T("on_ab_accepted_order:: 주문가격[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("ord_qty"), data, sizeof(data), 0);		// 주문수량 값을 가져옵니다.
		order_info["order_amount"] = _ttoi(data);
		LOGINFO(CMyLogger::getInstance(), _T("on_ab_accepted_order:: 주문수량[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("ord_rmqn"), data, sizeof(data), 0);		// 미체결수량 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_ab_accepted_order:: 미체결수량[%s]"), data);
		std::string remain_count = data;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mth_qty"), data, sizeof(data), 0);		// 체결수량 값을 가져옵니다.
		std::string filled_count = data;

		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("org_no"), data, sizeof(data), 0);		// 원주문번호 값을 가져옵니다.
		std::string original_order_no = data;
		VtStringUtil::trim(original_order_no);
		std::string first_order_no = original_order_no;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("frntrd_akprc_tp_cd"), data, sizeof(data), 0);		// 주문구분 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_ab_accepted_order:: 주문구분[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("frntrd_prc_cond_tp_cd"), data, sizeof(data), 0);		// 유형 값을 가져옵니다.
		LOGINFO(CMyLogger::getInstance(), _T("on_ab_accepted_order:: 유형[%s]"), data);

		order_info["order_type"] = "1";
		order_info["order_event"] = OrderEvent::OE_Accepted;
		order_info["account_no"] = account_no;
		order_info["order_no"] = order_no;
		order_info["symbol_code"] = symbol_code;
		//order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
		order_info["original_order_no"] = static_cast<const char*>(original_order_no.c_str());
		order_info["first_order_no"] = static_cast<const char*>(first_order_no.c_str());
		//order_info["order_type"] = static_cast<const char*>(strMan.Trim());
		order_info["remain_count"] = _ttoi(remain_count.c_str());
		order_info["cancelled_count"] = 0;
		order_info["modified_count"] = 0;
		order_info["filled_count"] = _ttoi(filled_count.c_str());
		order_info["order_sequence"] = 1;
		order_info["custom_info"] = "";
		order_info["order_date"] = "20231210";
		order_info["order_time"] = "121300";

		//order_info["filled_price"] = static_cast<const char*>(strFilledPrice.Trim());
		//order_info["filled_amount"] = static_cast<const char*>(strFilledAmount.Trim());

		//order_info["filled_date"] = static_cast<const char*>(strFilledDate.Trim());
		//order_info["filled_time"] = static_cast<const char*>(strFilledTime.Trim());


		mainApp.total_order_manager()->on_order_event(std::move(order_info));
	}

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_ab_symbol_quote(const YA_REQ_INFO& req_info)
{
	auto found = request_map_.find(req_info.request_id);
	if (found == request_map_.end()) {
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[810001] 해외선물_현재가 - 출력블록

	TCHAR data[1024] = { 0, };
	nlohmann::json quote;
	g_iYuantaAPI.YOA_SetTRInfo(_T("810001"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	quote["symbol_code"] = data;
	const std::string symbol_code = data;
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);

	g_iYuantaAPI.YOA_SetTRInfo(_T("810001"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongname"), data, sizeof(data), 0);		// 종목코드설명 값을 가져옵니다.
	quote["symbol_name_kr"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data), 0);		// 등락율 값을 가져옵니다.
	quote["updown_rate"] = data;
	double updown_rate = _ttof(data);
	if (updown_rate > 0) {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		quote["delta_day_sign"] = "-";
		quote["up_down"] = -1;
	}
	else {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 0;
	}
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// 고가시간 값을 가져옵니다.
	quote["time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
	quote["low"] = convert_to_int(symbol_code, data);
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// 거래량 값을 가져옵니다.
	quote["volume"] = _ttoi(data);
	quote["cumulative_amount"] = _ttoi(data);
	quote["preday_volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevlast"), data, sizeof(data), 0);		// 전일종가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("exchangecode"), data, sizeof(data), 0);		// 거래소 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("underclass"), data, sizeof(data), 0);		// 상품구분 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("currencycode"), data, sizeof(data), 0);		// 거래통화 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendate"), data, sizeof(data), 0);		// 거래개시일 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lastdate"), data, sizeof(data), 0);		// 최종거래일 값을 가져옵니다.
	if (symbol) symbol->LastTradeDay(data);
	if (symbol) symbol->ExpireDate(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("fnd"), data, sizeof(data), 0);		// 최종통보일 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("remainday"), data, sizeof(data), 0);		// 잔존일수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expirysettlement"), data, sizeof(data), 0);		// 만기결재방식 값을 가져옵니다.
 	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tradeyn"), data, sizeof(data), 0);		// 거래여부 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("contractsize"), data, sizeof(data), 0);		// 계약크기 값을 가져옵니다.
	symbol->CtrtSize(_ttof(data));
	symbol->seung_su(_ttoi(data));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("ticksize"), data, sizeof(data), 0);		// 틱사이즈 값을 가져옵니다.
	symbol->TickSize(_ttof(data));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tickvalue"), data, sizeof(data), 0);		// 틱가치 값을 가져옵니다.
	symbol->TickValue(_ttof(data));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opentime"), data, sizeof(data), 0);		// 장개시 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("closetime"), data, sizeof(data), 0);		// 장마감 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// 미결제약정 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("marginrate1"), data, sizeof(data), 0);		// 위탁증거금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("marginrate2"), data, sizeof(data), 0);		// 단위증거금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("pointsize"), data, sizeof(data), 0);		// 소수점크기 값을 가져옵니다.
	symbol->decimal(_ttoi(data));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dispscale"), data, sizeof(data), 0);		// 표시진법 값을 가져옵니다.

	ya_stock_client_.OnSymbolQuote(std::move(quote));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_ab_symbol_hoga(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[810002] 해외선물현재가_호가체결 - 출력블록

	TCHAR data[1024] = { 0, };
	nlohmann::json hoga;
	g_iYuantaAPI.YOA_SetTRInfo(_T("810002"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	const std::string symbol_code = data;
	hoga["symbol_code"] = symbol_code;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_fut_hoga:: symbol code[%s]"), symbol_code.c_str());
	g_iYuantaAPI.YOA_SetTRInfo(_T("810002"), _T("HOKALIST"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < 5; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), i);		// 매도호가 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), i);		// 매도호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), i);		// 매도호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_cnt"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), i);		// 매수호가 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), i);		// 매수호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), i);		// 매수호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_cnt"] = _ttoi(data);
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("810002"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime"), data, sizeof(data), 0);		// 호가시간 값을 가져옵니다.
	hoga["hoga_time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 총매도잔량 값을 가져옵니다.
	hoga["tot_sell_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 총매수잔량 값을 가져옵니다.
	hoga["tot_buy_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// 총매도건수 값을 가져옵니다.
	hoga["tot_sell_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// 총매수건수 값을 가져옵니다.
	hoga["tot_buy_cnt"] = _ttoi(data);
	
	ya_stock_client_.OnDmSymbolHoga(std::move(hoga));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_ab_symbol_position(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	유안타증권 Open API 출력코드 예제입니다.
	//	[861001] 해외선물_계좌별보유평가조회 - 출력블록

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("861001"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// 총개수 값을 가져옵니다.
	const int list_cnt = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("next"), data, sizeof(data), 0);		// 다음버튼"1"일경우다음조회존재 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("861001"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < list_cnt; i++) {
		nlohmann::json symbol_position;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// 계좌번호 값을 가져옵니다.
		symbol_position["account_no"] = data;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("acnt_kor_nm"), data, sizeof(data), 0);		// 계좌명 값을 가져옵니다.
		symbol_position["account_name"] = data;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("stk_cd"), data, sizeof(data), 0);		// 종목 값을 가져옵니다.
		const std::string symbol_code = data;
		symbol_position["symbol_code"] = symbol_code;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("sb_tp_cd"), data, sizeof(data), 0);		// 매매구분 값을 가져옵니다.
		const std::string buy_sell_tp = data;
		symbol_position["symbol_position"] = buy_sell_tp == "매수" ? 1 : -1;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("avg_mth_prc"), data, sizeof(data), 0);		// 평균체결가 값을 가져옵니다.
		symbol_position["symbol_avg_price"] = convert_to_int(symbol_code, data);
		symbol_position["symbol_unit_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("opst_qty"), data, sizeof(data), 0);		// 미결제수량 값을 가져옵니다.
		symbol_position["symbol_open_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("fut_eval_pl"), data, sizeof(data), 0);		// 선물평가손익 값을 가져옵니다.
		symbol_position["symbol_open_profit_loss"] = _ttof(data);
		symbol_position["symbol_pre_open_qty"] = 0;


		mainApp.total_position_manager()->on_symbol_position(std::move(symbol_position));
	}

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_ab_symbol_profit_loss(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[861001] 해외선물_계좌별보유평가조회 - 출력블록

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("861001"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// 총개수 값을 가져옵니다.
	int list_cnt = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("next"), data, sizeof(data), 0);		// 다음버튼"1"일경우다음조회존재 값을 가져옵니다.

	std::string account_no;
	g_iYuantaAPI.YOA_SetTRInfo(_T("861001"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < list_cnt; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// 계좌번호 값을 가져옵니다.
		account_no = data;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("lst_acnt_no"), data, sizeof(data), 0);		// 계좌번호 값을 가져옵니다.
		const std::string account_no = data;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("acnt_kor_nm"), data, sizeof(data), 0);		// 계좌명 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("stk_cd"), data, sizeof(data), 0);		// 종목 값을 가져옵니다.
		const std::string symbol_code = data;
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("stk_nm"), data, sizeof(data), 0);		// 종목명 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("sb_tp_cd"), data, sizeof(data), 0);		// 매매구분 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("avg_mth_prc"), data, sizeof(data), 0);		// 평균체결가 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("opst_qty"), data, sizeof(data), 0);		// 미결제수량 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("clrn_psb_qty"), data, sizeof(data), 0);		// 청산가능수량 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("crprc"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("fut_eval_pl"), data, sizeof(data), 0);		// 선물평가손익 값을 가져옵니다.
		double fut_open_profit_loss = _ttof(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("opt_eval_pl"), data, sizeof(data), 0);		// 옵션평가손익 값을 가져옵니다.
		double opt_open_profit_loss = _ttof(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("opt_eval_amt"), data, sizeof(data), 0);		// 옵션평가금액 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("gnrt"), data, sizeof(data), 0);		// 수익률 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("crc_cd"), data, sizeof(data), 0);		// 통화코드 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("exci_rsrv_qty"), data, sizeof(data), 0);		// 행사예약수량 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("exci_req_qty"), data, sizeof(data), 0);		// 행사신청수량 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("fnd_dt"), data, sizeof(data), 0);		// 최초통보일 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("std_dt"), data, sizeof(data), 0);		// 기준일자 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("nmth_qty"), data, sizeof(data), 0);		// 미체결수량 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("futr_tp"), data, sizeof(data), 0);		// 선물옵션구분 값을 가져옵니다.


		nlohmann::json symbol_profit_loss;
		symbol_profit_loss["account_no"] = account_no;
		symbol_profit_loss["account_name"] = "";
		symbol_profit_loss["currency"] = "KRW";
		symbol_profit_loss["symbol_code"] = symbol_code;

		symbol_profit_loss["trade_profit_loss"] = 0;
		symbol_profit_loss["pure_trade_profit_loss"] = 0;

		symbol_profit_loss["trade_fee"] = 0.0f;

		symbol_profit_loss["open_profit_loss"] = fut_open_profit_loss + opt_open_profit_loss;

		mainApp.total_position_manager()->on_symbol_profit_loss(std::move(symbol_profit_loss));
	}

	mainApp.total_position_manager()->update_account_profit_loss(account_no);

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

int YaClient::ab_trade_profit_loss(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::AB_TRADE_PROFIT_LOSS)];
	const std::string trade_code = req_info.dso_name.substr(3);
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	const std::string start_date = arg.parameter_map["start_date"];
	const std::string end_date = arg.parameter_map["end_date"];
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	const std::string futop_tp = "%%";
	const std::string qty_tp = arg.parameter_map["qty_tp"];
	const std::string prc_crc_cd = arg.parameter_map["prc_crc_cd"];
	g_iYuantaAPI.YOA_SetTRInfo(_T("862011"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_pswd"), password.c_str(), 0);		// 계좌비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("str_dt"), start_date.c_str(), 0);		// 시작일자 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("end_dt"), end_date.c_str(), 0);		// 종료일자 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("frndrv_gds_cd"), symbol_code.c_str(), 0);		// 종목코드 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("ftop_tp"), "%", 0);		// 선물옵션구분(%:전체 F:선물 O:옵 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("qty_tp"), qty_tp.c_str(), 0);		// 구분(1:주문번호역순 2:주문번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("prc_crc_cd"), prc_crc_cd.c_str(), 0);		// 통화코드 값을 설정합니다.


	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] 해외 종목별 청산손익 조회를 요청하였습니다."), req_id);
		LOGINFO(CMyLogger::getInstance(), "Trade Code[%s], Request : %s", trade_code.c_str(), strMsg);
		request_map_[req_id] = arg;
		ya_request_map_[req_id] = req_info;
	}
	else
	{
		TCHAR msg[1024] = { 0, };

		int nErrorCode = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(nErrorCode, msg, sizeof(msg));

		CString strErrorMsg;
		strErrorMsg.Format(_T("Error code:[%d] Message[%s]"), nErrorCode, msg);

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]해외 종목별 청산손익 조회중 오류가 발생하였습니다.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

void YaClient::on_ab_trade_profit_loss(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };
	g_iYuantaAPI.YOA_SetTRInfo(_T("862011"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// 계좌번호 값을 설정합니다.
	const std::string account_no = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("frndrv_gds_cd"), data, sizeof(data), 0);		// 종목코드 값을 설정합니다.
	const std::string symbol_code = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prc_crc_cd"), data, sizeof(data), 0);		// 통화코드 값을 설정합니다.
	const std::string currency = data;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	유안타증권 Open API 출력코드 예제입니다.
	//	[862011] 해외선물 일별청산내역조회 - 출력블록


	g_iYuantaAPI.YOA_SetTRInfo(_T("862011"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tot_clrn_qty"), data, sizeof(data), 0);		// 청산수량계 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tot_clrn_pl_amt"), data, sizeof(data), 0);		// 청산손익계 값을 가져옵니다.
	double trade_profit_loss = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tot_bas_cmsn_amt"), data, sizeof(data), 0);		// 수수료계 값을 가져옵니다.
	double trade_fee = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tot_net_pl_amt"), data, sizeof(data), 0);		// 순손익계 값을 가져옵니다.
	double pure_trade_profit_loss = _ttof(data);


	nlohmann::json symbol_profit_loss;
	symbol_profit_loss["account_no"] = account_no;
	symbol_profit_loss["account_name"] = "";
	symbol_profit_loss["currency"] = currency;

	symbol_profit_loss["trade_profit_loss"] = trade_profit_loss;
	symbol_profit_loss["pure_trade_profit_loss"] = pure_trade_profit_loss;
	symbol_profit_loss["trade_fee"] = trade_fee;
	symbol_profit_loss["open_profit_loss"] = 0;

	auto position = mainApp.total_position_manager()->get_position(account_no, symbol_code);
	if (position) mainApp.CallbackMgr()->process_position_event(position);

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::new_order(const std::shared_ptr<OrderRequest>& order_req)
{
	if (!order_req || order_req->request_type == OrderRequestType::None) return;

	if (order_req->request_type == OrderRequestType::Abroad)
		ab_new_order(order_req);
	else
		dm_new_order(order_req);
}

void YaClient::change_order(const std::shared_ptr<OrderRequest>& order_req)
{
	if (!order_req || order_req->request_type == OrderRequestType::None) return;

	if (order_req->request_type == OrderRequestType::Abroad)
		ab_change_order(order_req);
	else
		dm_change_order(order_req);
}

void YaClient::cancel_order(const std::shared_ptr<OrderRequest>& order_req)
{
	if (!order_req || order_req->request_type == OrderRequestType::None) return;
	if (order_req->request_type == OrderRequestType::Abroad)
		ab_cancel_order(order_req);
	else
		dm_cancel_order(order_req);
}

void YaClient::on_ab_order_new(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_ab_order_mod(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_ab_order_cancel(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_ab_realtime_order()
{
	LOGINFO(CMyLogger::getInstance(), _T("////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////)"));
	nlohmann::json order_info;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	유안타증권 Open API 출력코드 예제입니다.
	//	[81] FF_REAL_F7 - 출력블록

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("81"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_data_tp"), data, sizeof(data), 0);		// 데이터 구분( F7 ) 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 데이터 구분( F7 ) 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_proc_tp"), data, sizeof(data), 0);		// 처리구분 01:주문확인 02: 체결 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 처리구분 01:주문확인 02: 체결[%s]"), data);
	const std::string order_event = data;
	if (order_event == "01")
		order_info["order_event"] = OrderEvent::OE_Accepted;
	else if (order_event == "02")
		order_info["order_event"] = OrderEvent::OE_Filled;
	else
		order_info["order_event"] = OrderEvent::OE_Unfilled;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_acct_no"), data, sizeof(data), 0);		// 계좌식별번호 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 계좌식별번호[%s]"), data);	
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_inst_cd"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 종목코드[%s]"), data);
	std::string symbol_code = data;
	order_info["symbol_code"] = symbol_code;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_ordr_tp"), data, sizeof(data), 0);		// 주문구분 N: 일반 M:반대매매 O:O 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 주문구분 N: 일반 M:반대매매 O:O 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_akprc_tp"), data, sizeof(data), 0);		// 호가구분(N신규 M정정 C취소) 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 호가구분(N신규 M정정 C취소) 값[%s]"), data);
	std::string order_type = data;
	if (order_type == "N") order_info["order_type"] = "1";
	else if (order_type == "M") order_info["order_type"] = "2";
	else if (order_type == "C")	order_info["order_type"] = "3";
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_acpt_tp"), data, sizeof(data), 0);		// 접수구분(4접수구분(ORS 접송) 5 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 접수구분(4접수구분(ORS 접송) 5 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_ways_tp"), data, sizeof(data), 0);		// 채널구분 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 채널구분 값 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_ordr_dt"), data, sizeof(data), 0);		// 주문일자 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 주문일자 값[%s]"), data);
	order_info["order_time"] = data;
	order_info["order_date"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_ordr_no"), data, sizeof(data), 0);		// 주문번호 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 주문번호 값[%s]"), data);
	const int order_no = _ttoi(data);
	order_info["order_no"] = std::to_string(order_no);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_orig_ordr_no"), data, sizeof(data), 0);		// 원주문번호 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 원주문번호 값[%s]"), data);
	const int original_order_no = _ttoi(data);
	order_info["original_order_no"] = std::to_string(original_order_no);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_frst_ordr_no"), data, sizeof(data), 0);		// 모주문번호 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 모주문번호 값[%s]"), data);
	const int first_order_no = _ttoi(data);
	order_info["first_order_no"] = std::to_string(first_order_no);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_ordr_grup_no"), data, sizeof(data), 0);		// 주문그룹번호 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 주문그룹번호 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_bysl_tp"), data, sizeof(data), 0);		// 매매구분 (B 매수  S 매도) 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 매매구분 (B 매수  S 매도) 값[%s]"), data);
	const std::string buy_sell = data;
	if (buy_sell == "B") order_info["position_type"] = "1";
	else if (buy_sell == "S") order_info["position_type"] = "2";
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_ordr_p"), data, sizeof(data), 0);		// 주문가격 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 주문가격 값[%s]"), data);
	order_info["order_price"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_ordr_q"), data, sizeof(data), 0);		// 주문수량 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 주문수량 값[%s]"), data);
	order_info["order_amount"] = _ttoi(data);
	order_info["remain_count"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_prce_tp"), data, sizeof(data), 0);		// 가격조건 (1시장가 2지정가 3STOP 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 가격조건 (1시장가 2지정가 3STOP 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_trad_cond"), data, sizeof(data), 0);		// 체결조건 (0 FAS 3FAK 4FOK) 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 체결조건 (0 FAS 3FAK 4FOK) 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_cond_p"), data, sizeof(data), 0);		// 조건가격 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 조건가격 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_user_id"), data, sizeof(data), 0);		// 사용자 아이디 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 사용자 아이디  값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_trad_dt"), data, sizeof(data), 0);		// 체결일자 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 체결일자 값[%s]"), data);
	order_info["filled_date"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_trad_no"), data, sizeof(data), 0);		// 체결번호 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 체결번호 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_trad_q"), data, sizeof(data), 0);		// 체결수량 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 체결수량 값[%s]"), data);
	const int filled_count = _ttoi(data);
	order_info["filled_count"] = filled_count;

	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: 체결수량[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_trad_p"), data, sizeof(data), 0);		// 체결가격 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 체결가격 값[%s]"), data);
	order_info["filled_price"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_trad_amt"), data, sizeof(data), 0);		// 체결금액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_curr_cd"), data, sizeof(data), 0);		// 통화코드 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 통화코드 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_ex_acpt_tm"), data, sizeof(data), 0);		// 거래소주문시간 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 거래소주문시간 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_ex_trad_tm"), data, sizeof(data), 0);		// 거래소체결시간 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 거래소체결시간 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_ordr_tm"), data, sizeof(data), 0);		// 한국주문시간 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 한국주문시간 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_trad_tm"), data, sizeof(data), 0);		// 한국체결시간 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 한국체결시간 값[%s]"), data);
	order_info["filled_time"] = data;

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_veto_msg"), data, sizeof(data), 0);		// 거부메세지 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 거부메세지 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_trad_stat"), data, sizeof(data), 0);		// 체결상태 ( 0미체결 1부분체결 2 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 체결상태 ( 0미체결 1부분체결 2 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_futr_tp"), data, sizeof(data), 0);		// 선물옵션구분 ( F 선물 O옵션) 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 선물옵션구분 ( F 선물 O옵션) 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_lst_acnt_no"), data, sizeof(data), 0);		// 계좌번호 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 계좌번호 값[%s]"), data);
	order_info["account_no"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_ld_prc_nttn_cnt"), data, sizeof(data), 0);		// 가격진법수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_ld_prc_dcpt_cnt"), data, sizeof(data), 0);		// 가격소수점수 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 가격소수점수 값[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("s_remn_q"), data, sizeof(data), 0);		// 주문잔량 값을 가져옵니다.
	LOGINFO(CMyLogger::getInstance(), _T("on_ab_realtime_order:: 주문잔량 값[%s]"), data);
	
	order_info["cancelled_count"] = _ttoi(data);
	order_info["modified_count"] = _ttoi(data);

	order_info["order_sequence"] = 1;
	order_info["custom_info"] = "";

	if (filled_count > 0) {
		order_info["remain_count"] = 0;
	}

	mainApp.total_order_manager()->on_order_event(std::move(order_info));
}

void YaClient::on_ab_realtime_quote()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[61] FF_SISE - 출력블록

	TCHAR data[1024] = { 0, };
	nlohmann::json quote;
	g_iYuantaAPI.YOA_SetTRInfo(_T("61"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongcode"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	std::string symbol_code = data;
	quote["symbol_code"] = symbol_code;
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (!symbol) return;

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
	quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// 미결제약정수량 값을 가져옵니다.
	
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// 미결제약정전일대비수량 값을 가져옵니다.
	quote["preday_volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// 누적체결수량 값을 가져옵니다.
	quote["cumulative_amount"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("changerate"), data, sizeof(data), 0);		// 등락률 값을 가져옵니다.
	quote["updown_rate"] = data;
	double updown_rate = _ttof(data);
	if (updown_rate > 0) {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 1;
	}
	else if (updown_rate < 0) {
		quote["delta_day_sign"] = "-";
		quote["up_down"] = -1;
	}
	else {
		quote["delta_day_sign"] = "+";
		quote["up_down"] = 0;
	}
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time2"), data, sizeof(data), 0);		// 시간_HH:MM:SS 값을 가져옵니다.
	quote["time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// 시간_HH:MM 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// 고가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowtime"), data, sizeof(data), 0);		// 저가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("change"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("nowvol"), data, sizeof(data), 0);		// 직전체결량 값을 가져옵니다.
	quote["volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler1"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("startchange"), data, sizeof(data), 0);		// 시가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highchange"), data, sizeof(data), 0);		// 고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowchange"), data, sizeof(data), 0);		// 저가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler2"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("chegyulrate"), data, sizeof(data), 0);		// 체결강도 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 매수체결합 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 매도체결합 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("pointsize"), data, sizeof(data), 0);		// 소수점크기 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dispscale"), data, sizeof(data), 0);		// 표시진법 값을 가져옵니다.

	ya_stock_client_.OnSymbolQuote(std::move(quote));

}

void YaClient::on_ab_realtime_hoga()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[62] FF_HOKA - 출력블록

	TCHAR data[1024] = { 0, };
	nlohmann::json hoga;
	g_iYuantaAPI.YOA_SetTRInfo(_T("62"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongcode"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	std::string symbol_code = data;
	hoga["symbol_code"] = symbol_code;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// 시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 최우선매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 최우선매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler1"), data, sizeof(data), 0);		// 4bytes맞추기위해 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// 최우선매도호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), 0);		// 최우선매도호가건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// 최우선매수호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), 0);		// 최우선매수호가건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("62"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	for (int i = 0; i < 5; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), i);		// 매도호가 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), i);		// 매수호가 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), i);		// 4bytes맞추기위해 값을 가져옵니다.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), i);		// 매도호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), i);		// 매수호가잔량 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), i);		// 매도호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["sell_cnt"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), i);		// 매수호가건수 값을 가져옵니다.
		hoga["hoga_items"][i]["buy_cnt"] = _ttoi(data);
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("62"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 매도호가수량 값을 가져옵니다.
	hoga["tot_sell_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 매수호가수량 값을 가져옵니다.
	hoga["tot_buy_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// 매도총호가건수 값을 가져옵니다.
	hoga["tot_sell_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// 매수총호가건수 값을 가져옵니다.
	hoga["tot_buy_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time2"), data, sizeof(data), 0);		// 시간 값을 가져옵니다.
	hoga["hoga_time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("diffmesumedo"), data, sizeof(data), 0);		// 매수잔량-매도잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("pointsize"), data, sizeof(data), 0);		// 소수점크기 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dispscale"), data, sizeof(data), 0);		// 표시진법 값을 가져옵니다.
	ya_stock_client_.OnRealtimeHoga(std::move(hoga));
}

int YaClient::register_ab_symbol(const std::string& symbol_code)
{
	g_iYuantaAPI.YOA_SetTRFieldString(_T("61"), _T("InBlock1"), _T("jongcode"), symbol_code.c_str());
	int nResult = g_iYuantaAPI.YOA_RegistAuto(GetSafeHwnd(), _T("61"));

	if (ERROR_MAX_CODE < nResult)
	{
		LOGINFO(CMyLogger::getInstance(), _T("[61] 해외선물 옵션 실시간체결이 등록 되었습니다."));
		//return 1;
	}
	else
	{
		TCHAR msg[2048] = { 0, };
		g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

		LOGINFO(CMyLogger::getInstance(), msg);
	}

	g_iYuantaAPI.YOA_SetTRFieldString(_T("62"), _T("InBlock1"), _T("jongcode"), symbol_code.c_str());
	nResult = g_iYuantaAPI.YOA_RegistAuto(GetSafeHwnd(), _T("62"));

	if (ERROR_MAX_CODE < nResult)
	{
		LOGINFO(CMyLogger::getInstance(), _T("[61] 해외선물 옵션 실시간체결이 등록 되었습니다."));
		//return 1;
	}
	else
	{
		TCHAR msg[2048] = { 0, };
		g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

		LOGINFO(CMyLogger::getInstance(), msg);
	}

	return 1;
}

int YaClient::unregister_ab_symbol(const std::string& symbol_code)
{
	int nResult = g_iYuantaAPI.YOA_UnregistAuto(GetSafeHwnd(), _T("61"));

	if (RESULT_SUCCESS == nResult)
	{
		LOGINFO(CMyLogger::getInstance(), _T("[61]해외 선물 옵션 종목[%s] 실시간체결이 해제 되었습니다."), symbol_code.c_str());
	}
	else
	{
		TCHAR msg[2048] = { 0, };
		g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

		LOGINFO(CMyLogger::getInstance(), msg);
	}

	nResult = g_iYuantaAPI.YOA_UnregistAuto(GetSafeHwnd(), _T("62"));

	if (RESULT_SUCCESS == nResult)
	{
		LOGINFO(CMyLogger::getInstance(), _T("[62]해외 선물 옵션 종목[%s] 실시간체결이 해제 되었습니다."), symbol_code.c_str());
	}
	else
	{
		TCHAR msg[2048] = { 0, };
		g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

		LOGINFO(CMyLogger::getInstance(), msg);
	}

	return 1;
}

int YaClient::register_ab_account(const std::string& account_no)
{
	int nResult = g_iYuantaAPI.YOA_RegistAuto(GetSafeHwnd(), _T("81"));

	if (ERROR_MAX_CODE < nResult)
	{
		LOGINFO(CMyLogger::getInstance(), _T("[81]해외 선물옵션 주문확인_체결 계좌[%s] 실시간체결이 등록 되었습니다."), account_no.c_str());
		return 1;
	}
	else
	{
		TCHAR msg[2048] = { 0, };
		g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

		LOGINFO(CMyLogger::getInstance(), msg);
	}

	return 1;
}

int YaClient::unregister_ab_account(const std::string& account_no)
{
	int nResult = g_iYuantaAPI.YOA_UnregistAuto(GetSafeHwnd(), _T("81"));

	if (RESULT_SUCCESS == nResult)
	{
		LOGINFO(CMyLogger::getInstance(), _T("[81]해외 선물 옵션 실시간체결이 해제 되었습니다."));
	}
	else
	{
		TCHAR msg[2048] = { 0, };
		g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

		LOGINFO(CMyLogger::getInstance(), msg);
	}

	return 1;
}

BOOL DarkHorse::YaClient::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
