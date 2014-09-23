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
    using string_t = std::string;

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
    // TODO: variant type
    union ParamValue{
        int64_t     integer_;
        double      double_;
        string_t    string_;
        bool        raw_exists_;
    };


    template<typename T>
    using connection_t = std::unordered_map<dps::param_index_t,std::pair<dps::Operation,T>>;
    template<typename T>
    using connection_list_t = std::vector<connection_t<T>>;
    template<typename T>
    using param_list_t = std::unordered_map<dps::param_index_t, T>;
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

    template <typename T>
    struct ValueNode {
        T value;
        std::unordered_set<chain_index_t> chains;
    };

    template <typename T>
    inline bool value_node_compare(ValueNode<T> a, ValueNode<T> b){
        return a.value < b.value;
    }

    template <typename T>
    struct OperationNode {
        OperationNode():val_nodes(value_node_compare<T>){}
        Operation op;
        std::set<ValueNode<T>,bool(*)(ValueNode<T>, ValueNode<T>)> val_nodes;
    };

    class ParamNode {
        param_index_t   id_;
        std::unordered_set<chain_index_t> chains_;        
    public:
        ParamNode() = delete;
        virtual ~ParamNode(){}
        ParamNode(param_index_t id): id_(id){}
        inline param_index_t id() const {return id_;}
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
        ParamNodeImpl(param_index_t id): ParamNode(id){}
        std::unordered_set<Operation> operation_set() const;
        void add_condition(chain_index_t chain, Operation op, const T val);
        void remove_condition(chain_index_t chain);

        // should use dynamic_cast:(
        void eval(const T *value, std::unordered_set<chain_index_t> &excluded_chains) const;
    };
}
#if TEST_ENV
    class ConnectionGraphTest;
#endif
namespace dps {
    class ConnectionGraph
    {
        #if TEST_ENV
        friend class ::ConnectionGraphTest;
        #endif

        std::unordered_map<param_index_t, std::shared_ptr<dps::ParamNode>> params_;
        std::vector<std::vector<std::shared_ptr<dps::ParamNode>>> param_graph_;   //tiers of same weight nodes
        std::unordered_map<chain_index_t, std::unordered_set<subscriber_index_t>> subscriber_map;
        chain_index_t next_chain_index_;

        void sort_graph();
    public:
        ConnectionGraph();
        ~ConnectionGraph();

        template<typename T>
        void eval(const param_list_t<T> &param_val, std::unordered_set<dps::subscriber_index_t> &subscribers);

        // TODO: add connection for existing chain

        template<typename T>
        chain_index_t add_connection(const connection_t<T> &chain, subscriber_index_t subscriber_id, chain_index_t chain_id = 0);

        void remove_connection(chain_index_t);
    };
}
#include "connectiongraph.tpl.hpp"


#endif // CONNECTIONGRAPH_H
