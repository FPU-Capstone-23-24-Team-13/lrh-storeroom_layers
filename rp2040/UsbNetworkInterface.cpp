//
// Created by rando on 1/19/24.
//

#include <iostream>
#include <iomanip>
#include "UsbNetworkInterface.h"
#include "pico/stdlib.h"

namespace lrhnet {
    UsbNetworkInterface::UsbNetworkInterface(int p_id) : NetworkInterface(p_id){}

    bool UsbNetworkInterface::is_byte_available(){
        return false;
    }
    bool UsbNetworkInterface::is_byte_available_wait(){
        return is_byte_available();
    }
    uint8_t UsbNetworkInterface::read_byte(){
        return std::cin.get();
    }
    uint8_t UsbNetworkInterface::read_byte_wait(){
        return read_byte();
    }
    void UsbNetworkInterface::empty_buffer() { }
    void UsbNetworkInterface::empty_buffer_wait() { }
    void UsbNetworkInterface::write_buffer(uint8_t* buffer, uint32_t buffer_size){
        //std::cout << "Writing " << buffer_size << " bytes to usb." << std::endl;
        std::cout.write(reinterpret_cast<const char *>(buffer), buffer_size);
    }
} // lrhnet