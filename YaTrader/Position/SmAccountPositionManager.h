#pragma once
#include <string>
#include <map>
#include <memory>

namespace DarkHorse {
	struct SmPosition;
	class SmAccountPositionManager
	{
	public:
		SmAccountPositionManager(const std::string& account_no) : _AccountNo(account_no) {};
		~SmAccountPositionManager() {};
		std::string AccountNo() const { return _AccountNo; }
		void AccountNo(std::string val) { _AccountNo = val; }
		std::shared_ptr<SmPosition> FindAddPosition(const std::string& symbol_code);
		std::shared_ptr<SmPosition> FindPosition(const std::string& symbol_code);
		void UpdatePosition(const std::string& symbol_code);
		const std::map<std::string, std::shared_ptr<SmPosition>>& GetPositionMap() {
			return _PositionMap;
		}
		double SumAccountPL();
	private:
		std::string _AccountNo;
		// Key : symbol code. value : SmPosition Object.
		std::map<std::string, std::shared_ptr<SmPosition>> _PositionMap;
	};
}

