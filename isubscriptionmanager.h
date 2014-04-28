#ifndef ISUBSCRIPTIONMANAGER_H
#define ISUBSCRIPTIONMANAGER_H
#include <string>
#include <vector>
#include <unordered_map>
#include <exception>
#include "dps.h"

namespace dps {

    class RecordNotFound: public exception{};
    class ISubscriptionManager
    {
    public:
        virtual ~ISubscriptionManager() {}
        /**
         * @brief register_node stores node info in registry
         * @param node node to store
         * @return index of node in registry
         */
        virtual node_index_t    register_node(const node_info_t& node) = 0;
        /**
         * @brief register_signal stores signal in registry and binds it to publisher's id
         * @param publisher_id id of publisher exporting signal
         * @param signal_name name of signal to export
         * @return index of signal in registry
         */
        virtual sig_index_t     register_signal(node_index_t publisher_id, const string& signal_name, const sig_param_list_t& params) = 0;

        /**
         * @brief deregister_node removes node from registry and unbinds all relative data
         * @param node_id id of node to remove
         */
        virtual void            deregister_node(node_index_t node_id) = 0;
        /**
         * @brief deregister_signal removes signal from registry and unbinds all relative data
         * @param signal_id id of signal to remove
         */
        virtual void            deregister_signal(sig_index_t signal_id) = 0;

        /**
         * @brief resolve_signal_index get signal index by name
         * @param signal_name name of signal to resolve
         * @return index of signal in registry
         * @throws RecordNotFound if no record in registry
         */
        virtual sig_index_t     resolve_signal_index(const string& signal_name) const = 0;
        /**
         * @brief resolve_node_index get node index by uid
         * @param node_uid uid of node to resolve
         * @return index of node in registry
         * @throws RecordNotFound if no record in registry
         */
        virtual node_index_t    resolve_node_index(const string& node_uid) const = 0;

        /**
         * @brief subscribe
         * @param subscriber_id
         * @param signal_name
         * @param condition
         */
        virtual void                    subscribe(node_index_t subscriber_id, sig_index_t signal_id, sig_condition_t condition) = 0;
        /**
         * @brief unsubscribe
         * @param subscription_id
         */
        virtual void                    unsubscribe(int subscription_id) = 0;
        /**
         * @brief match
         * @param signal
         * @return
         */
        virtual vector<node_info_t*>    match(const signal_t& signal) const = 0;
    };
}
#endif // ISUBSCRIPTIONMANAGER_H
