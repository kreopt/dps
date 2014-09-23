#ifndef __DPS_ISENDER__
#define __DPS_ISENDER__
#include <kcommon/buffer.hpp>
#include <stdexcept>
namespace dps {
    namespace net {
        enum class sender_type {
            Tcp,
            Unix
        };

        class net_error: public std::runtime_error {
            std::string reason_;
        public:
            net_error() = delete;
            explicit net_error(const std::string &reason): reason_(reason){}

            virtual const char* what() const override {
                return reason_.c_str();
            }
        };

        class sender {
        public:
            /// send buffer. throws net_error
            virtual void send(const KCommon::Buffer &buf) const;
        };

        std::shared_ptr<sender> create_sender(type) {
            switch (type){
                case sender_type::Tcp:
                    return;
                case sender_type::Unix:
                    return;
                default:
                    throw new std::logic_error("Unimplemented");
            }
        }
    }
}
#endif