#include "stdafx.h"
#include "SmOutSystemManager.h"
#include "../Util/IdGenerator.h"
#include "SmOutSignalDef.h"
#include "../Log/MyLogger.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "magic_enum/magic_enum.hpp"
#include "SmUsdSystem.h"
#include "../Util/SmUtil.h"
#include <chrono>

using namespace std::chrono;

namespace DarkHorse {
	int SmOutSystemManager::order_tick = 10;
	SmPriceType SmOutSystemManager::price_type = SmPriceType::Price;

	DarkHorse::SmUsdStrategy SmOutSystemManager::get_usd_strategy(const std::string& strategy_type) const
	{
		return usd_strategy_map_.at(strategy_type);
	}

	std::string SmOutSystemManager::get_usd_strategy_name()
	{
		std::string usd_strategy_name;
		usd_strategy_name = "usd_system_";
		usd_strategy_name.append(std::to_string(IdGenerator::get_id()));
		return usd_strategy_name;
	}

	SmOutSystemManager::SmOutSystemManager()
	{
		make_out_system_signal_map();
		init_usd_strategy();
		init_usd_strategy_vec();
		create_timer_for_usd_system();
	}


	SmOutSystemManager::~SmOutSystemManager()
	{
	}

	std::shared_ptr<SmOutSystem> SmOutSystemManager::create_out_system(
		const std::string& signal_name,
		const int seung_su,
		OrderType order_type,
		std::shared_ptr<SmAccount> account,
		std::shared_ptr<SmFund> fund,
		std::shared_ptr<SmSymbol> symbol
	)
	{
		std::shared_ptr<SmOutSystem> out_system = std::make_shared<SmOutSystem>(signal_name);
		out_system->seung_su(seung_su);
		out_system->order_type(order_type);
		out_system->account(account);
		out_system->fund(fund);
		out_system->symbol(symbol);
		out_system_vec_.push_back(out_system);

		//add_out_system_to_map(out_system);

		return out_system;
	}

	std::shared_ptr<DarkHorse::SmUsdSystem> SmOutSystemManager::create_usd_system(
		const std::string& strategy_type, 
		const int seung_su, 
		OrderType order_type, 
		std::shared_ptr<SmAccount> account, 
		std::shared_ptr<SmFund> fund, 
		std::shared_ptr<SmSymbol> symbol)
	{
		std::shared_ptr<SmUsdSystem> usd_system = std::make_shared<SmUsdSystem>(strategy_type);
		usd_system->seung_su(seung_su);
		usd_system->order_type(order_type);
		usd_system->account(account);
		usd_system->fund(fund);
		usd_system->symbol(symbol);
		usd_system_vec_.push_back(usd_system);
		return usd_system;
	}

	void SmOutSystemManager::remove_out_system(std::shared_ptr<SmOutSystem> out_system)
	{
		if (!out_system) return;

		//remove_out_system_from_map(out_system);
		remove_out_system_by_id(out_system->id());
	}

	void SmOutSystemManager::remove_usd_system(std::shared_ptr<SmUsdSystem> out_system)
	{
		if (!out_system) return;

		remove_usd_system_by_id(out_system->id());
	}

	void SmOutSystemManager::remove_out_system_from_map(std::shared_ptr<SmOutSystem> out_system)
	{
		if (!out_system) return;

		auto found = out_system_map_.find(out_system->name());
		if (found == out_system_map_.end()) return;
		SmOutSystemMap& system_map = found->second;
		auto it = system_map.find(out_system->id());
		if (it != system_map.end()) {
			system_map.erase(it);
		}
	}

	void SmOutSystemManager::remove_usd_system_from_map(std::shared_ptr<SmUsdSystem> usd_system)
	{
		if (!usd_system) return;

		auto found = usd_system_map_.find(usd_system->name());
		if (found == usd_system_map_.end()) return;
		usd_system_map_.erase(found);
	}

	void SmOutSystemManager::add_out_system_to_map(std::shared_ptr<SmOutSystem> out_system)
	{
		if (!out_system) return;

		auto it = out_system_map_.find(out_system->name());
		if (it != out_system_map_.end())
		{
			SmOutSystemMap& system_map = it->second;
			system_map.insert(std::make_pair(out_system->id(), out_system));
		}
		else {
			SmOutSystemMap system_map;
			system_map.insert(std::make_pair(out_system->id(), out_system));
			out_system_map_.insert(std::make_pair(out_system->name(), system_map));

		}
	}

	void SmOutSystemManager::add_usd_system_to_map(std::shared_ptr<SmUsdSystem> usd_system)
	{
		if (!usd_system) return;

		auto it = usd_system_map_.find(usd_system->name());
		if (it != usd_system_map_.end())
		{
			usd_system_map_.insert(std::make_pair(usd_system->name(), usd_system));
		}
	}

	void SmOutSystemManager::OnTimer()
	{
		for (auto& usd_system : usd_system_vec_) {
			usd_system->on_timer();
		}
	}

	void SmOutSystemManager::create_timer_for_usd_system()
	{
		std::vector<int> date_time = SmUtil::GetLocalDateTime();
		int waitTime = 60 - date_time[5];
		// Add to the timer.
		auto id = _Timer.add(seconds(waitTime), std::bind(&SmOutSystemManager::OnTimer, this), seconds(60));
		// Add to the request map.
		_TimerMap["usd_system"] = id;
	}

	void SmOutSystemManager::put_order(const std::string& signal_name, int order_kind, int order_amount)
	{
		auto found = out_system_map_.find(signal_name);
		if (found == out_system_map_.end()) return;
		const SmOutSystemMap& system_map = found->second;
		for (auto& [system_id, out_system] : system_map) {
			out_system->put_order(signal_name, order_kind, order_amount);
		}
	}

	void SmOutSystemManager::remove_out_system_by_id(const int& system_id)
	{
		std::erase_if(out_system_vec_, [&](const std::shared_ptr<SmOutSystem>& out_system) {
			return out_system->id() == system_id; });
	}

	void SmOutSystemManager::remove_usd_system_by_id(const int& system_id)
	{
		std::erase_if(usd_system_vec_, [&](const std::shared_ptr<SmUsdSystem>& usd_system) {
			return usd_system->id() == system_id; });
	}

	void SmOutSystemManager::make_out_system_signal_map()
	{
		for (int i = 0; i < 100; i++) {
			auto out_system_signal = std::make_shared<SmOutSignalDef>(IdGenerator::get_id());
			out_system_signal->name = "T" + std::to_string(i + 1);
			out_system_signal->desc = "T" + std::to_string(i + 1);
			out_system_signal_vec_.push_back(out_system_signal);
		}
	}



	void SmOutSystemManager::StartProcess() noexcept
	{
		start();
	}

	void SmOutSystemManager::StopProcess() noexcept
	{
		if (!m_runMode) return;
		// 먼저 큐를 비운다.
		ClearTasks();
		m_stop = true;
		nlohmann::json empty_signal;
		empty_signal["symbol_code"] = "";
		_SignalQueue.add(std::move(empty_signal));
		// 쓰레드가 끝날때까지 기다린다.
		if (m_thread.joinable())
			m_thread.join();
		if (m_runMode)
			m_runMode = false;
	}

	unsigned int SmOutSystemManager::ThreadHandlerProc(void)
	{
		while (true) {
			// 종료 신호를 보내면 루프를 나간다.
			if (isStop()) {
				break;
			}
			nlohmann::json signal;
			_SignalQueue.take(signal);
			ProcessSignal(std::move(signal));
		}

		return 1;
	}

	void SmOutSystemManager::AddSignal(nlohmann::json&& signal) noexcept
	{
		if (!_Enable) return;

		_SignalQueue.try_add(signal);
	}


	std::string SmOutSystemManager::GetLastLine(const std::string& filename) {
		std::string lastline;
		try {
			std::ifstream fs;
			fs.open(filename.c_str(), std::fstream::in);
			if (fs.is_open()) {
				
				//Got to the last character before EOF
				fs.seekg(-1, std::ios_base::end);
				if (fs.peek() == '\n') {
					//Start searching for \n occurrences
					fs.seekg(-1, std::ios_base::cur);
					// Returns the position of the current character in the input stream
					int i = (int)fs.tellg();
					for (i; i > 0; i--) {
						if (fs.peek() == '\n') {
							//Found
							// Extracts characters from the stream, as unformatted input :
							fs.get();
							break;
						}
						//Move one character back
						fs.seekg(i, std::ios_base::beg);
					}
				}
				getline(fs, lastline);
				
				//lastline = getLastLineInFile(fs);
			}
			else {
				std::cout << "Could not find end line character" << std::endl;
				std::string msg = "Could not find end line character";
				LOGINFO(CMyLogger::getInstance(), _T("OnFileChanged : 파일 읽기 오류 msg = %s, 파일이름 : %s"), msg, filename);
			}
			fs.close();
		}
		catch (std::exception& e) {
			LOGINFO(CMyLogger::getInstance(), _T(" %s, MSG : %s"), __FUNCTION__, e.what());
		}
		catch (...) {
			LOGINFO(CMyLogger::getInstance(), _T(" %s 알수없는 오류"), __FUNCTION__);
		}

		return lastline;
	}

	bool SmOutSystemManager::moveToStartOfLine(std::ifstream& fs)
	{
		fs.seekg(-1, std::ios_base::cur);
		for (long i = (long)fs.tellg(); i > 0; i--)
		{
			if (fs.peek() == '\n')
			{
				fs.get();
				return true;
			}
			fs.seekg(i, std::ios_base::beg);
		}
		return false;
	}

	std::string SmOutSystemManager::getLastLineInFile(std::ifstream& fs)
	{
		// Go to the last character before EOF
		fs.seekg(-1, std::ios_base::end);
		if (!moveToStartOfLine(fs))
			return "";

		std::string lastline = "";
		getline(fs, lastline);
		return lastline;
	}

	void SmOutSystemManager::add_active_out_system(std::shared_ptr<SmOutSystem> out_system)
	{
		if (!out_system) return;
		add_out_system_to_map(out_system);
		auto found = active_out_system_map_.find(out_system->id());
		if (found != active_out_system_map_.end())return;
		active_out_system_map_.insert(std::make_pair(out_system->id(), out_system));
	}

	void SmOutSystemManager::remove_active_out_system(std::shared_ptr<SmOutSystem> out_system)
	{
		if (!out_system) return;
		remove_out_system_from_map(out_system);
		auto found = active_out_system_map_.find(out_system->id());
		if (found == active_out_system_map_.end())return;
		active_out_system_map_.erase(found);
	}

	void SmOutSystemManager::add_active_usd_system(std::shared_ptr<SmUsdSystem> usd_system)
	{
		if (!usd_system) return;
		add_out_system_to_map(usd_system);
		auto found = active_usd_system_map_.find(usd_system->id());
		if (found != active_usd_system_map_.end())return;
		active_usd_system_map_.insert(std::make_pair(usd_system->id(), usd_system));
	}

	void SmOutSystemManager::remove_active_usd_system(std::shared_ptr<SmUsdSystem> usd_system)
	{
		if (!usd_system) return;
		remove_usd_system_from_map(usd_system);
		auto found = active_usd_system_map_.find(usd_system->id());
		if (found == active_usd_system_map_.end())return;
		active_usd_system_map_.erase(found);
	}

	void SmOutSystemManager::ProcessSignal(nlohmann::json&& signal)
	{
		try {	
			if (!signal.contains("file_name")) return;

			const std::string file_name = signal["file_name"];
			LOGINFO(CMyLogger::getInstance(), _T("ProcessSignal : 파일이름 : %s"), file_name.c_str());

			std::string lastline;
			try {
				lastline = GetLastLine(file_name);
			}
			catch (std::exception& e) {
				LOGINFO(CMyLogger::getInstance(), _T(" %s, MSG : %s"), __FUNCTION__, e.what());
			}
			catch (...) {
				LOGINFO(CMyLogger::getInstance(), _T(" %s 알수없는 오류"), __FUNCTION__);
			}

			if (lastline.empty()) return;

			std::string msg;
			msg.append(file_name);
			msg.append(_T(","));
			msg.append(lastline);

			execute_order(msg);
		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	bool SmOutSystemManager::ProcessSignal(const std::array<nlohmann::json, BulkOutSystemSize2>& arr, const int& taken)
	{
		return true;
	}

	void SmOutSystemManager::execute_order(std::string order_signal)
	{
		try {
		std::vector<std::string> tokens = split(order_signal, ',');
		if (tokens.size() > 7 && tokens[0].length() > 0) {
			std::filesystem::path full_path(tokens[0]);
			std::string signame = full_path.filename().string();
			auto pos = signame.find_first_of(_T("-"));
			signame = signame.substr(0, pos);
			CString strOrderKind(tokens[4].c_str());
			int order_kind = _ttoi(strOrderKind);
			CString strAmount(tokens[6].c_str());
			int order_amount = _ttoi(strAmount);
			put_order(signame, order_kind, order_amount);
		}
		else {
			LOGINFO(CMyLogger::getInstance(), _T("execute_order :: 주문 실행 오류 [%s]"), order_signal);
		}
		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	void SmOutSystemManager::init_usd_strategy()
	{
		SmUsdStrategy strategy;
		std::string strategy_name = magic_enum::enum_name(DarkHorse::SmStrategyType::TR_US1).data();
		strategy.type(strategy_name);
		GroupArg grp_arg;
		SysArg arg;
		grp_arg.sys_args.clear();
		// 매수 진입 조건 
		arg.name = "ubc>uac";
		arg.data_source1 = "ubc";
		arg.data_source2 = "uac";
		arg.desc = "ubc>uac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.6";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "ubs>uas";
		arg.data_source1 = "ubs";
		arg.data_source2 = "uas";
		arg.desc = "ubs>uas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.7";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kac>kbc";
		arg.data_source1 = "kac";
		arg.data_source2 = "kbc";
		arg.desc = "kac>kbc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kas>kbs";
		arg.data_source1 = "kas";
		arg.data_source2 = "kbs";
		arg.desc = "kas>kbs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매수진입";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////
		// 매도 진입 조건
		grp_arg.sys_args.clear();
		arg.name = "uac>ubc";
		arg.data_source1 = "uac";
		arg.data_source2 = "ubc";
		arg.desc = "uac>ubc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.6";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "uas>ubs";
		arg.data_source1 = "uas";
		arg.data_source2 = "ubs";
		arg.desc = "uas>ubs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.7";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbc>kac";
		arg.data_source1 = "kbc";
		arg.data_source2 = "kac";
		arg.desc = "kbc>kac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbs>kas";
		arg.data_source1 = "kbs";
		arg.data_source2 = "kas";
		arg.desc = "kbs>kas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매도진입";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////
		// 매수 청산 조건
		grp_arg.sys_args.clear();
		arg.name = "uac>ubc";
		arg.data_source1 = "uac";
		arg.data_source2 = "ubc";
		arg.desc = "uac>ubc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "uas>ubs";
		arg.data_source1 = "uas";
		arg.data_source2 = "ubs";
		arg.desc = "uas>ubs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbc>kac";
		arg.data_source1 = "kbc";
		arg.data_source2 = "kac";
		arg.desc = "kbc>kac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbs>kas";
		arg.data_source1 = "kbs";
		arg.data_source2 = "kas";
		arg.desc = "kbs>kas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매수청산";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////

		// 매도 청산 조건
		grp_arg.sys_args.clear();
		arg.name = "ubc>uac";
		arg.data_source1 = "ubc";
		arg.data_source2 = "uac";
		arg.desc = "ubc>uac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "ubs>uas";
		arg.data_source1 = "ubs";
		arg.data_source2 = "uas";
		arg.desc = "ubs>uas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kac>kbc";
		arg.data_source1 = "kac";
		arg.data_source2 = "kbc";
		arg.desc = "kac>kbc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kas>kbs";
		arg.data_source1 = "kas";
		arg.data_source2 = "kbs";
		arg.desc = "kas>kbs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매도청산";
		strategy.group_args.push_back(grp_arg);

		usd_strategy_map_.insert(std::make_pair(strategy_name, strategy));


		strategy_name = magic_enum::enum_name(DarkHorse::SmStrategyType::TR_US2).data();
		strategy.type(strategy_name);


		// 매수 진입 조건 
		grp_arg.sys_args.clear();
		arg.name = "ubc>uac";
		arg.data_source1 = "ubc";
		arg.data_source2 = "uac";
		arg.desc = "ubc>uac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.66";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "ubs>uas";
		arg.data_source1 = "ubs";
		arg.data_source2 = "uas";
		arg.desc = "ubs>uas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kac>kbc";
		arg.data_source1 = "kac";
		arg.data_source2 = "kbc";
		arg.desc = "kac>kbc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kas>kbs";
		arg.data_source1 = "kas";
		arg.data_source2 = "kbs";
		arg.desc = "kas>kbs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매수진입";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////
		// 매도 진입 조건
		grp_arg.sys_args.clear();
		arg.name = "uac>ubc";
		arg.data_source1 = "uac";
		arg.data_source2 = "ubc";
		arg.desc = "uac>ubc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.66";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "uas>ubs";
		arg.data_source1 = "uas";
		arg.data_source2 = "ubs";
		arg.desc = "uas>ubs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbc>kac";
		arg.data_source1 = "kbc";
		arg.data_source2 = "kac";
		arg.desc = "kbc>kac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbs>kas";
		arg.data_source1 = "kbs";
		arg.data_source2 = "kas";
		arg.desc = "kbs>kas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매도진입";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////
		// 매수 청산 조건
		grp_arg.sys_args.clear();
		arg.name = "uac>ubc";
		arg.data_source1 = "uac";
		arg.data_source2 = "ubc";
		arg.desc = "uac>ubc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "uas>ubs";
		arg.data_source1 = "uas";
		arg.data_source2 = "ubs";
		arg.desc = "uas>ubs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbc>kac";
		arg.data_source1 = "kbc";
		arg.data_source2 = "kac";
		arg.desc = "kbc>kac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbs>kas";
		arg.data_source1 = "kbs";
		arg.data_source2 = "kas";
		arg.desc = "kbs>kas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매수청산";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////

		// 매도 청산 조건
		grp_arg.sys_args.clear();
		arg.name = "ubc>uac";
		arg.data_source1 = "ubc";
		arg.data_source2 = "uac";
		arg.desc = "ubc>uac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "ubs>uas";
		arg.data_source1 = "ubs";
		arg.data_source2 = "uas";
		arg.desc = "ubs>uas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kac>kbc";
		arg.data_source1 = "kac";
		arg.data_source2 = "kbc";
		arg.desc = "kac>kbc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kas>kbs";
		arg.data_source1 = "kas";
		arg.data_source2 = "kbs";
		arg.desc = "kas>kbs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매도청산";
		strategy.group_args.push_back(grp_arg);

		usd_strategy_map_.insert(std::make_pair(strategy_name, strategy));


		strategy_name = magic_enum::enum_name(DarkHorse::SmStrategyType::TR_US3).data();
		strategy.type(strategy_name);

		// 매수 진입 조건 
		grp_arg.sys_args.clear();
		arg.name = "ubc>uac";
		arg.data_source1 = "ubc";
		arg.data_source2 = "uac";
		arg.desc = "ubc>uac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.7";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "ubs>uas";
		arg.data_source1 = "ubs";
		arg.data_source2 = "uas";
		arg.desc = "ubs>uas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.8";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kac>kbc";
		arg.data_source1 = "kac";
		arg.data_source2 = "kbc";
		arg.desc = "kac>kbc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kas>kbs";
		arg.data_source1 = "kas";
		arg.data_source2 = "kbs";
		arg.desc = "kas>kbs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매수진입";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////
		// 매도 진입 조건
		grp_arg.sys_args.clear();
		arg.name = "uac>ubc";
		arg.data_source1 = "uac";
		arg.data_source2 = "ubc";
		arg.desc = "uac>ubc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.7";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "uas>ubs";
		arg.data_source1 = "uas";
		arg.data_source2 = "ubs";
		arg.desc = "uas>ubs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.8";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbc>kac";
		arg.data_source1 = "kbc";
		arg.data_source2 = "kac";
		arg.desc = "kbc>kac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbs>kas";
		arg.data_source1 = "kbs";
		arg.data_source2 = "kas";
		arg.desc = "kbs>kas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매도진입";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////
		// 매수 청산 조건
		grp_arg.sys_args.clear();
		arg.name = "uac>ubc";
		arg.data_source1 = "uac";
		arg.data_source2 = "ubc";
		arg.desc = "uac>ubc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "uas>ubs";
		arg.data_source1 = "uas";
		arg.data_source2 = "ubs";
		arg.desc = "uas>ubs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.9";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbc>kac";
		arg.data_source1 = "kbc";
		arg.data_source2 = "kac";
		arg.desc = "kbc>kac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbs>kas";
		arg.data_source1 = "kbs";
		arg.data_source2 = "kas";
		arg.desc = "kbs>kas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매수청산";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////

		// 매도 청산 조건
		grp_arg.sys_args.clear();
		arg.name = "ubc>uac";
		arg.data_source1 = "ubc";
		arg.data_source2 = "uac";
		arg.desc = "ubc>uac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "ubs>uas";
		arg.data_source1 = "ubs";
		arg.data_source2 = "uas";
		arg.desc = "ubs>uas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.9";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kac>kbc";
		arg.data_source1 = "kac";
		arg.data_source2 = "kbc";
		arg.desc = "kac>kbc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kas>kbs";
		arg.data_source1 = "kas";
		arg.data_source2 = "kbs";
		arg.desc = "kas>kbs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매도청산";
		strategy.group_args.push_back(grp_arg);

		usd_strategy_map_.insert(std::make_pair(strategy_name, strategy));


		strategy_name = magic_enum::enum_name(DarkHorse::SmStrategyType::US_IN1).data();
		strategy.type(strategy_name);

		// 매수 진입 조건 
		grp_arg.sys_args.clear();
		arg.name = "ubc>uac";
		arg.data_source1 = "ubc";
		arg.data_source2 = "uac";
		arg.desc = "ubc>uac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "ubs>uas";
		arg.data_source1 = "ubs";
		arg.data_source2 = "uas";
		arg.desc = "ubs>uas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kac>kbc";
		arg.data_source1 = "kac";
		arg.data_source2 = "kbc";
		arg.desc = "kac>kbc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.7";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kas>kbs";
		arg.data_source1 = "kas";
		arg.data_source2 = "kbs";
		arg.desc = "kas>kbs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매수진입";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////
		// 매도 진입 조건
		grp_arg.sys_args.clear();
		arg.name = "uac>ubc";
		arg.data_source1 = "uac";
		arg.data_source2 = "ubc";
		arg.desc = "uac>ubc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "uas>ubs";
		arg.data_source1 = "uas";
		arg.data_source2 = "ubs";
		arg.desc = "uas>ubs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbc>kac";
		arg.data_source1 = "kbc";
		arg.data_source2 = "kac";
		arg.desc = "kbc>kac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.7";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbs>kas";
		arg.data_source1 = "kbs";
		arg.data_source2 = "kas";
		arg.desc = "kbs>kas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매도진입";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////
		// 매수 청산 조건
		grp_arg.sys_args.clear();
		arg.name = "uac>ubc";
		arg.data_source1 = "uac";
		arg.data_source2 = "ubc";
		arg.desc = "uac>ubc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.6";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "uas>ubs";
		arg.data_source1 = "uas";
		arg.data_source2 = "ubs";
		arg.desc = "uas>ubs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.7";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbc>kac";
		arg.data_source1 = "kbc";
		arg.data_source2 = "kac";
		arg.desc = "kbc>kac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.9";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbs>kas";
		arg.data_source1 = "kbs";
		arg.data_source2 = "kas";
		arg.desc = "kbs>kas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매수청산";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////

		// 매도 청산 조건
		grp_arg.sys_args.clear();
		arg.name = "ubc>uac";
		arg.data_source1 = "ubc";
		arg.data_source2 = "uac";
		arg.desc = "ubc>uac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.6";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "ubs>uas";
		arg.data_source1 = "ubs";
		arg.data_source2 = "uas";
		arg.desc = "ubs>uas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.7";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kac>kbc";
		arg.data_source1 = "kac";
		arg.data_source2 = "kbc";
		arg.desc = "kac>kbc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.9";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kas>kbs";
		arg.data_source1 = "kas";
		arg.data_source2 = "kbs";
		arg.desc = "kas>kbs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매도청산";
		strategy.group_args.push_back(grp_arg);

		usd_strategy_map_.insert(std::make_pair(strategy_name, strategy));


		strategy_name = magic_enum::enum_name(DarkHorse::SmStrategyType::US_IN2).data();
		strategy.type(strategy_name);

		// 매수 진입 조건 
		grp_arg.sys_args.clear();
		arg.name = "ubc>uac";
		arg.data_source1 = "ubc";
		arg.data_source2 = "uac";
		arg.desc = "ubc>uac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "ubs>uas";
		arg.data_source1 = "ubs";
		arg.data_source2 = "uas";
		arg.desc = "ubs>uas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kac>kbc";
		arg.data_source1 = "kac";
		arg.data_source2 = "kbc";
		arg.desc = "kac>kbc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.66";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kas>kbs";
		arg.data_source1 = "kas";
		arg.data_source2 = "kbs";
		arg.desc = "kas>kbs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매수진입";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////
		// 매도 진입 조건
		grp_arg.sys_args.clear();
		arg.name = "uac>ubc";
		arg.data_source1 = "uac";
		arg.data_source2 = "ubc";
		arg.desc = "uac>ubc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "uas>ubs";
		arg.data_source1 = "uas";
		arg.data_source2 = "ubs";
		arg.desc = "uas>ubs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbc>kac";
		arg.data_source1 = "kbc";
		arg.data_source2 = "kac";
		arg.desc = "kbc>kac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.66";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbs>kas";
		arg.data_source1 = "kbs";
		arg.data_source2 = "kas";
		arg.desc = "kbs>kas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매도진입";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////
		// 매수 청산 조건
		grp_arg.sys_args.clear();
		arg.name = "uac>ubc";
		arg.data_source1 = "uac";
		arg.data_source2 = "ubc";
		arg.desc = "uac>ubc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.6";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "uas>ubs";
		arg.data_source1 = "uas";
		arg.data_source2 = "ubs";
		arg.desc = "uas>ubs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.7";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbc>kac";
		arg.data_source1 = "kbc";
		arg.data_source2 = "kac";
		arg.desc = "kbc>kac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.9";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbs>kas";
		arg.data_source1 = "kbs";
		arg.data_source2 = "kas";
		arg.desc = "kbs>kas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매수청산";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////

		// 매도 청산 조건
		grp_arg.sys_args.clear();
		arg.name = "ubc>uac";
		arg.data_source1 = "ubc";
		arg.data_source2 = "uac";
		arg.desc = "ubc>uac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "ubs>uas";
		arg.data_source1 = "ubs";
		arg.data_source2 = "uas";
		arg.desc = "ubs>uas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kac>kbc";
		arg.data_source1 = "kac";
		arg.data_source2 = "kbc";
		arg.desc = "kac>kbc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.9";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kas>kbs";
		arg.data_source1 = "kas";
		arg.data_source2 = "kbs";
		arg.desc = "kas>kbs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매도청산";
		strategy.group_args.push_back(grp_arg);

		usd_strategy_map_.insert(std::make_pair(strategy_name, strategy));

		strategy_name = magic_enum::enum_name(DarkHorse::SmStrategyType::US_IN3).data();
		strategy.type(strategy_name);

		// 매수 진입 조건 
		grp_arg.sys_args.clear();
		arg.name = "ubc>uac";
		arg.data_source1 = "ubc";
		arg.data_source2 = "uac";
		arg.desc = "ubc>uac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "ubs>uas";
		arg.data_source1 = "ubs";
		arg.data_source2 = "uas";
		arg.desc = "ubs>uas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kac>kbc";
		arg.data_source1 = "kac";
		arg.data_source2 = "kbc";
		arg.desc = "kac>kbc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.6";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kas>kbs";
		arg.data_source1 = "kas";
		arg.data_source2 = "kbs";
		arg.desc = "kas>kbs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매수진입";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////
		// 매도 진입 조건
		grp_arg.sys_args.clear();
		arg.name = "uac>ubc";
		arg.data_source1 = "uac";
		arg.data_source2 = "ubc";
		arg.desc = "uac>ubc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "uas>ubs";
		arg.data_source1 = "uas";
		arg.data_source2 = "ubs";
		arg.desc = "uas>ubs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbc>kac";
		arg.data_source1 = "kbc";
		arg.data_source2 = "kac";
		arg.desc = "kbc>kac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "0.6";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbs>kas";
		arg.data_source1 = "kbs";
		arg.data_source2 = "kas";
		arg.desc = "kbs>kas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매도진입";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////
		// 매수 청산 조건
		grp_arg.sys_args.clear();
		arg.name = "uac>ubc";
		arg.data_source1 = "uac";
		arg.data_source2 = "ubc";
		arg.desc = "uac>ubc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "uas>ubs";
		arg.data_source1 = "uas";
		arg.data_source2 = "ubs";
		arg.desc = "uas>ubs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbc>kac";
		arg.data_source1 = "kbc";
		arg.data_source2 = "kac";
		arg.desc = "kbc>kac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kbs>kas";
		arg.data_source1 = "kbs";
		arg.data_source2 = "kas";
		arg.desc = "kbs>kas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매수청산";
		strategy.group_args.push_back(grp_arg);
		//////////////////////////////////////////////////////////////////////////

		// 매도 청산 조건
		grp_arg.sys_args.clear();
		arg.name = "ubc>uac";
		arg.data_source1 = "ubc";
		arg.data_source2 = "uac";
		arg.desc = "ubc>uac 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "ubs>uas";
		arg.data_source1 = "ubs";
		arg.data_source2 = "uas";
		arg.desc = "ubs>uas 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kac>kbc";
		arg.data_source1 = "kac";
		arg.data_source2 = "kbc";
		arg.desc = "kac>kbc 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);

		arg.name = "kas>kbs";
		arg.data_source1 = "kas";
		arg.data_source2 = "kbs";
		arg.desc = "kas>kbs 값을 설정 합니다.";
		arg.enable = false;
		arg.param = "1.0";
		arg.result = false;
		grp_arg.sys_args.push_back(arg);
		grp_arg.name = "매도청산";
		strategy.group_args.push_back(grp_arg);

		usd_strategy_map_.insert(std::make_pair(strategy_name, strategy));
	}

	void SmOutSystemManager::init_usd_strategy_vec()
	{
		usd_strategy_vec_.push_back("TR_US1");
		usd_strategy_vec_.push_back("TR_US2");
		usd_strategy_vec_.push_back("TR_US3");
		usd_strategy_vec_.push_back("US_IN1");
		usd_strategy_vec_.push_back("US_IN2");
		usd_strategy_vec_.push_back("US_IN3");
	}

	void SmOutSystemManager::ClearTasks()
	{
		_SignalQueue.flush();
	}

	std::vector<std::string> SmOutSystemManager::split(const std::string& input, char delimiter) {
		std::vector<std::string> tokens;
		std::istringstream stream(input);
		std::string token;

		while (std::getline(stream, token, delimiter)) {
			tokens.push_back(token);
		}

		return tokens;
	}
}

