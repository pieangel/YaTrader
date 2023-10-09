#pragma once
// 호가를 처리한다.
// 호가가 오면 해당 종목에 호가를 업데이트 한다.
// 실시간 호가만 처리한다.
#include "../Json/json.hpp"
#include "../Common/BlockingCollection.h"
#include "../Common/common.h"
#include <memory>
#include <map>
namespace DarkHorse {
	using namespace code_machina;
	const int BulkHogaTaskSize = 100;
	class SmHogaProcessor;
	struct SmHoga;
	class SmHogaManager : public Runnable
	{
	public:
		SmHogaManager();
		~SmHogaManager() override;
		void StartProcess() noexcept;
		void StopProcess() noexcept;
		virtual unsigned int ThreadHandlerProc(void);
		void AddHoga(nlohmann::json&& hoga) noexcept;
		bool Enable() const { return _Enable; }
		void Enable(bool val) { _Enable = val; }
		void StopAllHogaProcess();
		std::shared_ptr<SmHoga> add_hoga(const std::string& symbol_code);
		std::shared_ptr<SmHoga> find_hoga(const std::string& symbol_code);
		std::shared_ptr<SmHoga> get_hoga(const std::string& symbol_code);
	private:
		void ClearTasks();
		bool _Enable{ true };
		bool _BatchProcess{ false };
		BlockingCollection<nlohmann::json> _HogaQueue;
		void ProcessHoga(nlohmann::json&& hoga);
		// arr : 데이터가 들어 있는 배열, taken : 실제 데이터가 들어 있는 갯수
		bool ProcessHoga(const std::array<nlohmann::json, BulkHogaTaskSize>& arr, const int& taken);
		std::map<std::string, std::shared_ptr<SmHogaProcessor>> _HogaProcessorMap;
		// key : symbol code, value : SmHoga object.
		std::map<std::string, std::shared_ptr<SmHoga>> hoga_map_;
	};
}

