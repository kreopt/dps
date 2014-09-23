#ifndef DPS_H
#define DPS_H
#include <string>
#include <unordered_map>
#include <functional>

namespace dps {

    enum class Protocol {
        TCP,
        UDP,
        UNIX_SOCK
    };

    enum class SigParamType{
        Int,
        Uint,
        String,
        Double,
        Bool,
        Object,
        IntArray,
        UIntArray,
        StringArray,
        DoubleArray,
        BoolArray,
        ObjectArray,
        Raw
    };

    struct environment_info_t {
        std::string      uid;
        Protocol    protocol;
        std::string      address;
    };

    struct node_info_t {
        std::string      uid;
        std::string      env_uid;
    };

    struct signal_t {
        std::string      name;
        std::unordered_map<std::string, void*>   body;
    };

    enum class IndexType {
        Node,
        Signal,
        Parameter
    };

    typedef uint16_t  index_t;
    typedef index_t   node_index_t;
    typedef index_t   sig_index_t;
    typedef uint8_t   sig_param_index_t;

    enum class Operation: uint8_t {
        LT,
        GT,
        LTE,
        GTE,
        EQ,
        NE
    };
    const size_t OperationSize = 3;
    const size_t ParamIndexSize = sizeof(sig_param_index_t) * 8 - OperationSize;

    struct sig_param_t {
        Operation             op : OperationSize;
        sig_param_index_t     param_idx : ParamIndexSize;

        operator int() const {
            return (static_cast<sig_param_index_t>(this->op)<<dps::ParamIndexSize) | this->param_idx;
        }
    };
    inline size_t sig_param_hash( const sig_param_t &param ){
        return std::hash<int>()(static_cast<int>(param));
    };

    typedef const std::unordered_map<sig_param_t,variant_t,decltype(&sig_param_hash)>& sig_condition_t;
    typedef std::unordered_map<std::string,SigParamType> sig_param_list_t;

    struct condition_item_t{
        void*       value;
        std::vector<const node_info_t*> match_nodes;
    };

    //typedef vector<condition_item_t> condition_t;
}

#endif // DPS_H
