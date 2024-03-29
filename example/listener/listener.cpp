//
// Created by rando on 2/4/24.
//

#include <core/NetworkInterface.h>
#include <core/Packet.h>
#include <core/util.h>
#include <rlc/TtyNetworkInterface.h>
#include <core/Frame.h>

#define TIME_PORT 13

void callback_time(uint64_t source, uint8_t port, uint8_t *message, uint32_t length){
    printf("get_time!");
    lrhnet::debug_port_callback(source, port, message, length);
    long long int mn;
    while (length > 0 && (message[0] == '\n' || message[0] == '\r' || message[0] == '\t' || message[0] == ' ')){
        message += sizeof(message[0]);
        length--;
    }
    sscanf(reinterpret_cast<const char *>(message), "get_time(%lld)", &mn);

    time_t t = time(nullptr);
    struct tm tm = *localtime(&t);

    int16_t year = tm.tm_year + 1900;
    int16_t month = tm.tm_mon + 1;
    int16_t day = tm.tm_mday;
    int16_t dotw = tm.tm_wday;
    int16_t hour = tm.tm_hour;
    int16_t min = tm.tm_min;
    int16_t sec = tm.tm_sec;

    int buf_size = 1024;
    uint8_t response_message[buf_size];
    uint32_t response_length = snprintf(reinterpret_cast<char *>(response_message), buf_size, "\nset_time(%lld, %hd, %hd, %hd, %hd, %hd, %hd, %hd)\n", mn, year, month, day, dotw, hour, min, sec);

    printf("response->%s\n", reinterpret_cast<const char *>(response_message));
    lrhnet::send_message(source, TIME_PORT, response_message, response_length);
}

int main() {
    lrhnet::device_id = 0x0000000000000000;

    lrhnet::NetworkInterface* ni[] = {
            new lrhnet::TtyNetworkInterface(0, const_cast<char*>("/dev/serial/by-id/usb-Florida_Polytechnic_LRHNET_Interface_E661640843945622-if00"), 115200)
    };
    lrhnet::network_interfaces = ni;
    lrhnet::network_interface_count = 1;

    for (int i = 0; i < 256; ++i){
        lrhnet::port_callbacks[i] = lrhnet::debug_port_callback;
    }
    lrhnet::port_callbacks[TIME_PORT] = callback_time;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true){
        lrhnet::poll();
    }
#pragma clang diagnostic pop
}