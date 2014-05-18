#include "connectiongraph.h"

using namespace dps;

void ConnectionGraph::sort_graph()
{

}

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
            if (eval_result.find(param->id()) != eval_result.end()){
                // don't eval this node again
                continue;
            }
            if (param_val.find(param->id()) == param_val.end()){
                // mark as excluded cause parameter was not passed
                eval_result[param->id()] = false;
                for (auto chain: param->chains()){
                    excluded_chains.insert(chain);
                }
                continue;
            }
            //eval node
            // FIXME: Dangerous code!
            if (param->type()==typeid(int)){
                const ParamNodeImpl<int64_t> *int_node = dynamic_cast<const ParamNodeImpl<int64_t>*>(param);
                auto v = param_val.find(param->id());
                int64_t *val = nullptr;
                if (v != param_val.end()){
                    val = const_cast<int64_t*>(&v->second.integer_);
                }
                int_node->eval(val, excluded_chains);
            } else if (param->type()==typeid(std::string)){
                const ParamNodeImpl<char*> *string_node = dynamic_cast<const ParamNodeImpl<char*>*>(param);
                char *val = nullptr;
                auto v = param_val.find(param->id());
                if (v != param_val.end()){
                    val = v->second.string_;
                }
                string_node->eval(&val,excluded_chains);
            }
            eval_result[param->id()] = true;
        }
    }
    // second pass: find subscribers
    std::unordered_set<chain_index_t> all_chains;
    for (const auto &tier: param_graph_){
        for (auto &param: tier) {
            all_chains.insert(param->chains().begin(), param->chains().end());
        }
    }
    for (const auto &chain: all_chains){
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
    for (auto param: chain){
        if (params_.find(param.first) == params_.end()){
            // create new parameter
            params_[param.first]=ParamNodeImpl<>(param.first);
        }
        auto &param_node = params_[param.first];

    }
}

void ConnectionGraph::remove_connection(chain_index_t chain)
{
    for (auto param: params_){
        param.second.remove_chain(chain);
        if (param.second.chains().size() == 0){
            params_.erase(param.first);
        }
    }
    sort_graph();
}
