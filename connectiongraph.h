#ifndef CONNECTIONGRAPH_H
#define CONNECTIONGRAPH_H
#include <list>
#include <vector>
#include <cstdint>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>

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
        uint8_t         weight_;
        bool            is_chain_start_;
        // TODO: Find better solution
        std::type_index type_;        
        std::unordered_set<chain_index_t> chains_;
    public:        
        ParamNode() = delete;
        virtual ~ParamNode(){}
        ParamNode(param_index_t id, std::type_index type): id_(id), weight_(0), is_chain_start_(false), type_(type){}
        inline bool is_starter(){return is_chain_start_;}
        inline bool id(){return id_;}
        inline std::type_index type(){return type_;}
        inline std::unordered_set<chain_index_t> chains(){return chains_;}
    };

    template <typename T>
    class ParamNodeImpl : public ParamNode {
        std::unordered_map<Operation, OperationNode<T>>  op_nodes;
        std::function<bool(const T, const T)> get_comparator(Operation op);
    public:
        ParamNodeImpl() = delete;
        ParamNodeImpl(param_index_t id): ParamNode(id, typeid(T)){}
        std::unordered_set<Operation> operation_set();

        // should use dynamic_cast:(
        void eval(const T *value, std::unordered_set<chain_index_t> &excluded_chains);
    };
}
#include "connectiongraph.tpl.hpp"
namespace dps {
    class ConnectionGraph
    {
        std::vector<std::vector<dps::ParamNode>> param_graph_;   //tiers of same weight nodes
        std::unordered_map<chain_index_t, std::unordered_set<subscriber_index_t>> subscriber_map;
    public:
        ConnectionGraph();
        void eval(const std::unordered_map<dps::param_index_t, dps::ParamValue> &param_val, std::unordered_set<dps::subscriber_index_t> &subscribers);
        chain_index_t add_connection(const std::unordered_map<dps::param_index_t, std::pair<dps::Operation, dps::ParamValue>> &chain);
        void remove_connection(chain_index_t);
    };
}



#endif // CONNECTIONGRAPH_H
