#pragma once
// �ü��� �����Ѵ�. 
// �ü��� ���� �ش� ���� �����ϰ� ������Ʈ �Ѵ�.
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
		// arr : �����Ͱ� ��� �ִ� �迭, taken : ���� �����Ͱ� ��� �ִ� ����
		bool ProcessQuote(const std::array<nlohmann::json, BulkSize>& arr, const int& taken);
		// Key : Symbol Code. value : SmQuoteProcessor object.
		std::map<std::string, std::shared_ptr<SmQuoteProcessor>> _QuoteProcessorMap;
		// key : symbol code, value : SmQuote object.
		std::map<std::string, std::shared_ptr<SmQuote>> quote_map_;
	};
}

