#ifndef TABLESUBSCRIPTIONMANAGER_H
#define TABLESUBSCRIPTIONMANAGER_H

#include <vector>
#include <unordered_map>
#include <map>
#include <utility>
#include <set>

#include "dps.h"
#include "isubscriptionmanager.h"
#include "registry.h"

using namespace std;
namespace dps {

    struct condition_node_t;

    struct value_t {
        condition_node_t* op;
        variant_t val;
    };

    struct condition_node_t {
        condition_node_t() : freq(1), ref_nodes(nullptr){}
        //sig_param_t             param;
        int     freq;
        map<variant_t,vector<const value_t*>>       values;
        vector<node_index_t>*   ref_nodes;
    };

    class TableSubscriptionManager : public ISubscriptionManager
    {
        unordered_map<sig_index_t, unordered_map<sig_param_index_t,vector<condition_item_t>>> connection_table;
        unordered_map<node_index_t, node_info_t>                    nodes;
        unordered_map<sig_index_t, pair<string,sig_param_list_t>>   signal_list;
        unordered_map<sig_index_t, set<sig_index_t>>                signal_publishers;
        unordered_map<sig_param_index_t,vector<condition_item_t>>   _state;


        unordered_map<sig_index_t, unordered_map<sig_param_t,condition_node_t*,decltype(&sig_param_hash)>>  condition_freq;

        unordered_map<sig_index_t, vector<const condition_node_t*>>        primary_nodes;
        unordered_map<sig_index_t, unordered_map<sig_param_t, const condition_node_t*,decltype(&sig_param_hash)>>  condition_nodes;


        vector<const node_info_t*> check_condition(sig_param_index_t param_condition, const void* value) const;

        Registry    *registry;

        void        cache_subscrption(node_index_t subscriber_id, sig_index_t signal_id, sig_condition_t condition);
        void        cache_node(node_index_t node_index, const node_info_t& node);
        void        cache_signal(sig_index_t signal_id, node_index_t publisher_id, const string& signal_name, const sig_param_list_t& params);
        void        sort_subcriptions();

        void        forget_subscription(node_index_t subscriber_id, sig_index_t signal_id, sig_condition_t condition);
        void        forget_node(node_index_t node_index);
        void        forget_signal(sig_index_t sig_index);

    public:
        TableSubscriptionManager();
        ~TableSubscriptionManager();

        virtual node_index_t    register_node(const node_info_t& node) override;
        virtual sig_index_t     register_signal(node_index_t publisher_id, const string& signal_name, const sig_param_list_t& params) override;

        virtual void            deregister_node(node_index_t node_id) override;
        virtual void            deregister_signal(sig_index_t signal_id) override;

        virtual sig_index_t     resolve_signal_index(const string& signal_name) const override;
        virtual node_index_t    resolve_node_index(const string& node_uid) const override;

        virtual void                    subscribe(node_index_t subscriber_id, sig_index_t signal_id, sig_condition_t condition) override;
        virtual void                    unsubscribe(int subscription_id) override;
        virtual vector<node_info_t*>    match(const signal_t& signal) const override;
    };

}
#endif // TABLESUBSCRIPTIONMANAGER_H
