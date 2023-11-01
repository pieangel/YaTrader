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
		void remove_out_system(std::shared_ptr<SmOutSystem> out_system);
		void remove_out_system_from_map(std::shared_ptr<SmOutSystem> out_system);
		void add_out_system_to_map(std::shared_ptr<SmOutSystem> out_system);
		void make_out_system_signal_map();
		const std::vector<std::shared_ptr<SmOutSignalDef>>& get_out_system_signal_map() const { return out_system_signal_vec_; }
		size_t get_out_system_count() const { return out_system_vec_.size(); }
		const std::vector<std::shared_ptr<SmOutSystem>>& get_out_system_vector() const { return out_system_vec_; }


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
		const std::map<int, std::shared_ptr<SmOutSystem>>& get_active_out_system_map() const { return active_out_system_map_; }
		static int order_tick;
		static SmPriceType price_type;
	private:
		void put_order(const std::string& signal_name, int order_kind, int order_amount);
		void remove_out_system_by_id(const int& system_id);
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
		// arr : �����Ͱ� ��� �ִ� �迭, taken : ���� �����Ͱ� ��� �ִ� ����
		bool ProcessSignal(const std::array<nlohmann::json, BulkOutSystemSize2>& arr, const int& taken);

		void execute_order(std::string&& order_signal);
		// key : strategy type, value : strategy object.
		std::map<std::string, SmUsdStrategy> usd_strategy_map_;
		void init_usd_strategy();
	};
}

/*
arg.Name = _T("Ubc>Uac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = true;
	arg.Desc = _T("Ubc>Uac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Ubs>Uas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = true;
	arg.Desc = _T("Ubs>Uas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);


	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kac>Kbc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Kas>Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kas>Kbs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Uac>Ubc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = true;
	arg.Desc = _T("Uac-Ubc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Uas>Ubs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = true;
	arg.Desc = _T("Uas-Ubs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Kbs>Kas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbs>Kas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Uac>Ubc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Uac-Ubc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Uas>Ubs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Uas-Ubs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);


	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Kbs>Kas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbs>Kas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);


	arg.Name = _T("Ubc>Uac");	
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Ubc>Uac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Ubs>Uas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Ubs>Uas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Kas>Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbs>Kas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);
*/