//
// Created by rando on 1/19/24.
//

#include <iostream>
#include <iomanip>
#include "DummyNetworkInterface.h"

namespace lrhnet {
    DummyNetworkInterface::DummyNetworkInterface(int p_id, uint8_t* p_data, std::size_t p_length) : NetworkInterface(p_id), data(p_data), length(p_length){}

    bool DummyNetworkInterface::is_byte_available(){
        return length > 0;
    }
    bool DummyNetworkInterface::is_byte_available_wait(){
        return is_byte_available();
    }
    uint8_t DummyNetworkInterface::read_byte(){
        if (is_byte_available()){
            uint8_t val = *data;
            data += sizeof(uint8_t);
            --length;
            return val;
        }
        return 0x00;
    }
    uint8_t DummyNetworkInterface::read_byte_wait(){
        return read_byte();
    }
    void DummyNetworkInterface::empty_buffer() { }
    void DummyNetworkInterface::empty_buffer_wait() { }
    void DummyNetworkInterface::write_buffer(uint8_t* buffer, uint32_t buffer_size){
        std::cout << "Dummy Network Interface " << id << " sending frame: 0x";
        for (uint32_t i = 0; i < buffer_size; i++)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(buffer[i]);
        }
        std::cout << std::endl;
    }
} // lrhnet