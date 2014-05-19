#include "connectiongraph.h"

using namespace dps;

ConnectionGraph::ConnectionGraph(): next_chain_index_(1)
{
}

ConnectionGraph::~ConnectionGraph()
{
    param_graph_.clear();
    params_.clear();
}

void ConnectionGraph::eval(const std::unordered_map<param_index_t, ParamValue> &param_val, std::unordered_set<subscriber_index_t> &subscribers)
{
    std::unordered_map<param_index_t, bool> eval_result;
    std::unordered_set<chain_index_t> excluded_chains;
    // first pass: find available chains
    for (auto tier: param_graph_){
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
                const auto int_node = std::dynamic_pointer_cast<ParamNodeImpl<int64_t>>(param);
                auto v = param_val.find(param->id());
                int64_t *val = nullptr;
                if (v != param_val.end()){
                    val = const_cast<int64_t*>(&v->second.integer_);
                }
                int_node->eval(val, excluded_chains);
            } else if (param->type()==typeid(std::string)){
                const auto string_node = std::dynamic_pointer_cast<ParamNodeImpl<char*>>(param);
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

void ConnectionGraph::sort_graph()
{
    param_graph_.clear();    
    std::multimap<int32_t, std::shared_ptr<dps::ParamNode>> weight_map;
    for (auto param_record: params_){
        weight_map.emplace(param_record.second->chains().size(), param_record.second);
    }
    for (auto weight_record: weight_map){
        auto range = weight_map.equal_range(weight_record.first);
        std::vector<std::shared_ptr<dps::ParamNode>> tier;
        for (auto it=range.first; it != range.second; it++){
            tier.push_back(it->second);
        }
        param_graph_.push_back(tier);
    }
}

void ConnectionGraph::remove_connection(chain_index_t chain)
{
    for (auto param: params_){
        param.second->remove_chain(chain);
        if (param.second->chains().size() == 0){
            params_.erase(param.first);
        }
    }
    sort_graph();
}
