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
	case ERROR_TIMEOUT_DATA:			// ������ �ð� �ȿ� �����κ��� ������ ���� ���, Ÿ�Ӿƿ��� �߻��մϴ�. (�⺻ 10��)
	{
		strMsg.Format(_T("Trade Code[%s] [%s] Request id[%d] :: Timeout %s ��û�� ������ �����ϴ�."), trade_code.c_str(), desc.c_str(), request_id);
	}
	break;
	case ERROR_REQUEST_FAIL:			// �������� ��ȸTR(DSO) ó���� ������ �ִ� ��� �߻��մϴ�.
	{
		TCHAR msg[2048] = { 0, };
		error_code = g_iYuantaAPI.YOA_GetLastError();
		g_iYuantaAPI.YOA_GetErrorMessage(request_id, msg, sizeof(msg));	// ó�� �� ������ ���� �޽����� ���� �� �ֽ��ϴ�.
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
		on_dm_accepted(found->second);
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
	return 0;
}

LRESULT YaClient::OnReceiveRealData(WPARAM wParam, LPARAM lParam)
{
	LPRECV_DATA pRecvData = (LPRECV_DATA)lParam;
	CString strAutoID(pRecvData->szTrCode);

	if (0 == strAutoID.Compare(_T("41")))
	{
		on_realtime_quote();
	}
	else if (0 == strAutoID.Compare(_T("71")))
	{
		on_realtime_order();
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

		LOGINFO(CMyLogger::getInstance(), "�α��� ���� ����� ���� user id = %s", mainApp.LoginMgr()->id().c_str());
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
			message =_T("�α����� �����Ͽ����ϴ�.\n");
			message.Append(_T("�����κ��� �α��� ������ �����ϴ�. �ٽ� �õ��Ͽ� �ֽʽÿ�."));
		}
		else
		{
			message = _T("�α����� �����Ͽ����ϴ�.\n");

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
			AfxMessageBox(_T("�������ڷ� �����մϴ�.\n���������� ���º�й�ȣ�� 0000�Դϴ�."));

		LOGINFO(CMyLogger::getInstance(), _T("����Ÿ Open API�� �ʱ�ȭ�Ǿ����ϴ�."));

		g_bInitial = TRUE;
	}
	else
	{
		LOGINFO(CMyLogger::getInstance(), _T("����Ÿ Open API�� �ʱ�ȭ�� �����Ͽ����ϴ�."), 1);

		return;
	}
}

BOOL YaClient::CheckInit()
{
	if (FALSE == g_bInitial)
	{
		AfxMessageBox("����Ÿ Open API �ʱ�ȭ�� �ʿ��մϴ�.", 1);
	}

	return g_bInitial;
}

BOOL YaClient::CheckLogin()
{
	if (FALSE == g_bLogin)
	{
		AfxMessageBox("�α����� �ʿ��մϴ�.", 1);
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
			LOGINFO(CMyLogger::getInstance(), _T("�α��� ��û�� �Ǿ����ϴ�."));
		}
		else
		{
			LOGINFO(CMyLogger::getInstance(), _T("�α��� ��û�� �����Ͽ����ϴ�."));
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
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), order_req->account_no.c_str(), 0);		// ���¹�ȣ ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), order_req->password.c_str(), 0);		// ��й�ȣ ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("jumun_gubun"), order_req->position_type == SmPositionType::Sell ? "1" : "2", 0);		// �ֹ�����1�ŵ�2�ż� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("meme_gubun"), order_req->price_type == SmPriceType::Price ? "L" : "M", 0);		// �Ÿű���L����M����C���Ǻ�B���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("jong_code"), order_req->symbol_code.c_str(), 0);		// �����ڵ� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldLong(_T("order_cnt"), order_req->order_amount, 0);		// �ֹ����� ���� �����մϴ�.

	CString format_price;
	format_price.Format("%.2f", static_cast<double>(static_cast<double>(order_req->order_price) / 100.0));
	std::string order_price = std::string(static_cast<const char*>(format_price));

	g_iYuantaAPI.YOA_SetFieldString(_T("order_price"), order_price.c_str(), 0);		// �ֹ����� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("jang_gubun"), std::to_string(order_req->future_or_option).c_str(), 0);		// �����ɼǱ���0����1�ɼ�2����3�� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("futu_ord_if"), _T("S"), 0);		// �ֹ�����S�Ϲ�I�Ϻ�����F�������� ���� �����մϴ�.


	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: �ñ׳��̸�[%s]"), order_req->order_context.signal_name.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: ���¹�ȣ[%s]"), order_req->account_no.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: ��й�ȣ[%s]"), order_req->password.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: �����ڵ�[%s]"), order_req->symbol_code.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: �ż�/�ŵ�[%s]"), std::to_string((int)order_req->position_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: �ֹ�����[%s]"), order_price.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: �ֹ�����[%s]"), std::to_string((int)order_req->order_amount).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: �ֹ�Ÿ��[%s]"), std::to_string((int)order_req->order_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: ����Ÿ��[%s]"), std::to_string((int)order_req->price_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("dm_new_order:: ����/�ɼ�[%s]"), std::to_string((int)order_req->future_or_option).c_str());

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] �������� �ű��ֹ� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]�������� �ű��ֹ� ��û�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

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
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), order_req->account_no.c_str(), 0);		// ���¹�ȣ ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), order_req->password.c_str(), 0);		// ��й�ȣ ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("meme_gubun"), order_req->price_type == SmPriceType::Price ? "L" : "M", 0);		// �Ÿű���L����M����C���Ǻ�B���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("org_order_no"), order_req->original_order_no.c_str(), 0);		// ���ֹ���ȣ ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldLong(_T("order_cnt"), order_req->order_amount, 0);		// �����ֹ����� ���� �����մϴ�.

	CString format_price;
	format_price.Format("%.2f", static_cast<double>(order_req->order_price / 100.0));
	std::string order_price = std::string(CT2CA(format_price));

	g_iYuantaAPI.YOA_SetFieldString(_T("order_price"), order_price.c_str(), 0);		// �ֹ����� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("jang_gubun"), std::to_string(order_req->future_or_option).c_str(), 0);		// �����ɼǱ���0����1�ɼ�2����3�� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("futu_ord_if"), _T("S"), 0);		// �ֹ�����S�Ϲ�I�Ϻ�����F�������� ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] �������� �����ֹ� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]�������� �����ֹ� ��û�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		//on_task_request_error(arg.argument_id);
		//return -1;
	}
}

void YaClient::dm_cancel_order(const std::shared_ptr<OrderRequest>& order_req)
{
	ya_request_map_.clear();

	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: �ñ׳��̸�[%s]"), order_req->order_context.signal_name.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: �ؿ�/����[%s]"), std::to_string((int)order_req->request_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: ���¹�ȣ[%s]"), order_req->account_no.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: ��й�ȣ[%s]"), order_req->password.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: ���ֹ���ȣ[%s]"), order_req->original_order_no.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: �����ڵ�[%s]"), order_req->symbol_code.c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: �ż�/�ŵ�[%s]"), std::to_string((int)order_req->position_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: �ֹ�����[%s]"), std::to_string((int)order_req->order_price).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: �ֹ�����[%s]"), std::to_string((int)order_req->order_amount).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: �ֹ�Ÿ��[%s]"), std::to_string((int)order_req->order_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: ����Ÿ��[%s]"), std::to_string((int)order_req->price_type).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: ����/�ɼ�[%s]"), std::to_string((int)order_req->future_or_option).c_str());
	LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: �ֹ�����[%s]"), std::to_string((int)order_req->fill_condition).c_str());


	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_ORDER_CANCEL)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), order_req->account_no.c_str(), 0);		// ���¹�ȣ ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), order_req->password.c_str(), 0);		// ��й�ȣ ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("org_order_no"), order_req->original_order_no.c_str(), 0);		// ���ֹ���ȣ ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldLong(_T("order_cnt"), order_req->order_amount, 0);		// ����ֹ����� ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] �������� ����ֹ� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]�������� ����ֹ� ��û�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

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
		strMsg.Format(_T("[ReqID:%d] �����ɼ��ֹ�ü�� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]�����ɼ��ֹ�ü�� ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_order_orderable(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("250012"), _T("InBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("�Է°�"), 0);		// ���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("�Է°�"), 0);		// ���º�й�ȣ ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("jang_gubun"), _T("�Է°�"), 0);		// ���»�ǰ���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("jong_code"), _T("�Է°�"), 0);		// �����ڵ� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("buy_sell_tp"), _T("�Է°�"), 0);		// �ŵ��ż����� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("ord_tp"), _T("�Է°�"), 0);		// �ֹ��������� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("qry_tp"), _T("�Է°�"), 0);		// ��ȸ���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("order_qty"), _T("�Է°�"), 0);		// �ֹ����� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("order_idx"), _T("�Է°�"), 0);		// �ֹ����� ���� �����մϴ�.

	return 1;
}

int YaClient::dm_account_asset(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_ASSET)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// ���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), password.c_str(), 0);		// ���º�й�ȣ ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] �ڻ����� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]�ڻ����� ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

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
	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("InBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("�Է°�"), 0);		// ���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("�Է°�"), 0);		// ���º�й�ȣ ���� �����մϴ�.
	return 1;
}

int YaClient::dm_accepted(DhTaskArg arg)
{
	/*
	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("InBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("�Է°�"), 0);		// ���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("�Է°�"), 0);		// ���º�й�ȣ ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("date"), _T("�Է°�"), 0);		// ��ȸ������ ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("ord_tp"), _T("�Է°�"), 0);		// �������� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("work_tp"), _T("�Է°�"), 0);		// �������� ���� �����մϴ�.
	*/
	//return 1;


	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_ACCEPTED)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// ���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), password.c_str(), 0);		// ���º�й�ȣ ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] ��ü�� �ֹ� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]��ü�� �ֹ� ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

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
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// ���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldByte(_T("work_tp"), 1);		// �������� ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] ���º� ������ ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]���º� ������ ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		

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
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// ���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), password.c_str(), 0);		// ��й�ȣ ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] ���º� ���� ���� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]���º� ���� ������ȸ ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		

		//AfxMessageBox(strErrorMsg);
		on_task_request_error(arg.argument_id);
		return -1;
	}

	return 1;
}

int YaClient::dm_liquidable_qty(DhTaskArg arg)
{
	g_iYuantaAPI.YOA_SetTRInfo(_T("251007"), _T("InBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), _T("�Է°�"), 0);		// ���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("passwd"), _T("�Է°�"), 0);		// ��й�ȣ ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("qry_tp"), _T("�Է°�"), 0);		// ��ȸ���� ���� �����մϴ�.
	return 1;
}

int YaClient::dm_trade_profit_loss(DhTaskArg arg)
{
	YA_REQ_INFO& req_info = ya_req_info_list_[static_cast<int>(SERVER_REQ::DM_TRADE_PROFIT_LOSS)];
	const std::string trade_code = req_info.dso_name.substr(3);
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string account_no = arg.parameter_map["account_no"];
	const std::string password = arg.parameter_map["password"];
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// ���� ���� �����մϴ�.
	//g_iYuantaAPI.YOA_SetFieldByte(_T("work_tp"), 1);		// �������� ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] ���º� �������Ͱ������� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]���º� ���º� �������Ͱ������� ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		

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
	g_iYuantaAPI.YOA_SetFieldString(_T("acnt_aid"), account_no.c_str(), 0);		// ���� ���� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldByte(_T("work_tp"), 1);		// �������� ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] ���º� ��ü�� �ֹ� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]���º� ��ü�� �ֹ� ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		
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
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// ���� ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] ���������ü� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]���������ü� ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);

		

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
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// ���� ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] �������� ȣ�� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]�������� ȣ�� ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



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
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// ���� ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] ���� �ɼǽü� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]���� �ɼǽü�  ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



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
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// ���� ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] ���� �ɼ� ȣ�� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]���� �ɼ� ȣ�� ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



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
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// ���� ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] ��ǰ���� �ü� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]��ǰ���� �ü� ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



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
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// ���� ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] ��ǰ���� ȣ�� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]��ǰ���� ȣ�� ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



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
	req_info.desc = "�����ɼ��ֹ�ü��";
	req_info.dso_name = "DSO250009";
	req_info.req = SERVER_REQ::DM_ORDER_FILLED;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�����ɼ��ֹ����ɼ�����ȸ_����";
	req_info.dso_name = "DSO250012";
	req_info.req = SERVER_REQ::DM_ORDER_ORDERABLE;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�����ɼ����űݻ󼼳���";
	req_info.dso_name = "DSO250013";
	req_info.req = SERVER_REQ::DM_ASSET;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�����ɼǰ����꿹����Ȳ";
	req_info.dso_name = "DSO250014";
	req_info.req = SERVER_REQ::DM_PROVISIONAL_SETTLEMENT;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�����ɼ� ��ü�� ��ȸ";
	req_info.dso_name = "DSO250031";
	req_info.req = SERVER_REQ::DM_ACCEPTED;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�����ɼ� �ܰ� ��ȸ";
	req_info.dso_name = "DSO250032";
	req_info.req = SERVER_REQ::DM_POSITION_INFO;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�����ɼǴ�����";
	req_info.dso_name = "DSO251002";
	req_info.req = SERVER_REQ::DM_DAILY_PROFIT_LOSS;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�����ɼ�û�갡�ɼ���";
	req_info.dso_name = "DSO251007";
	req_info.req = SERVER_REQ::DM_LIQUIDABLE_QTY;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�����ɼǽ������Ͱ���������ȸ";
	req_info.dso_name = "DSO251009";
	req_info.req = SERVER_REQ::DM_TRADE_PROFIT_LOSS;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�̰�������������ȸ";
	req_info.dso_name = "DSO280002";
	req_info.req = SERVER_REQ::DM_OUTSTANDING_ORDER;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�������簡";
	req_info.dso_name = "DSO350001";
	req_info.req = SERVER_REQ::DM_FUT_SISE;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�������簡_ȣ��ü��";
	req_info.dso_name = "DSO350002";
	req_info.req = SERVER_REQ::DM_FUT_HOGA;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�ɼ����簡";
	req_info.dso_name = "DSO360001";
	req_info.req = SERVER_REQ::DM_OPT_SISE;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�ɼ����簡_ȣ��ü��";
	req_info.dso_name = "DSO360002";
	req_info.req = SERVER_REQ::DM_OPT_HOGA;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "��ǰ�������簡";
	req_info.dso_name = "DSO391001";
	req_info.req = SERVER_REQ::DM_COMMODITY_SISE;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "��ǰ�������簡_ȣ��ü��";
	req_info.dso_name = "DSO391002";
	req_info.req = SERVER_REQ::DM_COMMODITY_HOGA;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "íƮn�к�";
	req_info.dso_name = "DSO402001";
	req_info.req = SERVER_REQ::DM_CHART_N_MIN;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�����ɼǽű��ֹ�";
	req_info.dso_name = "DSO160001";
	req_info.req = SERVER_REQ::DM_ORDER_NEW;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�����ɼ������ֹ�";
	req_info.dso_name = "DSO160003";
	req_info.req = SERVER_REQ::DM_ORDER_MOD;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�����ɼ�����ֹ�";
	req_info.dso_name = "DSO160004";
	req_info.req = SERVER_REQ::DM_ORDER_CANCEL;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�̴ϼ������簡";
	req_info.dso_name = "DSO368001";
	req_info.req = SERVER_REQ::DM_FUT_MINI_SISE;
	req_info.request_id = 0;
	ya_req_info_list_.push_back(req_info);

	req_info.desc = "�̴ϼ������簡_ȣ��ü��";
	req_info.dso_name = "DSO368002";
	req_info.req = SERVER_REQ::DM_FUT_MINI_HOGA;
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

	g_iYuantaAPI.YOA_SetTRInfo(_T("250012"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("ksq_brk_prof"), data, sizeof(data), 0);		// ��Ź���ű� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("ksq_brk_prof_cash"), data, sizeof(data), 0);		// �������ű� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("new_qty"), data, sizeof(data), 0);		// �űԼ��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("clear_qty"), data, sizeof(data), 0);		// û����� ���� �����ɴϴ�.

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
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// ���� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: ����[%s]"), data);
	account_asset["account_no"] = std::string(data);
	g_iYuantaAPI.YOA_SetTRInfo(_T("250013"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kyejwa"), data, sizeof(data), 0);		// ���� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: kyejwa[%s]"), data);
	

	memset(data, 0x00, sizeof(data));
	BYTE jang_gubun = 0;
	g_iYuantaAPI.YOA_GetFieldByte(_T("jang_gubun"), &jang_gubun, 0);		// ���»�ǰ���� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250013"), _T("OutBlock2"));			// TR����(TR��, Block��)�� �����մϴ�.
	

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tot"), data, sizeof(data), 0);// ��Ź�Ѿ� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: ��Ź�Ѿ�[%s]"), data);
	account_asset["entrust_total"] = _ttof(data);
	account_asset["balance"] = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("deposittot"), data, sizeof(data), 0);// ��Ź���ű��Ѿ� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: ��Ź���ű��Ѿ�[%s]"), data);
	
	account_asset["open_trust_total"] = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("orddeposit"), data, sizeof(data), 0);// �ֹ����������ű� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: �ֹ����������ű�[%s]"), data);
	account_asset["order_deposit"] = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("drawtot"), data, sizeof(data), 0);// ���Ⱑ���Ѿ� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: ���Ⱑ���Ѿ�[%s]"), data);
	account_asset["outstanding_deposit"] = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("keepdeposit"), data, sizeof(data), 0);// �������ű��Ѿ� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: �������ű��Ѿ�[%s]"), data);
	account_asset["entrust_deposit"] = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("addtot"), data, sizeof(data), 0);// �߰����ű��Ѿ� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_asset:: �߰����ű��Ѿ�[%s]"), data);
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

	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("yesu_yetak_tot"), data, sizeof(data), 0);		// �����ݿ�Ź�Ѿ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("yesu_cash"), data, sizeof(data), 0);		// ���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("yesu_daeyoung"), data, sizeof(data), 0);		// �����ݴ�� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("OutBlock2"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("futjungsan_yt"), data, sizeof(data), 0);		// ������������_��Ź�Ѿ׿� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("optmaesu_yt"), data, sizeof(data), 0);		// �ɼǸż����_��Ź�Ѿ׿� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("optmaedo_yt"), data, sizeof(data), 0);		// �ɼǸŵ����_��Ź�Ѿ׿� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("b_n11_1"), data, sizeof(data), 0);		// ���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("b_n12_1"), data, sizeof(data), 0);		// ��������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("susuryo_yt"), data, sizeof(data), 0);		// ������_��Ź�Ѿ׿� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("OutBlock3"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("futjungsan_cash"), data, sizeof(data), 0);		// ������������_���ݿ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("optmaesu_cash"), data, sizeof(data), 0);		// �ɼǸż����_���ݿ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("optmaedo_cash"), data, sizeof(data), 0);		// �ɼǸŵ����_���ݿ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("b_n11_2"), data, sizeof(data), 0);		// ���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("b_n12_2"), data, sizeof(data), 0);		// ��������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("susuryo_cash"), data, sizeof(data), 0);		// ������_���ݿ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("OutBlock4"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("yesterdaeyoung_yt"), data, sizeof(data), 0);		// ���ϴ��ŵ��ݾ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("todaydaeyoung_yt"), data, sizeof(data), 0);		// ���ϴ��ŵ��ݾ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("OutBlock5"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("yesterdaeyoung_cash"), data, sizeof(data), 0);		// ���ϴ��ŵ��ݾ�_���ݿ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("todaydaeyoung_cash"), data, sizeof(data), 0);		// ���ϴ��ŵ��ݾ�_���ݿ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("250014"), _T("OutBlock6"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("gajung_yetak_tot"), data, sizeof(data), 0);		// �����꿹����_��Ź�Ѿ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("gajung_cash"), data, sizeof(data), 0);		// �����꿹����_���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("gajung_daeyoung"), data, sizeof(data), 0);		// �����꿹����_��� ���� �����ɴϴ�.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_accepted(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };
	nlohmann::json order_info;

	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// ���¹�ȣ ���� �����ɴϴ�.
	const std::string account_no = data;
	order_info["account_no"] = account_no;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: ���¹�ȣ[%s]"), data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// ����ȸ�Ǽ� ���� �����ɴϴ�.
	const int list_cnt = _ttoi(data);
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: ����ȸ�Ǽ�[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jang_gubun"), data, sizeof(data), 0);		// ���±��� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: ���±���[%s]"), data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("OutBlock2"));			// TR����(TR��, Block��)�� �����մϴ�.
	for (int i = 0; i < list_cnt; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("ord_no"), data, sizeof(data), i);		// �ֹ���ȣ ���� �����ɴϴ�.
		order_info["order_no"] = data;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: �ֹ���ȣ[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_code"), data, sizeof(data), i);		// �����ڵ� ���� �����ɴϴ�.
		order_info["symbol_code"] = data;
		const std::string symbol_code = data;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: �����ڵ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_name"), data, sizeof(data), i);		// ����� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: �����[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("buy_sell_tp"), data, sizeof(data), i);		// �ŵ��ż����� ���� �����ɴϴ�.
		std::string buy_sell_tp = data;
		auto npos = buy_sell_tp.find("�ŵ�");
		if (npos != std::string::npos) {
			buy_sell_tp = "2";
		}
		else {
			buy_sell_tp = "1";
		}
		order_info["position_type"] = buy_sell_tp;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: �ŵ��ż�����[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("order_qty"), data, sizeof(data), i);		// �ֹ����� ���� �����ɴϴ�.
		order_info["order_amount"] = _ttoi(data);
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: �ֹ�����[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("order_idx"), data, sizeof(data), i);		// �ֹ����� ���� �����ɴϴ�.
		order_info["order_price"] = convert_to_int(symbol_code, data);
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: �ֹ�����[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("che_remn_qty"), data, sizeof(data), i);		// ��ü���ܷ� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: ��ü���ܷ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("ord_tp"), data, sizeof(data), i);		// �ֹ����� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_accepted:: �ֹ�����[%s]"), data);


		order_info["order_event"] = OrderEvent::OE_Accepted;
		order_info["order_time"] = "00:00:00";
		order_info["order_date"] = "20231020";
		order_info["order_type"] = "1";
		//order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
		order_info["original_order_no"] = "";
		order_info["first_order_no"] = "";
		//order_info["order_type"] = static_cast<const char*>(strMan.Trim());
		order_info["remain_count"] = 0;
		order_info["cancelled_count"] = 0;
		order_info["modified_count"] = 0;
		order_info["filled_count"] = 0;
		order_info["order_sequence"] = 0;
		order_info["custom_info"] = "";

		mainApp.total_order_manager()->on_order_event(std::move(order_info));
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("250031"), _T("OutBlock3"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("next"), data, sizeof(data), 0);		// ������ư ���� �����ɴϴ�.
	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_symbol_position(const YA_REQ_INFO& req_info)
{

	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("250032"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// ���¹�ȣ ���� �����ɴϴ�.
	const std::string account_no = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: ���¹�ȣ[%s]"), data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("250032"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// ����ȸ�Ǽ� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: ����ȸ�Ǽ�[%s]"), data);

	const int list_cnt = _ttoi(data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("250032"), _T("OutBlock2"));			// TR����(TR��, Block��)�� �����մϴ�.
	for (int i = 0; i < list_cnt; i++) {
		nlohmann::json symbol_position;
		symbol_position["account_no"] = account_no;
		symbol_position["account_name"] = "";
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_code"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
		const std::string symbol_code = data;
		symbol_position["symbol_code"] = symbol_code;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: �����ڵ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_name"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: �����[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("buy_sell_tp"), data, sizeof(data), 0);		// �Ÿű��� ���� �����ɴϴ�.
		const std::string buy_sell_tp = data;
		symbol_position["symbol_position"] = buy_sell_tp == "�ż�" ? 1 : -1;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: �Ÿű���[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("avg_price"), data, sizeof(data), 0);		// ��հ� ���� �����ɴϴ�.
		symbol_position["symbol_avg_price"] = convert_to_int(symbol_code, data);
		symbol_position["symbol_unit_price"] = convert_to_int(symbol_code, data);
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: ��հ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("cur_price"), data, sizeof(data), 0);		// ���簡 ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: ���簡[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("cnt"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
		symbol_position["symbol_open_qty"] = _ttoi(data);
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: ��������[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("cntclear"), data, sizeof(data), 0);		// û�갡�ɼ��� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: û�갡�ɼ���[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("eval_amt"), data, sizeof(data), 0);		// �򰡱ݾ� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: �򰡱ݾ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("eval_pl"), data, sizeof(data), 0);		// �򰡼��� ���� �����ɴϴ�.
		symbol_position["symbol_open_profit_loss"] = _ttof(data);
		symbol_position["symbol_pre_open_qty"] = 0;
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: �򰡼���[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("rate"), data, sizeof(data), 0);		// ���ͷ� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: ���ͷ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("unit"), data, sizeof(data), 0);		// ���񺰴����ݾ� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: ���񺰴����ݾ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("autocode"), data, sizeof(data), 0);		// ����ü��������ڵ� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: ����ü��������ڵ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("meaip"), data, sizeof(data), 0);		// ���԰��� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: ���԰���[%s]"), data);

		mainApp.total_position_manager()->on_symbol_position(std::move(symbol_position));
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("250032"), _T("OutBlock3"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("next"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("fut_eval_pl"), data, sizeof(data), 0);		// �����򰡼��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opt_eval_pl"), data, sizeof(data), 0);		// �ɼ��򰡼��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tot_eval_pl"), data, sizeof(data), 0);		// ���򰡼��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tot_rate"), data, sizeof(data), 0);		// ���򰡼��� ���� �����ɴϴ�.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);	
}

void YaClient::on_dm_daily_profit_loss(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("251002"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// ���¹�ȣ ���� �����ɴϴ�.
	const std::string account_no = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: ���¹�ȣ[%s]"), data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("251002"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// ����ȸ�Ǽ� ���� �����ɴϴ�.
	const int list_cnt = _ttoi(data);

	LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: ����ȸ�Ǽ�[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jang_gubun"), data, sizeof(data), 0);		// ���屸�� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: ���屸��[%s]"), data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("251002"), _T("OutBlock2"));			// TR����(TR��, Block��)�� �����մϴ�.
	for (int i = 0; i < list_cnt; i++) {

		nlohmann::json symbol_profit_loss;
		symbol_profit_loss["account_no"] = account_no;
		symbol_profit_loss["account_name"] = "";
		symbol_profit_loss["currency"] = "KRW";
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_code"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
		const std::string symbol_code = data;
		symbol_profit_loss["symbol_code"] = symbol_code;
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �����ڵ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("jong_name"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �����[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("buy_sell_tp"), data, sizeof(data), 0);		// �ż�/�ŵ����� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �ż�/�ŵ�����[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("now_amount"), data, sizeof(data), 0);		// �����ܷ� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �����ܷ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("buy_price"), data, sizeof(data), 0);		// ���԰� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: ���԰�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("total_buy_price"), data, sizeof(data), 0);		// ���԰��� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: ���԰���[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("now_price"), data, sizeof(data), 0);		// ���簡 ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: ���簡[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("total_now_price"), data, sizeof(data), 0);		// ���簡�� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: ���簡��[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// �ŵ�ȣ�� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �ŵ�ȣ��[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// �ŵ��ܷ� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �ŵ��ܷ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// �ż�ȣ�� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �ż�ȣ��[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// �ż��ܷ� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �ż��ܷ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: ��������[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("openvol"), data, sizeof(data), 0);		// �̰����������� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �̰�����������[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("maemae_son"), data, sizeof(data), 0);		// �Ÿż��� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �Ÿż���[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("clear_son"), data, sizeof(data), 0);		// û����� ���� �����ɴϴ�.
		symbol_profit_loss["trade_profit_loss"] = _ttof(data);
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: û�����[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("total_son"), data, sizeof(data), 0);		// �Ѽ��� ���� �����ɴϴ�.
		symbol_profit_loss["pure_trade_profit_loss"] = _ttof(data);
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �Ѽ���[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("unit"), data, sizeof(data), 0);		// ���񺰴����ݾ� ���� �����ɴϴ�.
		LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: ���񺰴����ݾ�[%s]"), data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("autocode"), data, sizeof(data), 0);		// ����ü��������ڵ� ���� �����ɴϴ�.

		symbol_profit_loss["trade_fee"] = 0.0f;
		
		symbol_profit_loss["open_profit_loss"] = 0.0f;

		mainApp.total_position_manager()->on_symbol_profit_loss(std::move(symbol_profit_loss));
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("251002"), _T("OutBlock3"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("next"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("son"), data, sizeof(data), 0);		// �򰡼��� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �򰡼���[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("clear_son"), data, sizeof(data), 0);		// û��м��� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: û��м���[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("total_son"), data, sizeof(data), 0);		// �Ѽ��� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �Ѽ���[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("today_total_son"), data, sizeof(data), 0);		// �����Ѽ��� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �����Ѽ���[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("total_property"), data, sizeof(data), 0);		// ������Ź ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: ������Ź[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("buy_opt_amt"), data, sizeof(data), 0);		// �ż��ɼ����簡�� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �ż��ɼ����簡��[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sell_opt_amt"), data, sizeof(data), 0);		// �ŵ��ɼ����簡�� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: �ŵ��ɼ����簡��[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("d1yesu"), data, sizeof(data), 0);		// d+1������_����ó���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cmsn"), data, sizeof(data), 0);		// ������ ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("dm_daily_pl:: ������[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("call_sell_amt"), data, sizeof(data), 0);		// �ݸŵ��ɼ��򰡾� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("call_buy_amt"), data, sizeof(data), 0);		// �ݸż��ɼ��򰡾� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("put_sell_amt"), data, sizeof(data), 0);		// ǲ�ŵ��ɼ��򰡾� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("put_buy_amt"), data, sizeof(data), 0);		// ǲ�ż��ɼ��򰡾� ���� �����ɴϴ�.


	mainApp.total_position_manager()->update_account_profit_loss(account_no);

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_liquidable_qty(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("251007"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("list_cnt"), data, sizeof(data), 0);		// ����ȸ�Ǽ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("251007"), _T("OutBlock2"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jong_code"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jong_name"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("result_tp"), data, sizeof(data), 0);		// ������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("qty"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("clear_qty"), data, sizeof(data), 0);		// û�갡�ɼ��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("avg_price"), data, sizeof(data), 0);		// ��հ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("miche_qty"), data, sizeof(data), 0);		// ��ü����� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("251007"), _T("OutBlock3"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("next"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_trade_profit_loss(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	nlohmann::json trade_profit_loss;

	g_iYuantaAPI.YOA_SetTRInfo(_T("251009"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("acnt_aid"), data, sizeof(data), 0);		// ���¹�ȣ ���� �����ɴϴ�.
	const std::string account_no = data;
	trade_profit_loss["account_no"] = account_no;
	LOGINFO(CMyLogger::getInstance(), _T("on_dm_symbol_position:: ���¹�ȣ[%s]"), data);

	g_iYuantaAPI.YOA_SetTRInfo(_T("251009"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sonik"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	trade_profit_loss["trade_profit_loss"] = _ttof(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cmsn"), data, sizeof(data), 0);		// ������ ���� �����ɴϴ�.
	trade_profit_loss["trade_fee"] = _ttof(data);


	mainApp.total_position_manager()->on_trade_profit_loss(std::move(trade_profit_loss));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_outstanding_order(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("280002"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongcode"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medojango"), data, sizeof(data), 0);		// �ŵ��̰����ܰ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesujango"), data, sizeof(data), 0);		// �ż��̰����ܰ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medomoney"), data, sizeof(data), 0);		// �ŵ������ݾ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesumoney"), data, sizeof(data), 0);		// �ż������ݾ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoopen"), data, sizeof(data), 0);		// �ŵ���ü���ܰ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuopen"), data, sizeof(data), 0);		// �ż���ü���ܰ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("curprice"), data, sizeof(data), 0);		// ���簡 ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("profit"), data, sizeof(data), 0);		// �򰡼��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jangbuga"), data, sizeof(data), 0);		// ��ΰ� ���� �����ɴϴ�.

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
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	quote["symbol_code"] = data;
	const std::string symbol_code = data;
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	g_iYuantaAPI.YOA_SetTRInfo(_T("350001"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongname"), data, sizeof(data), 0);		// �����ڵ弳�� ���� �����ɴϴ�.
	quote["symbol_name_kr"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// ���簡 ���� �����ɴϴ�.
	quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// ���ϴ�� ���� �����ɴϴ�.
	quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
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
	g_iYuantaAPI.YOA_GetFieldString(_T("standardprice"), data, sizeof(data), 0);		// ���ذ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// �ŵ�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// �ż�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// �ð� ���� �����ɴϴ�.
	quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("startdebi"), data, sizeof(data), 0);		// �ð���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// �� ���� �����ɴϴ�.
	quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highdebi"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// ���ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	quote["time"] = data;
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// ���� ���� �����ɴϴ�.
	quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowdebi"), data, sizeof(data), 0);		// ������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowtime"), data, sizeof(data), 0);		// �����ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice"), data, sizeof(data), 0);		// ���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice"), data, sizeof(data), 0);		// ���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxcb"), data, sizeof(data), 0);		// cb���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mincb"), data, sizeof(data), 0);		// cb���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospi"), data, sizeof(data), 0);		// �ڽ��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospidebi"), data, sizeof(data), 0);		// ���ϴ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospirate"), data, sizeof(data), 0);		// �ڽ��ǵ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theoreticalprice"), data, sizeof(data), 0);		// �̷а� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rate"), data, sizeof(data), 0);		// ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basist"), data, sizeof(data), 0);		// �̷к��̽ý� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basism"), data, sizeof(data), 0);		// ���庣�̽ý� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	quote["volume"] = _ttoi(data);
	quote["cumulative_amount"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("svolume"), data, sizeof(data), 0);		// ����������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("money"), data, sizeof(data), 0);		// �������(�鸸) ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("smoney"), data, sizeof(data), 0);		// ���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// �̰������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevvolume"), data, sizeof(data), 0);		// ���Ͼ������� ���� �����ɴϴ�.
	quote["preday_volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sprevvolume"), data, sizeof(data), 0);		// ���Ͻ���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevmoney"), data, sizeof(data), 0);		// ���Ͼ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sprevmoney"), data, sizeof(data), 0);		// ���Ͻ��������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// ���Ϲ̰���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopeninterest"), data, sizeof(data), 0);		// ���̰������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopendebi"), data, sizeof(data), 0);		// ���ϼ��̰���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendate"), data, sizeof(data), 0);		// �ŷ������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lastdate"), data, sizeof(data), 0);		// �����ŷ��� ���� �����ɴϴ�.
	if (symbol) symbol->LastTradeDay(data);
	if (symbol) symbol->ExpireDate(data);

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("remainday"), data, sizeof(data), 0);		// �����ϼ� ���� �����ɴϴ�.
	if (symbol) symbol->RemainDays(_ttoi(data));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highprice"), data, sizeof(data), 0);		// �ְ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowprice"), data, sizeof(data), 0);		// ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebi"), data, sizeof(data), 0);		// �ְ���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebi"), data, sizeof(data), 0);		// ��������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebirate"), data, sizeof(data), 0);		// �ְ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebirate"), data, sizeof(data), 0);		// ����������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedate"), data, sizeof(data), 0);		// �ְ����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedate"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("weightedavg"), data, sizeof(data), 0);		// ������հ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cdinterest"), data, sizeof(data), 0);		// CD�ݸ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mememulti"), data, sizeof(data), 0);		// �ŷ��¼� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("restriction_gbn"), data, sizeof(data), 0);		// �ǽð����Ѱ����뿩�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice2"), data, sizeof(data), 0);		// ���߻��Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice2"), data, sizeof(data), 0);		// �������Ѱ� ���� �����ɴϴ�.


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
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	const std::string symbol_code = data;
	hoga["symbol_code"] = symbol_code;
	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	for (int i = 0; i < 5; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), i);		// �ŵ�ȣ�� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), i);		// �ŵ�ȣ���ܷ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), i);		// �ŵ�ȣ���Ǽ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_cnt"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), i);		// �ż�ȣ�� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), i);		// �ż�ȣ���ܷ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), i);		// �ż�ȣ���Ǽ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_cnt"] = _ttoi(data);
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock2"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// ȣ���Ǽ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock3"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// ȣ���Ǽ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock4"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// �����ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// ���ϴ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("nowvol"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock5"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime"), data, sizeof(data), 0);		// ȣ���ð� ���� �����ɴϴ�.
	hoga["hoga_time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// �Ѹŵ��ܷ� ���� �����ɴϴ�.
	hoga["tot_sell_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// �Ѹż��ܷ� ���� �����ɴϴ�.
	hoga["tot_buy_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// �Ѹŵ��Ǽ� ���� �����ɴϴ�.
	hoga["tot_sell_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// �Ѹż��Ǽ� ���� �����ɴϴ�.
	hoga["tot_buy_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// ȣ���ð�2 ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// ���簡 ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dongsi"), data, sizeof(data), 0);		// ���ñ��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectjuka"), data, sizeof(data), 0);		// ����ü�ᰡ ���� �����ɴϴ�.


	ya_stock_client_.OnDmSymbolHoga(std::move(hoga));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_opt_sise(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����Ÿ���� Open API ����ڵ� �����Դϴ�.
//	[360001] �ɼ����簡 - ��º��
	nlohmann::json quote;
	TCHAR data[1024] = { 0, };
	g_iYuantaAPI.YOA_SetTRInfo(_T("360001"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	quote["symbol_code"] = data;
	const std::string symbol_code = data;
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	g_iYuantaAPI.YOA_SetTRInfo(_T("360001"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongname"), data, sizeof(data), 0);		// �����(����) ���� �����ɴϴ�.
	quote["symbol_name_kr"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// ���簡 ���� �����ɴϴ�.
	quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// ���ϴ�� ���� �����ɴϴ�.
	quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
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
	g_iYuantaAPI.YOA_GetFieldString(_T("standardprice"), data, sizeof(data), 0);		// ���ذ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// �ŵ�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// �ż�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// �ð� ���� �����ɴϴ�.
	quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("startdebi"), data, sizeof(data), 0);		// �ð���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// �� ���� �����ɴϴ�.
	quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highdebi"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// ���ð� ���� �����ɴϴ�.
	quote["time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// ���� ���� �����ɴϴ�.
	quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowdebi"), data, sizeof(data), 0);		// ������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowtime"), data, sizeof(data), 0);		// �����ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice"), data, sizeof(data), 0);		// ���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice"), data, sizeof(data), 0);		// ���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospi"), data, sizeof(data), 0);		// �ڽ��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospidebi"), data, sizeof(data), 0);		// ���ϴ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospirate"), data, sizeof(data), 0);		// �ڽ��ǵ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theoreticalprice"), data, sizeof(data), 0);		// �̷а� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volatility"), data, sizeof(data), 0);		// ���纯���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("introvalue"), data, sizeof(data), 0);		// ���簡ġ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("delta"), data, sizeof(data), 0);		// ��Ÿ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("gamma"), data, sizeof(data), 0);		// ���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theta"), data, sizeof(data), 0);		// ��Ÿ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vega"), data, sizeof(data), 0);		// ���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rho"), data, sizeof(data), 0);		// �� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	quote["volume"] = _ttoi(data);
	quote["cumulative_amount"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("money"), data, sizeof(data), 0);		// ������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// �̰������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevvolume"), data, sizeof(data), 0);		// ���Ͼ������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevmoney"), data, sizeof(data), 0);		// ���ϰŷ���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// ���Ϲ̰���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevopen"), data, sizeof(data), 0);		// ���Ϲ̰������� ���� �����ɴϴ�.
	quote["preday_volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highprice"), data, sizeof(data), 0);		// �ְ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebi"), data, sizeof(data), 0);		// �ְ���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebirate"), data, sizeof(data), 0);		// �ְ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedate"), data, sizeof(data), 0);		// �ְ����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowprice"), data, sizeof(data), 0);		// ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebi"), data, sizeof(data), 0);		// ��������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebirate"), data, sizeof(data), 0);		// ����������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedate"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendate"), data, sizeof(data), 0);		// �ŷ������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lastdate"), data, sizeof(data), 0);		// �����ŷ��� ���� �����ɴϴ�.
	if (symbol) symbol->LastTradeDay(data);
	if (symbol) symbol->ExpireDate(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("remainday"), data, sizeof(data), 0);		// �����ϼ� ���� �����ɴϴ�.
	if (symbol) symbol->RemainDays(_ttoi(data));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("weightedavg"), data, sizeof(data), 0);		// ������հ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mememulti"), data, sizeof(data), 0);		// �ŷ��¼� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("restriction_gbn"), data, sizeof(data), 0);		// �ǽð����Ѱ����뿩�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice2"), data, sizeof(data), 0);		// ���߻��Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice2"), data, sizeof(data), 0);		// �������Ѱ� ���� �����ɴϴ�.

	ya_stock_client_.OnSymbolQuote(std::move(quote));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_opt_hoga(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����Ÿ���� Open API ����ڵ� �����Դϴ�.
//	[360002] �ɼ����簡_ȣ��ü�� - ��º��

	TCHAR data[1024] = { 0, };
	nlohmann::json hoga;
	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	const std::string symbol_code = data;
	hoga["symbol_code"] = symbol_code;
	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	for (int i = 0; i < 5; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), i);		// �ŵ�ȣ�� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), i);		// �ŵ�ȣ���ܷ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), i);		// �ŵ�ȣ���Ǽ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_cnt"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), i);		// �ż�ȣ�� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), i);		// �ż�ȣ���ܷ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), i);		// �ż�ȣ���Ǽ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_cnt"] = _ttoi(data);
	}
	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("OutBlock2"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// ȣ���Ǽ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("OutBlock3"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// ȣ���Ǽ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("OutBlock4"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// �����ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// ���ϴ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("nowvol"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("360002"), _T("OutBlock5"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime"), data, sizeof(data), 0);		// ȣ���ð� ���� �����ɴϴ�.
	hoga["hoga_time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// �Ѹŵ��ܷ� ���� �����ɴϴ�.
	hoga["tot_sell_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// �Ѹż��ܷ� ���� �����ɴϴ�.
	hoga["tot_buy_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// �Ѹŵ��Ǽ� ���� �����ɴϴ�.
	hoga["tot_sell_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// �Ѹż��Ǽ� ���� �����ɴϴ�.
	hoga["tot_buy_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// ȣ���ð�2 ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// ���簡 ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dongsi"), data, sizeof(data), 0);		// ���ñ��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectjuka"), data, sizeof(data), 0);		// ����ü�ᰡ ���� �����ɴϴ�.

	ya_stock_client_.OnDmSymbolHoga(std::move(hoga));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_commodity_sise(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����Ÿ���� Open API ����ڵ� �����Դϴ�.
//	[391001] ��ǰ�������簡 - ��º��

	TCHAR data[1024] = { 0, };
	nlohmann::json quote;
	g_iYuantaAPI.YOA_SetTRInfo(_T("391001"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	quote["symbol_code"] = data;
	const std::string symbol_code = data;
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_SetTRInfo(_T("391001"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongname"), data, sizeof(data), 0);		// �����ڵ弳�� ���� �����ɴϴ�.
	quote["symbol_name_kr"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// ���簡 ���� �����ɴϴ�.
	quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// ���ϴ�� ���� �����ɴϴ�.
	quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
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
	g_iYuantaAPI.YOA_GetFieldString(_T("standardprice"), data, sizeof(data), 0);		// ���ذ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// �ŵ�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// �ż�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// �ð� ���� �����ɴϴ�.
	quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("startdebi"), data, sizeof(data), 0);		// �ð���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// �� ���� �����ɴϴ�.
	quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highdebi"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// ���ð� ���� �����ɴϴ�.
	quote["time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// ���� ���� �����ɴϴ�.
	quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowdebi"), data, sizeof(data), 0);		// ������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowtime"), data, sizeof(data), 0);		// �����ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice"), data, sizeof(data), 0);		// ���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice"), data, sizeof(data), 0);		// ���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxcb"), data, sizeof(data), 0);		// cb���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mincb"), data, sizeof(data), 0);		// cb���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("baseid"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basename"), data, sizeof(data), 0);		// ��������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("baseprice"), data, sizeof(data), 0);		// �������簡 ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basedebi"), data, sizeof(data), 0);		// �������ϴ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("baserate"), data, sizeof(data), 0);		// ��������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theoreticalprice"), data, sizeof(data), 0);		// �̷а� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rate"), data, sizeof(data), 0);		// ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basist"), data, sizeof(data), 0);		// �̷к��̽ý� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basism"), data, sizeof(data), 0);		// ���庣�̽ý� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	quote["volume"] = _ttoi(data);
	quote["cumulative_amount"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("svolume"), data, sizeof(data), 0);		// ����������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("money"), data, sizeof(data), 0);		// �������(�鸸) ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("smoney"), data, sizeof(data), 0);		// ���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// �̰������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevvolume"), data, sizeof(data), 0);		// ���Ͼ������� ���� �����ɴϴ�.
	quote["preday_volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sprevvolume"), data, sizeof(data), 0);		// ���Ͻ���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevmoney"), data, sizeof(data), 0);		// ���Ͼ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sprevmoney"), data, sizeof(data), 0);		// ���Ͻ��������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// ���Ϲ̰���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopeninterest"), data, sizeof(data), 0);		// ���̰������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopendebi"), data, sizeof(data), 0);		// ���ϼ��̰���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendate"), data, sizeof(data), 0);		// �ŷ������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lastdate"), data, sizeof(data), 0);		// �����ŷ��� ���� �����ɴϴ�.
	if (symbol) symbol->LastTradeDay(data);
	if (symbol) symbol->ExpireDate(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("remainday"), data, sizeof(data), 0);		// �����ϼ� ���� �����ɴϴ�.
	if (symbol) symbol->RemainDays(_ttoi(data));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highprice"), data, sizeof(data), 0);		// �ְ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowprice"), data, sizeof(data), 0);		// ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebi"), data, sizeof(data), 0);		// �ְ���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebi"), data, sizeof(data), 0);		// ��������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebirate"), data, sizeof(data), 0);		// �ְ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebirate"), data, sizeof(data), 0);		// ����������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedate"), data, sizeof(data), 0);		// �ְ����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedate"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("weightedavg"), data, sizeof(data), 0);		// ������հ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("restriction_gbn"), data, sizeof(data), 0);		// �ǽð����Ѱ����뿩�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice2"), data, sizeof(data), 0);		// ���߻��Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice2"), data, sizeof(data), 0);		// �������Ѱ� ���� �����ɴϴ�.

	ya_stock_client_.OnSymbolQuote(std::move(quote));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);

}

void YaClient::on_dm_commodity_hoga(const YA_REQ_INFO& req_info)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����Ÿ���� Open API ����ڵ� �����Դϴ�.
//	[391002] ��ǰ�������簡_ȣ��ü�� - ��º��

	TCHAR data[1024] = { 0, };
	nlohmann::json hoga;
	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("InBlock1"));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	const std::string symbol_code = data;
	hoga["symbol_code"] = symbol_code;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	for (int i = 0; i < 5; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), i);		// �ŵ�ȣ�� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), i);		// �ŵ�ȣ���ܷ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), i);		// �ŵ�ȣ���Ǽ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_cnt"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), i);		// �ż�ȣ�� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), i);		// �ż�ȣ���ܷ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), i);		// �ż�ȣ���Ǽ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_cnt"] = _ttoi(data);
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("OutBlock2"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// ȣ���Ǽ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("OutBlock3"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// ȣ���Ǽ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("OutBlock4"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// �����ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// ���ϴ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("nowvol"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("391002"), _T("OutBlock5"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime"), data, sizeof(data), 0);		// ȣ���ð� ���� �����ɴϴ�.
	hoga["hoga_time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// �Ѹŵ��ܷ� ���� �����ɴϴ�.
	hoga["tot_sell_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// �Ѹż��ܷ� ���� �����ɴϴ�.
	hoga["tot_buy_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// �Ѹŵ��Ǽ� ���� �����ɴϴ�.
	hoga["tot_sell_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// �Ѹż��Ǽ� ���� �����ɴϴ�.
	hoga["tot_buy_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// ȣ���ð�2 ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// ���簡 ���� �����ɴϴ�.

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

	g_iYuantaAPI.YOA_SetTRInfo(_T("160001"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("msg"), data, sizeof(data), 0);		// �޼��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("org_no"), data, sizeof(data), 0);		// ���ֹ���ȣ ���� �����ɴϴ�.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_change_order(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("160003"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("msg"), data, sizeof(data), 0);		// �޼��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("org_no"), data, sizeof(data), 0);		// ���ֹ���ȣ ���� �����ɴϴ�.

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

void YaClient::on_dm_cancel_order(const YA_REQ_INFO& req_info)
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("160004"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("msg"), data, sizeof(data), 0);		// �޼��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("org_no"), data, sizeof(data), 0);		// ���ֹ���ȣ ���� �����ɴϴ�.

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
		AfxMessageBox(_T("���������� �����ϴ�."));
	}
}

#define CODETYPE_FUTURE  4
#define CODETYPE_OPTION  5
#define INFOTYPE_CODE     0

void YaClient::get_symbol_list(const int market_type)
{
	mainApp.SymMgr()->read_domestic_productfile();
	mainApp.SymMgr()->read_dm_masterfile_ya();
}


void YaClient::on_realtime_order()
{
	LOGINFO(CMyLogger::getInstance(), _T("////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////)"));
//	����Ÿ���� Open API ����ڵ� �����Դϴ�.
//	[71] REAL_PA - ��º��

	TCHAR data[1024] = { 0, };
	nlohmann::json order_info;

	g_iYuantaAPI.YOA_SetTRInfo(_T("71"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tr_code"), data, sizeof(data), 0);		// TR CODE(1) 0:OR01 1:CH01 2:HO01 ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: TR CODE(1) 0:OR01 1:CH01 2:HO01[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("ordno"), data, sizeof(data), 0);		// �ֹ�������ȣ(�ŷ���) ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: �ֹ�������ȣ(�ŷ���)[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("autocancel"), data, sizeof(data), 0);		// �ڵ���ҿ��� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: �ڵ���ҿ���[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mdmtp"), data, sizeof(data), 0);		// ��ü���� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: ��ü����[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), 0);		// filler ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: filler[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cheno"), data, sizeof(data), 0);		// ü���ȣ(c.yak_num) ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: ü���ȣ[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("origno"), data, sizeof(data), 0);		// ���ֹ���ȣ(0) ���� �����ɴϴ�.
	order_info["original_order_no"] = data;
	order_info["first_order_no"] = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: ���ֹ���ȣ[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jumunno"), data, sizeof(data), 0);		// �����ֹ���ȣ ���� �����ɴϴ�.
	//order_info["order_no"] = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: �����ֹ���ȣ[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("bpjumunno"), data, sizeof(data), 0);		// ������ �ֹ���ȣ ���� �����ɴϴ�.
	// �̹�ȣ�� ���� ó���� �ֹ� ��ȣ�� �˴ϴ�. �׸��� ����, ��� �ֹ��� �� �ֹ� ��ȣ�� ����ؾ� �մϴ�. 
	order_info["order_no"] = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: ������ �ֹ���ȣ[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jumunuv"), data, sizeof(data), 0);		// �ֹ��ܰ�(jumun_su) ���� �����ɴϴ�.
	order_info["order_price"] = _ttoi(data);
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: �ֹ��ܰ�[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jumunqty"), data, sizeof(data), 0);		// �ֹ�����(jumun_price) ���� �����ɴϴ�.
	order_info["order_amount"] = _ttoi(data);
	order_info["remain_count"] = _ttoi(data);
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: �ֹ�����[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("corqty"), data, sizeof(data), 0);		// ����/��� ���� ���� �����ɴϴ�.
	const int modified_count = _ttoi(data);
	
	order_info["cancelled_count"] = modified_count;
	order_info["modified_count"] = modified_count;

	if (modified_count > 0) 
		order_info["order_type"] = "2";
	else
		order_info["order_type"] = "1";

	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order::  ����/��� ���� [%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cheuv"), data, sizeof(data), 0);		// ü��ܰ�(c.yak_price) ���� �����ɴϴ�.
	order_info["filled_price"] = _ttoi(data);
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: ü��ܰ�[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cheqty"), data, sizeof(data), 0);		// ü�����(c.yak_su) ���� �����ɴϴ�.
	const int filled_count = _ttoi(data);
	order_info["filled_count"] = filled_count;

	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: ü�����[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("chesum"), data, sizeof(data), 0);		// ü������� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: ü�������[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("shyak"), data, sizeof(data), 0);		// S �ٿ��� ü�ᰡ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lnyak"), data, sizeof(data), 0);		// S ������ ü�ᰡ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("bp_time"), data, sizeof(data), 0);		// data ���� �߻��ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tr_time"), data, sizeof(data), 0);		// ü��ð�(c.yak_time) ���� �����ɴϴ�.
	order_info["filled_date"] = data;
	order_info["filled_time"] = data;

	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: ü��ð�[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("op_id"), data, sizeof(data), 0);		// �� Login ID(user_id) ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: �� Login ID([%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("custseq"), data, sizeof(data), 0);		// ���¹�ȣ(cust_no) ���� �����ɴϴ�.
	order_info["account_no"] = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: ���¹�ȣ[%s]"), data);
	memset(data, 0x00, sizeof(data));
	BYTE buy_or_sell;
	g_iYuantaAPI.YOA_GetFieldByte(_T("gubun48"), &buy_or_sell);		// �ż�(4) �ŵ�(8) ���� �����ɴϴ�.
	
	switch (buy_or_sell)
	{
		case '1':
		case '4':
			order_info["position_type"] = "1";
			break;
		case '2':
		case '8':
			order_info["position_type"] = "2";
			break;
		case 'c':
			order_info["position_type"] = "1";
			order_info["order_type"] = "3";
			break;
	}
	//order_info["position_type"] = ((buy_or_sell == '4') || (buy_or_sell == '1')) ? "1" : "2";
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: �ż�/�ŵ�gubun48[%c]"), buy_or_sell);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("stkcode"), data, sizeof(data), 0);		// �����ڵ�(c.jongcode) ���� �����ɴϴ�.

	std::string symbol_code = data;
	if (symbol_code.substr(0, 1).at(0) == '1' && symbol_code.length() > 5)
		symbol_code = symbol_code.substr(0, 5);

	order_info["symbol_code"] = symbol_code;
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: �����ڵ�[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("stkname"), data, sizeof(data), 0);		// �����(' ... ') ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: �����[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jununjong"), data, sizeof(data), 0);		// L:������ M:���尡 C:���Ǻ����� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: L:������ M:���尡 C:���Ǻ�����[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("reject"), data, sizeof(data), 0);		// �źλ��� �ڵ� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: �źλ��� Code[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatp"), data, sizeof(data), 0);		// ȣ������ ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: ȣ������[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("futu_ord_if"), data, sizeof(data), 0);		// �ֹ����� ���� �����ɴϴ�.
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: �ֹ�����[%s]"), data);

	
	

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("trtime2"), data, sizeof(data), 0);		// �ð� HH:MM:SS ���� �����ɴϴ�.
	order_info["order_time"] = data;
	order_info["order_date"] = data;
	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: �ð�[%s]"), data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// ü�ᰡ �Ǵ� �ֹ��� ���� �����ɴϴ�.
	//order_info["order_price"] = convert_to_int(symbol_code, data);
	//order_info["filled_price"] = convert_to_int(symbol_code, data);

	LOGINFO(CMyLogger::getInstance(), _T("on_realtime_order:: ü�ᰡ �Ǵ� �ֹ���[%s]"), data);

	if (filled_count > 0) {
		order_info["order_event"] = OrderEvent::OE_Filled;
		order_info["remain_count"] = 0;
	}
	else
		order_info["order_event"] = OrderEvent::OE_Accepted;
	
	order_info["order_sequence"] = 0;
	//order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
	
	
	//order_info["order_type"] = static_cast<const char*>(strMan.Trim());
	
	
	
	
	//order_info["order_date"] = static_cast<const char*>(strOrderDate.Trim());
	//order_info["order_time"] = static_cast<const char*>(strOrderTime.Trim());

	//order_info["filled_price"] = static_cast<const char*>(strFilledPrice.Trim());
	//order_info["filled_amount"] = static_cast<const char*>(strFilledAmount.Trim());

	//order_info["filled_date"] = static_cast<const char*>(strFilledDate.Trim());
	//order_info["filled_time"] = static_cast<const char*>(strFilledTime.Trim());

	order_info["custom_info"] = "";

	mainApp.total_order_manager()->on_order_event(std::move(order_info));
}

void YaClient::on_realtime_quote()
{
	TCHAR data[1024] = { 0, };
	nlohmann::json quote;
	nlohmann::json hoga;
	g_iYuantaAPI.YOA_SetTRInfo(_T("41"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongcode"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	std::string symbol_code = data;
	if (symbol_code.substr(0, 1).at(0) == '1')
		symbol_code = symbol_code.substr(0, 5);
	quote["symbol_code"] = symbol_code;
	hoga["symbol_code"] = symbol_code;
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// �ð� ���� �����ɴϴ�.
	quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// �� ���� �����ɴϴ�.
	quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// ���� ���� �����ɴϴ�.
	quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// ���簡 ���� �����ɴϴ�.
	quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// �̰����������� ���� �����ɴϴ�.
	quote["volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// �̰����������ϴ����� ���� �����ɴϴ�.
	quote["preday_volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopeninterest"), data, sizeof(data), 0);		// ���̰�����������_KOFEX ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopendebi"), data, sizeof(data), 0);		// ���̰����������ϴ�����_KOFEX ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// ����ü����� ���� �����ɴϴ�.
	quote["cumulative_amount"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("svolume"), data, sizeof(data), 0);		// �������崩��ü����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("money"), data, sizeof(data), 0);		// �����ŷ����_�鸸 ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("smoney"), data, sizeof(data), 0);		// �������崩���ŷ����_�鸸 ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// �ŵ�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// �ż�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler1"), data, sizeof(data), 0);		// 4bytes���߱����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("changerate"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
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
	g_iYuantaAPI.YOA_GetFieldString(_T("mbasis"), data, sizeof(data), 0);		// ����BASIS ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("tbasis"), data, sizeof(data), 0);		// �̷�BASIS ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theory"), data, sizeof(data), 0);		// �̷а� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volatility"), data, sizeof(data), 0);		// ���纯���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("delta"), data, sizeof(data), 0);		// ��Ÿ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rate"), data, sizeof(data), 0);		// ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time2"), data, sizeof(data), 0);		// �ð�_HH:MM:SS ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// �ð�_HH:MM ���� �����ɴϴ�.
	quote["time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// ���ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowtime"), data, sizeof(data), 0);		// �����ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("change"), data, sizeof(data), 0);		// ���ϴ�� ���� �����ɴϴ�.
	quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("nowvol"), data, sizeof(data), 0);		// ����ü�ᷮ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), 0);		// 4bytes���߱����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highchange"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowchange"), data, sizeof(data), 0);		// ������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler2"), data, sizeof(data), 0);		// 4bytes���߱����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// �ֿ켱�ŵ�ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), 0);		// �ֿ켱�ŵ�ȣ���Ǽ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// �ֿ켱�ż�ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), 0);		// �ֿ켱�ż�ȣ���Ǽ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("41"), _T("OutBlock2"));			// TR����(TR��, Block��)�� �����մϴ�.
	for (int i = 0; i < 5; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), i);		// �ŵ�ȣ�� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), i);		// �ż�ȣ�� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), i);		// 4bytes���߱����� ���� �����ɴϴ�.
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), i);		// �ŵ�ȣ���ܷ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), i);		// �ż�ȣ���ܷ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), i);		// �ŵ�ȣ���Ǽ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_cnt"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), i);		// �ż�ȣ���Ǽ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_cnt"] = _ttoi(data);
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("41"), _T("OutBlock3"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// �ŵ�ȣ������ ���� �����ɴϴ�.
	hoga["tot_sell_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// �ż�ȣ������ ���� �����ɴϴ�.
	hoga["tot_buy_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// �ŵ���ȣ���Ǽ� ���� �����ɴϴ�.
	hoga["tot_sell_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// �ż���ȣ���Ǽ� ���� �����ɴϴ�.
	hoga["tot_buy_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// ȣ���ð� ���� �����ɴϴ�.
	hoga["hoga_time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("diffmesumedo"), data, sizeof(data), 0);		// �ż��ܷ�-�ŵ��ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("weightedavg"), data, sizeof(data), 0);		// ������հ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("gamma"), data, sizeof(data), 0);		// ���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theta"), data, sizeof(data), 0);		// ��Ÿ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vega"), data, sizeof(data), 0);		// ���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rho"), data, sizeof(data), 0);		// �� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler3"), data, sizeof(data), 0);		// 4bytes���߱����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("restriction_gbn"), data, sizeof(data), 0);		// �ǽð������Ѱ����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice"), data, sizeof(data), 0);		// ���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice"), data, sizeof(data), 0);		// ���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler4"), data, sizeof(data), 0);		// 4bytes���߱����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vilevel"), data, sizeof(data), 0);		// �����������ܰ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vimaxprice"), data, sizeof(data), 0);		// �������������Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("viminprice"), data, sizeof(data), 0);		// �������������Ѱ� ���� �����ɴϴ�.

	ya_stock_client_.OnSymbolQuote(std::move(quote));
	ya_stock_client_.OnDmSymbolHoga(std::move(hoga));
}

void YaClient::on_realtime_hoga()
{
	TCHAR data[1024] = { 0, };
	nlohmann::json hoga;
	g_iYuantaAPI.YOA_SetTRInfo(_T("42"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongcode"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	std::string symbol_code = data;
	if (symbol_code.substr(0, 1).at(0) == '1' && symbol_code.length() > 5)
		symbol_code = symbol_code.substr(0, 5);

	hoga["symbol_code"] = symbol_code;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// �ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), 0);		// 4bytes���߱����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// �ֿ켱�ŵ�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// �ֿ켱�ż�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler1"), data, sizeof(data), 0);		// 4bytes���߱����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// �ֿ켱�ŵ�ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), 0);		// �ֿ켱�ŵ�ȣ���Ǽ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// �ֿ켱�ż�ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), 0);		// �ֿ켱�ż�ȣ���Ǽ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("42"), _T("OutBlock2"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// �ŵ�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// �ż�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("filler"), data, sizeof(data), 0);		// 4bytes���߱����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), 0);		// �ŵ�ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), 0);		// �ż�ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), 0);		// �ŵ�ȣ���Ǽ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), 0);		// �ż�ȣ���Ǽ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("42"), _T("OutBlock3"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// �ŵ�ȣ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// �ż�ȣ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// �ŵ���ȣ���Ǽ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// �ż���ȣ���Ǽ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time2"), data, sizeof(data), 0);		// �ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("diffmesumedo"), data, sizeof(data), 0);		// �ż��ܷ�-�ŵ��ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectjuka"), data, sizeof(data), 0);		// ����ü�ᰡ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectvol"), data, sizeof(data), 0);		// ����ü����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectrate"), data, sizeof(data), 0);		// ����ü������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectdebi"), data, sizeof(data), 0);		// ����ü���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dongsigubun"), data, sizeof(data), 0);		// ���ñ��� ���� �����ɴϴ�.
}

void YaClient::on_realtime_accepted_count()
{
	TCHAR data[1024] = { 0, };

	g_iYuantaAPI.YOA_SetTRInfo(_T("44"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongcode"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// �̰����������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// �̰����������ϴ����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopeninterest"), data, sizeof(data), 0);		// ���̰�����������_KOFEX ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopendebi"), data, sizeof(data), 0);		// ���̰����������ϴ�����_KOFEX ���� �����ɴϴ�.
}

int YaClient::confirm_account_password(DhTaskArg arg)
{
	return dm_account_asset(arg);
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

int YaClient::register_symbol(const std::string& symbol_code)
{
	std::string symbol_code_to_send = symbol_code;
	if (symbol_code.substr(0, 1).at(0) == '1')
		symbol_code_to_send.append("000");
	g_iYuantaAPI.YOA_SetTRFieldString(_T("41"), _T("InBlock1"), _T("jongcode"), symbol_code_to_send.c_str());
	int nResult = g_iYuantaAPI.YOA_RegistAuto(GetSafeHwnd(), _T("41"));

	if (ERROR_MAX_CODE < nResult)
	{
		//LOGINFO(CMyLogger::getInstance(), _T("[41]���� ���� �ɼ� �ǽð�ü���� ��� �Ǿ����ϴ�."));
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

int YaClient::unregister_symbol(const std::string& symbol_code)
{
	int nResult = g_iYuantaAPI.YOA_UnregistAuto(GetSafeHwnd(), _T("41"));

	if (RESULT_SUCCESS == nResult)
	{
		LOGINFO(CMyLogger::getInstance(), _T("[41]���� ���� �ɼ� �ǽð�ü���� ���� �Ǿ����ϴ�."));
	}
	else
	{
		TCHAR msg[2048] = { 0, };
		g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

		LOGINFO(CMyLogger::getInstance(), msg);
	}

	return 1;

}

int YaClient::register_account(const std::string& account_no)
{
	int nResult = g_iYuantaAPI.YOA_RegistAuto(GetSafeHwnd(), _T("71"));

	if (ERROR_MAX_CODE < nResult)
	{
		//LOGINFO(CMyLogger::getInstance(), _T("[71]���� �����ɼ� �ֹ�Ȯ��_ü�� �ǽð�ü���� ��� �Ǿ����ϴ�."));
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

int YaClient::unregister_account(const std::string& account_no)
{
	int nResult = g_iYuantaAPI.YOA_UnregistAuto(GetSafeHwnd(), _T("71"));

	if (RESULT_SUCCESS == nResult)
	{
		LOGINFO(CMyLogger::getInstance(), _T("[71]���� ���� �ɼ� �ǽð�ü���� ���� �Ǿ����ϴ�."));
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

	g_iYuantaAPI.YOA_SetTRInfo(_T("368001"), _T("InBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// �����ڵ� ���� �����մϴ�.


	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] �����̴� �����ü� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]���������ü� ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



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
	g_iYuantaAPI.YOA_SetTRInfo(trade_code.c_str(), _T("InBlock1"));
	const std::string symbol_code = arg.parameter_map["symbol_code"];
	g_iYuantaAPI.YOA_SetFieldString(_T("code"), symbol_code.c_str(), 0);		// ���� ���� �����մϴ�.

	const int req_id = g_iYuantaAPI.YOA_Request(GetSafeHwnd(), trade_code.c_str());
	req_info.request_id = req_id;
	if (ERROR_MAX_CODE < req_id)
	{
		CString strMsg;
		strMsg.Format(_T("[ReqID:%d] �����̴ϼ��� ȣ�� ��ȸ�� ��û�Ͽ����ϴ�."), req_id);
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

		LOGINFO(CMyLogger::getInstance(), _T("Trade Code[%s]�����̴ϼ��� ȣ�� ��ȸ�� ������ �߻��Ͽ����ϴ�.Error Message[%s]"), trade_code.c_str(), strErrorMsg);



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
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	quote["symbol_code"] = data;
	const std::string symbol_code = data;
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	g_iYuantaAPI.YOA_SetTRInfo(_T("368001"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("jongname"), data, sizeof(data), 0);		// �����ڵ弳�� ���� �����ɴϴ�.
	quote["symbol_name_kr"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// ���簡 ���� �����ɴϴ�.
	quote["close"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// ���ϴ�� ���� �����ɴϴ�.
	quote["delta_day"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
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
	g_iYuantaAPI.YOA_GetFieldString(_T("standardprice"), data, sizeof(data), 0);		// ���ذ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), 0);		// �ŵ�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), 0);		// �ż�ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("start"), data, sizeof(data), 0);		// �ð� ���� �����ɴϴ�.
	quote["open"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("startdebi"), data, sizeof(data), 0);		// �ð���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("high"), data, sizeof(data), 0);		// �� ���� �����ɴϴ�.
	quote["high"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highdebi"), data, sizeof(data), 0);		// ����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hightime"), data, sizeof(data), 0);		// ���ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	quote["time"] = data;
	g_iYuantaAPI.YOA_GetFieldString(_T("low"), data, sizeof(data), 0);		// ���� ���� �����ɴϴ�.
	quote["low"] = convert_to_int(symbol_code, data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowdebi"), data, sizeof(data), 0);		// ������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowtime"), data, sizeof(data), 0);		// �����ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice"), data, sizeof(data), 0);		// ���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice"), data, sizeof(data), 0);		// ���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxcb"), data, sizeof(data), 0);		// cb���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mincb"), data, sizeof(data), 0);		// cb���Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospi"), data, sizeof(data), 0);		// �ڽ��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospidebi"), data, sizeof(data), 0);		// ���ϴ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("kospirate"), data, sizeof(data), 0);		// �ڽ��ǵ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("theoreticalprice"), data, sizeof(data), 0);		// �̷а� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("rate"), data, sizeof(data), 0);		// ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basist"), data, sizeof(data), 0);		// �̷к��̽ý� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("basism"), data, sizeof(data), 0);		// ���庣�̽ý� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	quote["volume"] = _ttoi(data);
	quote["cumulative_amount"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("svolume"), data, sizeof(data), 0);		// ����������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("money"), data, sizeof(data), 0);		// �������(�鸸) ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("smoney"), data, sizeof(data), 0);		// ���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("openinterest"), data, sizeof(data), 0);		// �̰������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevvolume"), data, sizeof(data), 0);		// ���Ͼ������� ���� �����ɴϴ�.
	quote["preday_volume"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sprevvolume"), data, sizeof(data), 0);		// ���Ͻ���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("prevmoney"), data, sizeof(data), 0);		// ���Ͼ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("sprevmoney"), data, sizeof(data), 0);		// ���Ͻ��������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendebi"), data, sizeof(data), 0);		// ���Ϲ̰���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopeninterest"), data, sizeof(data), 0);		// ���̰������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("netopendebi"), data, sizeof(data), 0);		// ���ϼ��̰���������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("opendate"), data, sizeof(data), 0);		// �ŷ������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lastdate"), data, sizeof(data), 0);		// �����ŷ��� ���� �����ɴϴ�.
	if (symbol) symbol->LastTradeDay(data);
	if (symbol) symbol->ExpireDate(data);

	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("remainday"), data, sizeof(data), 0);		// �����ϼ� ���� �����ɴϴ�.
	if (symbol) symbol->RemainDays(_ttoi(data));
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highprice"), data, sizeof(data), 0);		// �ְ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowprice"), data, sizeof(data), 0);		// ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebi"), data, sizeof(data), 0);		// �ְ���� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebi"), data, sizeof(data), 0);		// ��������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedebirate"), data, sizeof(data), 0);		// �ְ������ ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedebirate"), data, sizeof(data), 0);		// ����������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("highpricedate"), data, sizeof(data), 0);		// �ְ����� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("lowpricedate"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("weightedavg"), data, sizeof(data), 0);		// ������հ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("cdinterest"), data, sizeof(data), 0);		// CD�ݸ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("mememulti"), data, sizeof(data), 0);		// �ŷ��¼� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("restriction_gbn"), data, sizeof(data), 0);		// �ǽð����Ѱ����뿩�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("maxprice2"), data, sizeof(data), 0);		// ���߻��Ѱ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("minprice2"), data, sizeof(data), 0);		// �������Ѱ� ���� �����ɴϴ�.


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
	g_iYuantaAPI.YOA_GetFieldString(_T("code"), data, sizeof(data), 0);		// �����ڵ� ���� �����ɴϴ�.
	const std::string symbol_code = data;
	hoga["symbol_code"] = symbol_code;
	g_iYuantaAPI.YOA_SetTRInfo(_T("368002"), _T("OutBlock1"));			// TR����(TR��, Block��)�� �����մϴ�.
	for (int i = 0; i < 5; i++) {
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medoprice"), data, sizeof(data), i);		// �ŵ�ȣ�� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medovol"), data, sizeof(data), i);		// �ŵ�ȣ���ܷ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("medocount"), data, sizeof(data), i);		// �ŵ�ȣ���Ǽ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["sell_cnt"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuprice"), data, sizeof(data), i);		// �ż�ȣ�� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_price"] = convert_to_int(symbol_code, data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesuvol"), data, sizeof(data), i);		// �ż�ȣ���ܷ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_qty"] = _ttoi(data);
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetFieldString(_T("mesucount"), data, sizeof(data), i);		// �ż�ȣ���Ǽ� ���� �����ɴϴ�.
		hoga["hoga_items"][i]["buy_cnt"] = _ttoi(data);
	}

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock2"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// ȣ���Ǽ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock3"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// ȣ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("vol"), data, sizeof(data), 0);		// ȣ���ܷ� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("count"), data, sizeof(data), 0);		// ȣ���Ǽ� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock4"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("time"), data, sizeof(data), 0);		// �����ð� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("price"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data), 0);		// ���ϴ�� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("nowvol"), data, sizeof(data), 0);		// �������� ���� �����ɴϴ�.

	g_iYuantaAPI.YOA_SetTRInfo(_T("350002"), _T("OutBlock5"));			// TR����(TR��, Block��)�� �����մϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime"), data, sizeof(data), 0);		// ȣ���ð� ���� �����ɴϴ�.
	hoga["hoga_time"] = data;
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedovol"), data, sizeof(data), 0);		// �Ѹŵ��ܷ� ���� �����ɴϴ�.
	hoga["tot_sell_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesuvol"), data, sizeof(data), 0);		// �Ѹż��ܷ� ���� �����ɴϴ�.
	hoga["tot_buy_qty"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmedocount"), data, sizeof(data), 0);		// �Ѹŵ��Ǽ� ���� �����ɴϴ�.
	hoga["tot_sell_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("totmesucount"), data, sizeof(data), 0);		// �Ѹż��Ǽ� ���� �����ɴϴ�.
	hoga["tot_buy_cnt"] = _ttoi(data);
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("hokatime2"), data, sizeof(data), 0);		// ȣ���ð�2 ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("last"), data, sizeof(data), 0);		// ���簡 ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("dongsi"), data, sizeof(data), 0);		// ���ñ��� ���� �����ɴϴ�.
	memset(data, 0x00, sizeof(data));
	g_iYuantaAPI.YOA_GetFieldString(_T("expectjuka"), data, sizeof(data), 0);		// ����ü�ᰡ ���� �����ɴϴ�.


	ya_stock_client_.OnDmSymbolHoga(std::move(hoga));

	on_task_complete(req_info.request_id);
	g_iYuantaAPI.YOA_ReleaseData(req_info.request_id);
}

BOOL DarkHorse::YaClient::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
