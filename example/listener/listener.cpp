//
// Created by rando on 2/4/24.
//

#include <core/NetworkInterface.h>
#include <core/Packet.h>
#include <core/util.h>
#include <rlc/TtyNetworkInterface.h>
#include <core/Frame.h>

int main() {
    lrhnet::device_id = 0x0000000000000000;

    lrhnet::NetworkInterface* ni[] = {
            new lrhnet::TtyNetworkInterface(0, const_cast<char*>("/dev/ttyACM0"), 115200)
    };
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