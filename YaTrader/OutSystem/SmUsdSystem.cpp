#include "stdafx.h"
#include "SmUsdSystem.h"
#include "../Hoga/SmHogaProcessor.h"
#include "../Global/SmTotalManager.h"
#include "../OutSystem/SmOutSystemManager.h"
#include "../Util/VtTime.h"
namespace DarkHorse {

	bool SmUsdSystem::check_condition(const SysArg& arg)
	{
		int data_source1 = mainApp.out_system_manager()->usd_system_data().get_data(arg.data_source1);
		int data_source2 = mainApp.out_system_manager()->usd_system_data().get_data(arg.data_source2);
		if (data_source1 <= 0 || data_source2 <= 0) return false;
		double param = std::stod(arg.param);
		if (data_source1 * param > data_source2) return true;
		return false;
	}

	void SmUsdSystem::check_group_condition(const GroupArg& group_arg, std::vector<bool>& arg_cond)
	{
		for (int i = 0; i < 4; i++) {
			bool result = check_condition(group_arg.sys_args[i]);
			arg_cond.push_back(result);
		}
	}

	bool SmUsdSystem::check_entrance(const int index)
	{
		std::vector<bool> arg_cond;
		const GroupArg& group_arg = strategy_.group_args[index];
		check_group_condition(group_arg, arg_cond);
		if (arg_cond.size() == 0)
			return false;

		// 하나의 조건이라도 거짓이면 신호 없음. 모두가 참이면 매수 반환
		auto it = std::find(arg_cond.begin(), arg_cond.end(), false);
		if (it != arg_cond.end())
			return false;
		else
			return true;
	}

	bool SmUsdSystem::CheckEntranceBar()
	{
		VtTime time = VtTimeUtil::GetLocalTime();
		if (time.hour == start_time_begin_.hour && time.min == start_time_begin_.min)
			return false;
		else
			return true;
	}

	bool SmUsdSystem::CheckEntranceForBuy()
	{
		return check_entrance(0);
	}

	bool SmUsdSystem::CheckEntranceForSell()
	{
		return check_entrance(1);
	}

	bool SmUsdSystem::CheckLiqForBuy()
	{
		return check_entrance(2);
	}

	bool SmUsdSystem::CheckLiqForSell()
	{
		return check_entrance(3);
	}

	bool SmUsdSystem::CheckOrderLimit()
	{
		if (entrance_count_ > order_limit_count_)
			return false;
		else
			return true;
	}

	bool SmUsdSystem::CheckEnterableByTime()
	{
		int curTime = VtTimeUtil::GetTime(VtTimeUtil::GetLocalTime());
		int startTime = VtTimeUtil::GetTime(start_time_begin_);
		int endTime = VtTimeUtil::GetTime(start_time_end_);
		if (curTime < startTime || curTime > endTime)
			return false;
		else
			return true;
	}

	bool SmUsdSystem::CheckLigByTime()
	{
		int curTime = VtTimeUtil::GetTime(VtTimeUtil::GetLocalTime());
		int liqTime = VtTimeUtil::GetTime(end_time_);
		if (curTime >= liqTime) 
			return true;
		else
			return false;
	}

	void SmUsdSystem::on_timer()
	{
		if (!enable_) return;

		if (was_liq) return;

		if (CheckLigByTime())
			liq_all();


		if (!CheckEntranceBar()) return;
	}

	SmUsdSystem::SmUsdSystem(std::string strategy_type)
		: SmOutSystem(strategy_type)
	{
		CString name;
		name.Format("%s_%d", "usd_system", id_);
		name_ = static_cast<const char*>(name);
		strategy_type_ = strategy_type;
	}

} // namespace DarkHorse