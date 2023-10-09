#pragma once
#include "../ViewModel/VmHoga.h"
#include "../Util/IdGenerator.h"
#include <functional>
#include <memory>
namespace DarkHorse {
	struct SmHoga;
class HogaControl
{
public:
	HogaControl();
	~HogaControl();
	void update_hoga(std::shared_ptr<SmHoga> hoga);
	const VmHoga& get_hoga()
	{
		return hoga_;
	}
	const int get_id()
	{
		return id_;
	}
	void set_symbol_id(const int symbol_id) {
		symbol_id_ = symbol_id;
	}
	void set_event_handler(std::function<void()> event_handler) {
		event_handler_ = event_handler;
	}
private:
	int symbol_id_{ 0 };
	void subscribe_hoga_control();
	VmHoga hoga_;
	int id_;
	std::function<void()> event_handler_;
};
}

