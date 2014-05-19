#ifndef CONNECTIONGRAPH_TPL_HPP
#define CONNECTIONGRAPH_TPL_HPP
#include <functional>
namespace dps {
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
            op_record->second.op = op;
        }
        for (auto val_node: op_record->second.val_nodes){
            if (val_node.value == val){
                val_node.chains.insert(chain);
                return;
            }
        }
        ValueNode<T> val_node = {val, {chain}};
        op_record->second.val_nodes.insert(val_node);
        std::sort(op_record->second.val_nodes.begin(), op_record->second.val_nodes.end(), [](T a, T b){return a < b;});
    }

    template <typename T>
    void ParamNodeImpl<T>::remove_condition(chain_index_t chain){
        //TODO: implement
    }
}
#endif // CONNECTIONGRAPH_TPL_HPP
