#pragma once
#include <string>
#include <vector>
#include "../Xml/pugixml.hpp"
namespace DarkHorse {
	class SmLoginManager
	{
	private:
		bool _IsLoggedIn = false;
		std::string _id;
		std::string _pwd;
		std::string _cert;
		bool _Save = false;
		std::vector<std::string> yuanta_server_list_;
		int ya_server_index_ = 0;
	public:
		SmLoginManager();
		~SmLoginManager();
		const std::vector<std::string>& yuanta_server_list() const { return yuanta_server_list_; }
		void SaveToXml(pugi::xml_node& node);
		void LoadFromXml(pugi::xml_node& node);

		std::string id() const { return _id; }
		std::string pwd() const { return _pwd; }
		std::string cert() const { return _cert; }
		bool IsLoggedIn() const { return _IsLoggedIn; }
		void IsLoggedIn(bool val) { _IsLoggedIn = val; }
		void SaveUserInfo(const std::string& id, const std::string& pwd, const std::string& cert);
		int ya_server_index() const { return ya_server_index_; }
		void ya_server_index(int val) { ya_server_index_ = val; }
	};
}

