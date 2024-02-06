//
// Created by rando on 1/19/24.
//

#include <cstdio>
#include <chrono>
#include <thread>
#include "TtyNetworkInterface.h"
#include "../core/util.h"
#include <iostream>

namespace lrhnet {
    TtyNetworkInterface::TtyNetworkInterface(int p_id, char* tty_name, int baud) : NetworkInterface(p_id){
        // Open serial port
        serialHandle = open(tty_name, O_RDWR);
        // Check for error
        if (serialHandle < 0) {
            printf("Error %i from open: %s\n", errno, strerror(errno));
        }
        // Create new termios struct, we call it 'tty' for convention
        // No need for "= {0}" at the end as we'll immediately write the existing
        // config to this struct
        struct termios tty;

        // Read in existing settings, and handle any error
        // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
        // must have been initialized with a call to tcgetattr() overwise behaviour
        // is undefined
        if(tcgetattr(serialHandle, &tty) != 0) {
            printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        }
        tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
        tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
        tty.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
        tty.c_cflag |= CS8; // 8 bits per byte (most common)
        tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
        tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
        tty.c_lflag &= ~ICANON; // Disable canonical mode
        tty.c_lflag &= ~ECHO; // Disable echo
        tty.c_lflag &= ~ECHOE; // Disable erasure
        tty.c_lflag &= ~ECHONL; // Disable new-line echo
        tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
        tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
        tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
        tty.c_cc[VTIME] = 0; // Don't wait
        tty.c_cc[VMIN] = 1; // Don't wait
        // Set in/out baud rate to be 9600
        cfsetispeed(&tty, baud); // set baud to integer, works on our compiler but might not on others
        cfsetospeed(&tty, baud);

        if (tcsetattr(serialHandle, TCSANOW, &tty) != 0) {
            printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        }
    }
    TtyNetworkInterface::~TtyNetworkInterface(){
        close(serialHandle);
    }

    bool TtyNetworkInterface::is_byte_available(){
        //if (has_char){
        //    return true;
        //}
        //int result = getchar_timeout_us(0);
        //int result = 5;
        //if (result == PICO_ERROR_TIMEOUT) return false;
        //has_char = true;
        //last_char = result;
        //return true;
        int bytes = 0;
        int error = ioctl(serialHandle, FIONREAD, &bytes);
        if (bytes > 0)
            return true;
        return false;
    }

    bool TtyNetworkInterface::is_byte_available_wait(){
        //return true;
        // TODO: Maybe make this better by re-running is_byte_available() after a brief delay, or even better by
        //  checking is_byte_available() every x many ms up to y total ms. Use the TTY_DELAY macro for the total time
        //  (defined inthe .h file)
        // timers for tracking time
        auto start_time = std::chrono::steady_clock::now();
        auto current_time = start_time;

        while (true) {
            // Immediately available, go
            if (is_byte_available()) {
                return true;  // Byte is available
            }

            // Count the elapsed time using current - start
            current_time = std::chrono::steady_clock::now();
            auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();

            // Compare with TTY_DELAY which is max time (10s) and break for timeout if reached
            if (elapsed_time >= TTY_DELAY) {
                break;
            }

            // Put a thread to sleep for INC_DELAY which is the amount of time between checks (10ms) then continue loop
            std::this_thread::sleep_for(std::chrono::milliseconds(INC_DELAY));
        }
        // If byte not available return false
        return false;
    }

    uint8_t TtyNetworkInterface::read_byte(){
        if (is_byte_available()){
            char read_buf;
            int count = read(serialHandle, &read_buf, sizeof(read_buf));
            //std::cout << "Received raw byte: " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(read_buf) << std::endl;
            if (count <= 0){
                printf("error");
            }
            return read_buf;
        }
        return 0x00;
    }
    uint8_t TtyNetworkInterface::read_byte_wait(){
        //if (is_byte_available_wait()){
            char read_buf[] = {0x00};
            int count = read(serialHandle, read_buf, sizeof(read_buf));
            //std::cout << "Received raw byte: " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(read_buf[0]) << std::endl;
            if (count <= 0){
                printf("err");
            }
            return read_buf[0];
        //}
        //return 0x00;
    }
    void TtyNetworkInterface::empty_buffer() { }
    void TtyNetworkInterface::empty_buffer_wait() { }

    void TtyNetworkInterface::write_buffer(uint8_t* buffer, uint32_t buffer_size){
        std::cout << "Writing " << buffer_size << " bytes to usb." << std::endl;
        write(serialHandle, buffer, buffer_size);
    }
} // lrhnet