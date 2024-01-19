//
// Created by rando on 1/17/24.
//

#ifndef LRH_STOREROOM_LAYERS_PACKET_H
#define LRH_STOREROOM_LAYERS_PACKET_H

#include <cstdint>

#include "NetworkInterface.h"

namespace lrhnet {
    class Packet {
    public:
        Packet(uint32_t p_source, uint32_t p_destination, uint8_t p_ttl, uint8_t *message, uint32_t p_length);

        Packet(uint8_t *raw_packet, uint32_t p_length);

        [[nodiscard]] uint8_t *encode() const;

        bool decay();

        uint32_t source;
        uint32_t destination;
        uint8_t ttl;
        uint32_t length;
        uint8_t *message;
    };

    int process_packet_bytes(uint8_t *raw_packet, uint32_t length,
                             NetworkInterface *interface);  // called by layer 2, does stuff and calls receive_data in layer 4
    void send_packet(Packet *p,
                     NetworkInterface *interface); // called by layer 4, does stuff and calls prepare_bytes_send_frame in layer 2
}

#endif //LRH_STOREROOM_LAYERS_PACKET_H