#pragma once
#include <ctime>

namespace DarkHorse {
#if _WIN32
	inline int localtime_safe(struct tm* _tm, const time_t* _time) { return localtime_s(_tm, _time); }

	inline int gmtime_safe(struct tm* _tm, const time_t* _time) { return gmtime_s(_tm, _time); }
#else
	inline int localtime_safe(struct tm* _tm, const time_t* _time) {
		return localtime_r(_time, _tm) ? 0 : -1;
	}

	inline int gmtime_safe(struct tm* _tm, const time_t* _time) {
		return gmtime_r(_time, _tm) ? 0 : -1;
	}
#endif

}