#include <iostream>
#include <iomanip>
#include "core/Frame.h"
#include "core/Packet.h"
#include "core/util.h"
#include "rlc/TtyNetworkInterface.h"
//basic tester with is_byte_available_wait()
void poll_with_wait(lrhnet::NetworkInterface *interface) {
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

    lrhnet::Frame f = lrhnet::Frame(message_length, frame_message);

    uint32_t checksum = 0;
    for (int i = sizeof(uint32_t) - 1; i >= 0; i--){
        checksum |= ((uint32_t)read_escaped(interface)) << (8*i);
    }

    // uint8_t *e_f = f.encode();
    //int l = 0;
    //while (e_f[l] != 0x00) l++;
    //l++;
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

    //delete[] e_f;

    delete[] frame_message;
}
int main() {
    lrhnet::device_id = 0x0000000000000000;
    std::cout << "Generating and sending a test packet." << std::endl;
    lrhnet::NetworkInterface* ni[] = {
            new lrhnet::TtyNetworkInterface(0, const_cast<char*>("/dev/ttyACM0"), 115200)
    };
    std::cout << "Successfully created TTY Interface." << std::endl;
    lrhnet::network_interfaces = ni;
    lrhnet::network_interface_count = 1;

    for (int i = 0; i < 256; ++i){
        lrhnet::port_callbacks[i] = lrhnet::debug_port_callback;
    }
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true){
        poll_with_wait(ni[0]);
    }
#pragma clang diagnostic pop
}
