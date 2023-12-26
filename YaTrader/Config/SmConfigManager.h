#pragma once
#include <string>
#include "SystemConfig.h"
#include "../json/json.hpp"
#include <istream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "../Util/SmUtil.h"
using namespace nlohmann;
namespace DarkHorse {
	class SmConfigManager
	{
	public:
		SmConfigManager();
		~SmConfigManager();

		static std::string GetApplicationPath();
		const SystemConfig& system_config() const { return system_config_; }
		void set_system_config(const SystemConfig& val) { system_config_ = val; }
		// Function to save system configurations to a JSON file
		void saveConfig(const std::string& filePath) {
			nlohmann::json j;
			j["app_name"] = system_config_.app_name;
			j["version"] = system_config_.version;
			j["yes_path"] = SmUtil::MultiByteToUtf8(system_config_.yes_path);

			std::ofstream file(filePath);
			file << std::setw(4) << j << std::endl;
		}

		// Function to load system configurations from a JSON file
		void loadConfig(const std::string& filePath) {
			try {
				std::ifstream file(filePath);
				if (!file.is_open()) {
					std::cerr << "Failed to open file for restore: " << filePath << std::endl;
					return;
				}
				nlohmann::json j;
				file >> j;

				system_config_.app_name = j["app_name"];
				system_config_.version = j["version"];
				system_config_.yes_path = SmUtil::Utf8ToMultiByte(j["yes_path"]);
				system_config_.ab_yes_path = SmUtil::Utf8ToMultiByte(j["ab_yes_path"]);
			}
			catch (const nlohmann::json::exception& e) {
				std::cerr << "Error loading JSON: " << e.what() << std::endl;
			}
		}
	private:
		SystemConfig system_config_;
	};
}

