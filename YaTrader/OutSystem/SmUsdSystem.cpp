#include "stdafx.h"
#include "SmUsdSystem.h"
namespace DarkHorse {

	SmUsdSystem::SmUsdSystem(std::string strategy_type)
		: SmOutSystem(strategy_type)
	{
		CString name;
		name.Format("%s_%d", "usd_system", id_);
		name_ = static_cast<const char*>(name);
		strategy_type_ = strategy_type;
	}

} // namespace DarkHorse