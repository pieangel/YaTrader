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
		g_iYuantaAPI.YOA_GetErrorMessage(request_id, msg, sizeof(msg));	// 처리 중 오류에 대한 메시지를 얻을 수 있습니다.
		strMsg.Format(_T("Trade code[%s] [%s] Request id[%d]\n%s"), trade_code.c_str(), desc.c_str(), msg);
		strMsg.TrimRight();
	}
	break;
	}
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
		on_req_dm_order_filled(found->second);
	else if (found->second.req == SERVER_REQ::DM_ORDER_ORDERABLE)
		on_req_dm_order_orderable(found->second);
	else if (found->second.req == SERVER_REQ::DM_ASSET)
		on_req_dm_asset(found->second);
	else if (found->second.req == SERVER_REQ::DM_PROVISIONAL_SETTLEMENT)
		on_req_dm_provisional_settlement(found->second);
	else if (found->second.req == SERVER_REQ::DM_ACCEPTED)
		on_req_dm_accepted(found->second);
	else if (found->second.req == SERVER_REQ::DM_POSITION_INFO)
		on_req_dm_position_info(found->second);
	else if (found->second.req == SERVER_REQ::DM_DAILY_PROFIT_LOSS)
		on_req_dm_daily_profit_loss(found->second);
	else if (found->second.req == SERVER_REQ::DM_LIQUIDABLE_QTY)
		on_req_dm_liquidable_qty(found->second);
	else if (found->second.req == SERVER_REQ::DM_TRADE_PROFIT_LOSS)
		on_req_dm_trade_profit_loss(found->second);
	else if (found->second.req == SERVER_REQ::DM_OUTSTANDING_ORDER)
		on_req_dm_outstanding_order(found->second);
	else if (found->second.req == SERVER_REQ::DM_FUT_SISE)
		on_req_dm_fut_sise(found->second);
	else if (found->second.req == SERVER_REQ::DM_FUT_HOGA)
		on_req_dm_fut_hoga(found->second);
	else if (found->second.req == SERVER_REQ::DM_OPT_SISE)
		on_req_dm_opt_sise(found->second);
	else if (found->second.req == SERVER_REQ::DM_OPT_HOGA)
		on_req_dm_opt_hoga(found->second);
	else if (found->second.req == SERVER_REQ::DM_COMMODITY_SISE)
		on_req_dm_commodity_sise(found->second);
	else if (found->second.req == SERVER_REQ::DM_COMMODITY_HOGA)
		on_req_dm_commodity_hoga(found->second);
	else if (found->second.req == SERVER_REQ::DM_CHART_N_MIN)
		on_req_dm_chart_n_min(found->second);
	else if (found->second.req == SERVER_REQ::DM_ORDER_NEW)
		on_req_dm_new_order(found->second);
	else if (found->second.req == SERVER_REQ::DM_ORDER_MOD)
		on_req_dm_change_order(found->second);
	else if (found->second.req == SERVER_REQ::DM_ORDER_CANCEL)
		on_req_dm_cancel_order(found->second);
	return 0;
}

LRESULT YaClient::OnReceiveRealData(WPARAM wParam, LPARAM lParam)
{
	LPRECV_DATA pRecvData = (LPRECV_DATA)lParam;
	CString strAutoID(pRecvData->szTrCode);

	if (0 == strAutoID.Compare(_T("11")))
	{
		long nData = 0;
		TCHAR data[1024] = { 0, };

		//g_iYuantaAPI.YOA_GetTRFieldLong( _T("11"), _T("OutBlock1"), _T("curjuka"), &nData );
		g_iYuantaAPI.YOA_GetTRFieldString(_T("11"), _T("OutBlock1"), _T("curjuka"), data, sizeof(data));
		//GetDlgItem(IDC_BT_CURJUKA)->SetWindowText(data);

		g_iYuantaAPI.YOA_GetTRFieldString(_T("11"), _T("OutBlock1"), _T("debi"), data, sizeof(data));
		//GetDlgItem(IDC_BT_DEBI)->SetWindowText(data);

		g_iYuantaAPI.YOA_GetTRFieldString(_T("11"), _T("OutBlock1"), _T("debirate"), data, sizeof(data));
		//GetDlgItem(IDC_BT_DEBIRATE)->SetWindowText(data);

		g_iYuantaAPI.YOA_GetTRFieldString(_T("11"), _T("OutBlock1"), _T("volume"), data, sizeof(data));
		//GetDlgItem(IDC_BT_VOLUME)->SetWindowText(data);
	}
	else if (0 == strAutoID.Compare(_T("71")))
	{
		BYTE gubun48 = 0;
		g_iYuantaAPI.YOA_GetTRFieldByte(_T("71"), _T("OutBlock1"), _T("gubun48"), &gubun48);

		TCHAR data[1024] = { 0, };
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetTRFieldString(_T("71"), _T("OutBlock1"), _T("custseq"), data, sizeof(data));

		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetTRFieldString(_T("71"), _T("OutBlock1"), _T("origno"), data, sizeof(data));

		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetTRFieldString(_T("71"), _T("OutBlock1"), _T("ordno"), data, sizeof(data));

		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetTRFieldString(_T("71"), _T("OutBlock1"), _T("jumunno"), data, sizeof(data));

		__int64 cheprcise = 0;
		int nResult = g_iYuantaAPI.YOA_GetTRFieldLong64(_T("71"), _T("OutBlock1"), _T("jumunuv"), &cheprcise);

		if (ERROR_MAX_CODE < nResult)
		{

		}
		else	// 조회 요청 실패 시, 오류 처리
		{
			TCHAR msg[2048] = { 0, };
			g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

			//LogMessage(msg);
		}
	}

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

void YaClient::dm_new_order(const std::shared_ptr<SmOrderRequest>& order_req)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("160001"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("입력값"), 0);		// 비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("jumun_gubun"), _T("입력값"), 0);		// 주문구분1매도2매수 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("meme_gubun"), _T("입력값"), 0);		// 매매구분L지정M시장C조건부B최유 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("jong_code"), _T("입력값"), 0);		// 종목코드 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("order_cnt"), _T("입력값"), 0);		// 주문수량 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("order_price"), _T("입력값"), 0);		// 주문가격 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("jang_gubun"), _T("입력값"), 0);		// 선물옵션구분0선물1옵션2개별3코 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("futu_ord_if"), _T("입력값"), 0);		// 주문조건S일반I일부충족F전량충족 값을 설정합니다.

	

}

void YaClient::dm_change_order(const std::shared_ptr<SmOrderRequest>& order_req)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("160003"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("입력값"), 0);		// 비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("meme_gubun"), _T("입력값"), 0);		// 매매구분L지정M시장C조건부B최유 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("org_order_no"), _T("입력값"), 0);		// 원주문번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("order_cnt"), _T("입력값"), 0);		// 정정주문수량 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("order_price"), _T("입력값"), 0);		// 주문가격 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("jang_gubun"), _T("입력값"), 0);		// 선물옵션구분0선물1옵션2개별3코 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("futu_ord_if"), _T("입력값"), 0);		// 주문조건S일반I일부충족F전량충족 값을 설정합니다.
}

void YaClient::dm_cancel_order(const std::shared_ptr<SmOrderRequest>& order_req)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("160004"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("입력값"), 0);		// 비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("org_order_no"), _T("입력값"), 0);		// 원주문번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("order_cnt"), _T("입력값"), 0);		// 취소주문수량 값을 설정합니다.
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


	g_iYuantaAPI.YOA_SetTRInfo(_T("250009"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("입력값"), 0);		// 계좌비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("date"), _T("입력값"), 0);		// 조회시작일 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("ord_tp"), _T("입력값"), 0);		// 순서구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("qry_tp"), _T("입력값"), 0);		// 조회구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("work_tp"), _T("입력값"), 0);		// 업무구분 값을 설정합니다.

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

int YaClient::dm_asset(DhTaskArg arg)
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

int YaClient::dm_accepted(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("입력값"), 0);		// 계좌비밀번호 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("date"), _T("입력값"), 0);		// 조회시작일 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("ord_tp"), _T("입력값"), 0);		// 순서구분 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("work_tp"), _T("입력값"), 0);		// 업무구분 값을 설정합니다.
	return 1;
}

int YaClient::dm_position_info(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("250032"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("work_tp"), _T("입력값"), 0);		// 업무구분 값을 설정합니다.

	return 1;
}

int YaClient::dm_daily_profit_loss(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("251002"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌 값을 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("입력값"), 0);		// 계좌비밀번호 값을 설정합니다.

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
	g_iYuantaAPI.YOA_SetTRInfo(_T("251009"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌 값을 설정합니다.
	return 1;
}

int YaClient::dm_outstanding_order(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("280002"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("입력값"), 0);		// 계좌번호 값을 설정합니다.
	return 1;
}

int YaClient::dm_fut_sise(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("350001"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), _T("입력값"), 0);		// 선물코드 값을 설정합니다.

	return 1;
}

int YaClient::dm_fut_hoga(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), _T("입력값"), 0);		// 선물코드 값을 설정합니다.

	return 1;
}

int YaClient::dm_opt_sise(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("360001"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), _T("입력값"), 0);		// 옵션코드 값을 설정합니다.

	return 1;
}

int YaClient::dm_opt_hoga(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), _T("입력값"), 0);		// 옵션코드 값을 설정합니다.

	return 1;
}

int YaClient::dm_commodity_sise(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("391001"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), _T("입력값"), 0);		// 주식선물코드 값을 설정합니다.

	return 1;
}

int YaClient::dm_commodity_hoga(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("InBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), _T("입력값"), 0);		// 선물코드 값을 설정합니다.

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
	req_info.dso_name = "DSO251002";
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
	req_info.req = SERVER_REQ::DM_CHART_N_MIN;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵셥정정주문";
	req_info.dso_name = "DSO160003";
	req_info.req = SERVER_REQ::DM_CHART_N_MIN;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "선물옵션취소주문";
	req_info.dso_name = "DSO160004";
	req_info.req = SERVER_REQ::DM_CHART_N_MIN;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);
}

void YaClient::on_req_dm_symbol_profit_loss(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_order_filled(const YA_REQ_INFO& req_info)
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
}

void YaClient::on_req_dm_order_orderable(const YA_REQ_INFO& req_info)
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
}

void YaClient::on_req_dm_asset(const YA_REQ_INFO& req_info)
{

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

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("250013"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kyejwa"), data, sizeof(data), 0);		// 계좌 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jang_gubun"), data, sizeof(data), 0);		// 계좌상품구분 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250013"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tot"), data, sizeof(data), 0);		// 예탁총액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("deposittot"), data, sizeof(data), 0);		// 위탁증거금총액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("orddeposit"), data, sizeof(data), 0);		// 주문가능총증거금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("drawtot"), data, sizeof(data), 0);		// 인출가능총액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("keepdeposit"), data, sizeof(data), 0);		// 유지증거금총액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("addtot"), data, sizeof(data), 0);		// 추가증거금총액 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250013"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cash"), data, sizeof(data), 0);		// 예탁현금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("depositcash"), data, sizeof(data), 0);		// 위탁증거금현금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("ordtotcash"), data, sizeof(data), 0);		// 주문가능총현금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("drawcash"), data, sizeof(data), 0);		// 인출가능현금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("keepcash"), data, sizeof(data), 0);		// 유지증거금총현금 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("addcash"), data, sizeof(data), 0);		// 추가증거금현금 값을 가져옵니다.
}

void YaClient::on_req_dm_provisional_settlement(const YA_REQ_INFO& req_info)
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

}

void YaClient::on_req_dm_accepted(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// 총조회건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jang_gubun"), data, sizeof(data), 0);		// 계좌구분 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("ord_no"), data, sizeof(data), 0);		// 주문번호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jong_code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jong_name"), data, sizeof(data), 0);		// 종목명 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("buy_sell_tp"), data, sizeof(data), 0);		// 매도매수구분 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("order_qty"), data, sizeof(data), 0);		// 주문수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("order_idx"), data, sizeof(data), 0);		// 주문지수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("che_remn_qty"), data, sizeof(data), 0);		// 미체결잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("ord_tp"), data, sizeof(data), 0);		// 주문유형 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("next"), data, sizeof(data), 0);		// 다음버튼 값을 가져옵니다.

}

void YaClient::on_req_dm_position_info(const YA_REQ_INFO& req_info)
{

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("250032"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// 총조회건수 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250032"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jong_code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jong_name"), data, sizeof(data), 0);		// 종목명 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("buy_sell_tp"), data, sizeof(data), 0);		// 매매구분 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("avg_price"), data, sizeof(data), 0);		// 평균가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cur_price"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// 매도잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// 매수잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// 약정수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openvol"), data, sizeof(data), 0);		// 미결제약정수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cnt"), data, sizeof(data), 0);		// 보유수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cntclear"), data, sizeof(data), 0);		// 청산가능수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("eval_amt"), data, sizeof(data), 0);		// 평가금액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("eval_pl"), data, sizeof(data), 0);		// 평가손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rate"), data, sizeof(data), 0);		// 수익률 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("unit"), data, sizeof(data), 0);		// 종목별단위금액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("autocode"), data, sizeof(data), 0);		// 오토시세용종목코드 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("meaip"), data, sizeof(data), 0);		// 매입가액 값을 가져옵니다.

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

}

void YaClient::on_req_dm_daily_profit_loss(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("251002"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// 총조회건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jang_gubun"), data, sizeof(data), 0);		// 시장구분 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("251002"), _T("OutBlock2"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jong_code"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jong_name"), data, sizeof(data), 0);		// 종목명 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("buy_sell_tp"), data, sizeof(data), 0);		// 매수/매도구분 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("now_amount"), data, sizeof(data), 0);		// 보유잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("buy_price"), data, sizeof(data), 0);		// 매입가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("total_buy_price"), data, sizeof(data), 0);		// 매입가액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("now_price"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("total_now_price"), data, sizeof(data), 0);		// 현재가액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// 매도잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// 매수잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// 약정수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openvol"), data, sizeof(data), 0);		// 미결제약정수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maemae_son"), data, sizeof(data), 0);		// 매매손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("clear_son"), data, sizeof(data), 0);		// 청산손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("total_son"), data, sizeof(data), 0);		// 총손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("unit"), data, sizeof(data), 0);		// 종목별단위금액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("autocode"), data, sizeof(data), 0);		// 오토시세용종목코드 값을 가져옵니다.

	g_iYuantaAPI.YOA_SetTRInfo(_T("251002"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("next"), data, sizeof(data), 0);		// 다음여부 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("son"), data, sizeof(data), 0);		// 평가손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("clear_son"), data, sizeof(data), 0);		// 청산분손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("total_son"), data, sizeof(data), 0);		// 총손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("today_total_son"), data, sizeof(data), 0);		// 당일총손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("total_property"), data, sizeof(data), 0);		// 추정예탁 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("buy_opt_amt"), data, sizeof(data), 0);		// 매수옵션현재가액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sell_opt_amt"), data, sizeof(data), 0);		// 매도옵션현재가액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("d1yesu"), data, sizeof(data), 0);		// d+1예수금_오토처리용 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cmsn"), data, sizeof(data), 0);		// 수수료 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("call_sell_amt"), data, sizeof(data), 0);		// 콜매도옵션평가액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("call_buy_amt"), data, sizeof(data), 0);		// 콜매수옵션평가액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("put_sell_amt"), data, sizeof(data), 0);		// 풋매도옵션평가액 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("put_buy_amt"), data, sizeof(data), 0);		// 풋매수옵션평가액 값을 가져옵니다.
}

void YaClient::on_req_dm_liquidable_qty(const YA_REQ_INFO& req_info)
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

}

void YaClient::on_req_dm_trade_profit_loss(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("251009"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sonik"), data, sizeof(data), 0);		// 실현손익 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cmsn"), data, sizeof(data), 0);		// 수수료 값을 가져옵니다.

}

void YaClient::on_req_dm_outstanding_order(const YA_REQ_INFO& req_info)
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


}

void YaClient::on_req_dm_fut_sise(const YA_REQ_INFO& req_info)
{

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("350001"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongname"), data, sizeof(data), 0);		// 종목코드설명 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data), 0);		// 등락율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("standardprice"), data, sizeof(data), 0);		// 기준가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("startdebi"), data, sizeof(data), 0);		// 시가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highdebi"), data, sizeof(data), 0);		// 고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// 고가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
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
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("remainday"), data, sizeof(data), 0);		// 잔존일수 값을 가져옵니다.
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
}

void YaClient::on_req_dm_fut_hoga(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// 매도호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), 0);		// 매도호가건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// 매수호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), 0);		// 매수호가건수 값을 가져옵니다.

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
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 총매도잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 총매수잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// 총매도건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// 총매수건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// 호가시간2 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dongsi"), data, sizeof(data), 0);		// 동시구분 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectjuka"), data, sizeof(data), 0);		// 예상체결가 값을 가져옵니다.
}

void YaClient::on_req_dm_opt_sise(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[360001] 옵션현재가 - 출력블록

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("360001"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongname"), data, sizeof(data), 0);		// 종목명(설명) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data), 0);		// 등락율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("standardprice"), data, sizeof(data), 0);		// 기준가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("startdebi"), data, sizeof(data), 0);		// 시가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highdebi"), data, sizeof(data), 0);		// 고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// 고가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
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
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("remainday"), data, sizeof(data), 0);		// 잔존일수 값을 가져옵니다.
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

}

void YaClient::on_req_dm_opt_hoga(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[360002] 옵션현재가_호가체결 - 출력블록

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// 매도호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), 0);		// 매도호가건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// 매수호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), 0);		// 매수호가건수 값을 가져옵니다.

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
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 총매도잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 총매수잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// 총매도건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// 총매수건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// 호가시간2 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dongsi"), data, sizeof(data), 0);		// 동시구분 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectjuka"), data, sizeof(data), 0);		// 예상체결가 값을 가져옵니다.

}

void YaClient::on_req_dm_commodity_sise(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[391001] 상품선물현재가 - 출력블록

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("391001"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongname"), data, sizeof(data), 0);		// 종목코드설명 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data), 0);		// 등락율 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("standardprice"), data, sizeof(data), 0);		// 기준가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("startdebi"), data, sizeof(data), 0);		// 시가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highdebi"), data, sizeof(data), 0);		// 고가대비 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// 고가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
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
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("remainday"), data, sizeof(data), 0);		// 잔존일수 값을 가져옵니다.
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

}

void YaClient::on_req_dm_commodity_hoga(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[391002] 상품선물현재가_호가체결 - 출력블록

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// 매도호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// 매도호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), 0);		// 매도호가건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// 매수호가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// 매수호가잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), 0);		// 매수호가건수 값을 가져옵니다.

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
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 총매도잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 총매수잔량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// 총매도건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// 총매수건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// 호가시간2 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.

}

void YaClient::on_req_dm_chart_n_min(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_new_order(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("160001"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("msg"), data, sizeof(data), 0);		// 메세지 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("org_no"), data, sizeof(data), 0);		// 원주문번호 값을 가져옵니다.
}

void YaClient::on_req_dm_change_order(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("160003"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("msg"), data, sizeof(data), 0);		// 메세지 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("org_no"), data, sizeof(data), 0);		// 원주문번호 값을 가져옵니다.
}

void YaClient::on_req_dm_cancel_order(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("160004"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("msg"), data, sizeof(data), 0);		// 메세지 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("org_no"), data, sizeof(data), 0);		// 원주문번호 값을 가져옵니다.
}

void YaClient::on_task_complete(const int& server_request_id)
{
	auto it = request_map_.find(server_request_id);
	if (it == request_map_.end()) return;
	const int argument_id = it->second.argument_id;
	request_map_.erase(it);
	g_iYuantaAPI.YOA_ReleaseData(server_request_id);
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

	int nCnt = g_iYuantaAPI.YOA_GetAccountCount();
	for (int i = 0; i < nCnt; i++)
	{
		memset(account, 0x00, sizeof(account));

		if (RESULT_SUCCESS == g_iYuantaAPI.YOA_GetAccount(i, account, sizeof(account)))
		{
			std::shared_ptr<DarkHorse::SmAccount> account_t = std::make_shared<DarkHorse::SmAccount>();
			const std::string account_type = "9";
			account_t->Type(account_type);
			account_t->No(account);
			memset(acctInfo, 0x00, sizeof(acctInfo));
			if (g_iYuantaAPI.YOA_GetAccountInfo(ACCOUNT_INFO_NAME, account, acctInfo, sizeof(acctInfo)))
			{
				account_t->Name(acctInfo);
			}
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
	mainApp.SymMgr()->read_dm_masterfile_ya();
}


void YaClient::on_realtime_order()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	유안타증권 Open API 출력코드 예제입니다.
//	[71] REAL_PA - 출력블록

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("71"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tr_code"), data, sizeof(data), 0);		// TR CODE(1) 0:OR01 1:CH01 2:HO01 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("ordno"), data, sizeof(data), 0);		// 주문접수번호(거래소) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("autocancel"), data, sizeof(data), 0);		// 자동취소여부 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mdmtp"), data, sizeof(data), 0);		// 매체구분 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), 0);		// filler 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cheno"), data, sizeof(data), 0);		// 체결번호(c.yak_num) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("origno"), data, sizeof(data), 0);		// 원주문번호(0) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jumunno"), data, sizeof(data), 0);		// 내부주문번호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("bpjumunno"), data, sizeof(data), 0);		// 지점별 주문번호 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jumunuv"), data, sizeof(data), 0);		// 주문단가(jumun_su) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jumunqty"), data, sizeof(data), 0);		// 주문수량(jumun_price) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("corqty"), data, sizeof(data), 0);		// 정정/취소 수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cheuv"), data, sizeof(data), 0);		// 체결단가(c.yak_price) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cheqty"), data, sizeof(data), 0);		// 체결수량(c.yak_su) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("chesum"), data, sizeof(data), 0);		// 체결수량합 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("shyak"), data, sizeof(data), 0);		// S 근월물 체결가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lnyak"), data, sizeof(data), 0);		// S 원월물 체결가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("bp_time"), data, sizeof(data), 0);		// data 수신 발생시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tr_time"), data, sizeof(data), 0);		// 체결시간(c.yak_time) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("op_id"), data, sizeof(data), 0);		// 고객 Login ID(user_id) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("custseq"), data, sizeof(data), 0);		// 계좌번호(cust_no) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("gubun48"), data, sizeof(data), 0);		// 매수(4) 매도(8) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("stkcode"), data, sizeof(data), 0);		// 종목코드(c.jongcode) 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("stkname"), data, sizeof(data), 0);		// 종목명(' ... ') 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jununjong"), data, sizeof(data), 0);		// L:지정가 M:시장가 C:조건부지정 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("reject"), data, sizeof(data), 0);		// 거부사유 코드 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatp"), data, sizeof(data), 0);		// 호가유형 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("futu_ord_if"), data, sizeof(data), 0);		// 주문조건 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("trtime2"), data, sizeof(data), 0);		// 시간 HH:MM:SS 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// 체결가 또는 주문가 값을 가져옵니다.
}

void YaClient::on_realtime_quote()
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("41"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongcode"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// 시가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// 고가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// 저가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// 현재가 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// 미결제약정수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// 미결제약정전일대비수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopeninterest"), data, sizeof(data), 0);		// 순미결제약정수량_KOFEX 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopendebi"), data, sizeof(data), 0);		// 순미결제약정전일대비수량_KOFEX 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// 누적체결수량 값을 가져옵니다.
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
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// 고가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowtime"), data, sizeof(data), 0);		// 저가시간 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("change"), data, sizeof(data), 0);		// 전일대비 값을 가져옵니다.
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

	g_iYuantaAPI.YOA_SetTRInfo(_T("41"), _T("OutBlock3"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// 매도호가수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// 매수호가수량 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// 매도총호가건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// 매수총호가건수 값을 가져옵니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// 호가시간 값을 가져옵니다.
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

}

void YaClient::on_realtime_hoga()
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("42"), _T("OutBlock1"));			// TR정보(TR명, Block명)를 설정합니다.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongcode"), data, sizeof(data), 0);		// 종목코드 값을 가져옵니다.
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

void YaClient::on_realtime_accepted()
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
	return dm_asset(arg);
}

BOOL DarkHorse::YaClient::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
