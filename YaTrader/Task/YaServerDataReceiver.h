#pragma once

//#include "../Global/TemplateSingleton.h"
//#include "../EBCtrl/EbGlobal.h"
#include <map>
#include <memory>
#include <vector>
#include "../Task/SmTaskDefine.h"
// �������� �����͸� �������� �۾��� �׷캰�� ����ȴ�.
// �ϳ��� �۾� �׷��� ť�� �ְ� �� �۾� �׷��� �ϼ��� �Ǹ� ���� �۾� �׷��� �����Ѵ�.
// �۾� �׷��� ����Ǵ� ������ ���α׷����ٸ� ���ؼ� ǥ�õȴ�.
class VtProgressDlg;

namespace DarkHorse {
	class SmProductYearMonth;
	class SmSymbol;
	class YaServerDataReceiver
	{
	private:
		// ���� ���¸� ��Ÿ����.
		//SmAppState _AppState = SmAppState::None;
		bool server_data_receive_on_{ true };
		// Key : id, value : argument
		//std::map<int, DhTaskArg> 	_RequestMap;
		void execute_request(DhTaskArg& arg);
		// ���� ��Ȳ�� ���³��� ��ȭ����
		//VtProgressDlg* _ProgressDlg;
		std::shared_ptr<VtProgressDlg> progress_dialog_;
		// ���� �۾� ���� �۾��� ������ ����Ѵ�.
		DhTaskInfo task_info_;
		// ���� �۾� ���¸� ǥ���Ѵ�.
		void set_task_state();
		// ���� �׷� �۾��� �����Ѵ�.
		void do_next_group_task();
	public:
		// ť�� ���� �ִ� �۾��� �� ������ �۾��� ���� �մϴ�.
		// ť�� ��� ������ �ƹ��ϵ� ���� �ʽ��ϴ�.
		void execute_next();
		DhTaskArg* find_task(const int& argument_id);
		void on_task_complete(const int& argument_id);
		void on_task_error(const int& argument_id);
		// �������� ���� �������� ������ �����Ų��.
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
		// �Ϻ� ������ �����ɴϴ�.
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
