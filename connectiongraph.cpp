#include "connectiongraph.h"

using namespace dps;

ConnectionGraph::ConnectionGraph()
{
}

void ConnectionGraph::eval(const std::unordered_map<param_index_t, ParamValue> &param_val, std::unordered_set<subscriber_index_t> &subscribers)
{
    std::unordered_map<param_index_t, bool> eval_result;
    std::unordered_set<chain_index_t> excluded_chains;
    // first pass: find available chains
    for (auto &tier: param_graph_){
        for (auto &param: tier) {
            if (param.is_starter()){
                auto is_found = eval_result.find(param.id());
                if (is_found != eval_result.end()){
                    // don't eval this node again
                    continue;
                }
            }
            //eval node
            // FIXME: Dangerous code!
            if (param.type()==typeid(int)){
                ParamNodeImpl<int64_t> &int_node = dynamic_cast<ParamNodeImpl<int64_t>&>(param);
                auto v = param_val.find(param.id());
                int64_t *val = nullptr;
                if (v != param_val.end()){
                    val = const_cast<int64_t*>(&v->second.integer_);
                }
                int_node.eval(val, excluded_chains);
            } else if (param.type()==typeid(std::string)){
                ParamNodeImpl<char*> &string_node = dynamic_cast<ParamNodeImpl<char*>&>(param);
                char *val = nullptr;
                auto v = param_val.find(param.id());
                if (v != param_val.end()){
                    val = v->second.string_;
                }
                string_node.eval(&val,excluded_chains);
            }
            eval_result[param.id()] = true;
        }
    }
    // second pass: find subscribers
    std::unordered_set<chain_index_t> all_chains;
    for (auto &tier: param_graph_){
        for (auto &param: tier) {
            all_chains.insert(param.chains().begin(), param.chains().end());
        }
    }
    for (auto chain: all_chains){
        if (excluded_chains.find(chain)!=excluded_chains.end()){
            continue;
        }
        auto chain_subscribers = subscriber_map.find(chain);
        if (chain_subscribers!=subscriber_map.end()){
            subscribers.insert(chain_subscribers->second.begin(), chain_subscribers->second.end());
        }
    }
}

chain_index_t ConnectionGraph::add_connection(const std::unordered_map<param_index_t, std::pair<Operation, ParamValue> > &chain)
{

}

void ConnectionGraph::remove_connection(chain_index_t)
{

}
