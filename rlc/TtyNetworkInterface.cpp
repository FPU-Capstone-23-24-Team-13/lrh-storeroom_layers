//
// Created by rando on 1/19/24.
//

#include <cstdio>
#include "TtyNetworkInterface.h"
#include "../core/util.h"


namespace lrhnet {
    TtyNetworkInterface::TtyNetworkInterface(int p_id, char* tty_name, int baud) : NetworkInterface(p_id){
        // TODO: Open the serial port file from tty_name. See this section of the mbedded.ninja guide for details:
        //  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#basic-setup-in-c
        // TODO: Set the speed to baud. See these sections of the mbedded.ninja guide for details:
        //  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#configuration-setup
        //  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#control-modes-c_cflags
        //  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#local-modes-c_lflag
        //  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#input-modes-c_iflag
        //  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#output-modes-c_oflag
        //  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#vmin-and-vtime-c_cc
        //  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#baud-rate
        //  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#saving-termios
        //  I recommend the following settings (these basically go in the order of the guide):
        //      No parity
        //      One stop bit
        //      8 bits per byte
        //      No hardware flow control
        //      CREAD and CLOCAL set
        //      Disable canonical mode
        //      Disable all echos (echo, erasure, new line)
        //      Disable signal characters
        //      Disable software flow control
        //      Disable special handling of bytes
        //      Prevent conversions of output characters
        //      VMIN 0 and VTIME 0 (read returns immediately)
        //      Baud 921600 (or 460800 if 921600 not possible easily)

    }

    bool TtyNetworkInterface::is_byte_available(){
        // TODO: Make this check for characters in the serial port. Check the mbedded.ninja guide:
        //  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#getting-the-number-of-rx-bytes-available
        //if (has_char){
        //    return true;
        //}
        //int result = getchar_timeout_us(0);
        //int result = 5;
        //if (result == PICO_ERROR_TIMEOUT) return false;
        //has_char = true;
        //last_char = result;
        return true;
    }

    bool TtyNetworkInterface::is_byte_available_wait(){
        // TODO: Maybe make this better by re-running is_byte_available() after a brief delay, or even better by
        //  checking is_byte_available() every x many ms up to y total ms. Use the TTY_DELAY macro for the total time
        //  (defined inthe .h file)
        return is_byte_available();
    }

    uint8_t TtyNetworkInterface::read_byte(){
        if (is_byte_available()){
            // TODO: Make this read a single character from the serial port, instead of using the has_char/last_char
            //  system. I'm not sure if reading a single byte with read(...) or getc(...) is better. For details on
            //  both, check these resources (remember the serial port is just a file with an open file stream, so normal
            //  file operations like getc *should* work):
            //  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#reading
            //  https://en.cppreference.com/w/c/io/fgetc
            //has_char = false;
            //return last_char;
        }
        return 0x00;
    }
    uint8_t TtyNetworkInterface::read_byte_wait(){
        if (is_byte_available_wait()){
            // TODO: Do whatever you do in read byte, but use is_byte_available_wait (already done above).
        }
        return 0x00;
    }
    void TtyNetworkInterface::empty_buffer() { }
    void TtyNetworkInterface::empty_buffer_wait() { }

    void TtyNetworkInterface::write_buffer(uint8_t* buffer, uint32_t buffer_size){
        //std::cout << "Writing " << buffer_size << " bytes to usb." << std::endl;

        //TODO: Do the same thing as below, but for your open serial port. See the mbedded.ninja guide for details:
        // https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#writing
        // It's very similar to what's here, just c syntax not c++.
        //std::cout.write(reinterpret_cast<const char *>(buffer), buffer_size);

        //for (size_t i = 0; i < buffer_size; ++i) {
        //    putchar(buffer[i]);
        //    //uart_get_hw(uart)->dr = *buffer++;
        //}
    }
} // lrhnet