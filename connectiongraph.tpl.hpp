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
        add_chain(chain);
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
    }

    template <typename T>
    void ParamNodeImpl<T>::remove_condition(chain_index_t chain){
        //TODO: implement
    }

    template<typename T>
    void ConnectionGraph::eval(const param_list_t<T> &param_val, std::unordered_set<subscriber_index_t> &subscribers)
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
                const auto node = std::dynamic_pointer_cast<ParamNodeImpl<T>>(param);
                auto v = param_val.find(param->id());
                node->eval(((v!=param_val.end()) ? &v->second : nullptr), excluded_chains);
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
}
#endif // CONNECTIONGRAPH_TPL_HPP
