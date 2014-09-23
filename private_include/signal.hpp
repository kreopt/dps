#ifndef __DPS_SIGNAL__
#define __DPS_SIGNAL__

#include <string>
#include "buffer.hpp"

namespace dps {

    /// buffer with header
    class signal : public buffer {
    public:
        class header {

        };

        signal() = delete;
        signal(const std::string &name);
    private:
        header name_;
    };

    /// store data in shared memory with ref count
    class shared_signal : public signal {

    };

    void emit(auto x){x++;};
}

#endif