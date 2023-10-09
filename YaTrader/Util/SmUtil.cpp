#include "stdafx.h"
#include "SmUtil.h"
#include <stdarg.h> 
#include <vector>
#include "DHTime.h"

#include "iprtrmib.h"
#include "tlhelp32.h"
#include "iphlpapi.h"
#include "../Log/MyLogger.h"
#pragma comment(lib, "Iphlpapi.lib")


using namespace DarkHorse;


void SmUtil::insert_decimal(std::string& value, const int decimal)
{
	try {
		//CString msg;
		//msg.Format("value = %s\n", value.c_str());
		//TRACE(msg);
		if (decimal <= 0) return;
		if (value.length() == static_cast<size_t>(decimal))
			value.insert(0, 1, '0');
		else if (value.length() < static_cast<size_t>(decimal))
			value.insert(0, 2, '0');
		value.insert(value.length() - decimal, 1, '.');
	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

std::tuple<int, int, int> SmUtil::GetLocalTime()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	return std::make_tuple(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

std::vector<int> SmUtil::GetLocalDate()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	std::vector<int> datevec;
	datevec.push_back(timeinfo.tm_year - 1900);
	datevec.push_back(timeinfo.tm_mon + 1);
	datevec.push_back(timeinfo.tm_mday);
	return datevec;
}

std::vector<int> SmUtil::GetDateTime(std::string datetime_string)
{
	int year = std::stoi(datetime_string.substr(0, 4));
	int month = std::stoi(datetime_string.substr(4, 2));
	int day = std::stoi(datetime_string.substr(6, 2));
	int hour = 0;
	int min = 0;
	int sec = 0;
	if (datetime_string.length() > 8) {
		hour = std::stoi(datetime_string.substr(8, 2));
		min = std::stoi(datetime_string.substr(10, 2));
		sec = std::stoi(datetime_string.substr(12, 2));
	}
	
	std::vector<int> result;
	result.push_back(year);
	result.push_back(month);
	result.push_back(day);
	result.push_back(hour);
	result.push_back(min);
	result.push_back(sec);
	return result;
}

std::vector<int> SmUtil::GetTime(std::string time_string)
{
	int hour = 0;
	int min = 0;
	int sec = 0;
	if (time_string.length() > 8) {
		hour = std::stoi(time_string.substr(8, 2));
		min = std::stoi(time_string.substr(10, 2));
		sec = std::stoi(time_string.substr(12, 2));
	}

	std::vector<int> result;
	result.push_back(hour);
	result.push_back(min);
	result.push_back(sec);
	return result;
}

std::vector<int> SmUtil::GetLocalDateTime()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	std::vector<int> date_time;
	date_time.push_back(timeinfo.tm_year + 1900);
	date_time.push_back(timeinfo.tm_mon + 1);
	date_time.push_back(timeinfo.tm_mday);
	date_time.push_back(timeinfo.tm_hour);
	date_time.push_back(timeinfo.tm_min);
	date_time.push_back(timeinfo.tm_sec);

	return date_time;
}

std::vector<int> SmUtil::GetUtcDateTime()
{
	time_t now = time(0);
	tm gmtm;
	gmtime_safe(&gmtm, &now);

	std::vector<int> date_time;
	date_time.push_back(gmtm.tm_year + 1900);
	date_time.push_back(gmtm.tm_mon + 1);
	date_time.push_back(gmtm.tm_mday);
	date_time.push_back(gmtm.tm_hour);
	date_time.push_back(gmtm.tm_min);
	date_time.push_back(gmtm.tm_sec);

	return date_time;
}


std::string Format(const char* fmt, ...)
{
	char textString[MAX_BUFFER * 5] = { '\0' };

	// -- Empty the buffer properly to ensure no leaks.
	memset(textString, '\0', sizeof(textString));

	va_list args;
	va_start(args, fmt);
	vsnprintf(textString, MAX_BUFFER * 5, fmt, args);
	va_end(args);
	std::string retStr = textString;
	return retStr;
}

std::string SmUtil::GetUTCDateTimeString()
{
	time_t now = time(0);
	tm gmtm;
	gmtime_safe(&gmtm, &now);

	std::vector<int> date_time;
	date_time.push_back(gmtm.tm_year + 1900);
	date_time.push_back(gmtm.tm_mon + 1);
	date_time.push_back(gmtm.tm_mday);
	date_time.push_back(gmtm.tm_hour);
	date_time.push_back(gmtm.tm_min);
	date_time.push_back(gmtm.tm_sec);

	std::string result = Format("%04d-%02d-%02dT%02d:%02d:%02dZ", date_time[0], date_time[1], date_time[2], date_time[3], date_time[4], date_time[5]);

	return result;
}

std::string SmUtil::GetUTCDateTimeStringForNowMin()
{
	time_t now = time(0);
	tm gmtm;
	gmtime_safe(&gmtm, &now);

	std::vector<int> date_time;
	date_time.push_back(gmtm.tm_year + 1900);
	date_time.push_back(gmtm.tm_mon + 1);
	date_time.push_back(gmtm.tm_mday);
	date_time.push_back(gmtm.tm_hour);
	date_time.push_back(gmtm.tm_min);
	date_time.push_back(0);

	std::string result = Format("%04d-%02d-%02dT%02d:%02d:%02dZ", date_time[0], date_time[1], date_time[2], date_time[3], date_time[4], date_time[5]);

	return result;
}

std::string SmUtil::GetUTCDateTimeStringForPreMin(int previousMinLen)
{
	time_t now = time(0);
	tm gmtm;
	gmtime_safe(&gmtm, &now);

	std::vector<int> date_time;
	date_time.push_back(gmtm.tm_year + 1900);
	date_time.push_back(gmtm.tm_mon + 1);
	date_time.push_back(gmtm.tm_mday);
	int hour = gmtm.tm_hour;
	int min = gmtm.tm_min - previousMinLen;
	if (min < 0) {
		hour--;
		min = 60 - min;
	}
	else if (min > 60) {
		hour++;
		min = min - 60;
	}
	date_time.push_back(hour);
	date_time.push_back(gmtm.tm_min - previousMinLen);
	date_time.push_back(0);

	std::string result = Format("%04d-%02d-%02dT%02d:%02d:%02dZ", date_time[0], date_time[1], date_time[2], date_time[3], date_time[4], date_time[5]);

	return result;
}

double SmUtil::GetDifTimeBySeconds(std::string newTime, std::string oldTime)
{
	struct tm new_time;
	struct tm old_time;
	double seconds;

	std::vector<int> newVec = GetDateTime(newTime);
	std::vector<int> oldVec = GetDateTime(oldTime);

	new_time.tm_year = newVec[0] - 1900;
	new_time.tm_mon = newVec[1] - 1;    //months since January - [0,11]
	new_time.tm_mday = newVec[2];          //day of the month - [1,31] 
	new_time.tm_hour = newVec[3];         //hours since midnight - [0,23]
	new_time.tm_min = newVec[4];          //minutes after the hour - [0,59]
	new_time.tm_sec = newVec[5];          //seconds after the minute - [0,59]

	old_time.tm_year = oldVec[0] - 1900;
	old_time.tm_mon = oldVec[1] - 1;    //months since January - [0,11]
	old_time.tm_mday = oldVec[2];          //day of the month - [1,31] 
	old_time.tm_hour = oldVec[3];         //hours since midnight - [0,23]
	old_time.tm_min = oldVec[4];          //minutes after the hour - [0,59]
	old_time.tm_sec = oldVec[5];          //seconds after the minute - [0,59]

	seconds = difftime(mktime(&new_time), mktime(&old_time));

	return seconds;
}

double SmUtil::GetDifTimeForNow(std::string srcTime)
{
	time_t now;
	struct tm new_time;
	double seconds;

	time(&now);  /* get current time; same as: now = time(NULL)  */

	std::vector<int> newVec = GetDateTime(srcTime);

	new_time.tm_year = newVec[0] - 1900;
	new_time.tm_mon = newVec[1] - 1;    //months since January - [0,11]
	new_time.tm_mday = newVec[2];          //day of the month - [1,31] 
	new_time.tm_hour = newVec[3];         //hours since midnight - [0,23]
	new_time.tm_min = newVec[4];          //minutes after the hour - [0,59]
	new_time.tm_sec = newVec[5];          //seconds after the minute - [0,59]


	seconds = difftime(mktime(&new_time), now);

	return seconds;
}


std::string SmUtil::Format(const char* fmt, ...)
{
	char textString[MAX_BUFFER * 5] = { '\0' };

	// -- Empty the buffer properly to ensure no leaks.
	memset(textString, '\0', sizeof(textString));

	va_list args;
	va_start(args, fmt);
	vsnprintf(textString, MAX_BUFFER * 5, fmt, args);
	va_end(args);
	std::string retStr = textString;
	return retStr;
}

std::vector<int> DarkHorse::SmUtil::IntToDate(const int& date)
{
	//int date = 20201006;

	int year = date / 10000;
	int mon = date - year * 10000;
	int month = mon / 100;
	int day = mon - month * 100;

	std::vector<int> result;
	result.push_back(year);
	result.push_back(month);
	result.push_back(day);

	return result;
}

std::vector<int> DarkHorse::SmUtil::IntToTime(const int& time)
{
	

	//int time = 120809;

	int hour = time / 10000;
	int minu = time - hour * 10000;
	int minute = minu / 100;
	int sec = minu - minute * 100;

	std::vector<int> result;
	result.push_back(hour);
	result.push_back(minute);
	result.push_back(sec);

	return result;
}

std::string DarkHorse::SmUtil::GetMacAddress()
{
	ULONG buffer_length = 0;
	/* ��Ʈ��ũ ����� ������ ��� ���� �޸��� ũ�⸦ ��´�.
	�Ʈ�� ������ IP_ADAPTER_INFO ����ü�� ���� ������
	��Ʈ��ũ ����� ����*sizeof(IP_ADAPTER_INFO) ��ŭ�� �޸𸮰� �ʿ��մϴ�.
	������, IP_ADAPTER_INFO ����ü�� time_t ������������ ����� ������ 2���� �־
	ũ�� ��꿡 �����ؾ� �մϴ�. �ֳ��ϸ� time_t�� ���������� �����Ϸ�������
	ũ�Ⱑ 4����Ʈ������ �ֱٿ��� 8����Ʈ�� �þ�� ��꿡 ������ ������ �ֽ��ϴ�.
	time_t�� 4����Ʈ�� ó���Ǵ� �����Ϸ������� sizeof(IP_ADAPTER_INFO)�� ũ�Ⱑ 640����Ʈ�̰�
	time_t�� 8����Ʈ�� ó���Ǵ� �����Ϸ������� sizeof(IP_ADAPTER_INFO)�� ũ�Ⱑ 648����Ʈ�Դϴ�.
	���� ��� ������ ���ϱ� ���ؼ� buffer_length / sizeof(IP_ADAPTER_INFO) ��� ������
	����ϸ� ������ �ʴ� ����� ���ü� �ִٴ� ���Դϴ�. �̰��� buffer_length�� 640�� ��� ������
	�����Ǿ� �ִµ�, sizeof(IP_ADAPTER_INFO) ���� 640�ϼ��� �ְ� 648�ϼ��� �ֱ� �����Դϴ�.
	�׷��� ����� ������ ���Ϸ��� �Ʒ��� ���� ������ �����ؾ� �մϴ�.
	int check_size = sizeof(IP_ADAPTER_INFO);
	if(sizeof(time_t) == 8) check_size -= 8;
	int count = buffer_length / check_size;
	*/
	std::vector<std::string> mac_list;

	if (::GetAdaptersInfo(NULL, &buffer_length) == ERROR_BUFFER_OVERFLOW) {
		CString str, adapter_info_string;
		char* p_adapter = new char[buffer_length];
		IP_ADAPTER_INFO* p_pos;

		if (GetAdaptersInfo((IP_ADAPTER_INFO*)p_adapter, &buffer_length) == ERROR_SUCCESS) {
			p_pos = (IP_ADAPTER_INFO*)p_adapter;
			while (p_pos != NULL) {
				//adapter_info_string.Format("%s(%s) : ", p_pos->Description, p_pos->IpAddressList.IpAddress.String);
				// MAC �ּҸ� 16���� ���·� ����Ѵ�. ��) AA-BB-CC-DD-EE-FF
				for (unsigned int i = 0; i < p_pos->AddressLength; i++) {
					str.Format("%02X", (unsigned int)(p_pos->Address[i] & 0x000000FF));
					if (i < p_pos->AddressLength - 1) str += "-";
					adapter_info_string += str;
				}
				//m_mac_list.InsertString(-1, adapter_info_string);
				mac_list.push_back(static_cast<const char*>(adapter_info_string));
				p_pos = p_pos->Next; // ���� ��͸� ���� ������ �̵��Ѵ�.
			}
		}

		delete[] p_adapter;
	}
	std::string first_mac("00:00:00:00:00:00");
	if (mac_list.size() > 0) first_mac = mac_list[0];

	return first_mac;
}




