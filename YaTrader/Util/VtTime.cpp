#include "stdafx.h"
#include "VtTime.h"


VtDate VtTimeUtil::GetDate(int date)
{
	int yearMon = date / 100;
	int day = date % (yearMon * 100);
	int year = yearMon / 100;
	int mon = yearMon % (year * 100);

	VtDate result;
	result.year = year;
	result.month = mon;
	result.day = day;

	return result;
}

int VtTimeUtil::GetDate(VtDate date)
{
	int timeNo = date.year * 10000;
	timeNo += date.month * 100;
	timeNo += date.day;

	return timeNo;
}

VtTime VtTimeUtil::GetLocalTime()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	VtTime curTime;
	curTime.hour = timeinfo.tm_hour;
	curTime.min = timeinfo.tm_min;
	curTime.sec = timeinfo.tm_sec;

	return curTime;
}

VtDate VtTimeUtil::GetLocalDate()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	VtDate curDate;
	curDate.year = (timeinfo.tm_year + 1900);
	curDate.month = (timeinfo.tm_mon + 1);
	curDate.day = timeinfo.tm_mday;

	return curDate;
}

VtTime VtTimeUtil::GetTime(int time)
{
	VtTime result;

	if (time == 0) {
		return result;
	}
	if (time < 99) {
		result.hour = 0;
		result.min = 0;
		result.sec = time;
		result.mil = 0;

		return result;
	}
	else if (time < 999) {
		int min = time / 100;
		int sec = time % 100;

		result.hour = 0;
		result.min = min;
		result.sec = sec;
		result.mil = 0;

		return result;
	}
	else if (time < 9999) {
		int min = time / 100;
		int sec = time % 100;
		int hour = 0;

		result.hour = hour;
		result.min = min;
		result.sec = sec;
		result.mil = 0;

		return result;
	}
	else {
		int hourMin = time / 100;
		int sec = time % (hourMin * 100);
		int hour = hourMin / 100;
		int min = hourMin % (hour * 100);

		result.hour = hour;
		result.min = min;
		result.sec = sec;
		result.mil = 0;

		return result;
	}
}

int VtTimeUtil::GetTime(VtTime time)
{
	int timeNo = time.hour * 10000;
	timeNo += time.min * 100;
	timeNo += time.sec;

	return timeNo;
}
