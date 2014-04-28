#include "registrymanager.h"

dps::RegistryManager::RegistryManager()
{
//    this->registry = new QSettingsRegistry();
}

dps::RegistryManager::~RegistryManager()
{
  //  delete this->registry;
}

dps::node_index_t dps::RegistryManager::register_node(const node_info_t &publisher)
{

}

void dps::RegistryManager::deregister_node(node_index_t publisher_index)
{

}

void dps::RegistryManager::add_publisher_signal(node_index_t publisher_index, const string &signal_signature)
{

}

void dps::RegistryManager::remove_publisher_signal(node_index_t publisher_index, const string &signal_signature)
{

}

void dps::RegistryManager::add_connection(node_index_t subscriber_index, const string &signal, const unordered_map<string,void*> &condition)
{

}

void dps::RegistryManager::remove_connection(node_index_t subscriber_index, const string &signal, const unordered_map<string, void*> &condition)
{

}
