#include <sstream>
#include <sqlite3x/sqlite3x.hpp>
#include "tablesubscriptionmanager.h"
#include "pal.h"

void dps::TableSubscriptionManager::cache_subscrption(dps::node_index_t subscriber_id, dps::sig_index_t signal_id, dps::sig_condition_t condition)
{
    // count condition frequencies
    vector<const value_t*>* prev = nullptr;
    condition_node_t* node = nullptr;
    for (auto condition_item: condition){
        if (this->condition_freq[signal_id].count(condition_item.first)<1){
            this->condition_freq[signal_id][condition_item.first]= new condition_node_t();
        } else {
            this->condition_freq[signal_id][condition_item.first]->freq++;
        }
        node = this->condition_freq[signal_id][condition_item.first];
        if (node->values.find(condition_item.second) == node->values.end()){
            node->values[condition_item.second] = vector<const value_t*>({});
            // bind operation?
        }
        if (prev){
            // add value reference to prev
            bool found = false;
            for (auto ref: *prev){
                if (ref->val == condition_item.second){
                    found = true;
                    break;
                }
            }
            if (!found){
                prev->push_back(new value_t({node,condition_item.second}));
            }
        }
        prev = &node->values[condition_item.second];

    }
    if (node && node->ref_nodes){
        node->ref_nodes=new vector<node_index_t>({subscriber_id});
    }
}

void dps::TableSubscriptionManager::cache_node(dps::node_index_t node_index, const dps::node_info_t &node)
{
    this->nodes[node_index] = node;
}

void dps::TableSubscriptionManager::cache_signal(dps::sig_index_t signal_id, dps::node_index_t publisher_id, const string &signal_name, const sig_param_list_t& params)
{
    if (this->signal_list.find(signal_id) == this->signal_list.end()){
        this->signal_list[signal_id] = pair<string,sig_param_list_t>(signal_name,sig_param_list_t());
    }
    this->signal_list[signal_id].second.insert(params.begin(), params.end());
    this->signal_publishers[signal_id].insert(publisher_id);
}

void dps::TableSubscriptionManager::forget_subscription(dps::node_index_t subscriber_id, dps::sig_index_t signal_id, dps::sig_condition_t condition)
{

}

void dps::TableSubscriptionManager::forget_node(dps::node_index_t node_index)
{
    this->nodes.erase(node_index);
}

void dps::TableSubscriptionManager::forget_signal(dps::sig_index_t sig_index)
{
    this->signal_list.erase(sig_index);
    this->signal_publishers.erase(sig_index);
}

dps::TableSubscriptionManager::TableSubscriptionManager()
{
    this->registry = new Registry();
}

dps::TableSubscriptionManager::~TableSubscriptionManager()
{
    delete this->registry;
}

dps::node_index_t dps::TableSubscriptionManager::register_node(const dps::node_info_t &node)
{
    this->registry->get_statement("insert or ignore into nodes(uid) values(?)",{node.uid}).exec();
    int node_id = this->registry->last_insert_id();

    if (!node_id){
        node_id = boost::get<int>(this->registry->get_statement("select id from nodes where uid = ?",{node.uid}).exec(SigParamType::Int));
    }

    return node_id;
}

dps::sig_index_t dps::TableSubscriptionManager::register_signal(dps::node_index_t publisher_id, const std::string &signal_name, const unordered_map<string,SigParamType>& params)
{
    this->registry->get_statement("insert or ignore into signals(name) values(?)",{signal_name}).exec();
    int signal_id = this->registry->last_insert_id();

    if (!signal_id){
        signal_id = boost::get<int>(this->registry->get_statement("select id from signals where name = ?",{signal_name}).exec(SigParamType::Int));
    }

    this->registry->get_statement("insert or ignore into signal_publishers(signal_id, publisher_id) values(?,?)",{signal_id, publisher_id}).exec();

    stringstream sql_stream;
    sql_stream << "insert or ignore into signal_params (signal_id, name, type) values";
    for (int i = params.size()-1; i > -1; i--){
        sql_stream <<"(?,?,?)";
        if (i>0){
            sql_stream << ",";
        }
    }
    vector<variant_t> param_list;
    for (auto param: params){
        param_list.push_back(signal_id);
        param_list.push_back(param.first);
        param_list.push_back(int(param.second));
    }
    this->registry->get_statement(sql_stream.str(), param_list).exec();
    return signal_id;
}

void dps::TableSubscriptionManager::deregister_node(dps::node_index_t node_id)
{
    this->registry->get_statement("delete from nodes where id = ?",{node_id}).exec();
}

void dps::TableSubscriptionManager::deregister_signal(dps::sig_index_t signal_id)
{
    this->registry->get_statement("delete from signals where id = ?",{signal_id}).exec();
}

dps::sig_index_t dps::TableSubscriptionManager::resolve_signal_index(const std::string &signal_name) const
{
    return boost::get<int>(this->registry->get_statement("select id from signals where name =  ? limit 1",{signal_name}).exec(SigParamType::Int));
}

dps::node_index_t dps::TableSubscriptionManager::resolve_node_index(const string &node_uid) const
{
    return boost::get<int>(this->registry->get_statement("select id from nodes where uid = ? limit 1",{node_uid}).exec(SigParamType::Int));
}

void dps::TableSubscriptionManager::subscribe(node_index_t subscriber_id, sig_index_t signal_id, sig_condition_t condition)
{
    // store to db
    this->registry->get_statement("insert into subscriptions(subscriber_id, signal_id) value(?,?)",{subscriber_id, signal_id}).exec();
    int subscription_id = this->registry->last_insert_id();

    stringstream sql_stream;
    sql_stream << "insert into subscription_conditions(subscription_id, relation, value) VALUES";
    for (int i=condition.size()-1;i>-1;i--){
        sql_stream <<"(?,?,?)";
        if (i>0){
            sql_stream << ",";
        }
    }
    vector<variant_t> param_list;
    for (auto &cond: condition){
        param_list.push_back(subscription_id);
        param_list.push_back(static_cast<int>(cond.first));
        param_list.push_back(cond.second);
    }
    this->registry->get_statement(sql_stream.str(), param_list).exec();
    // store to memory

    //this->connection_table[signal_id]=nullptr;
}

void dps::TableSubscriptionManager::unsubscribe(int subscription_id)
{
    this->registry->get_statement("delete from subscriptions where id = ?",{subscription_id}).exec();
}

vector<dps::node_info_t *> dps::TableSubscriptionManager::match(const signal_t &signal) const
{

}
