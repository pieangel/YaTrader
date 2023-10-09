#include "stdafx.h"
#include "SmSystemManager.h"
#include "SmSystem.h"

using namespace DarkHorse;

int SmSystemManager::_Id = 0;

std::shared_ptr<DarkHorse::SmSystem> DarkHorse::SmSystemManager::AddSystem(const std::string& system_name)
{
	std::shared_ptr<DarkHorse::SmSystem> system = std::make_shared<DarkHorse::SmSystem>(GetId(), system_name);
	system->BindSystemData();
	_SystemMap[system_name] = system;
	return system;
}
