#include "stdafx.h"
#include "SmUsdSystem.h"
#include "../Hoga/SmHogaProcessor.h"
#include "../Global/SmTotalManager.h"
#include "../OutSystem/SmOutSystemManager.h"
#include "../Util/VtTime.h"
#include "../Log/MyLogger.h"

namespace DarkHorse {

	bool SmUsdSystem::check_condition(std::string& group_arg_name, SysArg& arg)
	{
		int data_source1 = mainApp.out_system_manager()->usd_system_data().get_data(arg.data_source1);
		int data_source2 = mainApp.out_system_manager()->usd_system_data().get_data(arg.data_source2);
		double param = std::stod(arg.param);
		LOGINFO(CMyLogger::getInstance(), _T("check_condition ::group_arg_name[%s], enable[%s],  data_source1[%s],value1[%d], data_source2[%s], value2[%d], data_source1*param[%.2f], value2[%d]"), group_arg_name.c_str(), arg.enable ? "true" : "false", arg.data_source1.c_str(), data_source1, arg.data_source2.c_str(), data_source2, data_source1 * param, data_source2);
		if (arg.enable == false) return true;
		if (data_source1 <= 0 || data_source2 <= 0) return false;
		if (data_source1 * param > data_source2) return true;
		return false;
	}

	void SmUsdSystem::check_group_condition(GroupArg& group_arg, std::vector<bool>& arg_cond)
	{
		for (int i = 0; i < 4; i++) {
			bool result = check_condition(group_arg.name, group_arg.sys_args[i]);
			arg_cond.push_back(result);
		}
	}

	bool SmUsdSystem::check_entrance(const int index)
	{
		std::vector<bool> arg_cond;
		GroupArg& group_arg = strategy_.group_args[index];
		check_group_condition(group_arg, arg_cond);
		if (arg_cond.size() == 0)
			return false;

		bool enable = false;
		for (int i = 0; i < 4; i++) {
			if (group_arg.sys_args[i].enable) {
				enable = true;
				break;
			}
		}
		if (!enable) return false;

		// 하나의 조건이라도 거짓이면 신호 없음. 모두가 참이면 매수 반환
		auto it = std::find(arg_cond.begin(), arg_cond.end(), false);
		if (it != arg_cond.end())
			return false;
		else
			return true;
	}

	bool SmUsdSystem::check_entrance_by_time()
	{
		int curTime = VtTimeUtil::GetTime(VtTimeUtil::GetLocalTime());
		int startTime = VtTimeUtil::GetTime(start_time_begin_);
		int endTime = VtTimeUtil::GetTime(start_time_end_);
		if (curTime < startTime || curTime > endTime)
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
		bool result = check_entrance(0);

		LOGINFO(CMyLogger::getInstance(), _T("CheckEntranceForBuy :: result = [%s]"), result ? "true" : "false");

		return result;
	}

	bool SmUsdSystem::CheckEntranceForSell()
	{
		bool result = check_entrance(1);

		LOGINFO(CMyLogger::getInstance(), _T("CheckEntranceForSell :: result = [%s]"), result ? "true" : "false");

		return result;
	}

	bool SmUsdSystem::CheckLiqForBuy()
	{
		bool result = check_entrance(2);

		LOGINFO(CMyLogger::getInstance(), _T("CheckLiqForBuy :: result = [%s]"), result ? "true" : "false");

		return result;
	}

	bool SmUsdSystem::CheckLiqForSell()
	{
		bool result = check_entrance(3);

		LOGINFO(CMyLogger::getInstance(), _T("CheckLiqForSell :: result = [%s]"), result ? "true" : "false");

		return result;
	}

	bool SmUsdSystem::CheckOrderLimit()
	{
		if (entrance_count_ >= order_limit_count_)
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
		//LOGINFO(CMyLogger::getInstance(), _T("on_timer::%s"), __FUNCTION__);
		if (was_liq) {
			LOGINFO(CMyLogger::getInstance(), _T("%s was_liq[%s]"), __FUNCTION__, was_liq ? "true" : "false");
			return;
		}
		if (!enable_) {
			//LOGINFO(CMyLogger::getInstance(), _T("%s enable_[%s]"), __FUNCTION__, enable_ ? "true" : "false");
			return;
		}
		if (CheckLigByTime()) {
			if (buy_position_count_ <= 0 && sell_position_count_ <= 0) {
				//LOGINFO(CMyLogger::getInstance(), _T("%s buy_position_count[%d], sell_position_count[%d]"), __FUNCTION__, buy_position_count_, sell_position_count_);
				return;
			}
			LOGINFO(CMyLogger::getInstance(), _T("CheckLigByTime::%s time[%02d:%02d:%02d"), __FUNCTION__, end_time_.hour, end_time_.min, end_time_.sec);
			liq_all();
			was_liq = true;
		}
		if (!check_entrance_by_time()) {
			LOGINFO(CMyLogger::getInstance(), _T("check_entrance_by_time::%s"), __FUNCTION__);
			return;
		}
		if (!CheckEntranceBar()) {
			LOGINFO(CMyLogger::getInstance(), _T("CheckEntranceBar::%s"), __FUNCTION__);
			return;
		}

		if (CheckEntranceForBuy()) {
			if (!CheckOrderLimit()) {
				LOGINFO(CMyLogger::getInstance(), _T("CheckOrderLimit::%s, entrance_count_[%d]"), __FUNCTION__, entrance_count_);
				return;
			}
			if (buy_position_count_ > 0) return;
			LOGINFO(CMyLogger::getInstance(), _T("CheckEntranceForBuy::%s"), __FUNCTION__);
			entrance_count_++;
			buy_position_count_++;
			put_order(name_, 1, 1);
		}
		else if (CheckEntranceForSell()) {
			if (!CheckOrderLimit()) {
				LOGINFO(CMyLogger::getInstance(), _T("CheckOrderLimit::%s, entrance_count_[%d]"), __FUNCTION__, entrance_count_);
				return;
			}
			if (sell_position_count_ > 0) return;
			LOGINFO(CMyLogger::getInstance(), _T("CheckEntranceForSell::%s"), __FUNCTION__);
			entrance_count_++;
			sell_position_count_++;
			put_order(name_, 3, 1);
		}
		else if (CheckLiqForBuy()) {
			if (buy_position_count_ <= 0) {
				LOGINFO(CMyLogger::getInstance(), _T("%s buy_position_count[%d]"), __FUNCTION__, buy_position_count_);
				return;
			}
// 			if (!CheckOrderLimit()) {
// 				LOGINFO(CMyLogger::getInstance(), _T("CheckOrderLimit::%s, entrance_count_[%d]"), __FUNCTION__, entrance_count_);
// 				return;
// 			}
			LOGINFO(CMyLogger::getInstance(), _T("CheckLiqForBuy::%s"), __FUNCTION__);
			//entrance_count_++;
			buy_position_count_--;
			put_order(name_, 2, 1);
		}
		else if (CheckLiqForSell()) {
			if (sell_position_count_ <= 0) {
				LOGINFO(CMyLogger::getInstance(), _T("%s see_position_count[%d]"), __FUNCTION__, sell_position_count_);
				return;
			}
// 			if (!CheckOrderLimit()) {
// 				LOGINFO(CMyLogger::getInstance(), _T("CheckOrderLimit::%s, entrance_count_[%d]"), __FUNCTION__, entrance_count_);
// 				return;
// 			}
			LOGINFO(CMyLogger::getInstance(), _T("CheckLiqForSell::%s"), __FUNCTION__);
			//entrance_count_++;
			sell_position_count_--;
			put_order(name_, 4, 1);
		}
	}

	SmUsdSystem::SmUsdSystem(std::string strategy_type)
		: SmOutSystem(strategy_type)
	{
		CString name;
		name.Format("%s_%d", "usd_system", id_);
		name_ = static_cast<const char*>(name);
		strategy_type_ = strategy_type;

		start_time_begin_.hour = 8;
		start_time_begin_.min = 45;
		start_time_begin_.sec = 0;
		start_time_end_.hour = 15;
		start_time_end_.min = 45;
		start_time_end_.sec = 0;

		end_time_.hour = 15;
		end_time_.min = 45;
		end_time_.sec = 0;
	}

} // namespace DarkHorse