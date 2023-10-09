#pragma once
#include <vector>
#include <memory>
#include <map>
#include <string>

namespace DarkHorse {
	class SmSystem;
	class SmSystemManager
	{
	public:
		// Static Members
		static int _Id;
		static int GetId() { return ++_Id; }
		std::shared_ptr<DarkHorse::SmSystem> AddSystem(const std::string& system_name);
		const std::map<std::string, std::shared_ptr<DarkHorse::SmSystem>>& GetSystemMap() { return _SystemMap; }
	private:
		std::map<std::string, std::shared_ptr<DarkHorse::SmSystem>> _SystemMap;
	};
}

