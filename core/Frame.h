//
// Created by Andrew on 1/17/2024.
//

#ifndef NETWORKTESTER_FRAME_H
#define NETWORKTESTER_FRAME_H

#include <iostream>
#include <cstdint>
#include "NetworkInterface.h"

namespace lrhnet {
    class Frame {
    public:
        Frame(uint32_t _msg_len, uint8_t *_msg);

        void calculate_crc();

        [[nodiscard]] uint32_t get_crc() const;

        uint8_t *encode();

    private:
        uint8_t flag;
        uint32_t msg_len;
        uint8_t *msg;
        uint32_t crc;
    };

    void poll_interface(NetworkInterface *interface);

    void prepare_bytes_send_frame(uint8_t *message, uint32_t length, NetworkInterface *interface);

    void send_frame(Frame *f, NetworkInterface *interface);

#endif //NETWORKTESTER_FRAME_H
}