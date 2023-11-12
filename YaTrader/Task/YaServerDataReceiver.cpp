#include "stdafx.h"
#include "YaServerDataReceiver.h"
#include <vector>
#include "VtProgressDlg.h"
#include "../MainFrm.h"
#include "../Task/SmTaskDefine.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Client/ViStockClient.h"
#include "../Yuanta/YaStockClient.h"
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbolReader.h"
#include "../Symbol/SmSymbol.h"

#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmProduct.h"
#include "../Symbol/SmProductYearMonth.h"

#include <set>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
namespace DarkHorse {
	int YaServerDataReceiver::argument_id_ = 0;

	void YaServerDataReceiver::execute_request(DhTaskArg& arg)
	{
		arg.requested = true;
		switch (arg.task_type)
		{
		case DhTaskType::DmSymbolMasterFileDownload:
			mainApp.Client()->dm_symbol_master_file_download(arg); // 제외
			break;
		case DhTaskType::AbSymbolMasterFileDownload:
			mainApp.Client()->ab_symbol_master_file_download(arg); // 제외
			break;
		case DhTaskType::DmSymbolMaster:
			mainApp.Client()->dm_symbol_master(arg); // 제외
			break;
		case DhTaskType::AbSymbolMaster: 
			mainApp.Client()->ab_symbol_master(arg); // 제외
			break;
		case DhTaskType::DmAccountAsset:
			mainApp.Client()->dm_account_asset(arg); // 포함
			break;
		case DhTaskType::AbAccountAsset:
			mainApp.Client()->ab_account_asset(arg); // 제외
			break;
		case DhTaskType::AbAccountProfitLoss:
			mainApp.Client()->ab_account_profit_loss(arg); // 제외
			break;
		case DhTaskType::DmAccountProfitLoss:
			mainApp.Client()->dm_account_profit_loss(arg); // 포함
			break;
		case DhTaskType::AbAcceptedOrderList:
			mainApp.Client()->ab_accepted_order(arg); // 제외
			break;
		case DhTaskType::DmAcceptedOrderList:
			mainApp.Client()->dm_accepted_order(arg); // 포함
			break;
		case DhTaskType::AbSymbolQuote:
			mainApp.Client()->ab_symbol_quote(arg); // 제외
			break;
		case DhTaskType::AbSymbolHoga:
			mainApp.Client()->ab_symbol_hoga(arg); // 제외
			break;
		case DhTaskType::AbSymbolPosition:
			mainApp.Client()->ab_symbol_position(arg); // 제외
			break;
		case DhTaskType::DmSymbolPosition:
			mainApp.Client()->dm_symbol_position(arg); // 포함
			break;
		case DhTaskType::AbSymbolProfitLoss:
			mainApp.Client()->ab_symbol_profit_loss(arg); // 제외
			break;
		case DhTaskType::DmSymbolProfitLoss:
			mainApp.Client()->dm_daily_profit_loss(arg); // 포함. 
			break;
		case DhTaskType::DmSymbolQuote:
			mainApp.Client()->dm_symbol_quote(arg); // 포함. 
			break;
		case DhTaskType::DmSymbolHoga:
			mainApp.Client()->dm_symbol_hoga(arg); // 포함. 
			break;
		case DhTaskType::AbSymbolChartData:
			//mainApp.Client().GetAbChartData(arg);
			break;
		case DhTaskType::AbFilledOrderList:
			//mainApp.Client().GetAbFilled(arg);
			break;
		case DhTaskType::AbFilledDetail:
			//mainApp.Client().GetAbFilledOrderDetail(arg);
			break;
		default:
			break;
		}
	}

	void YaServerDataReceiver::set_task_state()
	{
		if (!progress_dialog_)
			return;
		progress_dialog_->TotalCount(task_info_.total_task_count);
		progress_dialog_->RemainCount(task_info_.remain_task_count);
		progress_dialog_->TaskTitle(task_info_.task_title);
		progress_dialog_->TaskDetail(task_info_.task_detail);
		progress_dialog_->RefreshProgress();
	}

	void YaServerDataReceiver::do_next_group_task()
	{
		// 기존 요청 목록을 먼저 모두 없앤다.
		task_info_.argument_map.clear();
		if (!server_data_receive_on_) return;
		switch (task_info_.task_type)
		{
		case DhTaskType::DmSymbolMasterFileDownload:
		{
			start_ab_symbol_master_file_download();
		}
		break;
		case DhTaskType::AbSymbolMasterFileDownload:
		{
			mainApp.SymMgr()->MakeDomesticMarket();
			mainApp.SymMgr()->ReadAbroadSymbols();
			mainApp.SymMgr()->read_domestic_productfile();
			mainApp.SymMgr()->read_dm_masterfile_si();
			mainApp.SymMgr()->sort_dm_option_symbol_vector();
			mainApp.SymMgr()->MakeAbFavorite();
			start_ab_symbol_master();
		}
		break;
		case DhTaskType::AbSymbolMaster:
		{
			start_dm_symbol_master();
		}
		break;
		case DhTaskType::DmSymbolMaster:
		{
			start_dm_account_asset();
		}
		break;
		case DhTaskType::DmAccountAsset:
		{
			start_dm_symbol_quote();
		}
		break;
		case DhTaskType::DmSymbolQuote:
		{
			start_dm_symbol_hoga();
		}
		break;
		case DhTaskType::DmSymbolHoga:
		{
			start_dm_symbol_position();
		}
		break;
		case DhTaskType::AbSymbolQuote:
		{
			start_ab_symbol_hoga();
		}
		break;
		case DhTaskType::AbSymbolHoga:
		{
			start_ab_account_asset();
		}
		break;

		case DhTaskType::AbAccountAsset:
		{
			start_ab_accepted_order();
		}
		break;
		case DhTaskType::AbAcceptedOrderList:
		{
			start_dm_accepted_order();
		}
		break;
		case DhTaskType::DmAcceptedOrderList:
		{
			register_realtime();

			server_data_receive_on_ = false;
			end_all_task();
		}
		break;
		case DhTaskType::AbSymbolPosition:
		{
			start_dm_symbol_position();
		}
		break;
		case DhTaskType::DmSymbolPosition:
		{
			start_dm_account_profit_loss();
		}
		break;

		case DhTaskType::DmAccountProfitLoss:
		{
			start_dm_accepted_order();
		}
		break;
		case DhTaskType::AbSymbolProfitLoss:
		{
			start_dm_symbol_profit_loss();
		}
		break;
		case DhTaskType::DmSymbolProfitLoss:
		{
			start_ab_account_profit_loss();
		}
		break;

		case DhTaskType::AbAccountProfitLoss:
		{
			server_data_receive_on_ = false;
			end_all_task();
		}
		break;

		default:
			break;
		}
	}

	void YaServerDataReceiver::execute_next()
	{
		if (task_info_.argument_map.size() == 0)
			return;
		// 가장 오래된 요소를 가져온다.
		auto front = task_info_.argument_map.begin();
		DhTaskArg& arg = front->second;
		task_info_.task_detail = arg.detail_task_description;
		// 작업 상태를 설정한다.
		set_task_state();
		// 요청하지 않았다면 요청을 한다.
		if (!arg.requested) {
			execute_request(arg);
		}
	}

	DhTaskArg* YaServerDataReceiver::find_task(const int& argument_id)
	{
		auto it = task_info_.argument_map.find(argument_id);
		if (it != task_info_.argument_map.end())
			return &it->second;

		return nullptr;
	}

	void YaServerDataReceiver::on_task_complete(const int& argument_id)
	{
		if (!server_data_receive_on_) return;
		// 요청이 완료된 일은 찾아 내어 맵에서 없애 준다.
		auto it = task_info_.argument_map.find(argument_id);
		if (it != task_info_.argument_map.end()) {
			task_info_.argument_map.erase(it);
		}

		// 남은 걧수를 다시 설정해 준다.
		task_info_.remain_task_count = task_info_.argument_map.size();
		set_task_state();
		if (task_info_.remain_task_count == 0) {
			((CMainFrame*)AfxGetMainWnd())->stop_timer();
			do_next_group_task();
		}
	}

	void YaServerDataReceiver::on_task_error(const int& argument_id)
	{
		if (!server_data_receive_on_) return;
		// 오류가 나면 건너 뛴다. 
		// 요청이 완료된 일은 찾아 내어 맵에서 없애 준다.
		auto it = task_info_.argument_map.find(argument_id);
		if (it != task_info_.argument_map.end()) {
			task_info_.argument_map.erase(it);
		}

		// 남은 걧수를 다시 설정해 준다.
		task_info_.remain_task_count = task_info_.argument_map.size();
		set_task_state();
		if (task_info_.remain_task_count == 0) {
			((CMainFrame*)AfxGetMainWnd())->stop_timer();
			do_next_group_task();
		}
	}

	void YaServerDataReceiver::make_ab_account_profit_loss()
	{
		std::vector<std::shared_ptr<SmAccount>> account_vec;
		mainApp.AcntMgr()->get_main_account_vector(account_vec);
		for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
			std::shared_ptr<SmAccount> account = *it;
			if (account->Type() != "1") continue;
			if (account->skip_confirm()) continue;
			if (account->is_subaccount()) continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbAccountProfitLoss;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "해외 계좌별 손익을 가져오는 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbAccountProfitLoss;
	}

	void YaServerDataReceiver::make_dm_account_profit_loss()
	{
		std::vector<std::shared_ptr<SmAccount>> account_vec;
		mainApp.AcntMgr()->get_main_account_vector(account_vec);
		for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
			std::shared_ptr<SmAccount> account = *it;
			if (account->Type() != "9") continue;
			if (account->skip_confirm()) continue;
			if (account->is_subaccount()) continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::DmAccountProfitLoss;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "국내 계좌별 손익을 가져오는 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmAccountProfitLoss;
	}



	void YaServerDataReceiver::make_dm_file_download()
	{
		const std::string file_name = "chocode.cod";
		DhTaskArg arg;
		arg.detail_task_description = file_name;
		arg.argument_id = YaServerDataReceiver::get_argument_id();
		arg.task_type = DhTaskType::DmSymbolMasterFileDownload;
		arg.parameter_map["file_name"] = file_name;

		task_info_.argument_map[arg.argument_id] = arg;


		task_info_.task_title = "국내 심볼 마스터 파일 다운로드 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmSymbolMasterFileDownload;
	}

	void YaServerDataReceiver::make_ab_file_download()
	{
		const std::string file_name = "futures";
		DhTaskArg arg;
		arg.detail_task_description = file_name;
		arg.argument_id = YaServerDataReceiver::get_argument_id();
		arg.task_type = DhTaskType::AbSymbolMasterFileDownload;
		arg.parameter_map["file_name"] = file_name;

		task_info_.argument_map[arg.argument_id] = arg;


		task_info_.task_title = "해외 심볼 마스터 파일 다운로드 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbSymbolMasterFileDownload;
	}

	void YaServerDataReceiver::make_ab_symbol_master()
	{
		const std::map<int, std::shared_ptr<SmSymbol>>& ab_symbol_favorite = mainApp.SymMgr()->get_ab_favorite_map();
		for (auto it = ab_symbol_favorite.begin(); it != ab_symbol_favorite.end(); it++) {
			DhTaskArg arg;
			arg.detail_task_description = it->second->SymbolCode();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbSymbolMaster;
			arg.parameter_map["symbol_code"] = it->second->SymbolCode();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "해외 심볼 마스터 다운로드 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbSymbolMaster;


		std::set<std::shared_ptr<SmSymbol>> ab_symbol_set;
		mainApp.SymMgr()->get_ab_recent_symbols(ab_symbol_set);
		if (ab_symbol_set.empty()) return;

		for (auto it = ab_symbol_set.begin(); it != ab_symbol_set.end(); it++) {
			DhTaskArg arg;
			arg.detail_task_description = (*it)->SymbolCode();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbSymbolMaster;
			arg.parameter_map["symbol_code"] = (*it)->SymbolCode();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "해외 심볼 마스터 다운로드 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbSymbolMaster;
	}



	void YaServerDataReceiver::make_dm_account_asset()
	{
		std::vector<std::shared_ptr<SmAccount>> account_vec;
		mainApp.AcntMgr()->get_main_account_vector(account_vec);
		for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
			std::shared_ptr<SmAccount> account = *it;
			if (account->skip_confirm()) continue;
			if (account->skip_confirm()) continue;
			if (account->is_subaccount()) continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::DmAccountAsset;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "국내 계좌별 자산정보를 가져오는 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmAccountAsset;
	}
	void YaServerDataReceiver::make_ab_account_asset()
	{
		std::vector<std::shared_ptr<SmAccount>> account_vec;
		mainApp.AcntMgr()->get_main_account_vector(account_vec);
		for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
			std::shared_ptr<SmAccount> account = *it;
			if (account->Type() != "1") continue;
			if (account->skip_confirm()) continue;
			if (account->is_subaccount()) continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbAccountAsset;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "해외 계좌별 자산 정보를 가져오는 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbAccountAsset;
	}

	void YaServerDataReceiver::make_dm_symbol_master()
	{
		const std::vector<DmFuture>& future_vec = mainApp.SymMgr()->get_dm_future_vec();
		for (size_t i = 0; i < future_vec.size(); i++) {
			const auto& year_month_map = future_vec[i].product->get_yearmonth_map();
			const auto& year_month = year_month_map.begin()->second;
			const auto& symbol = year_month->get_first_symbol();
			make_dm_symbol_master(symbol);
		}

		std::vector<DarkHorse::DmOption>& option_vec = mainApp.SymMgr()->get_dm_option_vec();
		for (size_t i = 0; i < option_vec.size(); i++) {
			const auto& call_year_month_map = option_vec[i].call_product->get_yearmonth_map();
			make_dm_symbol_master(call_year_month_map);
			const auto& put_year_month_map = option_vec[i].put_product->get_yearmonth_map();
			make_dm_symbol_master(put_year_month_map);
		}

		task_info_.task_title = "국내 심볼 마스터 다운로드 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmSymbolMaster;
	}

	void YaServerDataReceiver::make_dm_symbol_master(const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map)
	{
		if (year_month_map.empty()) return;

		auto& symbol_vec = year_month_map.begin()->second->get_symbol_vector();
		make_dm_symbol_master(symbol_vec);
	}

	void YaServerDataReceiver::make_dm_symbol_master(const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec)
	{
		if (symbol_vec.empty()) return;

		for (auto const& symbol : symbol_vec) {
			make_dm_symbol_master(symbol);
		}
	}

	void YaServerDataReceiver::make_dm_symbol_master(std::shared_ptr<DarkHorse::SmSymbol> symbol)
	{
		if (!symbol) return;
		DhTaskArg arg;
		arg.detail_task_description = symbol->SymbolCode();
		arg.argument_id = YaServerDataReceiver::get_argument_id();
		arg.task_type = DhTaskType::DmSymbolMaster;
		arg.parameter_map["symbol_code"] = symbol->SymbolCode();

		task_info_.argument_map[arg.argument_id] = arg;
	}

	void YaServerDataReceiver::make_dm_accepted_order()
	{
		std::vector<std::shared_ptr<SmAccount>> account_vec;
		mainApp.AcntMgr()->get_main_account_vector(account_vec);
		for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
			std::shared_ptr<SmAccount> account = *it;
			if (account->Type() != "9") continue;
			if (account->skip_confirm()) continue;
			if (account->is_subaccount()) continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::DmAcceptedOrderList;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "국내 접수주문을 가져오는 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmAcceptedOrderList;
	}
	void YaServerDataReceiver::make_ab_accepted_order()
	{
		std::vector<std::shared_ptr<SmAccount>> account_vec;
		mainApp.AcntMgr()->get_main_account_vector(account_vec);
		for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
			std::shared_ptr<SmAccount> account = *it;
			if (account->Type() != "1") continue;
			if (account->skip_confirm()) continue;
			if (account->is_subaccount()) continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbAcceptedOrderList;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "해외 접수주문을 가져오는 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbAcceptedOrderList;
	}

	void YaServerDataReceiver::make_ab_symbol_position()
	{
		std::vector<std::shared_ptr<SmAccount>> account_vec;
		mainApp.AcntMgr()->get_main_account_vector(account_vec);
		for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
			std::shared_ptr<SmAccount> account = *it;
			if (account->Type() != "1") continue;
			if (account->skip_confirm()) continue;
			if (account->is_subaccount()) continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbSymbolPosition;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "해외 미결제 목록을 가져오는 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbSymbolPosition;
	}

	void YaServerDataReceiver::make_dm_symbol_position()
	{
		std::vector<std::shared_ptr<SmAccount>> account_vec;
		mainApp.AcntMgr()->get_main_account_vector(account_vec);
		for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
			std::shared_ptr<SmAccount> account = *it;
			if (account->Type() != "9") continue;
			if (account->skip_confirm()) continue;
			if (account->is_subaccount()) continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::DmSymbolPosition;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "국내 미결제 목록을 가져오는 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmSymbolPosition;
	}


	void YaServerDataReceiver::register_realtime()
	{
		const std::vector<DmFuture>& future_vec = mainApp.SymMgr()->get_dm_future_vec();
		for (size_t i = 0; i < future_vec.size(); i++) {
			const auto& year_month_map = future_vec[i].product->get_yearmonth_map();
			const auto& year_month = year_month_map.begin()->second;
			const auto& symbol = year_month->get_first_symbol();
			register_symbol(symbol);
			//break;
		}
		
		std::vector<DarkHorse::DmOption>& option_vec = mainApp.SymMgr()->get_dm_option_vec();
		for (size_t i = 0; i < option_vec.size(); i++) {
			const auto& call_year_month_map = option_vec[i].call_product->get_yearmonth_map();
			register_symbol(call_year_month_map);
			const auto& put_year_month_map = option_vec[i].put_product->get_yearmonth_map();
			register_symbol(put_year_month_map);
		}
		

		register_account();
	}

	void YaServerDataReceiver::make_dm_symbol_quote()
	{
		const std::vector<DmFuture>& future_vec = mainApp.SymMgr()->get_dm_future_vec();
		for (size_t i = 0; i < future_vec.size(); i++) {
			const auto& year_month_map = future_vec[i].product->get_yearmonth_map();
			const auto& year_month = year_month_map.begin()->second;
			const auto& symbol = year_month->get_first_symbol();
			make_dm_symbol_quote(symbol);
		}

// 		std::vector<DarkHorse::DmOption>& option_vec = mainApp.SymMgr()->get_dm_option_vec();
// 		for (size_t i = 0; i < option_vec.size(); i++) {
// 			const auto& call_year_month_map = option_vec[i].call_product->get_yearmonth_map();
// 			make_dm_symbol_quote(call_year_month_map);
// 			const auto& put_year_month_map = option_vec[i].put_product->get_yearmonth_map();
// 			make_dm_symbol_quote(put_year_month_map);
// 		}

		task_info_.task_title = "국내 심볼 시세 다운로드 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmSymbolQuote;
	}

	void YaServerDataReceiver::make_dm_symbol_quote(std::shared_ptr<DarkHorse::SmSymbol> symbol)
	{
		if (!symbol) return;
		DhTaskArg arg;
		arg.detail_task_description = symbol->SymbolCode();
		arg.argument_id = YaServerDataReceiver::get_argument_id();
		arg.task_type = DhTaskType::DmSymbolQuote;
		arg.parameter_map["symbol_code"] = symbol->SymbolCode();
		arg.parameter_map["gubun_code"] = symbol->gubun_code();

		task_info_.argument_map[arg.argument_id] = arg;
	}

	void YaServerDataReceiver::make_dm_symbol_quote(const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map)
	{
		if (year_month_map.empty()) return;

		auto& symbol_vec = year_month_map.begin()->second->get_symbol_vector();
		make_dm_symbol_quote(symbol_vec);
	}

	void YaServerDataReceiver::make_dm_symbol_quote(const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec)
	{
		if (symbol_vec.empty()) return;

		for (auto const& symbol : symbol_vec) {
			make_dm_symbol_quote(symbol);
		}
	}

	void YaServerDataReceiver::register_symbol(std::shared_ptr<DarkHorse::SmSymbol> symbol)
	{
		if (!symbol) return;
		mainApp.Client()->RegisterSymbol(symbol->SymbolCode());
	}

	void YaServerDataReceiver::register_symbol(const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map)
	{
		if (year_month_map.empty()) return;

		auto& symbol_vec = year_month_map.begin()->second->get_symbol_vector();
		register_symbol(symbol_vec);
	}

	void YaServerDataReceiver::register_symbol(const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec)
	{
		if (symbol_vec.empty()) return;

		for (auto const& symbol : symbol_vec) {
			register_symbol(symbol);
		}
	}

	void YaServerDataReceiver::register_account()
	{
		std::vector<std::shared_ptr<SmAccount>> account_vec;
		mainApp.AcntMgr()->get_main_account_vector(account_vec);
		for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
			std::shared_ptr<SmAccount> account = *it;
			mainApp.Client()->RegisterAccount(account->No());
		}
	}

	void YaServerDataReceiver::make_dm_symbol_hoga()
	{
		const std::vector<DmFuture>& future_vec = mainApp.SymMgr()->get_dm_future_vec();
		for (size_t i = 0; i < future_vec.size(); i++) {
			const auto& year_month_map = future_vec[i].product->get_yearmonth_map();
			const auto& year_month = year_month_map.begin()->second;
			const auto& symbol = year_month->get_first_symbol();
			make_dm_symbol_hoga(symbol);
		}

// 		std::vector<DarkHorse::DmOption>& option_vec = mainApp.SymMgr()->get_dm_option_vec();
// 		for (size_t i = 0; i < option_vec.size(); i++) {
// 			const auto& call_year_month_map = option_vec[i].call_product->get_yearmonth_map();
// 			make_dm_symbol_hoga(call_year_month_map);
// 			const auto& put_year_month_map = option_vec[i].put_product->get_yearmonth_map();
// 			make_dm_symbol_hoga(put_year_month_map);
// 		}

		task_info_.task_title = "국내 심볼 호가 다운로드 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmSymbolHoga;
	}

	void YaServerDataReceiver::make_dm_symbol_hoga(std::shared_ptr<DarkHorse::SmSymbol> symbol)
	{
		if (!symbol) return;
		DhTaskArg arg;
		arg.detail_task_description = symbol->SymbolCode();
		arg.argument_id = YaServerDataReceiver::get_argument_id();
		arg.task_type = DhTaskType::DmSymbolHoga;
		arg.parameter_map["symbol_code"] = symbol->SymbolCode();
		arg.parameter_map["gubun_code"] = symbol->gubun_code();

		task_info_.argument_map[arg.argument_id] = arg;
	}

	void YaServerDataReceiver::make_dm_symbol_hoga(const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map)
	{
		if (year_month_map.empty()) return;

		auto& symbol_vec = year_month_map.begin()->second->get_symbol_vector();
		make_dm_symbol_hoga(symbol_vec);
	}

	void YaServerDataReceiver::make_dm_symbol_hoga(const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec)
	{
		if (symbol_vec.empty()) return;

		for (auto const& symbol : symbol_vec) {
			make_dm_symbol_hoga(symbol);
		}
	}

	void YaServerDataReceiver::make_ab_symbol_quote()
	{
		const std::map<int, std::shared_ptr<SmSymbol>>& ab_symbol_favorite = mainApp.SymMgr()->get_ab_favorite_map();
		for (auto it = ab_symbol_favorite.begin(); it != ab_symbol_favorite.end(); it++) {
			DhTaskArg arg;
			arg.detail_task_description = it->second->SymbolCode();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbSymbolQuote;
			arg.parameter_map["symbol_code"] = it->second->SymbolCode();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "해외 시세 다운로드 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbSymbolQuote;
	}

	void YaServerDataReceiver::make_ab_symbol_hoga()
	{
		const std::map<int, std::shared_ptr<SmSymbol>>& ab_symbol_favorite = mainApp.SymMgr()->get_ab_favorite_map();
		for (auto it = ab_symbol_favorite.begin(); it != ab_symbol_favorite.end(); it++) {
			DhTaskArg arg;
			arg.detail_task_description = it->second->SymbolCode();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbSymbolHoga;
			arg.parameter_map["symbol_code"] = it->second->SymbolCode();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "해외 호가 다운로드 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbSymbolHoga;
	}

	void YaServerDataReceiver::make_ab_symbol_profit_loss()
	{
		std::vector<std::shared_ptr<SmAccount>> account_vec;
		mainApp.AcntMgr()->get_main_account_vector(account_vec);
		for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
			std::shared_ptr<SmAccount> account = *it;
			if (account->Type() != "1") continue;
			if (account->skip_confirm()) continue;
			if (account->is_subaccount()) continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbSymbolProfitLoss;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "해외 종목별 손익을 가져오는 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbSymbolProfitLoss;
	}
	void YaServerDataReceiver::make_dm_symbol_profit_loss()
	{
		std::vector<std::shared_ptr<SmAccount>> account_vec;
		mainApp.AcntMgr()->get_main_account_vector(account_vec);
		for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
			std::shared_ptr<SmAccount> account = *it;
			if (account->Type() != "9") continue;
			if (account->skip_confirm()) continue;
			if (account->is_subaccount()) continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = YaServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::DmSymbolProfitLoss;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "국내 종목별 손익을 가져오는 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmSymbolProfitLoss;
	}


	void YaServerDataReceiver::end_all_task()
	{
		((CMainFrame*)AfxGetMainWnd())->LoadAfterServerData();
	}

	void YaServerDataReceiver::start_dm_symbol_master_file_download()
	{
		make_dm_file_download();
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

	void YaServerDataReceiver::start_ab_symbol_master_file_download()
	{
		make_ab_file_download();
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

	void YaServerDataReceiver::start_ab_symbol_master()
	{
		make_ab_symbol_master();
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

	void YaServerDataReceiver::start_dm_symbol_master()
	{
		make_dm_symbol_master();
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

	void YaServerDataReceiver::start_dm_account_asset()
	{
		make_dm_account_asset();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}
	void YaServerDataReceiver::start_ab_account_asset()
	{
		make_ab_account_asset();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void YaServerDataReceiver::start_ab_account_profit_loss()
	{
		make_ab_account_profit_loss();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void YaServerDataReceiver::start_dm_account_profit_loss()
	{
		make_dm_account_profit_loss();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void YaServerDataReceiver::start_dm_accepted_order()
	{
		make_dm_accepted_order();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}
	void YaServerDataReceiver::start_ab_accepted_order()
	{
		make_ab_accepted_order();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void YaServerDataReceiver::start_ab_symbol_position()
	{
		make_ab_symbol_position();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void YaServerDataReceiver::start_dm_symbol_position()
	{
		make_dm_symbol_position();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void YaServerDataReceiver::start_ab_symbol_profit_loss()
	{
		make_ab_symbol_profit_loss();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}
	void YaServerDataReceiver::start_dm_symbol_profit_loss()
	{
		make_dm_symbol_profit_loss();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void YaServerDataReceiver::start_ab_symbol_hoga()
	{
		make_ab_symbol_hoga();
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

	void YaServerDataReceiver::start_ab_symbol_quote()
	{
		make_ab_symbol_quote();
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

	void YaServerDataReceiver::start_dm_symbol_hoga()
	{
		make_dm_symbol_hoga();
		((CMainFrame*)AfxGetMainWnd())->start_timer(1000);
	}

	void YaServerDataReceiver::start_dm_symbol_quote()
	{
		make_dm_symbol_quote();
		((CMainFrame*)AfxGetMainWnd())->start_timer(1000);
	}

}