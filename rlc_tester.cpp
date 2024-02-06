#include <iostream>
#include <iomanip>
#include "core/Frame.h"
#include "core/Packet.h"
#include "core/util.h"
#include "rlc/TtyNetworkInterface.h"
//TODO: read and print from address of pico, source address destination address, ID is 0 for this device
//TODO: figure out how to write com serial connected devices
int main() {
    std::cout << "Generating and sending a test packet." << std::endl;
    lrhnet::NetworkInterface* ni[] = {
            new lrhnet::TtyNetworkInterface(0, const_cast<char*>("/dev/ttyACM1"), 921600)
    };
    std::cout << "Successfully created TTY Interface." << std::endl;
    lrhnet::network_interfaces = ni;
    lrhnet::network_interface_count = 1;

    for (int i = 0; i < 256; ++i){
        lrhnet::port_callbacks[i] = lrhnet::debug_port_callback;
    }
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true){
        lrhnet::poll();
    }
#pragma clang diagnostic pop
}
