//
// Created by rando on 1/19/24.
//

#include "SerialNetworkInterface.h"

#define SERIAL_DELAY 10

namespace lrhnet {
    SerialNetworkInterface::SerialNetworkInterface(int p_id, uart_inst_t* p_uart) : NetworkInterface(p_id), uart(p_uart){}

    bool SerialNetworkInterface::is_byte_available(){
        return uart_is_readable(uart);
    }
    bool SerialNetworkInterface::is_byte_available_wait(){
        return uart_is_readable_within_us(uart, SERIAL_DELAY);
    }
    uint8_t SerialNetworkInterface::read_byte(){
        if (is_byte_available()){
            return uart_getc(uart);
        }
        return 0x00;
    }
    uint8_t SerialNetworkInterface::read_byte_wait(){
        return read_byte();
    }
    void SerialNetworkInterface::write_buffer(uint8_t* buffer, uint32_t buffer_size){
        uart_write_blocking(uart, buffer, buffer_size);
    }
} // lrhnet