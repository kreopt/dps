#ifndef REGISTRYMANAGER_H
#define REGISTRYMANAGER_H
#include <string>
#include <unordered_map>

#include "dps.h"

namespace dps {
    using namespace std;

    class RegistryManager
    {
        // local nodes
        // external nodes
        // publisher signals
        // connections
        // external environments
        // last_id{node,signal,param}

        //IRegistry           *registry;
        unordered_map<index_t,unordered_map<string,index_t>>     index;

        void    sync();
    public:
        RegistryManager();
        ~RegistryManager();

        node_index_t     register_node(const node_info_t& publisher);
        void             deregister_node(node_index_t publisher_index);

        /**
         * @brief add_publisher_signal Add publisher link to signal registry
         * @param publisher_uid Emitter unique name
         * @param signal_signature Signal name
         */
        void    add_publisher_signal(node_index_t publisher_index, const string& signal_signature);
        /**
         * @brief remove_publisher_signal Remove publisher link from signal registry
         * @param publisher_uid Emitter unique name
         * @param signal_signature Signal name
         */
        void    remove_publisher_signal(node_index_t publisher_index, const string& signal_signature);

        /**
         * @brief add_connection Subscribe to signal
         * @param subscriber_uid
         * @param signal
         * @param condition
         */
        void    add_connection(node_index_t subscriber_index, const string& signal, const unordered_map<string,void*>& condition);
        /**
         * @brief remove_connection Remove subscription
         * @param subscriber_uid
         * @param signal
         * @param condition
         */
        void    remove_connection(node_index_t subscriber_index, const string& signal, const unordered_map<string,void*>& condition);
    };

};
#endif // REGISTRYMANAGER_H
