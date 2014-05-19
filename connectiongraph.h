#ifndef CONNECTIONGRAPH_H
#define CONNECTIONGRAPH_H
#include <list>
#include <vector>
#include <cstdint>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <functional>
#include <memory>
#include <algorithm>

namespace dps {
    using param_index_t = uint8_t;
    using chain_index_t = uint32_t;
    using subscriber_index_t = uint32_t;
    using cstr = char*;

    enum class Operation {
        Lt,
        Gt,
        Lte,
        Gte,
        Eq,
        Neq,
        Contains,
        Exists
    };
}
namespace std {
    template <>
        struct hash<dps::Operation>
        {
            size_t operator()(const dps::Operation& op) const
            {
                return hash<int>()(static_cast<int>(op));
            }
        };
}
namespace dps {

    union ParamValue{
        int64_t     integer_;
        double      double_;
        cstr        string_;
        bool        raw_exists_;        
    };

    template <typename T>
    struct ValueNode {
        T value;
        std::unordered_set<chain_index_t> chains;
    };

    template <typename T>
    struct OperationNode {
        Operation op;
        std::set<ValueNode<T>> val_nodes;    //should be sorted!
    };

    class ParamNode {
        param_index_t   id_;
        // TODO: Find better solution
        std::type_index type_;
        std::unordered_set<chain_index_t> chains_;        
    public:
        ParamNode() = delete;
        virtual ~ParamNode(){}
        ParamNode(param_index_t id, std::type_index type): id_(id), type_(type){}
        inline bool id() const {return id_;}
        inline std::type_index type() const {return type_;}
        inline std::unordered_set<chain_index_t> chains() const {return chains_;}
        inline void add_chain(chain_index_t chain){chains_.insert(chain);}
        inline void remove_chain(chain_index_t chain){chains_.erase(chain);}
    };

    template <typename T>
    class ParamNodeImpl : public ParamNode {
        std::unordered_map<Operation, OperationNode<T>>  op_nodes;
        std::function<bool(const T, const T)> get_comparator(Operation op) const;
    public:
        ParamNodeImpl() = delete;
        ParamNodeImpl(param_index_t id): ParamNode(id, typeid(T)){}
        std::unordered_set<Operation> operation_set() const;
        void add_condition(chain_index_t chain, Operation op, const T val);

        // should use dynamic_cast:(
        void eval(const T *value, std::unordered_set<chain_index_t> &excluded_chains) const;
    };
}
#include "connectiongraph.tpl.hpp"
namespace dps {
    class ConnectionGraph
    {
        std::unordered_map<param_index_t, std::shared_ptr<dps::ParamNode>> params_;
        std::vector<std::vector<std::shared_ptr<dps::ParamNode>>> param_graph_;   //tiers of same weight nodes
        std::unordered_map<chain_index_t, std::unordered_set<subscriber_index_t>> subscriber_map;
        chain_index_t next_chain_index_;

        void sort_graph();
    public:
        ConnectionGraph();
        ~ConnectionGraph();
        void eval(const std::unordered_map<dps::param_index_t, dps::ParamValue> &param_val, std::unordered_set<dps::subscriber_index_t> &subscribers);

        template<typename T>
        chain_index_t add_connection(const std::unordered_map<dps::param_index_t, std::pair<dps::Operation, T>> &chain){
            for (auto param: chain){
                if (params_.find(param.first) == params_.end()){
                    // create new parameter
                    params_.emplace(param.first, std::make_shared<ParamNodeImpl<T>>(new ParamNodeImpl<T>(param.first)));
                }
                auto &param_node = params_.at(param.first);
                param_node->add_condition(next_chain_index_, param.second.first, param.second.second);
            }
            sort_graph();
            return next_chain_index_++;
        }

        void remove_connection(chain_index_t);
    };
}



#endif // CONNECTIONGRAPH_H
