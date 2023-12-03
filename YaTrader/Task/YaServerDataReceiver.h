#pragma once

//#include "../Global/TemplateSingleton.h"
//#include "../EBCtrl/EbGlobal.h"
#include <map>
#include <memory>
#include <vector>
#include "../Task/SmTaskDefine.h"
// 서버에서 데이터를 가져오는 작업은 그룹별로 진행된다.
// 하나의 작업 그룹을 큐에 넣고 그 작업 그룹이 완성이 되면 다음 작업 그룹을 진행한다.
// 작업 그룹이 진행되는 과정은 프로그레스바를 통해서 표시된다.
class VtProgressDlg;

namespace DarkHorse {
	class SmProductYearMonth;
	class SmSymbol;
	class YaServerDataReceiver
	{
	private:
		// 앱의 상태를 나타낸다.
		//SmAppState _AppState = SmAppState::None;
		bool server_data_receive_on_{ true };
		// Key : id, value : argument
		//std::map<int, DhTaskArg> 	_RequestMap;
		void execute_request(DhTaskArg& arg);
		// 진행 상황을 나태내는 대화상자
		//VtProgressDlg* _ProgressDlg;
		std::shared_ptr<VtProgressDlg> progress_dialog_;
		// 현재 작업 중인 작업의 정보를 기억한다.
		DhTaskInfo task_info_;
		// 현재 작업 상태를 표시한다.
		void set_task_state();
		// 다음 그룹 작업을 시작한다.
		void do_next_group_task();
	public:
		// 큐에 남아 있는 작업중 젤 상위의 작업을 실행 합니다.
		// 큐가 비어 있으면 아무일도 하지 않습니다.
		void execute_next();
		DhTaskArg* find_task(const int& argument_id);
		void on_task_complete(const int& argument_id);
		void on_task_error(const int& argument_id);
		// 서버에서 정보 가져오는 과정을 종료시킨다.
		void end_all_task();
		static int argument_id_;
		static int get_argument_id() {
			return argument_id_++;
		}
		std::shared_ptr<VtProgressDlg> progress_dialog() const { return progress_dialog_; }
		void progress_dialog(std::shared_ptr<VtProgressDlg> val) { progress_dialog_ = val; }

		void start_dm_symbol_master_file_download();
		void start_ab_symbol_master_file_download();
		void start_ab_symbol_master();
		void start_dm_symbol_master();
		void start_dm_account_asset();
		void start_ab_account_asset();
		void start_ab_account_profit_loss();
		void start_dm_account_profit_loss();
		void start_dm_accepted_order();
		void start_ab_accepted_order();
		void start_ab_symbol_position();
		void start_dm_symbol_position();
		void start_ab_symbol_profit_loss();
		void start_dm_symbol_profit_loss();
		void start_ab_symbol_hoga();
		void start_ab_symbol_quote();
		void start_dm_symbol_hoga();
		void start_dm_symbol_quote();
		void start_dm_option_month_quote();
		void start_ab_trade_profit_loss();
	private:
		void make_ab_trade_profit_loss();
		void register_realtime();
		void make_dm_symbol_quote();
		void make_dm_symbol_hoga();
		void make_ab_symbol_quote();
		void make_ab_symbol_hoga();
		void make_ab_symbol_profit_loss();
		void make_dm_symbol_profit_loss();
		void make_ab_symbol_position();
		void make_dm_symbol_position();
		void make_dm_accepted_order();
		void make_ab_accepted_order();
		void make_dm_account_asset();
		void make_ab_account_asset();
		void make_dm_symbol_master();
		void make_ab_symbol_master();
		void make_dm_file_download();
		void make_ab_file_download();
		void make_dm_option_month_quote();
		// 일별 손익을 가져옵니다.
		void make_ab_account_profit_loss();
		void make_dm_account_profit_loss();
		void make_dm_symbol_master(const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map);
		void make_dm_symbol_master(const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec);
		void make_dm_symbol_master(std::shared_ptr<DarkHorse::SmSymbol> symbol);
		void make_dm_symbol_quote(std::shared_ptr<DarkHorse::SmSymbol> symbol);
		void make_dm_symbol_hoga(std::shared_ptr<DarkHorse::SmSymbol> symbol);
		void make_dm_symbol_quote(const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map);
		void make_dm_symbol_hoga(const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map);
		void make_dm_symbol_hoga(const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec);
		void make_dm_symbol_quote(const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec);
		void register_symbol(std::shared_ptr<DarkHorse::SmSymbol> symbol);
		void register_symbol(const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map);
		void register_symbol(const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec);
		void register_account();
	};
}
