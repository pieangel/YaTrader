#pragma once
#include <string>
#include <map>
#include <memory>
namespace DarkHorse {
	struct SmPosition;
	class SmAccountPositionManager;
	class SmTotalPositionManager
	{
	public:
		SmTotalPositionManager() {};
		~SmTotalPositionManager() {};

	private:
		// Key : Account No, Value : Account Position Manager Object.
		std::map<std::string, std::shared_ptr<SmAccountPositionManager>> _AccountPositionMap;
	public:
		std::shared_ptr<SmAccountPositionManager> FindAddAccountPositionManager(std::string account_no);
		std::shared_ptr<SmPosition> FindAddPosition(const std::string& account_no, const std::string& symbol_code);
		std::shared_ptr<SmPosition> FindPosition(const std::string& account_no, const std::string& symbol_code);
		void UpdatePosition(const std::string& symbol_code);
	};
}

