#include "connectiongraph.h"

using namespace dps;

template<>
inline std::unordered_set<Operation> ParamNodeImpl<void*>::operation_set() const {
    return {Operation::Exists};
}

template<>
inline std::unordered_set<Operation> ParamNodeImpl<char*>::operation_set() const {
    return {Operation::Contains};
}

template<>
inline std::unordered_set<Operation> ParamNodeImpl<int64_t>::operation_set() const{
    return {Operation::Eq, Operation::Neq, Operation::Lt, Operation::Gt, Operation::Lte, Operation::Gte};
}

template<>
std::function<bool(const int64_t, const int64_t)> ParamNodeImpl<int64_t>::get_comparator(Operation op) const{
    std::function<bool(const int64_t&, const int64_t&)> comparator;
    switch (op){
    case Operation::Eq:
        comparator = [](const int64_t a, const int64_t b){return a == b;};
        break;
    case Operation::Neq:
        comparator = [](const int64_t a, const int64_t b){return a != b;};
        break;
    case Operation::Lt:
        comparator = [](const int64_t a, const int64_t b){return a < b;};
        break;
    case Operation::Gt:
        comparator = [](const int64_t a, const int64_t b){return a > b;};
        break;
    case Operation::Lte:
        comparator = [](const int64_t a, const int64_t b){return a <= b;};
        break;
    case Operation::Gte:
        comparator = [](const int64_t a, const int64_t b){return a >= b;};
        break;
    default:
        comparator = [](const int64_t , const int64_t ){return false;};
        break;
    }
    return comparator;
}

template<>
std::function<bool(const cstr, const cstr)> ParamNodeImpl<cstr>::get_comparator(Operation op) const{
    std::function<bool(const cstr, const cstr)> comparator;
    switch (op){
    case Operation::Contains:
        comparator = [](const cstr a, const cstr b){return std::string(a).find(b)!=std::string(a).npos;};
        break;
    default:
        comparator = [](const cstr , const cstr ){return false;};
    }
    return comparator;
}

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
