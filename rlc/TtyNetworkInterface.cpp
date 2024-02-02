//
// Created by rando on 1/19/24.
//

#include <cstdio>
#include "TtyNetworkInterface.h"
#include "../core/util.h"


namespace lrhnet {
    TtyNetworkInterface::TtyNetworkInterface(int p_id, char* tty_name, int baud) : NetworkInterface(p_id){
        // TODO: open the serial port file from tty_name
        // TODO: set the speed to baud

    }

    bool TtyNetworkInterface::is_byte_available(){
        if (has_char){
            return true;
        }
        // TODO: make this read a character from the serial port
        //int result = getchar_timeout_us(0);
        int result = 5;
        //if (result == PICO_ERROR_TIMEOUT) return false;
        has_char = true;
        last_char = result;
        return true;
    }
    bool TtyNetworkInterface::is_byte_available_wait(){
        return is_byte_available();
    }
    uint8_t TtyNetworkInterface::read_byte(){
        if (is_byte_available()){
            has_char = false;
            return last_char;
        }
        return 0x00;
    }
    uint8_t TtyNetworkInterface::read_byte_wait(){
        return read_byte();
    }
    void TtyNetworkInterface::empty_buffer() { }
    void TtyNetworkInterface::empty_buffer_wait() { }

    void TtyNetworkInterface::write_buffer(uint8_t* buffer, uint32_t buffer_size){
        //std::cout << "Writing " << buffer_size << " bytes to usb." << std::endl;

        //TODO:  do the same thing as below, but for your open serial port
        //std::cout.write(reinterpret_cast<const char *>(buffer), buffer_size);

        //for (size_t i = 0; i < buffer_size; ++i) {
        //    putchar(buffer[i]);
        //    //uart_get_hw(uart)->dr = *buffer++;
        //}
    }
} // lrhnet