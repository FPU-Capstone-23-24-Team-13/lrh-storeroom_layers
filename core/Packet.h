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
        Packet(uint64_t p_source, uint64_t p_destination, uint8_t p_ttl, uint8_t p_port, uint8_t *message,
               uint32_t p_length);

        Packet(uint8_t *raw_packet, uint32_t p_length);

        [[nodiscard]] uint8_t *encode() const;

        bool decay();

        uint64_t source;
        uint64_t destination;
        uint8_t ttl;
        uint8_t port;
        uint32_t length;
        uint8_t *message;
    };

    int process_packet_bytes(uint8_t *raw_packet, uint32_t length,
                             NetworkInterface *interface);  // called by layer 2, does stuff and calls receive_data in layer 4
    void send_packet_from(Packet *p,
                          NetworkInterface *interface); // called by layer 4, does stuff and calls prepare_bytes_send_frame in layer 2
    [[maybe_unused]] void send_message(uint64_t destination, uint8_t port, uint8_t *message, uint32_t length);

    extern uint64_t device_id;

    void unbound_port_callback([[maybe_unused]] uint64_t, [[maybe_unused]] uint8_t port, [[maybe_unused]] uint8_t *, [[maybe_unused]] uint32_t);

    [[maybe_unused]] void debug_port_callback(uint64_t, uint8_t port, uint8_t *, uint32_t);

    extern void (*port_callbacks[0x255])(uint64_t, uint8_t port, uint8_t *, uint32_t);
}

#endif //LRH_STOREROOM_LAYERS_PACKET_H