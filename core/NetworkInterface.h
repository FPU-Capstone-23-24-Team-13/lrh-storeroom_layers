//
// Created by rando on 1/17/24.
//

#ifndef LRHNET_NETWORKINTERFACE_H
#define LRHNET_NETWORKINTERFACE_H

#include <cstdint>

namespace lrhnet {
    class NetworkInterface {
    public:
        explicit NetworkInterface(int p_id);

        int id;

        virtual bool is_byte_available() = 0;

        virtual bool is_byte_available_wait() = 0;

        virtual uint8_t read_byte() = 0;

        virtual uint8_t read_byte_wait() = 0;

        virtual void empty_buffer() = 0;
        virtual void empty_buffer_wait() = 0;

        virtual void write_buffer(uint8_t *buffer, uint32_t buffer_size) = 0;
    };

} // lrhnet

#endif //LRHNET_NETWORKINTERFACE_H
