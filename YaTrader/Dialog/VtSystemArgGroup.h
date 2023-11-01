#pragma once
#include "VtSystemArg.h"
#include <string>
#include <map>
#include <vector>
#include "../Xml/pugixml.hpp"

class VtSystemArgGroup
{
public:
	VtSystemArgGroup();
	~VtSystemArgGroup();
	std::string Name() const { return _Name; }
	void Name(std::string val) { _Name = val; }
	void AddSystemArg(std::string name, VtSystemArg arg);
	VtSystemArg* GetSystemArg(std::string name);
	std::vector<VtSystemArg>& GetArgVec() {
		return _ArgMap;
	}


	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);

private:
	std::string _Name;
	std::vector<VtSystemArg> _ArgMap;
};

