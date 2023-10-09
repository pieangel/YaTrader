#pragma once
#include <string>
namespace DarkHorse {
	class SmOutSignalDef
	{
	public:
		SmOutSignalDef(const int id) : id_(id) {};
		~SmOutSignalDef() {};
		std::string name;
		std::string desc;
		int id() const { return id_; }
	private:
		int id_;
	};

}

