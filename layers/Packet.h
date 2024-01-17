//
// Created by rando on 1/17/24.
//

#ifndef LRH_STOREROOM_LAYERS_PACKET_H
#define LRH_STOREROOM_LAYERS_PACKET_H


class Packet {
private:
    uint32_t source;
    uint32_t destination;
    uint8_t ttl;
    uint32_t length
    uint8_t* message;
};

void receive_packet(uint8_t* raw_packet, uint32_t length);  // called by layer 2, does stuff and calls receive_data in layer 4
void send_packet(uint32_t source, uint32_t destination, uint32_t length, uint8_t* message); // called by layer 4, does stuff and calls send_frame in layer 2


#endif //LRH_STOREROOM_LAYERS_PACKET_H