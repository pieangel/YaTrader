#pragma once
//#include "../Global/TemplateSingleton.h"
#include <string>
#include <set>
#include <queue>
#include <map>
namespace DarkHorse {
	class SmSymbolReader
	{
	public:
		SmSymbolReader();
		~SmSymbolReader();

	public:
		std::string GetWorkingDir();
		void ReadSymbolFromFile(int index, std::string fullPath);
		std::set<std::string> DomesticSymbolMasterFileSet;
		std::set<std::string> ProductSet;
		void ReadAbroadMarketFile();
		void ReadAbroadProductFile();
		void ReadAbroadSymbolFile();
	private:
		void InitProductSet();
		void ReadAbroadMarketFile(const std::string& fullPath) const;
		void ReadAbroadProductFile(const std::string& fullPath) const;
		void ReadAbroadSymbolFile(const std::string& fullPath) const ;

		void ReadKospiFutureFile(const std::string& fullPath) const;
		void ReadKospiOptionFile(const std::string& fullPath) const;
		void ReadKospiWeeklyOptionFile(const std::string& fullPath) const;
		void ReadKosdaqFutureFile(const std::string& fullPath) const;
		void ReadMiniKospiFutureFile(const std::string& fullPath) const;
		void ReadCommodityFutureFile(const std::string& fullPath) const;
		void ReadKospiFutureInfo(const std::string& fullPath) const;
		void ReadKospiOptionInfo(const std::string& fullPath) const;
		void ReadUsDollarFutureInfo(const std::string& fullPath) const;
		std::set<std::string> _DomesticList;
	};
}

