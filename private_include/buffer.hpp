#ifndef __DPS_BUFFER__
#define __DPS_BUFFER__

#include <cstdint>
#include <memory>

namespace dps {
    class buffer {
        std::shared_ptr<uint8_t> data_;
        size_t size_;
    public:
        virtual ~buffer(){}
        buffer() : data_(nullptr), size_(0){};
        buffer(const buffer& buf) : data_(buf.data_), size_(buf.size_) {};
        buffer(buffer&& buf) : data_(std::move(buf.data_)), size_(std::move(buf.size_)) {
            buf.data_ = nullptr;
            buf.size_ = 0;
        };
        buffer(void* raw, size_t size) : data_(std::shared_ptr<uint8_t>(reinterpret_cast<uint8_t*>(raw))), size_(size) {};

        buffer& operator=(const buffer& buf){
            data_ = buf.data_;
            size_ = buf.size_;
            return *this;
        }

        buffer& operator=(buffer&& buf){
            data_ = std::move(buf.data_);
            size_ = std::move(buf.size_);
            buf.data_ = nullptr;
            buf.size_ = 0;
            return *this;
        }

        virtual std::shared_ptr<uint8_t> data() const {
            return data_;
        };
    };
};
#endif