#include "connectiongraph.h"
#include <iostream>
using namespace dps;

template<>
inline std::unordered_set<Operation> ParamNodeImpl<void*>::operation_set() const {
    return {Operation::Exists};
}

template<>
inline std::unordered_set<Operation> ParamNodeImpl<string_t>::operation_set() const {
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
std::function<bool(const string_t, const string_t)> ParamNodeImpl<string_t>::get_comparator(Operation op) const{
    std::function<bool(const string_t, const string_t)> comparator;
    switch (op){
    case Operation::Contains:
        comparator = [](const string_t a, const string_t b){return std::string(a).find(b)!=std::string(a).npos;};
        break;
    default:
        comparator = [](const string_t , const string_t ){return false;};
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

void ConnectionGraph::sort_graph()
{
    param_graph_.clear();    
    std::multimap<int32_t, std::shared_ptr<dps::ParamNode>> weight_map;
    for (auto param_record: params_){
        weight_map.emplace(param_record.second->chains().size(), param_record.second);
    }
    for(auto it = weight_map.begin(), end = weight_map.end(); it != end; it = weight_map.upper_bound(it->first)) {
        auto range = weight_map.equal_range(it->first);
        std::vector<std::shared_ptr<dps::ParamNode>> tier;
        for (auto it=range.first; it != range.second; it++){
            tier.push_back(it->second);
        }
        param_graph_.insert(param_graph_.begin(), tier);
    }
}

void ConnectionGraph::remove_connection(chain_index_t chain)
{
    for (auto param=params_.begin(); param!=params_.end();){
        param->second->remove_chain(chain);
        auto old_p=param;
        param++;
        if (old_p->second->chains().size() == 0){
            params_.erase(old_p->first);
        }
    }    
    sort_graph();
}
