#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include "SmOutSystem.h"
#include "../Json/json.hpp"
#include "../Common/BlockingCollection.h"
#include "../Common/common.h"
#include "SmUsdStrategy.h"
#include <set>
#include <sstream>
#include <map>
using namespace code_machina;
namespace DarkHorse {
	const int BulkOutSystemSize2 = 100;
	class SmOutSystem;
	class SmOutSignalDef;
	class SmUsdSystem;
	class SmOutSystemManager : public Runnable
	{
	public:
		SmOutSystemManager();
		~SmOutSystemManager();
		std::shared_ptr<SmOutSystem> create_out_system(
			const std::string& signal_name,
			const int seung_su,
			OrderType order_type,
			std::shared_ptr<SmAccount> account,
			std::shared_ptr<SmFund> fund,
			std::shared_ptr<SmSymbol> symbol
		);

		std::shared_ptr<SmUsdSystem> create_usd_system(
			const std::string& strategy_type,
			const int seung_su,
			OrderType order_type,
			std::shared_ptr<SmAccount> account,
			std::shared_ptr<SmFund> fund,
			std::shared_ptr<SmSymbol> symbol
		);

		void remove_out_system(std::shared_ptr<SmOutSystem> out_system);
		void remove_usd_system(std::shared_ptr<SmUsdSystem> out_system);
		void remove_out_system_from_map(std::shared_ptr<SmOutSystem> out_system);
		void add_out_system_to_map(std::shared_ptr<SmOutSystem> out_system);
		void make_out_system_signal_map();
		const std::vector<std::shared_ptr<SmOutSignalDef>>& get_out_system_signal_map() const { return out_system_signal_vec_; }
		size_t get_out_system_count() const { return out_system_vec_.size(); }
		const std::vector<std::shared_ptr<SmOutSystem>>& get_out_system_vector() const { return out_system_vec_; }
		const std::vector<std::shared_ptr<SmUsdSystem>>& get_usd_system_vector() const { return usd_system_vec_; }

		void StartProcess() noexcept;
		void StopProcess() noexcept;
		virtual unsigned int ThreadHandlerProc(void);
		void AddSignal(nlohmann::json&& signal) noexcept;
		std::string GetLastLine(const std::string& filename);
		bool moveToStartOfLine(std::ifstream& fs);
		std::string getLastLineInFile(std::ifstream& fs);
		bool Enable() const { return _Enable; }
		void Enable(bool val) { _Enable = val; }
		void add_active_out_system(std::shared_ptr<SmOutSystem> out_system);
		void remove_active_out_system(std::shared_ptr<SmOutSystem> out_system);

		void add_active_usd_system(std::shared_ptr<SmUsdSystem> usd_system);
		void remove_active_usd_system(std::shared_ptr<SmUsdSystem> usd_system);
		const std::map<int, std::shared_ptr<SmOutSystem>>& get_active_out_system_map() const { return active_out_system_map_; }
		const std::map<int, std::shared_ptr<SmUsdSystem>>& get_active_usd_system_map() const { return active_usd_system_map_; }
		static int order_tick;
		static SmPriceType price_type;
		const std::vector<std::string>& get_usd_strategy_vec() const { return usd_strategy_vec_; }
		SmUsdStrategy get_usd_strategy(const std::string& strategy_type) const;
	private:
		void put_order(const std::string& signal_name, int order_kind, int order_amount);
		void remove_out_system_by_id(const int& system_id);
		void remove_usd_system_by_id(const int& system_id);
		std::vector<std::shared_ptr<SmOutSystem>> out_system_vec_;
		// key : system name, value : SmOutSystem object.
		std::map<std::string, SmOutSystemMap> out_system_map_;
		// key : system id, value : SmOutSystem object.
		std::map<int, std::shared_ptr<SmOutSystem>> active_out_system_map_;
		std::vector<std::shared_ptr<SmOutSignalDef>> out_system_signal_vec_;

		void ClearTasks();
		std::vector<std::string> split(const std::string& input, char delimiter);
		bool _Enable{ true };
		bool _BatchProcess{ false };
		BlockingCollection<nlohmann::json> _SignalQueue;
		void ProcessSignal(nlohmann::json&& signal);
		// arr : 데이터가 들어 있는 배열, taken : 실제 데이터가 들어 있는 갯수
		bool ProcessSignal(const std::array<nlohmann::json, BulkOutSystemSize2>& arr, const int& taken);

		void execute_order(std::string&& order_signal);
		// key : system name, value : strategy object.
		std::map<std::string, SmUsdStrategy> usd_strategy_map_;
		std::vector<std::shared_ptr<SmUsdSystem>> usd_system_vec_;
		std::map<int, std::shared_ptr<SmUsdSystem>> active_usd_system_map_;
		void init_usd_strategy();
		std::vector<std::string> usd_strategy_vec_;
		void init_usd_strategy_vec();
	};
}
