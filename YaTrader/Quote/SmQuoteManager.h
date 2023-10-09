#pragma once
// 시세를 관리한다. 
// 시세가 오면 해당 종목에 전달하고 업데이트 한다.
#include "../Json/json.hpp"
#include "../Common/BlockingCollection.h"
#include "../Common/common.h"
#include <memory>
#include <map>
namespace DarkHorse {
	using namespace code_machina;
	const int BulkSize = 100;
	class SmQuoteProcessor;
	struct SmQuote;
	class SmQuoteManager : public Runnable
	{
	public:
		SmQuoteManager();
		~SmQuoteManager() override;
		void StartProcess() noexcept;
		void StopProcess() noexcept;
		virtual unsigned int ThreadHandlerProc(void);
		void AddQuote(nlohmann::json&& quote) noexcept;
		bool Enable() const { return _Enable; }
		void Enable(bool val) { _Enable = val; }
		void SubscribeEvent(const std::string& symbol_code, HWND wnd);
		void UnsubscribeEvent(const std::string& symbol_code, HWND wnd);
		void StopAllQuoteProcess();
		std::shared_ptr<SmQuote> add_quote(const std::string& symbol_code);
		std::shared_ptr<SmQuote> find_quote(const std::string& symbol_code);
		std::shared_ptr<SmQuote> get_quote(const std::string& symbol_code);
	private:
		void ClearTasks();
		bool _Enable{ true };
		bool _BatchProcess{ false };
		BlockingCollection<nlohmann::json> _QuoteQueue;
		void ProcessQuote(nlohmann::json&& quote);
		// arr : 데이터가 들어 있는 배열, taken : 실제 데이터가 들어 있는 갯수
		bool ProcessQuote(const std::array<nlohmann::json, BulkSize>& arr, const int& taken);
		// Key : Symbol Code. value : SmQuoteProcessor object.
		std::map<std::string, std::shared_ptr<SmQuoteProcessor>> _QuoteProcessorMap;
		// key : symbol code, value : SmQuote object.
		std::map<std::string, std::shared_ptr<SmQuote>> quote_map_;
	};
}

