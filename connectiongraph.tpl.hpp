#ifndef CONNECTIONGRAPH_TPL_HPP
#define CONNECTIONGRAPH_TPL_HPP
#include <functional>
template<>
inline std::unordered_set<dps::Operation> dps::ParamNodeImpl<void*>::operation_set() {
    return {dps::Operation::Exists};
}

template<>
inline std::unordered_set<dps::Operation> dps::ParamNodeImpl<char*>::operation_set() {
    return {dps::Operation::Contains};
}

template<>
inline std::unordered_set<dps::Operation> dps::ParamNodeImpl<int64_t>::operation_set() {
    return {dps::Operation::Eq, dps::Operation::Neq, dps::Operation::Lt, dps::Operation::Gt, dps::Operation::Lte, dps::Operation::Gte};
}

template<>
std::function<bool(const int64_t, const int64_t)> dps::ParamNodeImpl<int64_t>::get_comparator(dps::Operation op){
    std::function<bool(const int64_t&, const int64_t&)> comparator;
    switch (op){
    case dps::Operation::Eq:
        comparator = [](const int64_t a, const int64_t b)->bool {return a == b;};
        break;
    case Operation::Neq:
        comparator = [](const int64_t a, const int64_t b)->bool {return a != b;};
        break;
    case dps::Operation::Lt:
        comparator = [](const int64_t a, const int64_t b)->bool {return a < b;};
        break;
    case dps::Operation::Gt:
        comparator = [](const int64_t a, const int64_t b)->bool {return a > b;};
        break;
    case dps::Operation::Lte:
        comparator = [](const int64_t a, const int64_t b)->bool {return a <= b;};
        break;
    case dps::Operation::Gte:
        comparator = [](const int64_t a, const int64_t b)->bool {return a >= b;};
        break;
    default:
        comparator = [](const int64_t , const int64_t )->bool {return false;};
        break;
    }
    return comparator;
}

template<>
std::function<bool(const dps::cstr, const dps::cstr)> dps::ParamNodeImpl<dps::cstr>::get_comparator(dps::Operation op){
    std::function<bool(const dps::cstr, const dps::cstr)> comparator;
    switch (op){
    case dps::Operation::Contains:
        comparator = [](const dps::cstr a, const dps::cstr b)->bool {return std::string(a).find(b)!=std::string(a).npos;};
        break;
    default:
        comparator = [](const dps::cstr , const dps::cstr )->bool {return false;};
    }
    return comparator;
}

template<typename T>
void dps::ParamNodeImpl<T>::eval(const T *value, std::unordered_set<chain_index_t> &excluded_chains){
    for (auto op: op_nodes){
        auto comparator=std::move(get_comparator(op.first));
        if (op.first==Operation::Exists){
            comparator=[](const T, const T)->bool {return true;};
        } else if (value==nullptr){
            comparator=[](const T, const T)->bool {return false;};
        }
        bool skip_next=false;
        for (auto &val: op.second.val_nodes){
            if (skip_next){
                excluded_chains.insert(val.chains.begin(), val.chains.end());
                continue;
            }
            // Here we assume that values are sorted in ascending order
            bool match = comparator(*value, val.value);
            if (match) {
                if (op.first==Operation::Gt || op.first==Operation::Gte){
                    break;
                } else {
                    skip_next=true;
                }
            } else {
                excluded_chains.insert(val.chains.begin(), val.chains.end());
                if (op.first==Operation::Lt || op.first==Operation::Lte){
                    break;
                }
            }
        }
    }
}

#endif // CONNECTIONGRAPH_TPL_HPP
