//
// Created by andre on 1/17/2024.
//

#include "util.h"
#include <cstring>
#include <iostream>
#include <iomanip>

#include "Frame.h"
#include "Packet.h"
//#include <pico/time.h>

namespace lrhnet {
    uint32_t crc_polynomial = 0xEDB88320;

    Frame::Frame(uint32_t _msg_len, uint8_t *_msg) {
        flag = FRAME_START;
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
            if (buf[i] == FRAME_START || buf[i] == FRAME_ESCAPE || buf[i] == 0x00) ++illegal_char_count;
        }

        auto *val = new uint8_t[msg_len + 10 + illegal_char_count];
        val[0] = FRAME_START;
        val[msg_len + 9 + illegal_char_count] = 0x00;
        int counter = 1;
        for (int i = 0; i < msg_len + 8; i++) {
            if (buf[i] == FRAME_START || buf[i] == FRAME_ESCAPE || buf[i] == 0x00) {
                val[counter++] = FRAME_ESCAPE;
                val[counter++] = buf[i] ^ 0x20;
            } else {
                val[counter++] = buf[i];
            }
        }

        return val;
    }

    uint8_t read_escaped(NetworkInterface *interface) {
        uint8_t received_byte = interface->read_byte_wait();
        for (int i = 0; i < 20 && received_byte == 0x00; ++i){
            received_byte = interface->read_byte_wait();
        }
        //TODO: Figure out a way to reset the whole frame processing process if 7E is received? (maybe an "illegal" goto? maybe return some value and make the later code handle the jump?)
        if (received_byte == FRAME_ESCAPE){
            received_byte = interface->read_byte_wait() ^ 0x20;
        }
        //std::cout << "Received unescaped byte: " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(received_byte) << std::endl;
        return received_byte;
    }

    void poll() {
        for (int i = 0; i < network_interface_count; i++){
            //std::cout << "===============================================================================" << std::endl;
            poll_interface(network_interfaces[i]);
        }
    }

    void poll_interface(NetworkInterface *interface) {
        bool frame_started = false;

        // read the buffer while it is full, emptying it until a start of frame is found
        //std::cout << "polling interface " << interface->id << ", has byte available: " << interface->is_byte_available() << std::endl;
        while (interface->is_byte_available()){
            uint8_t start_byte = interface->read_byte();
            //std::cout << "Checking byte: " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(start_byte) << std::endl;
            if (start_byte == FRAME_START){
                frame_started = true;
                break;
            }
        }

        // if the buffer was emptied, stop here
        if (!frame_started){
            return;
        }

    //frame_reset:  // where to put the label to reset everything, should we want to use it.
        //std::cout << "===============================================================================" << std::endl;
        //std::cout << "Reading interface " << interface->id << std::endl;

        uint32_t message_length = 0;
        for (int i = sizeof(uint32_t) - 1; i >= 0; i--){
            message_length |= ((uint32_t)read_escaped(interface)) << (8*i);
        }

        //std::cout << "message has a length of: " << message_length << std::endl;

        auto* frame_message = new uint8_t[message_length];

        for (int i = 0; i < message_length; i++){
            frame_message[i] = read_escaped(interface);
        }

        Frame f = Frame(message_length, frame_message);

        uint32_t checksum = 0;
        for (int i = sizeof(uint32_t) - 1; i >= 0; i--){
            checksum |= ((uint32_t)read_escaped(interface)) << (8*i);
        }

        uint8_t *e_f = f.encode();
        int l = 0;
        while (e_f[l] != 0x00) l++;
        l++;
        //std::cout << "Interface " << interface->id << " received frame: 0x";
        //for (uint32_t i = 0; i != l; i++)
        //{
        //    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(e_f[i]);
        //}
        //std::cout << std::endl;

        //std::cout << "Recv. CRC: 0x" << std::hex << checksum << std::endl;
        //std::cout << "Calc. CRC: 0x" << std::hex << f.get_crc() << std::endl;

        if (checksum == f.get_crc()){
            //std::cout << "Checksums match, processing further." << std::endl;
            process_packet_bytes(f.msg, f.msg_len, interface);
        }
        else {
            //std::cout << "Checksum fail!" << std::endl;
        }

        delete[] frame_message;
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
        //std::cout << std::endl;

        delete[] e_f;
    }
}