#pragma once
// ȣ���� ó���Ѵ�.
// ȣ���� ���� �ش� ���� ȣ���� ������Ʈ �Ѵ�.
// �ǽð� ȣ���� ó���Ѵ�.
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
		// arr : �����Ͱ� ��� �ִ� �迭, taken : ���� �����Ͱ� ��� �ִ� ����
		bool ProcessHoga(const std::array<nlohmann::json, BulkHogaTaskSize>& arr, const int& taken);
		std::map<std::string, std::shared_ptr<SmHogaProcessor>> _HogaProcessorMap;
		// key : symbol code, value : SmHoga object.
		std::map<std::string, std::shared_ptr<SmHoga>> hoga_map_;
	};
}

