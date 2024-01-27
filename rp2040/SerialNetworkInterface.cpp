//
// Created by rando on 1/19/24.
//

#include "SerialNetworkInterface.h"
#include "../core/util.h"


namespace lrhnet {
    SerialNetworkInterface::SerialNetworkInterface(int p_id, uart_inst_t* p_uart) : NetworkInterface(p_id), uart(p_uart){
        buffer_start = 0;
        buffer_end = 0;
        buffer = new uint8_t[READ_BUFFER_SIZE];
    }

    bool SerialNetworkInterface::is_byte_available(){
        if (buffer_start == buffer_end) empty_buffer();
        return buffer_start != buffer_end;
    }
    bool SerialNetworkInterface::is_byte_available_wait(){
        if (buffer_start == buffer_end) empty_buffer_wait();
        return buffer_start != buffer_end;
    }
    uint8_t SerialNetworkInterface::read_byte(){
        if (is_byte_available()){
            uint8_t byte = buffer[buffer_start++];
            buffer_start = buffer_start % READ_BUFFER_SIZE;
            return byte;
        }
        return 0x00;
    }
    uint8_t SerialNetworkInterface::read_byte_wait(){
        if (is_byte_available_wait()){
            uint8_t byte = buffer[buffer_start++];
            buffer_start = buffer_start % READ_BUFFER_SIZE;
            return byte;
        }
        return 0x00;
    }


    void SerialNetworkInterface::empty_buffer(){
        //std::cout << "emptying buffer for serial " << id << "." << std::endl;
        while(uart_is_readable(uart)){
            uint8_t byte = uart_getc(uart);
            buffer[buffer_end++] = byte;
            buffer_end = buffer_end % READ_BUFFER_SIZE;
        }
    }
    void SerialNetworkInterface::empty_buffer_wait(){
        while(uart_is_readable_within_us(uart, SERIAL_DELAY)){
            uint8_t byte = uart_getc(uart);
            buffer[buffer_end++] = byte;
            buffer_end = buffer_end % READ_BUFFER_SIZE;
        }
    }

    void SerialNetworkInterface::write_buffer(uint8_t* buffer, uint32_t buffer_size){
        //std::cout << "Writing " << buffer_size << " bytes to serial " << std::hex << std::setw(2) << std::setfill('0') << id << ": 0x";
        //for (uint32_t i = 0; i != buffer_size; i++)
        //{
        //    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(buffer[i]);
        //}
        //std::cout << std::endl;

        size_t written_count = 0;
        for (size_t i = 0; i < buffer_size; ++i) {
            if (written_count++ % WRITE_CHUNK_SIZE == 0){
                empty_buffers();
            }
            while (!uart_is_writable(uart))
                tight_loop_contents();
            uart_get_hw(uart)->dr = *buffer++;
        }
        for (int i = 0; i < network_interface_count; ++i) network_interfaces[i]->empty_buffer();
        for (int i = 0; i < network_interface_count; ++i) network_interfaces[i]->empty_buffer();
    }
} // lrhnet