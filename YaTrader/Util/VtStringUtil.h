#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
#include <ctime>
#include <windows.h>
namespace DarkHorse {
	class VtStringUtil
	{
	public:
		VtStringUtil();
		~VtStringUtil();
		static inline void ltrim(std::string& s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
				return !std::isspace(ch);
			}));
		}

		// trim from end (in place)
		static inline void rtrim(std::string& s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
				return !std::isspace(ch);
			}).base(), s.end());
		}

		// trim from both ends (in place)
		static inline void trim(std::string& s) {
			ltrim(s);
			rtrim(s);
		}
		static std::string format_with_thousand_separator(double value, int precision);
		static std::string get_format_value(const std::string& type, const double& value, const bool thousand_separator = false);
		static std::string get_format_value(const double& value, const int decimal, const bool thousand_separator = false);
		static std::pair<std::string, std::string> GetCurrentDateTime();
		static std::string getTimeStr();
		static std::string getCurentDate();
		static bool endsWith(const std::string& s, const std::string& suffix);
		static std::vector<std::string> split(const std::string& s, const std::string& delimiter, const bool& removeEmptyEntries = false);
		static std::string PadLeft(double input, char padding, int len, int decimal);
		static std::string PadRight(double input, char padding, int len, int decimal);
		static std::string PadLeft(int input, char padding, int len);
		static std::string PadRight(int input, char padding, int len);
		static std::string PadLeft(std::string input, char padding, int len);
		static std::string PadRight(std::string input, char padding, int len);
		static std::time_t GetUTCTimestamp(std::string datetime_string);
		static std::string GetLocalTime(std::string utc_time_format);
		static std::string GetLocalTime(time_t utc_time_t);
		static std::string GetLocalTimeByDatetimeString(std::string date_time);
		static std::time_t getEpochTime(const std::wstring& dateTime);
		static std::time_t GetEpochTime(const std::string& dateTime);
		static long long GetCurrentNanoseconds();
		static std::time_t GetUTCTimestampByDate();

		static std::string get_application_path() {
			std::string cur_path;
			char ownPth[MAX_PATH] = { 0 };
			const HMODULE hModule = GetModuleHandle(NULL);
			if (hModule != NULL) {
				// Use GetModuleFileName() with module handle to get the path
				GetModuleFileName(hModule, ownPth, (sizeof(ownPth)));
				cur_path = ownPth;
			}
			int pos = cur_path.rfind('\\');
			std::string app_path = cur_path.substr(0, pos);

			return app_path;
		}
		static std::string get_application_name() {
			std::string cur_path;
			char ownPth[MAX_PATH] = { 0 };
			const HMODULE hModule = GetModuleHandle(NULL);
			if (hModule != NULL)
			{
				// Use GetModuleFileName() with module handle to get the path
				GetModuleFileName(hModule, ownPth, (sizeof(ownPth)));
				cur_path = ownPth;
			}
			int pos = cur_path.rfind('\\');
			std::string app_name = cur_path.substr(pos + 1, cur_path.length() - pos);
			pos = app_name.rfind('.');
			app_name = app_name.substr(0, pos);
			return app_name;
		}

		static std::string get_str_between_two_str(const std::string& s,
			const std::string& start_delim,
			const std::string& stop_delim)
		{
			unsigned first_delim_pos = s.find(start_delim);
			unsigned end_pos_of_first_delim = first_delim_pos + start_delim.length();
			unsigned last_delim_pos = s.find(stop_delim);

			return s.substr(end_pos_of_first_delim,
				last_delim_pos - end_pos_of_first_delim);
		}
	};

}