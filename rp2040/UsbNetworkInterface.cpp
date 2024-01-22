//
// Created by rando on 1/19/24.
//

#include <iostream>
#include <iomanip>
#include "UsbNetworkInterface.h"

namespace lrhnet {
    UsbNetworkInterface::UsbNetworkInterface(int p_id, uint8_t* p_data, std::size_t p_length) : NetworkInterface(p_id), data(p_data), length(p_length){}

    bool UsbNetworkInterface::is_byte_available(){
        return length > 0;
    }
    bool UsbNetworkInterface::is_byte_available_wait(){
        return is_byte_available();
    }
    uint8_t UsbNetworkInterface::read_byte(){
        if (is_byte_available()){
            uint8_t val = *data;
            data += sizeof(uint8_t);
            --length;
            return val;
        }
        return 0x00;
    }
    uint8_t UsbNetworkInterface::read_byte_wait(){
        return read_byte();
    }
    void UsbNetworkInterface::write_buffer(uint8_t* buffer, uint32_t buffer_size){
        std::cout.write(reinterpret_cast<char*>(buffer), buffer_size);
    }
} // lrhnet