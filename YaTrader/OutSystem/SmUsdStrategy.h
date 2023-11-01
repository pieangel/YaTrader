#pragma once
#include <string>
#include <vector>
namespace DarkHorse {
	const int BUY_SIGNAL_IN = 0;
	const int SELL_SIGNAL_IN = 0;
	const int BUY_SIGNAL_OUT = 0;
	const int SELL_SIGNAL_OUT = 0;

	enum class SmStrategyType
	{
		None,
		TR_US1,
		TR_US2,
		TR_US3,
		US_IN1,
		US_IN2,
		US_IN3
	};

	enum class SignalType {
		BUY_SIGNAL_IN = 0,
		SELL_SIGNAL_IN = 1,
		BUY_SIGNAL_OUT = 2,
		SELL_SIGNAL_OUT = 3,
	};

	struct SysArg {
		bool enable = false;
		std::string data_source1;
		std::string data_source2;
		std::string desc;
		std::string param;
		std::string name;
		bool result = false;
	};
	struct GroupArg {
		std::string name;
		std::vector<SysArg> sys_args;
	};

	class SmUsdStrategy
	{
	public:
		SmUsdStrategy() {
		}
		std::string type() const { return type_; }
		void type(std::string val) { type_ = val; }
		std::vector<GroupArg> group_args;
	private:
		std::string type_;
	};

}