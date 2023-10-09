#pragma once
#include <string>
#include <memory>
#include <map>

namespace DarkHorse {
	class SmTimeData;
	class SmTimeDataManager
	{
	public:
		static int _Id;
		static int GetId() { return _Id++; }
		SmTimeDataManager() {};
		~SmTimeDataManager() {};
		std::shared_ptr<SmTimeData> FindAddTimeData(const std::string name);
	private:
		std::shared_ptr<SmTimeData> CreateTimeData(const std::string& name);
		// key : name, value : SmTimeData object.
		std::map<std::string, std::shared_ptr<SmTimeData>> _DataMap;
	};
}

