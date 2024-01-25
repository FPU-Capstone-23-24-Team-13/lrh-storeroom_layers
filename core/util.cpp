//
// Created by rando on 1/17/24.
//

#include "util.h"

namespace lrhnet {
    NetworkInterface **network_interfaces = nullptr;
    int network_interface_count = 0;

    uint64_t get_uint64_from_pointer(const uint8_t *pointer) {
        uint64_t val = 0x0000000000000000;
        val |= (uint64_t)*(pointer + 7) << 0;
        val |= (uint64_t)*(pointer + 6) << 8;
        val |= (uint64_t)*(pointer + 5) << 16;
        val |= (uint64_t)*(pointer + 4) << 24;
        val |= (uint64_t)*(pointer + 3) << 32;
        val |= (uint64_t)*(pointer + 2) << 40;
        val |= (uint64_t)*(pointer + 1) << 48;
        val |= (uint64_t)*(pointer + 0) << 56;
        return val;
    }

    void put_uint64_to_pointer(uint8_t *pointer, uint64_t val) {
        *(pointer + 7) = (val >> 0) & 0x00000000000000FF;
        *(pointer + 6) = (val >> 8) & 0x00000000000000FF;
        *(pointer + 5) = (val >> 16) & 0x00000000000000FF;
        *(pointer + 4) = (val >> 24) & 0x00000000000000FF;
        *(pointer + 3) = (val >> 32) & 0x00000000000000FF;
        *(pointer + 2) = (val >> 40) & 0x00000000000000FF;
        *(pointer + 1) = (val >> 48) & 0x00000000000000FF;
        *(pointer + 0) = (val >> 56) & 0x00000000000000FF;
    }

    uint32_t get_uint32_from_pointer(const uint8_t *pointer) {
        uint32_t val = 0x00000000;
        val |= (uint32_t)*(pointer + 3) << 0;
        val |= (uint32_t)*(pointer + 2) << 8;
        val |= (uint32_t)*(pointer + 1) << 16;
        val |= (uint32_t)*(pointer + 0) << 24;
        return val;
    }

    void put_uint32_to_pointer(uint8_t *pointer, uint32_t val) {
        *(pointer + 3) = (val >> 0) & 0x000000FF;
        *(pointer + 2) = (val >> 8) & 0x000000FF;
        *(pointer + 1) = (val >> 16) & 0x000000FF;
        *(pointer + 0) = (val >> 24) & 0x000000FF;
    }

    uint16_t get_uint16_from_pointer(const uint8_t *pointer) {
        uint16_t val = 0x00000000;
        val |= (uint16_t)*(pointer + 1) << 0;
        val |= (uint16_t)*(pointer + 0) << 8;
        return val;
    }

    void put_uint16_to_pointer(uint8_t *pointer, uint16_t val) {
        *(pointer + 1) = (val >> 0) & 0x00FF;
        *(pointer + 0) = (val >> 8) & 0x00FF;
    }
}