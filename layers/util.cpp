//
// Created by rando on 1/17/24.
//

#include "util.h"

NetworkInterface* network_interfaces = nullptr;
int network_interface_count = 0;

uint32_t get_uint32_from_pointer(const uint8_t* pointer){
    uint32_t val = 0x00000000;
    val |= *(pointer + 3) << 0;
    val |= *(pointer + 2) << 8;
    val |= *(pointer + 1) << 16;
    val |= *(pointer + 0) << 24;
    return val;
}

void put_uint32_to_pointer(uint8_t* pointer, uint32_t val){
    *(pointer + 3) = (val >> 0) & 0x000000FF;
    *(pointer + 2) = (val >> 8) & 0x000000FF;
    *(pointer + 1) = (val >> 16) & 0x000000FF;
    *(pointer + 0) = (val >> 24) & 0x000000FF;
}

uint16_t get_uint16_from_pointer(const uint8_t* pointer){
    uint16_t val = 0x00000000;
    val |= *(pointer + 1) << 0;
    val |= *(pointer + 0) << 8;
    return val;
}

void put_uint16_to_pointer(uint8_t* pointer, uint16_t val){
    *(pointer + 1) = (val >> 0) & 0x00FF;
    *(pointer + 0) = (val >> 8) & 0x00FF;
}