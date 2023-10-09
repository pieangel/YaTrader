#pragma once
#include <locale>
#include <string>

class SmNumPunct : public std::numpunct<char>
{
protected:
	virtual char do_thousands_sep() const { return ','; }
	virtual std::string do_grouping() const { return "\03"; }
};
