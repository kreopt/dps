#ifndef CONNECTIONGRAPH_TPL_HPP
#define CONNECTIONGRAPH_TPL_HPP
#include <functional>
namespace dps {
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

    template<typename T>
    void ParamNodeImpl<T>::eval(const T *value, std::unordered_set<chain_index_t> &excluded_chains) const{
        for (auto op: op_nodes){
            auto comparator=std::move(get_comparator(op.first));
            if (op.first==Operation::Exists){
                comparator=[](const T, const T){return true;};
            } else if (value==nullptr){
                comparator=[](const T, const T){return false;};
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

    template <typename T>
    void ParamNodeImpl<T>::add_condition(chain_index_t chain, Operation op, const T val){
        auto op_record = op_nodes.find(op);
        if (op_record==op_nodes.end()){
            auto res = op_nodes.emplace(op, OperationNode<T>());
            op_record = res.first;
            op_record.op = op;
        }
        for (auto val_node: op_record.val_nodes){
            if (val_node.value == val){
                val_node.chains.insert(chain);
                return;
            }
        }
        ValueNode<T> val_node = {val, {chain}};
        op_record.val_nodes.insert(val_node);
        std::sort(op_record.val_nodes.begin(), op_record.val_nodes.end(), [](T a, T b){return a < b;});
    }
}
#endif // CONNECTIONGRAPH_TPL_HPP
