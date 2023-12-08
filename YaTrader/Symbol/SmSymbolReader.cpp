#include "stdafx.h"
#include "SmSymbolReader.h"
#include <filesystem>
#include <sstream>
#include <string>
#include <fstream>
#include "SmMarket.h"
#include "SmProduct.h"
#include "SmSymbol.h"
#include "SmSymbolManager.h"
#include "../Config/SmConfigManager.h"
#include "../Xml/pugixml.hpp"
#include "../Global/SmTotalManager.h"
#include "SmSymbolManager.h"
#include "../Log/MyLogger.h"
#include "../Util/VtStringUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace DarkHorse;

SmSymbolReader::SmSymbolReader()
{
}

void SmSymbolReader::InitProductSet()
{
	ProductSet.insert("6A");
	ProductSet.insert("6B");
	ProductSet.insert("6E");
	ProductSet.insert("6J");
	ProductSet.insert("E7");
	ProductSet.insert("J7");
	ProductSet.insert("M6A");
	ProductSet.insert("M6B");
	ProductSet.insert("M6E");
	ProductSet.insert("ES");
	ProductSet.insert("NIY");
	ProductSet.insert("NKD");
	ProductSet.insert("NQ");
	ProductSet.insert("OES");
	ProductSet.insert("ONQ");
	ProductSet.insert("SP");
	ProductSet.insert("GE");
	ProductSet.insert("CL");
	ProductSet.insert("HO");
	ProductSet.insert("NG");
	ProductSet.insert("QG");
	ProductSet.insert("QM");
	ProductSet.insert("RB");
	ProductSet.insert("GC");
	ProductSet.insert("MGC");
	ProductSet.insert("OGC");
	ProductSet.insert("OSI");
	ProductSet.insert("QO");
	ProductSet.insert("SI");
	ProductSet.insert("YM");
	ProductSet.insert("ZF");
	ProductSet.insert("ZN");
	ProductSet.insert("ZQ");
	ProductSet.insert("OZC");
	ProductSet.insert("OZS");
	ProductSet.insert("OZW");
	ProductSet.insert("ZC");
	ProductSet.insert("ZL");
	ProductSet.insert("ZM");
	ProductSet.insert("ZS");
	ProductSet.insert("ZW");
	ProductSet.insert("CN");
	ProductSet.insert("NK");
	ProductSet.insert("TW");
	ProductSet.insert("BRN");
	ProductSet.insert("HSI");
	ProductSet.insert("VX");
	ProductSet.insert("GF");
	ProductSet.insert("HF");
	ProductSet.insert("HG");
	ProductSet.insert("LE");
}



SmSymbolReader::~SmSymbolReader()
{
}

std::string SmSymbolReader::GetWorkingDir()
{
	return SmConfigManager::GetApplicationPath();
}


void SmSymbolReader::ReadSymbolFromFile(int index, std::string fullPath)
{
	switch (index)
	{
	case 0:
		ReadKospiFutureFile(fullPath);
		break;
	case 1:
		ReadKospiOptionFile(fullPath);
		break;
	case 2:
		ReadKospiWeeklyOptionFile(fullPath);
		break;
	case 3:
		ReadKosdaqFutureFile(fullPath);
		break;
	case 4:
		ReadMiniKospiFutureFile(fullPath);
		break;
	case 5:
		ReadCommodityFutureFile(fullPath);
		break;
	case 6:
		ReadKospiFutureInfo(fullPath);
		break;
	case 7:
		ReadKospiOptionInfo(fullPath);
		break;
	case 8:
		ReadUsDollarFutureInfo(fullPath);
		break;
	case 9:
		ReadAbroadMarketFile(fullPath);
		break;
	case 10:
		ReadAbroadProductFile(fullPath);
		break;
	case 11:
		ReadAbroadSymbolFile(fullPath);
		break;
	default:
		break;
	}
}

void SmSymbolReader::ReadAbroadMarketFile()
{
	try {
		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(_T("table"));
		appPath.append(_T("\\"));
		std::string file_name = "MRKT.cod";
		//TRACE(file_name.c_str());
		std::string file_path = appPath + file_name;
		ReadAbroadMarketFile(file_path);
	}
	catch (std::exception& e)
	{
		const std::string error = e.what();

		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}

void SmSymbolReader::ReadAbroadMarketFile(const std::string& fullPath) const
{
	try {
		//CString msg;
		std::ifstream infile(fullPath);
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream iss(line);
			std::string market_name = line.substr(0, 20);
			std::string exchange_name = line.substr(20, 5);
			std::string product_code = line.substr(25, 3);
			std::string product_name_en = line.substr(28, 50);
			std::string product_name_kr = line.substr(78, 50);

			VtStringUtil::trim(market_name);
			VtStringUtil::trim(exchange_name);
			VtStringUtil::trim(product_code);
			VtStringUtil::trim(product_name_en);
			VtStringUtil::trim(product_name_kr);

			std::shared_ptr<SmMarket> market = mainApp.SymMgr()->AddMarket(market_name);
			mainApp.SymMgr()->add_ab_market(market_name);
			std::shared_ptr<SmProduct> product = market->AddProduct(product_code);
			product->MarketName(market_name);
			product->ExchangeName(exchange_name);
			product->ProductNameEn(product_name_en);
			product->ProductNameKr(product_name_kr);
		}
	}
	catch (std::exception& e)
	{
		const std::string error = e.what();

		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}

void SmSymbolReader::ReadAbroadProductFile()
{
	try {
		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(_T("mst"));
		appPath.append(_T("\\"));
		std::string file_name = "PMCODE.cod";
		//TRACE(file_name.c_str());
		std::string file_path = appPath + file_name;
		ReadAbroadProductFile(file_path);
	}
	catch (std::exception& e)
	{
		const std::string error = e.what();

		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}



void SmSymbolReader::ReadAbroadProductFile(const std::string& fullPath) const
{
	try {
		//CString msg;
		std::ifstream infile(fullPath);
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream iss(line);
			std::string market = line.substr(0, 20);
			std::string exIndexCode = line.substr(20, 4);
			std::string exChangeCode = line.substr(24, 5);
			std::string pmCode = line.substr(29, 5);
			std::string pmGubun = line.substr(34, 3);
			std::string pmGubun2 = line.substr(84, 30);
			//msg.Format(_T("market = %s, pmCode = %s, ename = %s\n"), market.c_str(), pmCode.c_str(), pmGubun2.c_str());
			//TRACE(msg);

			VtStringUtil::trim(market);
			VtStringUtil::trim(exIndexCode);
			VtStringUtil::trim(exChangeCode);
			VtStringUtil::trim(pmCode);
			VtStringUtil::trim(pmGubun);

		}
	}
	catch (std::exception& e)
	{
		const std::string error = e.what();

		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}

void SmSymbolReader::ReadAbroadSymbolFile()
{
	try {
		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(_T("mst"));
		appPath.append(_T("\\"));
		std::string file_name = "JMCODE.cod";
		//TRACE(file_name.c_str());
		std::string file_path = appPath + file_name;
		ReadAbroadSymbolFile(file_path);
	}
	catch (std::exception& e)
	{
		const std::string error = e.what();

		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}

void SmSymbolReader::ReadAbroadSymbolFile(const std::string& fullPath) const
{
	try {
		std::ifstream infile(fullPath);
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream iss(line);

			std::string symbol_code = line.substr(0, 32);
			/* �����ڵ�                             */

			std::string exchange_name = line.substr(32, 5);
			/* �ŷ���                               */

			std::string IndexCode = line.substr(37, 4);
			/* ǰ�� �ε����ڵ�                      */

			std::string product_code = line.substr(41, 5);
			/* ǰ���ڵ�                             */

			std::string ExchNo = line.substr(46, 5);
			/* �ŷ��� ��ȣ                          */

			std::string Pdesz = line.substr(51, 5);
			/* �Ҽ��� ����                          */

			std::string Rdesz = line.substr(56, 5);
			/* �Ҽ��� ����2                         */

			std::string CtrtSize = line.substr(61, 20);
			/* ���ũ��                             */

			std::string TickSize = line.substr(81, 20);
			/* Tick Size                            */

			std::string TickValue = line.substr(101, 20);
			/* Tick Value                           */

			std::string MltiPler = line.substr(121, 20);
			/* �ŷ��¼�                             */

			std::string DispDigit = line.substr(141, 10);
			/* ����                                 */

			std::string symbol_name_en = line.substr(151, 32);
			/* Full �����                          */

			std::string symbol_name_kr = line.substr(183, 32);
			/* Full ������ѱ�                      */

			std::string NearSeq = line.substr(215, 1);
			/* �ֱٿ���, �ֿ�����ǥ��               */

			std::string StatTp = line.substr(216, 1);
			/* �ŷ����ɿ���                         */

			std::string LockDt = line.substr(217, 8);
			/* �ű԰ŷ�������                       */

			std::string TradFrDt = line.substr(225, 8);
			/* ���ʰŷ���                           */

			std::string last_date = line.substr(233, 8);
			/* �����ŷ���                           */

			std::string ExprDt = line.substr(241, 8);
			/* ������, ����������                   */

			std::string RemnCnt = line.substr(249, 4);
			/* �����ϼ�                             */

			std::string HogaMthd = line.substr(253, 30);
			/* ȣ�����                             */

			std::string MinMaxRt = line.substr(283, 6);
			/* ������������                         */

			std::string BaseP = line.substr(289, 20);
			/* ���ذ�                               */

			std::string MaxP = line.substr(309, 20);
			/* ���Ѱ�                               */

			std::string MinP = line.substr(329, 20);
			/* ���Ѱ�                               */

			std::string TrstMgn = line.substr(349, 20);
			/* �ű��ֹ����ű�                       */

			std::string MntMgn = line.substr(369, 20);
			/* �������ű�                           */

			std::string CrcCd = line.substr(389, 3);
			/* ������ȭ�ڵ�                         */

			std::string BaseCrcCd = line.substr(392, 3);
			/* BASE CRC CD                          */

			std::string CounterCrcCd = line.substr(395, 3);
			/* COUNTER CRC CD                       */

			std::string PipCost = line.substr(398, 20);
			/* PIP COST                             */

			std::string BuyInt = line.substr(418, 20);
			/* �ż�����                             */

			std::string SellInt = line.substr(438, 20);
			/* �ŵ�����                             */

			std::string RoundLots = line.substr(458, 6);
			/* LOUND LOTS                           */

			std::string ScaleChiper = line.substr(464, 10);
			/* �����ڸ���                           */

			std::string decimalchiper = line.substr(474, 5);
			/* �Ҽ��� ����(KTB����)                 */

			std::string JnilVolume = line.substr(479, 10);
			/* ���ϰŷ���                           */


			VtStringUtil::trim(symbol_code);
			VtStringUtil::trim(symbol_name_en);
			VtStringUtil::trim(symbol_name_kr);
			VtStringUtil::trim(product_code);
			//msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), Series.c_str(), SeriesNm.c_str(), SeriesNmKor.c_str());
			//TRACE(msg);

			std::shared_ptr<SmProduct> product = mainApp.SymMgr()->FindProduct(product_code);
			if (!product) continue;

			std::shared_ptr<SmSymbol> symbol = product->AddSymbol(std::move(symbol_code));
			if (!symbol) continue;

			//CString msg;
			//msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), symbol_code.c_str(), symbol_name_en.c_str(), symbol_name_kr.c_str());
			//TRACE(msg);

			//product->AddToYearMonth(sym->ShortCode, sym);

			symbol->ProductCode(product->ProductCode());
			symbol->MarketName(product->MarketName());
			symbol->SymbolNameKr(symbol_name_kr);
			symbol->SymbolNameEn(symbol_name_en);
			symbol->symbol_type(SymbolType::Abroad);
			// �Ҽ���
			symbol->decimal(std::stoi(Pdesz));
			// ����
			symbol->seung_su(std::stoi(MltiPler));
			// ��� ũ��
			symbol->CtrtSize(std::stod(CtrtSize));
			// ƽ ��ġ
			symbol->TickValue(std::stod(TickValue));
			// ƽ ũ��
			symbol->TickSize(std::stod(TickSize));
			// ���� ƽ ũ�⸦ ����Ͽ� �־� �ش�.
			//symbol->intTickSize = (int)(sym->TickSize * std::pow(10, sym->Decimal));
			symbol->ExpireDate(last_date);
			symbol->PreDayVolume(std::stoi(JnilVolume));
		}
	}
	catch (std::exception& e)
	{
		const std::string error = e.what();

		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}

// ������ �տ� 3�ڸ��� ��ǰ�� ��Ÿ����. �� ���� ���ڸ��� �⵵�� ��Ÿ���� �� ���� ���ڸ��� ���� ��Ÿ����.
// ������ ���ڸ��� ��簡�� �ǹ��Ѵ�.
void SmSymbolReader::ReadKospiFutureFile(const std::string& fullPath) const
{

}

void SmSymbolReader::ReadKospiOptionFile(const std::string& fullPath) const
{

}

void SmSymbolReader::ReadKospiWeeklyOptionFile(const std::string& fullPath) const
{

}

void SmSymbolReader::ReadKosdaqFutureFile(const std::string& fullPath) const
{

}

void SmSymbolReader::ReadMiniKospiFutureFile(const std::string& fullPath) const
{

}

void SmSymbolReader::ReadCommodityFutureFile(const std::string& fullPath) const
{

}

void SmSymbolReader::ReadKospiFutureInfo(const std::string& fullPath) const
{


}

void SmSymbolReader::ReadKospiOptionInfo(const std::string& fullPath) const
{
	
}

void SmSymbolReader::ReadUsDollarFutureInfo(const std::string& fullPath) const
{
	
}

void SmSymbolReader::read_abroad_symbol_file(const std::string& fullPath)
{
	try {
		std::ifstream infile(fullPath);
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream iss(line);

			std::string recode_gubun = line.substr(0, 2); // ���ڵ� ����

			std::string symbol_code = line.substr(2, 32); // �����ڵ�
			//LOGINFO(CMyLogger::getInstance(), "symbol_code : %s", symbol_code.c_str());

			std::string symbol_name_kr = line.substr(34, 50); // �����
			std::string symbol_name_en = line.substr(34, 50); // �����

			std::string exchange_name = line.substr(84, 10); // �ŷ��� �ڵ� 

			std::string product_type = line.substr(94, 3); // ��ǰ ����

			std::string product_code = line.substr(97, 15); // ��ǰ �ڵ�

			VtStringUtil::trim(symbol_code);
			VtStringUtil::trim(symbol_name_en);
			VtStringUtil::trim(symbol_name_kr);
			VtStringUtil::trim(product_code);
			//msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), Series.c_str(), SeriesNm.c_str(), SeriesNmKor.c_str());
			//TRACE(msg);

			std::shared_ptr<SmProduct> product = mainApp.SymMgr()->FindProduct(product_code);
			if (!product) continue;

			std::shared_ptr<SmSymbol> symbol = product->AddSymbol(std::move(symbol_code));
			if (!symbol) continue;

			std::string future_option = line.substr(112, 1); // �����ɼ� ����

			std::string strike = line.substr(113, 20); // ��簡��

			std::string tick_size = line.substr(123, 20); // ƽ ũ��

			std::string tick_value = line.substr(143, 20); // ƽ ��ġ

			std::string decimal = line.substr(163, 3); // �Ҽ��� ũ��

			std::string MltiPler = line.substr(166, 3); // ǥ�� ����

			std::string currency = line.substr(169, 3); // ��ȭ �ڵ�

			std::string recent = line.substr(172, 3); // �ٿ��� ����

			std::string recent_code = line.substr(175, 32); // ���� ���� �ڵ�

			std::string NearSeq = line.substr(207, 1); // ���ӿ��� ���� 

			std::string StatTp = line.substr(208, 1); // �ü���ȸ����

			std::string last_date = line.substr(209, 8); // �����ŷ���

			std::string expire_date = line.substr(217, 8); // ������

			std::string sb_date = line.substr(225, 8); // sb date

			std::string contract_size = line.substr(233, 20); // ��� ũ��

			std::string crc_cvrt_dt = line.substr(253, 5); // crc_cvrt_dt

			std::string opnn_time = line.substr(258, 6); // opnn_time

			std::string mked_time = line.substr(264, 6); // mked_time

			std::string std_price = line.substr(270, 20); // std_price 

			std::string hstl_price = line.substr(290, 20); // hstl_price

			std::string lstl_price = line.substr(310, 20); // lstl_price

			std::string mrk_m = line.substr(320, 4); // mrk_m

			std::string rmnd_dds = line.substr(324, 6); // rmnd_dds

			std::string call_put = line.substr(330, 1); // call_put

			std::string atm_type = line.substr(331, 1); // atm_type

			std::string us_option = line.substr(332, 1); // us_option

			std::string us_eft_gubun = line.substr(333, 1); // us_eft_gubun


			

			//product->AddToYearMonth(sym->ShortCode, sym);

			symbol->ProductCode(product->ProductCode());
			symbol->MarketName(product->MarketName());
			symbol->SymbolNameKr(symbol_name_kr);
			symbol->SymbolNameEn(symbol_name_en);
			//LOGINFO(CMyLogger::getInstance(), "symbol_name_kr : %s", symbol_name_kr.c_str());
			symbol->symbol_type(SymbolType::Abroad);
			// �Ҽ���
			symbol->decimal(std::stoi(decimal));
			//LOGINFO(CMyLogger::getInstance(), "decimal : %s", decimal.c_str());
			// ����
			symbol->seung_su(std::stoi(crc_cvrt_dt));
			//LOGINFO(CMyLogger::getInstance(), "crc_cvrt_dt : %s", crc_cvrt_dt.c_str());
			// ��� ũ��
			symbol->CtrtSize(std::stod(contract_size));
			//LOGINFO(CMyLogger::getInstance(), "contract_size : %s", contract_size.c_str());
			// ƽ ��ġ
			symbol->TickValue(std::stod(tick_value));
			//LOGINFO(CMyLogger::getInstance(), "tick_value : %s", tick_value.c_str());
			// ƽ ũ��
			symbol->TickSize(std::stod(tick_size));
			//LOGINFO(CMyLogger::getInstance(), "tick_size : %s", tick_size.c_str());
			// ���� ƽ ũ�⸦ ����Ͽ� �־� �ش�.
			//symbol->intTickSize = (int)(sym->TickSize * std::pow(10, sym->Decimal));
			symbol->ExpireDate(last_date);
			//LOGINFO(CMyLogger::getInstance(), "last_date : %s", last_date.c_str());
			symbol->PreDayVolume(0);
		}
	}
	catch (std::exception& e)
	{
		const std::string error = e.what();

		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}

void SmSymbolReader::read_abroad_symbol_file()
{
	try {
		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(_T("table"));
		appPath.append(_T("\\"));
		std::string file_name = "series_new.tbl";
		//TRACE(file_name.c_str());
		std::string file_path = appPath + file_name;
		read_abroad_symbol_file(file_path);
	}
	catch (std::exception& e)
	{
		const std::string error = e.what();

		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}

