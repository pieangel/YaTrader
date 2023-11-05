#pragma once

#include <ctime>
#include <string>
#include <time.h>

struct VtDate
{
	int year = 0;
	int month = 0;
	int day = 0;
};

struct VtTime
{
	int hour = 0;
	int min = 0;
	int sec = 0;
	int mil = 0;
};


class VtTimeUtil
{
public:

	VtTimeUtil() {}

	~VtTimeUtil() {}

	static struct tm Now()
	{
		time_t timer;
		struct tm t;

		timer = time(nullptr); 
		localtime_s(&t, &timer);

		return t;
	}

	static std::string GetDate(char* src)
	{
		std::string date;
		char buff[16] = { 0 };
		memset(buff, 0x00, sizeof(buff));
		memcpy(buff, src, 4);
		date.append(buff);
		date.append("³â");
		memset(buff, 0x00, sizeof(buff));
		memcpy(buff, src + 4, 2);
		date.append(buff);
		date.append("¿ù");
		memset(buff, 0x00, sizeof(buff));
		memcpy(buff, src + 6, 2);
		date.append(buff);
		date.append("ÀÏ");
		return date;
	}

	static VtTime GetTime(int time);
	static VtDate GetDate(int date);
	static VtTime GetLocalTime();
	static VtDate GetLocalDate();
	static int GetDate(VtDate date);
	static int GetTime(VtTime time);
};

