#pragma once
#include "../Json/json.hpp"
#include "../Common/BlockingCollection.h"
#include "../Common/common.h"
#include <set>

namespace DarkHorse {
	using namespace code_machina;
	const int BulkQuoteSize = 100;
	class SmQuoteProcessor : public Runnable
	{
	public:
		SmQuoteProcessor();
		~SmQuoteProcessor() override;
		void StartProcess() noexcept;
		void StopProcess() noexcept;
		virtual unsigned int ThreadHandlerProc(void);
		void AddQuote(nlohmann::json && quote) noexcept;
		bool Enable() const { return _Enable; }
		void Enable(bool val) { _Enable = val; }

		void SubscribeQuoteWndCallback(HWND id) {
			_WndQuoteSet.insert(id);
		}
		void UnsubscribeQuoteWndCallback(HWND id) {
			auto it = _WndQuoteSet.find(id);
			if (it != _WndQuoteSet.end()) {
				_WndQuoteSet.erase(it);
			}
		}

		size_t GetSubscribedWndCount() {
			return _WndQuoteSet.size();
		}

	private:
		void ClearTasks();
		bool _Enable{ true };
		bool _BatchProcess{ false };
		BlockingCollection<nlohmann::json> _QuoteQueue;
		void ProcessQuote(nlohmann::json&& quote);
		// arr : �����Ͱ� ��� �ִ� �迭, taken : ���� �����Ͱ� ��� �ִ� ����
		bool ProcessQuote(const std::array<nlohmann::json, BulkQuoteSize>& arr, const int& taken);
		void SendQuoteChangeEvent();
		std::set<HWND> _WndQuoteSet;
	};
}

