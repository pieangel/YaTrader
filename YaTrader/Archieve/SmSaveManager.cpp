#include "stdafx.h"
#include "SmSaveManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <filesystem>
#include "../Config/SmConfigManager.h"
#include "../Fund/SmFundManager.h"
#include "../Account/SmAccountManager.h"
//#include "VtOrderDialogManager.h"
#include "../Symbol/SmSymbol.h"
//#include "../Order/VtOrderManagerSelector.h"
//#include "HdWindowManager.h"
#include "../Util/VtStringUtil.h"

#include <exception>
#include <fstream>
//#include "VtSystemGroupManager.h"
//#include "VtStrategyWndManager.h"
//#include "VtOutSystemManager.h"
//#include "../Global/MainBeetle.h"
#include "../Util/cryptor.hpp"
#include "../Login/SmLoginManager.h"
#include "../Log/MyLogger.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Account/SmAccount.h"
//#include "Format/format_string.h"
#include "../Global/SmTotalManager.h"
#include "../Xml/pugixml.hpp"
#include "../Json/json.hpp"
#include "../Dialog/TotalAssetProfitLossDialog.h"
#include "../Dialog/MiniJangoDialog.h"
#include "../resource.h"
#include "../Order/OrderUi/DmAccountOrderWindow.h"
#include "../Order/OrderUi/DmFundOrderWindow.h"
#include "../Order/OrderUi/DmAccountOrderCenterWindow.h"
#include "../Account/SmAccountManager.h"
#include "../Account/SmAccount.h"
#include "../Fund/SmFundManager.h"
#include "../Fund/SmFund.h"
#include "../OutSystem/SmOutSystemManager.h"
#include "../OutSystem/SmOutSystem.h"
#include "../OutSystem/SmUsdSystem.h"
#include "../Util/SmUtil.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"

#include <iostream>
#include <filesystem>
#include <chrono>
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace std;
using namespace pugi;
namespace fs = std::filesystem;
using namespace nlohmann;


template <typename TP>
time_t to_time_t(TP tp) {
	using namespace chrono;
	auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
	return system_clock::to_time_t(sctp);
}


namespace DarkHorse {

	SmSaveManager::SmSaveManager()
	{
	}


	SmSaveManager::~SmSaveManager()
	{
	}

	void SmSaveManager::SaveAccountListToXml()
	{
		try {

			std::string id = mainApp.LoginMgr()->id();
			// 아이디가 없으면 그냥 반환한다.
			if (id.length() == 0)
				return;

			std::string appPath;
			appPath = SmConfigManager::GetApplicationPath();
			appPath.append(_T("\\"));
			appPath.append(id);
			// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.

			appPath.append(_T("\\"));
			appPath.append(VtStringUtil::getTimeStr());
			appPath.append(_T(".xml"));
			if (fs::exists(appPath)) {
				return;
			}

			/// [load xml file]
			// Create empty XML document within memory
			pugi::xml_document doc;
			// Load XML file into memory
			// Remark: to fully read declaration entries you have to specify
			// "pugi::parse_declaration"
			pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
				pugi::parse_default | pugi::parse_declaration);
			if (!result) {
				return;
			}

			pugi::xml_node application = doc.child("application");

			// 	application.remove_child("login_info");
			// 	pugi::xml_node login_info = application.append_child("login_info");
			// 	VtLoginManager::GetInstance()->SaveToXml(login_info);

			application.remove_child("account_list");
			pugi::xml_node account_list = application.append_child("account_list");
			//mainApp.AcntMgr().SaveToXml(account_list);

			doc.save_file(appPath.c_str());

		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	void SmSaveManager::SaveFundListToXml()
	{
		try {

			std::string id = mainApp.LoginMgr()->id();
			// 아이디가 없으면 그냥 반환한다.
			if (id.length() == 0)
				return;

			std::string appPath;
			appPath = SmConfigManager::GetApplicationPath();
			appPath.append(_T("\\"));
			appPath.append(id);
			// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.

			appPath.append(_T("\\"));
			appPath.append(VtStringUtil::getTimeStr());
			appPath.append(_T(".xml"));
			filesystem::path path1(appPath);
			if (fs::exists(appPath)) {
				return;
			}

			/// [load xml file]
			// Create empty XML document within memory
			pugi::xml_document doc;
			// Load XML file into memory
			// Remark: to fully read declaration entries you have to specify
			// "pugi::parse_declaration"
			pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
				pugi::parse_default | pugi::parse_declaration);
			if (!result) {
				return;
			}

			pugi::xml_node application = doc.child("application");

			application.remove_child("fund_list");
			pugi::xml_node fund_list = application.append_child("fund_list");
			//mainApp.FundMgr().SaveToXml(fund_list);

			doc.save_file(appPath.c_str());

		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	void SmSaveManager::WriteSettings()
	{
		try {

			std::string id = mainApp.LoginMgr()->id();
			// 아이디가 없으면 그냥 반환한다.
			if (id.length() == 0)
				return;

			std::string appPath;
			appPath = SmConfigManager::GetApplicationPath();
			appPath.append(_T("\\"));
			appPath.append("user");
			appPath.append(_T("\\"));
			appPath.append(id);
			// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
			if (!fs::exists(appPath)) {
				fs::create_directory(appPath);
			}
			appPath.append(_T("\\"));
			appPath.append(VtStringUtil::getTimeStr());
			appPath.append(_T(".xml"));
			if (!fs::exists(appPath)) {
				std::ofstream ofs(appPath);
				ofs << "this is dark horse config file.\n";
				ofs.close();
				/// [create new xml file]
				// Generate new XML document within memory
				pugi::xml_document doc;
				// Alternatively store as shared pointer if tree shall be used for longer
				// time or multiple client calls:
				// std::shared_ptr<pugi::xml_document> spDoc = std::make_shared<pugi::xml_document>();
				// Generate XML declaration
				auto declarationNode = doc.append_child(pugi::node_declaration);
				declarationNode.append_attribute("version") = "1.0";
				declarationNode.append_attribute("encoding") = "euc-kr";
				declarationNode.append_attribute("standalone") = "yes";
				// A valid XML doc must contain a single root node of any name
				auto root = doc.append_child("application");
				// Save XML tree to file.
				// Remark: second optional param is indent string to be used;
				// default indentation is tab character.
				bool saveSucceeded = doc.save_file(appPath.c_str(), PUGIXML_TEXT("  "));
#ifdef DEBUG
				assert(saveSucceeded);
#endif
			}

			/// [load xml file]
			// Create empty XML document within memory
			pugi::xml_document doc;
			// Load XML file into memory
			// Remark: to fully read declaration entries you have to specify
			// "pugi::parse_declaration"
			pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
				pugi::parse_default | pugi::parse_declaration);
			if (!result)
			{
				std::cout << "Parse error: " << result.description()
					<< ", character pos= " << result.offset;
			}

			pugi::xml_node application = doc.child("application");

			application.remove_child("login_info");
			pugi::xml_node login_info = application.append_child("login_info");
			mainApp.LoginMgr()->SaveToXml(login_info);

			application.remove_child("account_list");
			pugi::xml_node account_list = application.append_child("account_list");
			//mainApp.AcntMgr().SaveToXml(account_list);

			application.remove_child("fund_list");
			pugi::xml_node fund_list = application.append_child("fund_list");
			mainApp.FundMgr()->SaveToXml(fund_list);

			/*
			application.remove_child("external_system_list");
			pugi::xml_node external_system_list = application.append_child("external_system_list");
			VtOutSystemManager::GetInstance()->SaveToXml(external_system_list);

			application.remove_child("system_group_list");
			pugi::xml_node system_group_list = application.append_child("system_group_list");
			VtSystemGroupManager::GetInstance()->SaveToXml(system_group_list);

			application.remove_child("order_window_list");
			pugi::xml_node window_list = application.append_child("order_window_list");
			VtOrderDialogManager::GetInstance()->SaveToXml(window_list);

			application.remove_child("stratege_window_list");
			pugi::xml_node stratege_window_list = application.append_child("stratege_window_list");
			VtStrategyWndManager::GetInstance()->SaveToXml(stratege_window_list);

			application.remove_child("etc_window_list");
			pugi::xml_node etc_window_list = application.append_child("etc_window_list");
			HdWindowManager::GetInstance()->SaveToXml(etc_window_list);
			*/

			doc.save_file(appPath.c_str());

		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	void SmSaveManager::ReadAccountsAndFunds()
	{

	}

	bool SmSaveManager::ListContents(std::map<std::string, std::string>& dest, std::string dir, std::string filter, bool recursively)
	{
		try {
			WIN32_FIND_DATAA ffd;
			HANDLE hFind = INVALID_HANDLE_VALUE;
			DWORD dwError = 0;

			// Prepare string
			//if (dir.back() != '\\') dir += "\\";

			// Safety check
			if (dir.length() >= MAX_PATH) {
				LOGINFO(CMyLogger::getInstance(), "Cannot open folder %s: path too long", dir.c_str());
				return false;
			}

			// First entry in directory
			hFind = FindFirstFileA((dir + filter).c_str(), &ffd);

			if (hFind == INVALID_HANDLE_VALUE) {
				LOGINFO(CMyLogger::getInstance(), "Cannot open folder in folder %s: error accessing first entry.", dir.c_str());
				return false;
			}

			// List files in directory
			do {
				// Ignore . and .. folders, they cause stack overflow
				if (strcmp(ffd.cFileName, ".") == 0) continue;
				if (strcmp(ffd.cFileName, "..") == 0) continue;


				// Is directory?
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// Go inside recursively
					//if (recursively)
					//	ListContents(dest, dir + ffd.cFileName, filter, recursively, content_type);
					continue;
				}
				// Add file to our list
				else {

					SYSTEMTIME stLocal;

					// Convert the last-write time to local time.
					FileTimeToSystemTime(&ffd.ftLastWriteTime, &stLocal);
					std::string local_time;
					local_time += std::to_string(stLocal.wYear);
					local_time += std::to_string(stLocal.wMonth);
					local_time += std::to_string(stLocal.wDay);
					CString local_file_time;
					local_file_time.Format("%04d%02d%02d", stLocal.wYear, stLocal.wMonth, stLocal.wDay);
					dest[(LPCTSTR)local_file_time] = dir + ffd.cFileName;
				}

			} while (FindNextFileA(hFind, &ffd));

			// Get last error
			dwError = GetLastError();
			if (dwError != ERROR_NO_MORE_FILES) {
				LOGINFO(CMyLogger::getInstance(), "Error reading file list in folder %s.", dir.c_str());
				return false;
			}

		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}

		return true;
	}

	void SmSaveManager::ReadWindows()
	{

	}

	std::string SmSaveManager::GetLastestFile(std::string file_name)
	{
		FILETIME bestDate = { 0, 0 };
		FILETIME curDate;
		string name = "";
		CFileFind finder;

		finder.FindFile(file_name.c_str());
		while (finder.FindNextFile())
		{
			finder.GetCreationTime(&curDate);

			if (CompareFileTime(&curDate, &bestDate) > 0)
			{
				bestDate = curDate;
				name = finder.GetFileName().GetString();
				AfxMessageBox(name.c_str());
			}
		}
		return name;
	}

	void SmSaveManager::SaveOrders()
	{
		SaveOrders(_T("orderlist.dat"));
	}

	void SmSaveManager::SaveOrders(std::string fileName)
	{

	}

	void SmSaveManager::SaveFundList(std::string fileName)
	{

	}

	void SmSaveManager::SaveFundList()
	{
		SaveFundList(_T("fundlist.dat"));
	}

	void SmSaveManager::LoadFundList(std::string fileName)
	{

	}

	void SmSaveManager::SaveAccountList(std::string fileName)
	{

	}

	void SmSaveManager::SaveAccountList()
	{
		SaveAccountList(_T("accountlist.dat"));
	}

	void SmSaveManager::LoadAccountList(std::string fileName)
	{

	}

	void SmSaveManager::SaveOrderWndList(std::string fileName, CMainFrame* mainFrm)
	{

	}

	void SmSaveManager::LoadOrderWndList(std::string fileName, CMainFrame* mainFrm)
	{

	}

	void SmSaveManager::SaveDialogList(std::string fileName, CMainFrame* mainFrm)
	{

	}

	void SmSaveManager::LoadDialogList(std::string fileName, CMainFrame* mainFrm)
	{

	}

	void SmSaveManager::SaveTotal(std::string fileName, CMainFrame* mainFrm)
	{

	}

	void SmSaveManager::SaveLoginInfo(std::string fileName, std::string id, std::string pwd, std::string cert, bool save)
	{

	}

	void SmSaveManager::LoadLoginInfo(std::string fileName, std::string& id, std::string& pwd, std::string& cert, bool& save)
	{

	}

	void SmSaveManager::SaveSystems(std::string fileName)
	{

	}

	void SmSaveManager::LoadSystems(std::string fileName)
	{

	}

	void SmSaveManager::SaveOutSystems(std::string fileName)
	{

	}

	void SmSaveManager::LoadOutSystems(std::string fileName)
	{

	}

	void SmSaveManager::SaveOutSignal(std::string fileName)
	{

	}

	void SmSaveManager::LoadOutSignal(std::string fileName)
	{

	}

	void SmSaveManager::CreateUserDirectory()
	{
		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(mainApp.LoginMgr()->id());

		if (fs::exists(appPath)) { // 디렉토리가 존재하지 않을 경우
			fs::create_directory(appPath);
		}
	}

	bool SmSaveManager::IsAccountFileExist()
	{
		return false;
	}

	void SmSaveManager::SaveLoginInfoToXml(std::string id, std::string pwd, std::string cert, bool save)
	{

	}

	int SmSaveManager::LoadLoginInfoFromXml(std::string& id, std::string& pwd, std::string& cert, bool& save)
	{


		return -1;
	}

	int SmSaveManager::LoadLoginInfoFromXml()
	{

		return -1;
	}

	void SmSaveManager::LoadRunInfoFromXml()
	{

	}

	void SmSaveManager::GetWindowSymbolList(std::set<std::string>& symbol_list)
	{

	}

	void SmSaveManager::SaveAccountPasswords(const std::map<int, std::shared_ptr<DarkHorse::SmAccount>>& account_map)
	{
		if (account_map.empty()) return;

		try {


			std::string appPath;
			appPath = SmConfigManager::GetApplicationPath();
			appPath.append(_T("\\"));
			appPath.append(_T("acnt_pwd.xml"));

			if (!fs::exists(appPath)) {
				/// [create new xml file]
				// Generate new XML document within memory
				pugi::xml_document doc;
				// Alternatively store as shared pointer if tree shall be used for longer
				// time or multiple client calls:
				// std::shared_ptr<pugi::xml_document> spDoc = std::make_shared<pugi::xml_document>();
				// Generate XML declaration
				auto declarationNode = doc.append_child(pugi::node_declaration);
				declarationNode.append_attribute("version") = "1.0";
				declarationNode.append_attribute("encoding") = "euc-kr";
				declarationNode.append_attribute("standalone") = "yes";
				// A valid XML doc must contain a single root node of any name
				auto root = doc.append_child("application");
				// Save XML tree to file.
				// Remark: second optional param is indent string to be used;
				// default indentation is tab character.
				bool saveSucceeded = doc.save_file(appPath.c_str(), PUGIXML_TEXT("  "));
				assert(saveSucceeded);
			}

			/// [load xml file]
			// Create empty XML document within memory
			pugi::xml_document doc;
			// Load XML file into memory
			// Remark: to fully read declaration entries you have to specify
			// "pugi::parse_declaration"
			pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
				pugi::parse_default | pugi::parse_declaration);
			if (!result)
			{
				std::cout << "Parse error: " << result.description()
					<< ", character pos= " << result.offset;
			}

			pugi::xml_node application = doc.child("application");

			application.remove_child("account_list");
			pugi::xml_node account_list = application.append_child("account_list");


			for (auto it = account_map.begin(); it != account_map.end(); ++it) {
				std::shared_ptr<DarkHorse::SmAccount> account = it->second;
				auto enc_AccountNo = cryptor::encrypt(account->No());
				auto enc_Password = cryptor::encrypt(account->Pwd());

				pugi::xml_node account_node = account_list.append_child("account");

				pugi::xml_node child_node = account_node.append_child("account_no");
				child_node.append_child(pugi::node_pcdata).set_value(enc_AccountNo.c_str());
				child_node = account_node.append_child("account_pwd");
				child_node.append_child(pugi::node_pcdata).set_value(enc_Password.c_str());
			}


			doc.save_file(appPath.c_str());

		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}


	void SmSaveManager::LoadAccountPasswords()
	{
		try {


			std::string appPath;
			appPath = SmConfigManager::GetApplicationPath();
			appPath.append(_T("\\"));

			appPath.append(_T("acnt_pwd.xml"));


			std::string file_name = appPath;
			/// [load xml file]
			// Create empty XML document within memory
			pugi::xml_document doc;
			// Load XML file into memory
			// Remark: to fully read declaration entries you have to specify
			// "pugi::parse_declaration"
			pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
				pugi::parse_default | pugi::parse_declaration);
			if (!result)
			{
				std::cout << "Parse error: " << result.description()
					<< ", character pos= " << result.offset;
			}

			pugi::xml_node application = doc.child("application");
			pugi::xml_node account_list_node = application.child("account_list");
			if (account_list_node) {
				for (pugi::xml_node account_node = account_list_node.child("account"); account_node; account_node = account_node.next_sibling("account")) {
					std::string dec_AccountNo, dec_Password;
					dec_AccountNo = account_node.child_value("account_no");
					dec_Password = account_node.child_value("account_pwd");

					std::string account_no = cryptor::decrypt(dec_AccountNo);
					std::string pwd = cryptor::decrypt(dec_Password);

					mainApp.AcntMgr()->SetPassword(account_no, pwd);
				}
			}

		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	void SmSaveManager::GetSymbolMasters()
	{

	}

	void SmSaveManager::SaveSystemOrders()
	{

	}

	void SmSaveManager::LoadSystemOrders()
	{

	}

	void SmSaveManager::SaveSystemPositions()
	{

	}

	void SmSaveManager::LoadSystemPositions()
	{

	}

	void DarkHorse::SmSaveManager::ReadSettings()
	{
		try {

			std::string id = mainApp.LoginMgr()->id();
			// 아이디가 없으면 그냥 반환한다.
			if (id.length() == 0)
				return;

			std::string appPath;
			appPath = SmConfigManager::GetApplicationPath();
			appPath.append(_T("\\"));
			appPath.append(id);
			appPath.append(_T("\\"));
			std::string config_path = appPath;
			appPath.append(_T("*.*"));
			std::map<std::string, std::string> file_list;
			ListContents(file_list, config_path, "*.xml", false);

			if (file_list.size() == 0)
				return;

			std::string file_name = file_list.rbegin()->second;
			/// [load xml file]
			// Create empty XML document within memory
			pugi::xml_document doc;
			// Load XML file into memory
			// Remark: to fully read declaration entries you have to specify
			// "pugi::parse_declaration"
			pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
				pugi::parse_default | pugi::parse_declaration);
			if (!result)
			{
				std::cout << "Parse error: " << result.description()
					<< ", character pos= " << result.offset;
			}

			pugi::xml_node application = doc.child("application");
			pugi::xml_node login_info = application.child("login_info");
			if (login_info) {
				mainApp.LoginMgr()->LoadFromXml(login_info);
			}
			//pugi::xml_node account_list = application.child("account_list");
			//if (account_list) {
			//	VtAccountManager::GetInstance()->LoadFromXml(account_list);
			//}

			pugi::xml_node fund_list = application.child("fund_list");
			if (fund_list) {
				mainApp.FundMgr()->LoadFromXml(fund_list);
			}

			/*
			pugi::xml_node order_window_list = application.child("order_window_list");
			if (order_window_list) {
			VtOrderDialogManager::GetInstance()->LoadFromXml(order_window_list);
			}

			pugi::xml_node stratege_window_list = application.child("stratege_window_list");
			if (stratege_window_list) {
			VtStrategyWndManager::GetInstance()->LoadFromXml(stratege_window_list);
			}

			pugi::xml_node etc_window_list = application.child("etc_window_list");
			if (etc_window_list) {
			HdWindowManager::GetInstance()->LoadFromXml(etc_window_list);
			}
			*/
		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	void DarkHorse::SmSaveManager::ReadLoginInfo()
	{
		try {
			std::string appPath;
			appPath = SmConfigManager::GetApplicationPath();
			appPath.append(_T("\\"));
			appPath.append("user");
			//string path_name = "/bin";
			std::map<time_t, fs::directory_entry> sort_by_time;

			//--- sort the files in the map by time
			for (auto& entry : fs::directory_iterator(appPath))
				if (entry.is_directory()) {
					auto time = to_time_t(entry.last_write_time());
					sort_by_time[time] = entry;
				}

			//--- print the files sorted by time
			for (auto const& [time, entry] : sort_by_time) {
				string timestamp = asctime(std::localtime(&time));
				timestamp.pop_back(); // remove automatic linebreak
				//cout << timestamp << "\t " << entry.path().c_str() << endl;
			}

			if (sort_by_time.empty()) return;

			//std::string appPath;

			std::string config_path = std::prev(sort_by_time.end())->second.path().string();
			config_path.append("\\");
			appPath.append(_T("*.*"));
			std::map<std::string, std::string> file_list;
			ListContents(file_list, config_path, "*.xml", false);

			if (file_list.size() == 0)
				return;

			std::string file_name = file_list.rbegin()->second;
			/// [load xml file]
			// Create empty XML document within memory
			pugi::xml_document doc;
			// Load XML file into memory
			// Remark: to fully read declaration entries you have to specify
			// "pugi::parse_declaration"
			pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
				pugi::parse_default | pugi::parse_declaration);
			if (!result)
			{
				std::cout << "Parse error: " << result.description()
					<< ", character pos= " << result.offset;
			}

			pugi::xml_node application = doc.child("application");
			pugi::xml_node login_info = application.child("login_info");
			if (login_info) {
				mainApp.LoginMgr()->LoadFromXml(login_info);
			}


		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	void SmSaveManager::restore_dm_mini_jango_windows_from_json(CWnd* parent_window, const std::string& filename, std::map<HWND, std::shared_ptr<MiniJangoDialog>>& map_to_restore)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		const fs::path directoryPath = appPath;

		// Check if directory exists
		if (fs::exists(directoryPath)) {
			std::cout << "Directory already exists." << std::endl;
		}
		else {
			// Create the directory
			try {
				fs::create_directory(directoryPath);
				std::cout << "Directory created successfully." << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create directory: " << e.what() << std::endl;
			}
		}

		appPath.append(_T("\\"));
		appPath.append(filename);
		std::string full_file_name = appPath;

// 		std::string full_file_name;
// 		full_file_name = SmConfigManager::GetApplicationPath();
// 		full_file_name.append(_T("\\user\\"));
// 		full_file_name.append(filename);

		if (!fs::exists(full_file_name))
			return;

		std::ifstream file(full_file_name);
		json dialog_data;
		file >> dialog_data;
		file.close();

		for (const auto& dialog_json : dialog_data) {
			const int x = dialog_json["x"].get<int>();
			const int y = dialog_json["y"].get<int>();
			const int width = dialog_json["width"].get<int>();
			const int height = dialog_json["height"].get<int>();
			bool old_version = true;
			if (dialog_json.contains("type"))
				old_version = false;

			const int mode = dialog_json["mode"].get<int>();
			const std::string type = dialog_json["type"].get<std::string>();
			std::string target;
			if (mode == 0)
				target = dialog_json["accoount_no"].get<std::string>();
			else
				target = dialog_json["fund_name"].get<std::string>();

			// Create a new instance of DmAccountOrderWindow and associate it with a new HWND
			std::shared_ptr<MiniJangoDialog>  totalAssetDialog;
			if (!old_version)
				totalAssetDialog = std::make_shared<MiniJangoDialog>(parent_window, "9", mode, target);
			else
				totalAssetDialog = std::make_shared<MiniJangoDialog>(parent_window);
			totalAssetDialog->Create(IDD_JANGO, parent_window);
			map_to_restore[totalAssetDialog->GetSafeHwnd()] = totalAssetDialog;
			totalAssetDialog->MoveWindow(x, y, width, height, TRUE);
			totalAssetDialog->ShowWindow(SW_SHOW);
		}
	}


	void SmSaveManager::restore_total_asset_windows_from_json(CWnd* parent_window, const std::string& filename, std::map<HWND, std::shared_ptr<TotalAssetProfitLossDialog>>& map_to_restore)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		const fs::path directoryPath = appPath;

		// Check if directory exists
		if (fs::exists(directoryPath)) {
			std::cout << "Directory already exists." << std::endl;
		}
		else {
			// Create the directory
			try {
				fs::create_directory(directoryPath);
				std::cout << "Directory created successfully." << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create directory: " << e.what() << std::endl;
			}
		}

		appPath.append(_T("\\"));
		appPath.append(filename);
		std::string full_file_name = appPath;

//		std::string full_file_name;
// 		full_file_name = SmConfigManager::GetApplicationPath();
// 		full_file_name.append(_T("\\user\\"));
// 		full_file_name.append(filename);

		if (!fs::exists(full_file_name))
			return;

		std::ifstream file(full_file_name);
		json dialog_data;
		file >> dialog_data;
		file.close();

		for (const auto& dialog_json : dialog_data) {
			int x = dialog_json["x"].get<int>();
			int y = dialog_json["y"].get<int>();
			int width = dialog_json["width"].get<int>();
			int height = dialog_json["height"].get<int>();
			bool old_version = true;
			if (dialog_json.contains("type"))
				old_version = false;
			const std::string type = dialog_json["type"].get<std::string>();
			const std::string account_no = dialog_json["account_no"].get<std::string>();

			// Create a new instance of DmAccountOrderWindow and associate it with a new HWND
			std::shared_ptr< TotalAssetProfitLossDialog>  totalAssetDialog = nullptr;
			if (!old_version)
				totalAssetDialog = std::make_shared<TotalAssetProfitLossDialog>(parent_window, type, account_no);
			else
				totalAssetDialog = std::make_shared<TotalAssetProfitLossDialog>(parent_window);
			totalAssetDialog->Create(IDD_TOTAL_ASSET, parent_window);
			map_to_restore[totalAssetDialog->GetSafeHwnd()] = totalAssetDialog;
			totalAssetDialog->MoveWindow(x, y, width, height, TRUE);
			totalAssetDialog->ShowWindow(SW_SHOW);
		}
	}

	void SmSaveManager::save_total_asset_windows(const std::string& filename, const std::map<HWND, std::shared_ptr<TotalAssetProfitLossDialog>>& map_to_save)
	{

		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		const fs::path directoryPath = appPath;

		// Check if directory exists
		if (fs::exists(directoryPath)) {
			std::cout << "Directory already exists." << std::endl;
		}
		else {
			// Create the directory
			try {
				fs::create_directory(directoryPath);
				std::cout << "Directory created successfully." << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create directory: " << e.what() << std::endl;
			}
		}

		appPath.append(_T("\\"));
		appPath.append(filename);
		std::string full_file_name = appPath;

// 		std::string full_file_name;
// 		full_file_name = SmConfigManager::GetApplicationPath();
// 		full_file_name.append(_T("\\user\\"));
// 		full_file_name.append(filename);

		json dialog_data;

		for (const auto& pair : map_to_save) {
			HWND hwnd = pair.first;

			RECT rect;
			GetWindowRect(hwnd, &rect);

			json dialog_json;
			dialog_json["x"] = rect.left;
			dialog_json["y"] = rect.top;
			dialog_json["width"] = rect.right - rect.left;
			dialog_json["height"] = rect.bottom - rect.top;
			dialog_json["type"] = pair.second->type();
			dialog_json["account_no"] = pair.second->account_no();

			dialog_data.push_back(dialog_json);
		}

		std::ofstream file(full_file_name);
		file << dialog_data.dump(4);
		file.close();
	}

	void SmSaveManager::save_dm_mini_jango_windows(const std::string& filename, const std::map<HWND, std::shared_ptr<MiniJangoDialog>>& map_to_save)
	{
		if (map_to_save.empty()) return;

		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		const fs::path directoryPath = appPath;

		// Check if directory exists
		if (fs::exists(directoryPath)) {
			std::cout << "Directory already exists." << std::endl;
		}
		else {
			// Create the directory
			try {
				fs::create_directory(directoryPath);
				std::cout << "Directory created successfully." << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create directory: " << e.what() << std::endl;
			}
		}

		appPath.append(_T("\\"));
		appPath.append(filename);
		std::string full_file_name = appPath;

// 		std::string full_file_name;
// 		full_file_name = SmConfigManager::GetApplicationPath();
// 		full_file_name.append(_T("\\user\\"));
// 		full_file_name.append(filename);

		json dialog_data;

		for (const auto& pair : map_to_save) {
			HWND hwnd = pair.first;

			RECT rect;
			GetWindowRect(hwnd, &rect);

			json dialog_json;
			dialog_json["x"] = rect.left;
			dialog_json["y"] = rect.top;
			dialog_json["width"] = rect.right - rect.left;
			dialog_json["height"] = rect.bottom - rect.top;
			dialog_json["mode"] = pair.second->Mode();
			dialog_json["type"] = pair.second->Type();
			if (pair.second->Mode() == 0) // for account
				dialog_json["accoount_no"] = pair.second->account_no();
			else
				dialog_json["fund_name"] = pair.second->fund_name();

			dialog_data.push_back(dialog_json);
		}

		std::ofstream file(full_file_name);
		file << dialog_data.dump(4);
		file.close();
	}

	std::string SmSaveManager::find_latestfile_with_prefix(const std::string& directory, const std::string& prefix) {
		std::string latestFile;
		std::filesystem::file_time_type latestTime;

		for (const auto& entry : fs::directory_iterator(directory)) {
			if (entry.is_regular_file() && entry.path().filename().string().find(prefix) == 0) {
				std::filesystem::file_time_type fileTime = fs::last_write_time(entry.path());
				if (fileTime > latestTime) {
					latestTime = fileTime;
					latestFile = entry.path().filename().string();
				}
			}
		}

		return latestFile;
	}



	void SmSaveManager::save_dm_account_order_windows(const std::string& filename, const std::map<HWND, DmAccountOrderWindow*>& map_to_save)
	{
		if (map_to_save.empty()) return;
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		const fs::path directoryPath = appPath;

		// Check if directory exists
		if (fs::exists(directoryPath)) {
			std::cout << "Directory already exists." << std::endl;
		}
		else {
			// Create the directory
			try {
				fs::create_directory(directoryPath);
				std::cout << "Directory created successfully." << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create directory: " << e.what() << std::endl;
			}
		}

		appPath.append(_T("\\"));

		std::string full_file_name = filename;
		full_file_name.append("_");
		full_file_name.append(VtStringUtil::getTimeStr());
		full_file_name.append(".json");

		appPath.append(full_file_name);

		nlohmann::json jsonData;

		// Convert order_window_map_ to JSON
		for (const auto& [wnd_handle, orderWindow] : map_to_save) {
			nlohmann::json account_wnd_json;
			RECT rect;
			GetWindowRect(wnd_handle, &rect);

			account_wnd_json["x"] = rect.left;
			account_wnd_json["y"] = rect.top;
			account_wnd_json["width"] = rect.right - rect.left;
			account_wnd_json["height"] = rect.bottom - rect.top;

			account_wnd_json["account_no"] = orderWindow->get_account_no();
			account_wnd_json["center_window_count"] = orderWindow->get_center_window_count();

			const std::map<int, std::shared_ptr<DmAccountOrderCenterWindow>>& center_window_map = orderWindow->get_center_window_map();
			// Convert center_window_map_ to JSON
			nlohmann::json centerWindowMapJson;
			for (const auto& [windowId, centerWindow] : center_window_map) {
				nlohmann::json centerWindowJson;
				centerWindowJson["symbol_code"] = centerWindow->get_symbol_code();
				const auto& order_set = centerWindow->get_order_set();
				// Convert order_set_event_ to JSON
				centerWindowJson["window_id"] = order_set.window_id;
				centerWindowJson["message"] = order_set.message;
				centerWindowJson["grid_height"] = order_set.grid_height;
				centerWindowJson["stop_width"] = order_set.stop_width;
				centerWindowJson["order_width"] = order_set.order_width;
				centerWindowJson["count_width"] = order_set.count_width;
				centerWindowJson["qty_width"] = order_set.qty_width;
				centerWindowJson["quote_width"] = order_set.quote_width;
				centerWindowJson["stop_as_real_order"] = order_set.stop_as_real_order;
				centerWindowJson["show_symbol_tick"] = order_set.show_symbol_tick;
				centerWindowJson["show_bar_color"] = order_set.show_bar_color;
				centerWindowJson["align_by_alt"] = order_set.align_by_alt;
				centerWindowJson["cancel_by_right_click"] = order_set.cancel_by_right_click;
				centerWindowJson["order_by_space"] = order_set.order_by_space;
				centerWindowJson["show_order_column"] = order_set.show_order_column;
				centerWindowJson["show_stop_column"] = order_set.show_stop_column;
				centerWindowJson["show_count_column"] = order_set.show_count_column;

				centerWindowMapJson.push_back(centerWindowJson);
			}

			account_wnd_json["center_window_map"] = centerWindowMapJson;

			jsonData.push_back(account_wnd_json);
		}



		std::ofstream file(appPath);
		file << jsonData.dump(4);
		file.close();
	}

	void SmSaveManager::restore_dm_account_order_windows(CWnd* parent_window, const std::string& filename, std::map<HWND, DmAccountOrderWindow*>& map_to_restore)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);

		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		const fs::path directoryPath = appPath;

		// Check if directory exists
		if (fs::exists(directoryPath)) {
			std::cout << "Directory already exists." << std::endl;
		}
		else {
			// Create the directory
			try {
				fs::create_directory(directoryPath);
				std::cout << "Directory created successfully." << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create directory: " << e.what() << std::endl;
			}
		}

		if (std::filesystem::is_directory(appPath)) {
			if (std::filesystem::is_empty(appPath)) {
				std::cout << "The directory is empty." << std::endl;
				return;
			}
			else {
				std::cout << "The directory is not empty." << std::endl;
			}
		}
		else {
			std::cout << "The path is not a directory." << std::endl;
			return;
		}

		std::string full_file_name = filename;
		full_file_name.append("_");
		std::string latest_file = find_latestfile_with_prefix(appPath, full_file_name);
		if (latest_file.empty()) return;
		appPath.append("\\");
		appPath.append(latest_file);
		std::ifstream file(appPath);
		if (!file.is_open()) {
			std::cerr << "Failed to open file for restore: " << latest_file << std::endl;
			return;
		}

		nlohmann::json jsonData;
		try {
			file >> jsonData;
		}
		catch (const nlohmann::json::parse_error& e) {
			std::cerr << "Failed to parse JSON file: " << e.what() << std::endl;
			file.close();
			return;
		}
		file.close();

		// Clear existing data in map_to_restore
		map_to_restore.clear();

		// Restore data from jsonData
		for (const auto& account_wnd_json : jsonData) {
			std::string accountNo = account_wnd_json["account_no"].get<std::string>();
			size_t centerWindowCount = account_wnd_json["center_window_count"].get<int>();

			int x = account_wnd_json["x"].get<int>();
			int y = account_wnd_json["y"].get<int>();
			int width = account_wnd_json["width"].get<int>();
			int height = account_wnd_json["height"].get<int>();

			const nlohmann::json& centerWindowMapJson = account_wnd_json["center_window_map"];
			for (const auto& centerWindowJson : centerWindowMapJson) {
				std::string symbolCode = centerWindowJson["symbol_code"].get<std::string>();
				int windowId = centerWindowJson["window_id"].get<int>();
				std::string message = centerWindowJson["message"].get<std::string>();

				int grid_height = centerWindowJson["grid_height"].get<int>();
				int stop_width = centerWindowJson["stop_width"].get<int>();
				int order_width = centerWindowJson["order_width"].get<int>();
				int count_width = centerWindowJson["count_width"].get<int>();
				int qty_width = centerWindowJson["qty_width"].get<int>();
				int quote_width = centerWindowJson["quote_width"].get<int>();
				bool stop_as_real_order = centerWindowJson["stop_as_real_order"].get<bool>();

				bool show_symbol_tick = centerWindowJson["show_symbol_tick"].get<bool>();
				bool show_bar_color = centerWindowJson["show_bar_color"].get<bool>();
				bool align_by_alt = centerWindowJson["align_by_alt"].get<bool>();
				bool cancel_by_right_click = centerWindowJson["cancel_by_right_click"].get<bool>();
				bool order_by_space = centerWindowJson["order_by_space"].get<bool>();
				bool show_order_column = centerWindowJson["show_order_column"].get<bool>();
				bool show_stop_column = centerWindowJson["show_stop_column"].get<bool>();
				bool show_count_column = centerWindowJson["show_count_column"].get<bool>();
			}

			DmAccountOrderWindow* account_order_window = new DmAccountOrderWindow(parent_window, centerWindowCount, accountNo, centerWindowMapJson);
			account_order_window->Create(IDD_DM_ACNT_ORDER_MAIN, parent_window);
			map_to_restore[account_order_window->GetSafeHwnd()] = account_order_window;
			account_order_window->MoveWindow(x, y, width, height, TRUE);
			account_order_window->ShowWindow(SW_SHOW);
		}
	}

	void SmSaveManager::save_account(const std::string& filename)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		const fs::path directoryPath = appPath;

		// Check if directory exists
		if (fs::exists(directoryPath)) {
			std::cout << "Directory already exists." << std::endl;
		}
		else {
			// Create the directory
			try {
				fs::create_directory(directoryPath);
				std::cout << "Directory created successfully." << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create directory: " << e.what() << std::endl;
			}
		}

		appPath.append(_T("\\"));

		std::string full_file_name = filename;
		//full_file_name.append("_");
		//full_file_name.append(VtStringUtil::getTimeStr());
		//full_file_name.append(".json");

		appPath.append(full_file_name);

		nlohmann::json jsonData;

		std::vector<std::shared_ptr<DarkHorse::SmAccount>> main_account_vector;
		mainApp.AcntMgr()->get_main_account_vector(main_account_vector);
		for (auto it = main_account_vector.begin(); it != main_account_vector.end(); ++it) {
			auto account = *it;
			nlohmann::json account_json;

			account_json["name"] = SmUtil::MultiByteToUtf8(account->Name());
			account_json["account_no"] = account->No();
			account_json["account_type"] = account->Type();
			account_json["used_for_fund"] = account->UsedForFund();
			account_json["seung_su"] = account->SeungSu();
			account_json["ratio"] = account->Ratio();
			account_json["is_sub_account"] = account->is_subaccount();
			account_json["fund_name"] = SmUtil::MultiByteToUtf8(account->fund_name());
			account_json["parent_account_no"] = "";

			auto sub_accounts = account->get_sub_accounts();
			nlohmann::json sub_account_vector_json;
			for (size_t i = 0; i < sub_accounts.size(); i++) {
				auto sub_account = sub_accounts[i];
				nlohmann::json sub_account_json;
				sub_account_json["name"] = SmUtil::MultiByteToUtf8(sub_account->Name());
				sub_account_json["account_no"] = sub_account->No();
				sub_account_json["account_type"] = sub_account->Type();
				sub_account_json["used_for_fund"] = sub_account->UsedForFund();
				sub_account_json["seung_su"] = sub_account->SeungSu();
				sub_account_json["ratio"] = sub_account->Ratio();
				sub_account_json["is_sub_account"] = sub_account->is_subaccount();
				sub_account_json["fund_name"] = SmUtil::MultiByteToUtf8(sub_account->fund_name());
				sub_account_json["parent_account_no"] = account->No();
				sub_account_vector_json.push_back(sub_account_json);
			}
			account_json["sub_accounts"] = sub_account_vector_json;
			
			jsonData[account->No()] = account_json;
		}
		std::ofstream file(appPath);
		file << jsonData.dump(4);
		file.close();
	}


	// Define a function to restore account information from a JSON file
	void SmSaveManager::restore_account(const std::string& filename) {

		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);

		appPath.append("\\");
		appPath.append(filename);
		std::ifstream file(appPath);
		if (!file.is_open()) {
			std::cerr << "Failed to open file for restore: " << filename << std::endl;
			return;
		}

		nlohmann::json jsonData;
		try {
			// Parse the JSON data from the file
			file >> jsonData;
		}
		catch (const nlohmann::json::parse_error& e) {
			std::cerr << "Failed to parse JSON file: " << e.what() << std::endl;
			file.close();
			return;
		}
		file.close();

		try {
			// Process the JSON data and reconstruct account information
			for (json::iterator it = jsonData.begin(); it != jsonData.end(); ++it) {
				std::string accountNo = it.key();
				json accountData = it.value();

				// Extract and process account-related information
				std::string account_no = accountData["account_no"];
				auto account = mainApp.AcntMgr()->find_main_account(account_no);
				// If the account does not exist in the list of server accounts, skip it.
				if (!account) continue;

				std::string name = SmUtil::Utf8ToMultiByte(accountData["name"]);
				std::string account_type = accountData["account_type"];
				bool used_for_fund = accountData["used_for_fund"];
				int seung_su = accountData["seung_su"];
				double ratio = accountData["ratio"];
				bool is_sub_account = accountData["is_sub_account"];
				std::string fund_name = SmUtil::Utf8ToMultiByte(accountData["fund_name"]);
				std::string parent_account_no = accountData["parent_account_no"];

				// Process sub-accounts if available
				if (accountData.find("sub_accounts") != accountData.end()) {
					json subAccounts = accountData["sub_accounts"];
					for (json::iterator subIt = subAccounts.begin(); subIt != subAccounts.end(); ++subIt) {
						json subAccountData = *subIt;
						std::string t_account_no = subAccountData["account_no"];
						std::string t_name = SmUtil::Utf8ToMultiByte(subAccountData["name"]);
						std::string t_account_type = subAccountData["account_type"];
						bool t_used_for_fund = subAccountData["used_for_fund"];
						int t_seung_su = subAccountData["seung_su"];
						double t_ratio = subAccountData["ratio"];
						bool t_is_sub_account = subAccountData["is_sub_account"];
						std::string t_fund_name = SmUtil::Utf8ToMultiByte(subAccountData["fund_name"]);
						std::string t_parent_account_no = subAccountData["parent_account_no"];

						if (t_parent_account_no != account_no) continue;

						auto sub_account = account->CreateSubAccount(
							t_account_no, 
							t_name, 
							account->id(),
							t_account_type
						);

						sub_account->SeungSu(t_seung_su);
						sub_account->Ratio(t_ratio);
						sub_account->is_subaccount(t_is_sub_account);
						sub_account->fund_name(t_fund_name);
						sub_account->UsedForFund(t_used_for_fund);
					}
				}

				// Now you have the account information, you can use it as needed.
				// You can create objects or data structures to store this information.
				// For simplicity, we'll just print it here.
				std::cout << "Account No: " << accountNo << std::endl;
				std::cout << "Name: " << name << std::endl;
				// create some default sub accounts for the account that has no sub accounts. 
				account->make_default_sub_account();
			}
		}
		catch (const json::parse_error& e) {
			std::cerr << "Failed to parse JSON data: " << e.what() << std::endl;
		}
	}

	void SmSaveManager::save_fund(const std::string& filename)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		const fs::path directoryPath = appPath;

		// Check if directory exists
		if (fs::exists(directoryPath)) {
			std::cout << "Directory already exists." << std::endl;
		}
		else {
			// Create the directory
			try {
				fs::create_directory(directoryPath);
				std::cout << "Directory created successfully." << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create directory: " << e.what() << std::endl;
			}
		}

		appPath.append(_T("\\"));

		appPath.append(filename);

		nlohmann::json jsonData;

		auto func_map = mainApp.FundMgr()->GetFundMap();
		for (auto it = func_map.begin(); it != func_map.end(); ++it) {
			auto fund = it->second;
			nlohmann::json fund_json;

			fund_json["name"] = SmUtil::MultiByteToUtf8(fund->Name());
			fund_json["fund_type"] = fund->fund_type();
		

			auto sub_accounts = fund->GetAccountVector();
			nlohmann::json sub_account_vector_json;
			for (size_t i = 0; i < sub_accounts.size(); i++) {
				auto sub_account = sub_accounts[i];
				nlohmann::json sub_account_json;
				sub_account_json["account_no"] = sub_account->No();
				sub_account_vector_json.push_back(sub_account_json);
			}
			fund_json["sub_accounts"] = sub_account_vector_json;

			jsonData[fund->Name()] = fund_json;
		}
		std::ofstream file(appPath);
		file << jsonData.dump(4);
		file.close();
	}

	void SmSaveManager::save_out_system(const std::string& filename)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		const fs::path directoryPath = appPath;

		// Check if directory exists
		if (fs::exists(directoryPath)) {
			std::cout << "Directory already exists." << std::endl;
		}
		else {
			// Create the directory
			try {
				fs::create_directory(directoryPath);
				std::cout << "Directory created successfully." << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create directory: " << e.what() << std::endl;
			}
		}

		appPath.append(_T("\\"));

		appPath.append(filename);

		nlohmann::json jsonData;
		auto out_system_vec = mainApp.out_system_manager()->get_out_system_vector();
		for (auto it = out_system_vec.begin(); it != out_system_vec.end(); ++it) {
			auto out_system = *it;
			nlohmann::json out_system_json;

			out_system_json["name"] = SmUtil::MultiByteToUtf8(out_system->name());
			out_system_json["order_type"] = (int)out_system->order_type();
			out_system_json["seung_su"] = out_system->seung_su();
			out_system_json["account_no"] = out_system->account() ? out_system->account()->No() : "";
			out_system_json["symbol_code"] = out_system->symbol() ? out_system->symbol()->SymbolCode() : "";
			out_system_json["fund_name"] = out_system->fund() ? SmUtil::MultiByteToUtf8(out_system->fund()->Name()) : "";
			jsonData[std::to_string(out_system->id())] = out_system_json;
		}
		std::ofstream file(appPath);
		file << jsonData.dump(4);
		file.close();
	}

	void SmSaveManager::restore_fund(const std::string& filename)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);

		appPath.append("\\");
		appPath.append(filename);
		std::ifstream file(appPath);
		if (!file.is_open()) {
			std::cerr << "Failed to open file for restore: " << filename << std::endl;
			return;
		}

		nlohmann::json jsonData;
		try {
			// Parse the JSON data from the file
			file >> jsonData;
		}
		catch (const nlohmann::json::parse_error& e) {
			std::cerr << "Failed to parse JSON file: " << e.what() << std::endl;
			file.close();
			return;
		}
		file.close();

		try {
			// Process the JSON data and reconstruct account information
			for (json::iterator it = jsonData.begin(); it != jsonData.end(); ++it) {
				json fundData = it.value();

				// Extract and process account-related information
				const std::string name = SmUtil::Utf8ToMultiByte(fundData["name"]);
				const std::string fund_type = fundData["fund_type"];
				auto fund = mainApp.FundMgr()->FindAddFund(name, fund_type);

				//std::string fund_type = fundData["fund_type"];
				//fund->fund_type(fund_type);
				// Process sub-accounts if available
				if (fundData.find("sub_accounts") != fundData.end()) {
					json subAccounts = fundData["sub_accounts"];
					for (json::iterator subIt = subAccounts.begin(); subIt != subAccounts.end(); ++subIt) {
						json subAccountData = *subIt;
						std::string t_account_no = subAccountData["account_no"];
						auto account = mainApp.AcntMgr()->FindAccount(t_account_no);
						if (!account)continue;
						fund->AddAccount(account);
					}
				}

				// Now you have the account information, you can use it as needed.
				// You can create objects or data structures to store this information.
				// For simplicity, we'll just print it here.
				std::cout << "fund name: " << name << std::endl;
			}
		}
		catch (const json::parse_error& e) {
			std::cerr << "Failed to parse JSON data: " << e.what() << std::endl;
		}
	}

	void SmSaveManager::restore_out_system(const std::string& filename)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);

		appPath.append("\\");
		appPath.append(filename);
		std::ifstream file(appPath);
		if (!file.is_open()) {
			std::cerr << "Failed to open file for restore: " << filename << std::endl;
			return;
		}

		nlohmann::json jsonData;
		try {
			// Parse the JSON data from the file
			file >> jsonData;
		}
		catch (const nlohmann::json::parse_error& e) {
			std::cerr << "Failed to parse JSON file: " << e.what() << std::endl;
			file.close();
			return;
		}
		file.close();

		try {
			// Process the JSON data and reconstruct account information
			for (json::iterator it = jsonData.begin(); it != jsonData.end(); ++it) {
				std::string out_system_id = it.key();
				json out_system_data = it.value();

				// Extract and process account-related information
				const std::string name = SmUtil::Utf8ToMultiByte(out_system_data["name"]);
				const int t_order_type = out_system_data["order_type"];
				const int seung_su = out_system_data["seung_su"];
				DarkHorse::OrderType order_type = (DarkHorse::OrderType)t_order_type;
				const std::string account_no = out_system_data["account_no"];
				const std::string symbol_code = out_system_data["symbol_code"];
				const std::string fund_name = SmUtil::Utf8ToMultiByte(out_system_data["fund_name"]);
				auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
				auto account = mainApp.AcntMgr()->FindAccount(account_no);
				auto fund = mainApp.FundMgr()->FindFund(fund_name);
				int mode = 0;
				//if (!account || !symbol || !fund) continue;
				if (order_type == OrderType::MainAccount || order_type == OrderType::SubAccount) {
					if (!account) continue;
					mode = 0;
				}
				else if (order_type == OrderType::Fund) {
					if (!fund) continue;
					mode = 1;
				}
				if (!symbol) {
					continue;
				}

				auto out_system = mainApp.out_system_manager()->create_out_system(
					name,
					seung_su,
					order_type,
					mode == 0 ? account : nullptr,
					mode == 1 ? fund : nullptr,
					symbol
				);

				// Now you have the account information, you can use it as needed.
				// You can create objects or data structures to store this information.
				// For simplicity, we'll just print it here.
				std::cout << "out system id: " << out_system->id() << std::endl;
			}
		}
		catch (const json::parse_error& e) {
			std::cerr << "Failed to parse JSON data: " << e.what() << std::endl;
		}
	}

	void SmSaveManager::restore_dm_fund_order_windows(CWnd* parent_window, const std::string& filename, std::map<HWND, DmFundOrderWindow*>& map_to_restore)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);

		std::string full_file_name = filename;
		full_file_name.append("_");
		std::string latest_file = find_latestfile_with_prefix(appPath, full_file_name);
		if (latest_file.empty()) return;
		appPath.append("\\");
		appPath.append(latest_file);
		std::ifstream file(appPath);
		if (!file.is_open()) {
			std::cerr << "Failed to open file for restore: " << latest_file << std::endl;
			return;
		}

		nlohmann::json jsonData;
		try {
			file >> jsonData;
		}
		catch (const nlohmann::json::parse_error& e) {
			std::cerr << "Failed to parse JSON file: " << e.what() << std::endl;
			file.close();
			return;
		}
		file.close();

		// Clear existing data in map_to_restore
		map_to_restore.clear();

		// Restore data from jsonData
		for (const auto& account_wnd_json : jsonData) {
			std::string fund_name = account_wnd_json["fund_name"].get<std::string>();
			size_t centerWindowCount = account_wnd_json["center_window_count"].get<int>();

			int x = account_wnd_json["x"].get<int>();
			int y = account_wnd_json["y"].get<int>();
			int width = account_wnd_json["width"].get<int>();
			int height = account_wnd_json["height"].get<int>();

			const nlohmann::json& centerWindowMapJson = account_wnd_json["center_window_map"];
			for (const auto& centerWindowJson : centerWindowMapJson) {
				std::string symbolCode = centerWindowJson["symbol_code"].get<std::string>();
				int windowId = centerWindowJson["window_id"].get<int>();
				std::string message = centerWindowJson["message"].get<std::string>();

				int grid_height = centerWindowJson["grid_height"].get<int>();
				int stop_width = centerWindowJson["stop_width"].get<int>();
				int order_width = centerWindowJson["order_width"].get<int>();
				int count_width = centerWindowJson["count_width"].get<int>();
				int qty_width = centerWindowJson["qty_width"].get<int>();
				int quote_width = centerWindowJson["quote_width"].get<int>();
				bool stop_as_real_order = centerWindowJson["stop_as_real_order"].get<bool>();

				bool show_symbol_tick = centerWindowJson["show_symbol_tick"].get<bool>();
				bool show_bar_color = centerWindowJson["show_bar_color"].get<bool>();
				bool align_by_alt = centerWindowJson["align_by_alt"].get<bool>();
				bool cancel_by_right_click = centerWindowJson["cancel_by_right_click"].get<bool>();
				bool order_by_space = centerWindowJson["order_by_space"].get<bool>();
				bool show_order_column = centerWindowJson["show_order_column"].get<bool>();
				bool show_stop_column = centerWindowJson["show_stop_column"].get<bool>();
				bool show_count_column = centerWindowJson["show_count_column"].get<bool>();
			}

			DmFundOrderWindow* account_order_window = new DmFundOrderWindow(parent_window, centerWindowCount, fund_name, centerWindowMapJson);
			account_order_window->Create(IDD_DM_FUND_ORDER_MAIN, parent_window);
			map_to_restore[account_order_window->GetSafeHwnd()] = account_order_window;
			account_order_window->MoveWindow(x, y, width, height, TRUE);
			account_order_window->ShowWindow(SW_SHOW);
		}
	}


	void SmSaveManager::save_dm_fund_order_windows(const std::string& filename, const std::map<HWND, DmFundOrderWindow*>& map_to_save)
	{
		if (map_to_save.empty()) return;
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		//appPath.append(_T("\\user\\"));
		//appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		const fs::path directoryPath = appPath;

		// Check if directory exists
		if (fs::exists(directoryPath)) {
			std::cout << "Directory already exists." << std::endl;
		}
		else {
			// Create the directory
			try {
				fs::create_directory(directoryPath);
				std::cout << "Directory created successfully." << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create directory: " << e.what() << std::endl;
			}
		}

		appPath.append(_T("\\"));

		std::string full_file_name = filename;
		full_file_name.append("_");
		full_file_name.append(VtStringUtil::getTimeStr());
		full_file_name.append(".json");

		appPath.append(full_file_name);

		nlohmann::json jsonData;

		// Convert order_window_map_ to JSON
		for (const auto& [wnd_handle, orderWindow] : map_to_save) {
			nlohmann::json account_wnd_json;
			RECT rect;
			GetWindowRect(wnd_handle, &rect);

			account_wnd_json["x"] = rect.left;
			account_wnd_json["y"] = rect.top;
			account_wnd_json["width"] = rect.right - rect.left;
			account_wnd_json["height"] = rect.bottom - rect.top;

			account_wnd_json["fund_name"] = orderWindow->get_fund_name();
			account_wnd_json["center_window_count"] = orderWindow->get_center_window_count();

			const std::map<int, std::shared_ptr<DmAccountOrderCenterWindow>>& center_window_map = orderWindow->get_center_window_map();
			// Convert center_window_map_ to JSON
			nlohmann::json centerWindowMapJson;
			for (const auto& [windowId, centerWindow] : center_window_map) {
				nlohmann::json centerWindowJson;
				centerWindowJson["symbol_code"] = centerWindow->get_symbol_code();
				const auto& order_set = centerWindow->get_order_set();
				// Convert order_set_event_ to JSON
				centerWindowJson["window_id"] = order_set.window_id;
				centerWindowJson["message"] = order_set.message;
				centerWindowJson["grid_height"] = order_set.grid_height;
				centerWindowJson["stop_width"] = order_set.stop_width;
				centerWindowJson["order_width"] = order_set.order_width;
				centerWindowJson["count_width"] = order_set.count_width;
				centerWindowJson["qty_width"] = order_set.qty_width;
				centerWindowJson["quote_width"] = order_set.quote_width;
				centerWindowJson["stop_as_real_order"] = order_set.stop_as_real_order;
				centerWindowJson["show_symbol_tick"] = order_set.show_symbol_tick;
				centerWindowJson["show_bar_color"] = order_set.show_bar_color;
				centerWindowJson["align_by_alt"] = order_set.align_by_alt;
				centerWindowJson["cancel_by_right_click"] = order_set.cancel_by_right_click;
				centerWindowJson["order_by_space"] = order_set.order_by_space;
				centerWindowJson["show_order_column"] = order_set.show_order_column;
				centerWindowJson["show_stop_column"] = order_set.show_stop_column;
				centerWindowJson["show_count_column"] = order_set.show_count_column;

				centerWindowMapJson.push_back(centerWindowJson);
			}

			account_wnd_json["center_window_map"] = centerWindowMapJson;

			jsonData.push_back(account_wnd_json);
		}



		std::ofstream file(appPath);
		file << jsonData.dump(4);
		file.close();
	}

	void SmSaveManager::save_system_config(const std::string& filename)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		const fs::path directoryPath = appPath;

		// Check if directory exists
		if (fs::exists(directoryPath)) {
			std::cout << "Directory already exists." << std::endl;
		}
		else {
			// Create the directory
			try {
				fs::create_directory(directoryPath);
				std::cout << "Directory created successfully." << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create directory: " << e.what() << std::endl;
			}
		}

		appPath.append(_T("\\"));

		std::string full_file_name = filename;
		//full_file_name.append("_");
		//full_file_name.append(VtStringUtil::getTimeStr());
		//full_file_name.append(".json");

		appPath.append(full_file_name);

		mainApp.config_manager()->saveConfig(appPath);
	}

	void SmSaveManager::restore_system_config(const std::string& filename)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		//appPath.append(_T("\\user\\"));
		//appPath.append(id);

		appPath.append("\\");
		appPath.append(filename);
		std::ifstream file(appPath);
		if (!file.is_open()) {
			std::cerr << "Failed to open file for restore: " << filename << std::endl;
			return;
		}
		file.close();

		mainApp.config_manager()->loadConfig(appPath);
	}

	void SmSaveManager::save_usd_system(const std::string& filename)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		const fs::path directoryPath = appPath;

		// Check if directory exists
		if (fs::exists(directoryPath)) {
			std::cout << "Directory already exists." << std::endl;
		}
		else {
			// Create the directory
			try {
				fs::create_directory(directoryPath);
				std::cout << "Directory created successfully." << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create directory: " << e.what() << std::endl;
			}
		}

		appPath.append(_T("\\"));

		appPath.append(filename);

		nlohmann::json jsonData;
		auto out_system_vec = mainApp.out_system_manager()->get_usd_system_vector();
		for (auto it = out_system_vec.begin(); it != out_system_vec.end(); ++it) {
			auto out_system = *it;
			nlohmann::json out_system_json;

			out_system_json["name"] = SmUtil::MultiByteToUtf8(out_system->name());
			out_system_json["order_type"] = (int)out_system->order_type();
			out_system_json["seung_su"] = out_system->seung_su();
			out_system_json["account_no"] = out_system->account() ? out_system->account()->No() : "";
			out_system_json["symbol_code"] = out_system->symbol() ? out_system->symbol()->SymbolCode() : "";
			out_system_json["fund_name"] = out_system->fund() ? SmUtil::MultiByteToUtf8(out_system->fund()->Name()) : "";


			json jsonStrategy;
			jsonStrategy["type"] = out_system->strategy().type();

			// Serialize group_args vector
			for (const auto& group : out_system->strategy().group_args) {
				json jsonGroup;
				jsonGroup["name"] = group.name;

				// Serialize sys_args vector within the group
				for (const auto& sysArg : group.sys_args) {
					json jsonSysArg;
					jsonSysArg["enable"] = sysArg.enable;
					jsonSysArg["data_source1"] = sysArg.data_source1;
					jsonSysArg["data_source2"] = sysArg.data_source2;
					jsonSysArg["desc"] = sysArg.desc;
					jsonSysArg["param"] = sysArg.param;
					jsonSysArg["name"] = sysArg.name;
					jsonSysArg["current_value"] = sysArg.current_value;
					jsonSysArg["result"] = sysArg.result;

					jsonGroup["sys_args"].push_back(jsonSysArg);
				}

				jsonStrategy["group_args"].push_back(jsonGroup);
			}

			out_system_json["strategy"] = jsonStrategy;
			jsonData[std::to_string(out_system->id())] = out_system_json;
		}
		std::ofstream file(appPath);
		file << jsonData.dump(4);
		file.close();
	}

	void SmSaveManager::restore_usd_system(const std::string& filename)
	{
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\user\\"));
		appPath.append(id);

		appPath.append("\\");
		appPath.append(filename);
		std::ifstream file(appPath);
		if (!file.is_open()) {
			std::cerr << "Failed to open file for restore: " << filename << std::endl;
			return;
		}

		nlohmann::json jsonData;
		try {
			// Parse the JSON data from the file
			file >> jsonData;
		}
		catch (const nlohmann::json::parse_error& e) {
			std::cerr << "Failed to parse JSON file: " << e.what() << std::endl;
			file.close();
			return;
		}
		file.close();

		try {
			// Process the JSON data and reconstruct account information
			for (json::iterator it = jsonData.begin(); it != jsonData.end(); ++it) {
				std::string out_system_id = it.key();
				json out_system_data = it.value();

				// Extract and process account-related information
				const std::string name = SmUtil::Utf8ToMultiByte(out_system_data["name"]);
				const int t_order_type = out_system_data["order_type"];
				const int seung_su = out_system_data["seung_su"];
				DarkHorse::OrderType order_type = (DarkHorse::OrderType)t_order_type;
				const std::string account_no = out_system_data["account_no"];
				const std::string symbol_code = out_system_data["symbol_code"];
				const std::string fund_name = SmUtil::Utf8ToMultiByte(out_system_data["fund_name"]);
				auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
				auto account = mainApp.AcntMgr()->FindAccount(account_no);
				auto fund = mainApp.FundMgr()->FindFund(fund_name);
				int mode = 0;
				//if (!account || !symbol || !fund) continue;
				if (order_type == OrderType::MainAccount || order_type == OrderType::SubAccount) {
					if (!account) continue;
					mode = 0;
				}
				else if (order_type == OrderType::Fund) {
					if (!fund) continue;
					mode = 1;
				}
				if (!symbol) {
					continue;
				}

				json jsonStrategy = out_system_data["strategy"];
				std::string type_ = jsonStrategy["type"];
				SmUsdStrategy stratege;
				stratege.type(type_);
				std::vector<GroupArg> group_args;
				group_args.clear();

				if (jsonStrategy.contains("group_args") && jsonStrategy["group_args"].is_array()) {
					for (const auto& groupData : jsonStrategy["group_args"]) {
						GroupArg group;
						group.name = groupData["name"];

						if (groupData.contains("sys_args") && groupData["sys_args"].is_array()) {
							for (const auto& sysArgData : groupData["sys_args"]) {
								SysArg sysArg;
								sysArg.enable = sysArgData["enable"];
								sysArg.data_source1 = sysArgData["data_source1"];
								sysArg.data_source2 = sysArgData["data_source2"];
								sysArg.desc = sysArgData["desc"];
								sysArg.param = sysArgData["param"];
								sysArg.name = sysArgData["name"];
								sysArg.current_value = sysArgData["current_value"];
								sysArg.result = sysArgData["result"];

								group.sys_args.push_back(sysArg);
							}
						}

						group_args.push_back(group);
					}
					stratege.group_args = group_args;
				}

				auto out_system = mainApp.out_system_manager()->create_out_system(
					name,
					seung_su,
					order_type,
					mode == 0 ? account : nullptr,
					mode == 1 ? fund : nullptr,
					symbol
				);

				// Now you have the account information, you can use it as needed.
				// You can create objects or data structures to store this information.
				// For simplicity, we'll just print it here.
				std::cout << "out system id: " << out_system->id() << std::endl;
			}
		}
		catch (const json::parse_error& e) {
			std::cerr << "Failed to parse JSON data: " << e.what() << std::endl;
		}
	}

}
