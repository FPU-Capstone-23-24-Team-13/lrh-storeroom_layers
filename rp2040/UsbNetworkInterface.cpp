//
// Created by rando on 1/19/24.
//

#include "cstdio"
#include "UsbNetworkInterface.h"
#include "pico/stdlib.h"
#include "../core/util.h"


namespace lrhnet {
    UsbNetworkInterface::UsbNetworkInterface(int p_id) : NetworkInterface(p_id), has_char(false), last_char(0){}

    bool UsbNetworkInterface::is_byte_available(){
        if (has_char){
            return true;
        }
        int result = getchar_timeout_us(0);
        if (result == PICO_ERROR_TIMEOUT) return false;
        has_char = true;
        last_char = result;
        return true;
    }
    bool UsbNetworkInterface::is_byte_available_wait(){
        if (has_char){
            return true;
        }
        int result = getchar_timeout_us(SERIAL_DELAY);
        if (result == PICO_ERROR_TIMEOUT) return false;
        has_char = true;
        last_char = result;
        return true;
    }
    uint8_t UsbNetworkInterface::read_byte(){
        if (is_byte_available()){
            has_char = false;
            return last_char;
        }
        return 0x00;
    }
    uint8_t UsbNetworkInterface::read_byte_wait(){
        if (is_byte_available_wait()){
            has_char = false;
            return last_char;
        }
        return 0x00;
    }
    void UsbNetworkInterface::empty_buffer() { }
    void UsbNetworkInterface::empty_buffer_wait() { }
    void UsbNetworkInterface::write_buffer(uint8_t* buffer, uint32_t buffer_size){
        //std::cout << "Writing " << buffer_size << " bytes to usb." << std::endl;
        //std::cout.write(reinterpret_cast<const char *>(buffer), buffer_size);
        size_t written_count = 0;
        for (size_t i = 0; i < buffer_size; ++i) {
            if (written_count++ % WRITE_CHUNK_SIZE == 0){
                empty_buffers();
            }
            putchar(buffer[i]);
            //uart_get_hw(uart)->dr = *buffer++;
        }
        for (int i = 0; i < network_interface_count; ++i) network_interfaces[i]->empty_buffer();
        for (int i = 0; i < network_interface_count; ++i) network_interfaces[i]->empty_buffer();
    }
} // lrhnet