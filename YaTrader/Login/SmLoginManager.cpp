#include "stdafx.h"
#include "SmLoginManager.h"
using namespace DarkHorse;

void SmLoginManager::SaveUserInfo(const std::string& id, const std::string& pwd, const std::string& cert)
{
	_id = id;
	_pwd = pwd;
	_cert = cert;
}

SmLoginManager::SmLoginManager()
{
	yuanta_server_list_.push_back("simul.tradar.api.com");
	yuanta_server_list_.push_back("real.tradar.api.com");
	yuanta_server_list_.push_back("simul.tradarglobal.api.com");
	yuanta_server_list_.push_back("real.tradarglobal.api.com");
}

SmLoginManager::~SmLoginManager()
{

}

void DarkHorse::SmLoginManager::SaveToXml(pugi::xml_node& login_info_node)
{
	pugi::xml_node login_info_child = login_info_node.append_child("login_id");
	login_info_child.append_child(pugi::node_pcdata).set_value(_id.c_str());
	login_info_child = login_info_node.append_child("login_password");
	login_info_child.append_child(pugi::node_pcdata).set_value(_pwd.c_str());
	login_info_child = login_info_node.append_child("login_cert");
	login_info_child.append_child(pugi::node_pcdata).set_value(_cert.c_str());
}

void DarkHorse::SmLoginManager::LoadFromXml(pugi::xml_node& login_info_node)
{
	_id = login_info_node.child_value("login_id");
	_pwd = login_info_node.child_value("login_password");
	_cert = login_info_node.child_value("login_cert");
}
