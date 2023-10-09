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
	const int nReqID = wParam;
	auto found = ya_request_map_.find(nReqID);
	if (found == ya_request_map_.end()) return 1;
	const std::string& trade_code = found->second.dso_name.substr(3);
	const std::string& desc = found->second.desc;
	CString strMsg;
	switch (lParam)
	{
	case ERROR_TIMEOUT_DATA:			// 설정된 시간 안에 서버로부터 응답이 없는 경우, 타임아웃이 발생합니다. (기본 10초)
	{
		strMsg.Format(_T("Trade Code[%s] [%s] Request id[%d] :: Timeout %s 요청의 응답이 없습니다."), trade_code.c_str(), desc.c_str(), nReqID);
	}
	break;
	case ERROR_REQUEST_FAIL:			// 서버에서 조회TR(DSO) 처리중 오류가 있는 경우 발생합니다.
	{
		TCHAR msg[2048] = { 0, };
		g_iYuantaAPI.YOA_GetErrorMessage(nReqID, msg, sizeof(msg));	// 처리 중 오류에 대한 메시지를 얻을 수 있습니다.
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
		byte gubun48 = 0;
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
	mainApp.LoginMgr()->IsLoggedIn(true);

	LOGINFO(CMyLogger::getInstance(), "로그인 성공 사용자 저장 user id = %s", mainApp.LoginMgr()->id().c_str());

	get_account_list();

	((CMainFrame*)AfxGetMainWnd())->SetAccountInfo();

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

	//if ( RESULT_SUCCESS == g_iYuantaAPI.YOA_Initial( strServer, GetSafeHwnd(), NULL, WMU_STARTMSGID ) )
	if (RESULT_SUCCESS == g_iYuantaAPI.YOA_Initial(strServer, GetSafeHwnd(), _T("C:\\YuantaAPI"), WMU_STARTMSGID))
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

}

void YaClient::dm_change_order(const std::shared_ptr<SmOrderRequest>& order_req)
{

}

void YaClient::dm_cancel_order(const std::shared_ptr<SmOrderRequest>& order_req)
{

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
	return 1;
}

int YaClient::dm_asset(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_provisional_settlement(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_accepted(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_position_info(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_daily_profit_loss(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_liquidable_qty(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_trade_profit_loss(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_outstanding_order(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_fut_sise(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_fut_hoga(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_opt_sise(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_opt_hoga(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_commodity_sise(DhTaskArg arg)
{
	return 1;
}

int YaClient::dm_commodity_hoga(DhTaskArg arg)
{
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

}

void YaClient::on_req_dm_asset(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_provisional_settlement(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_accepted(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_position_info(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_daily_profit_loss(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_liquidable_qty(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_trade_profit_loss(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_outstanding_order(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_fut_sise(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_fut_hoga(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_opt_sise(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_opt_hoga(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_commodity_sise(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_commodity_hoga(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_chart_n_min(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_new_order(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_change_order(const YA_REQ_INFO& req_info)
{

}

void YaClient::on_req_dm_cancel_order(const YA_REQ_INFO& req_info)
{

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

void YaClient::read_domestic_symbols(const int market_type)
{
	int nCount = g_iYuantaAPI.YOA_GetCodeCount(market_type);
	for (int i = 0; i < nCount; i++) {
		TCHAR code[20] = { 0, };
		g_iYuantaAPI.YOA_GetCodeInfoByIndex(MARKET_TYPE_INTERNAL, CODE_INFO_CODE, i, code, sizeof(code));
		TCHAR standard_code[20] = { 0, };
		g_iYuantaAPI.YOA_GetCodeInfoByIndex(MARKET_TYPE_INTERNAL, CODE_INFO_STANDARD_CODE, i, standard_code, sizeof(standard_code));
		TCHAR name[20] = { 0, };
		g_iYuantaAPI.YOA_GetCodeInfoByIndex(MARKET_TYPE_INTERNAL, CODE_INFO_NAME, i, name, sizeof(name));
		TCHAR eng_name[20] = { 0, };
		g_iYuantaAPI.YOA_GetCodeInfoByIndex(MARKET_TYPE_INTERNAL, CODE_INFO_ENG_NAME, i, eng_name, sizeof(eng_name));
		TCHAR jang_gubun[20] = { 0, };
		g_iYuantaAPI.YOA_GetCodeInfoByIndex(MARKET_TYPE_INTERNAL, CODE_INFO_JANG_GUBUN, i, jang_gubun, sizeof(jang_gubun));
	}
}


BOOL DarkHorse::YaClient::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
