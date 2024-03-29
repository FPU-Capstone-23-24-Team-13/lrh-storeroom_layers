//
// Created by rando on 1/17/24.
//

#include <cstdint>

#include "NetworkInterface.h"

#ifndef LRH_STOREROOM_LAYERS_UTIL_H
#define LRH_STOREROOM_LAYERS_UTIL_H

namespace lrhnet {
    uint64_t get_uint64_from_pointer(const uint8_t *pointer);

    void put_uint64_to_pointer(uint8_t *pointer, uint64_t val);

    uint32_t get_uint32_from_pointer(const uint8_t *pointer);

    void put_uint32_to_pointer(uint8_t *pointer, uint32_t val);

    uint16_t get_uint16_from_pointer(const uint8_t *pointer);

    void put_uint16_to_pointer(uint8_t *pointer, uint16_t val);

    [[maybe_unused]] void empty_buffers();

    extern NetworkInterface **network_interfaces;
    extern int network_interface_count;
}

#endif //LRH_STOREROOM_LAYERS_UTIL_H
