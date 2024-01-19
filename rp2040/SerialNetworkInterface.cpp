//
// Created by rando on 1/19/24.
//

#include "SerialNetworkInterface.h"

namespace lrhnet {
    SerialNetworkInterface::SerialNetworkInterface(int p_id, uart_inst_t* p_uart) : NetworkInterface(p_id), uart(p_uart){}

    bool SerialNetworkInterface::is_byte_available(){
        return length > 0;
    }
    bool SerialNetworkInterface::is_byte_available_wait(){
        return is_byte_available();
    }
    uint8_t SerialNetworkInterface::read_byte(){
        if (is_byte_available()){
            uint8_t val = *data;
            data += sizeof(uint8_t);
            --length;
            return val;
        }
        return 0x00;
    }
    uint8_t SerialNetworkInterface::read_byte_wait(){
        return read_byte();
    }
    void SerialNetworkInterface::write_buffer(uint8_t* buffer, uint32_t buffer_size){
        std::cout << "Dummy Network Interface " << id << " sending frame: 0x";
        for (uint32_t i = 0; i < buffer_size; i++)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(buffer[i]);
        }
        std::cout << std::endl;
    }
} // lrhnet