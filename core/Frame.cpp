//
// Created by andre on 1/17/2024.
//

#include "util.h"
#include <cstring>
#include <iostream>
#include <iomanip>

#include "Frame.h"

namespace lrhnet {
    uint32_t crc_polynomial = 0xEDB88320;

    Frame::Frame(uint32_t _msg_len, uint8_t *_msg) {
        flag = 0x7E;
        msg = _msg;
        msg_len = _msg_len;
        crc = 0xFFFFFFFF;
        calculate_crc();
    }

    void Frame::calculate_crc() {
        // Initialize CRC with the correct initial value
        crc = 0xFFFFFFFF;

        for (short i = sizeof(msg_len) - 1; i >= 0; --i) {
            unsigned byte = (msg_len >> (8 * i)) & 0x000000FF;            // Get next byte.
            crc = crc ^ byte;
            for (int j = 7; j >= 0; j--) {    // Do eight times.
                unsigned mask = -(crc & 1);
                crc = (crc >> 1) ^ (0xEDB88320 & mask);
            }
        }

        for (uint32_t i = 0; i < msg_len; ++i) {
            unsigned byte = msg[i];            // Get next byte.
            crc = crc ^ byte;
            for (int j = 7; j >= 0; j--) {    // Do eight times.
                unsigned mask = -(crc & 1);
                crc = (crc >> 1) ^ (0xEDB88320 & mask);
            }
        }

        // The final CRC value is the one's complement of the calculated CRC
        crc = ~crc;
    }

    uint32_t Frame::get_crc() const {
        return crc;
    }

    uint8_t *Frame::encode() {
        uint8_t buf[msg_len + 8];
        put_uint32_to_pointer(buf + 0, msg_len);
        std::memcpy(buf + 4, msg, (msg_len) * sizeof(uint8_t));
        put_uint32_to_pointer(buf + 4 + msg_len * sizeof(uint8_t), get_crc());

        int illegal_char_count = 0;
        for (int i = 0; i < msg_len + 8; i++) {
            if (buf[i] == 0x7E || buf[i] == 0x7D || buf[i] == 0x00) ++illegal_char_count;
        }

        auto *val = new uint8_t[msg_len + 10 + illegal_char_count];
        val[0] = 0x7E;
        val[msg_len + 9 + illegal_char_count] = 0x00;
        int counter = 1;
        for (int i = 0; i < msg_len + 8; i++) {
            if (buf[i] == 0x7E || buf[i] == 0x7D || buf[i] == 0x00) {
                val[counter++] = 0x7D;
                val[counter++] = buf[i] ^ 0x20;
            } else {
                val[counter++] = buf[i];
            }
        }

        return val;
    }

    void poll_interface(NetworkInterface *interface) {

    }

    void prepare_bytes_send_frame(uint8_t *message, uint32_t length, NetworkInterface *interface) {
        Frame f = Frame(length, message);
        send_frame(&f, interface);
    }

    void send_frame(Frame *f, NetworkInterface *interface) {
        uint8_t *e_f = f->encode();
        int l = 0;
        while (e_f[l] != 0x00) l++;
        l++;
        interface->write_buffer(e_f, l);
        //std::cout << "Interface " << interface->id << " sending frame: 0x";
        //for (uint32_t i = 0; i != l; i++)
        //{
        //    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(e_f[i]);
        //}
        //std::cout << " at " << std::hex << reinterpret_cast<void *>(e_f) << std::endl;
        delete[] e_f;
    }
}