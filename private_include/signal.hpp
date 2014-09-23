#ifndef __DPS_SIGNAL__
#define __DPS_SIGNAL__

#include <string>
#include <unordered_map>
#include <type_traits>
#include <cstdint>
#include "buffer.hpp"

namespace dps {

    class signal {
        enum class param_type {
            String,
            Double,
            Int,
            Raw
        };
        struct param_t {
            param_type type;
            size_t  length;
            void*   data;
        };
    public:
        struct header_t {
            std::string name;
        };

        signal() = delete;
        signal(const std::string &_name){
            header_.name = _name;
        };

        void set_param(const std::string& key, std::string& val) {
            payload_.emplace(key, {param_type::String, val.size(), val.c_str()});
        }
        void set_param(const std::string& key, double val) {
            payload_.emplace(key, {param_type::String, sizeof(double), new double(val)});
        }
        void set_param(const std::string& key, uint64_t val) {
            payload_.emplace(key, {param_type::String, sizeof(uint64_t), new uint64_t(val)});
        }
        void set_param(const std::string& key, const void* val, size_t size) {
            payload_.emplace(key, {param_type::String, size, val});
        }

    private:
        header_t header_;
        std::unordered_map<std::string, param_t> payload_;
    };

}

#endif