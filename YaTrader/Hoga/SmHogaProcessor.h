#pragma once
#include "../Json/json.hpp"
#include "../Common/BlockingCollection.h"
#include "../Common/common.h"
#include "../OutSystem/SmUsdSystemData.h"
namespace DarkHorse {
	using namespace code_machina;
	const int BulkHogaSize = 100;
	class SmHogaProcessor : public Runnable
	{
	public:
		SmHogaProcessor();
		~SmHogaProcessor() override;
		void StartProcess() noexcept;
		void StopProcess() noexcept;
		unsigned int ThreadHandlerProc(void) override;
		void AddHoga(nlohmann::json&& hoga) noexcept;
		bool Enable() const { return _Enable; }
		void Enable(bool val) { _Enable = val; }
	private:
		void ClearTasks() noexcept;
		bool _Enable{ true };
		bool _BatchProcess{ false };
		BlockingCollection<nlohmann::json> _HogaQueue;
		void ProcessHoga(nlohmann::json&& hoga);
		// arr : 데이터가 들어 있는 배열, taken : 실제 데이터가 들어 있는 갯수
		bool ProcessHoga(const std::array<nlohmann::json, BulkHogaSize>& arr, const int& taken);
		//SmUsdSystemData usd_system_data_;
	};
}



