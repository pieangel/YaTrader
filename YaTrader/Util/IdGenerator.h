#pragma once
//#include "../Log/MyLogger.h"
namespace DarkHorse {
class IdGenerator
{
public:
	// Static Members
	static int _id;
	static int get_id() { 
		//LOGINFO(CMyLogger::getInstance(), "get_id() id = %d", _id);
		return _id++; 
	}
};
}

