//
// Created by rando on 1/17/24.
//

#include "util.h"
#include "Packet.h"
#include "Frame.h"
#include <cstring>

Packet::Packet(uint32_t p_source, uint32_t p_destination, uint8_t p_ttl, uint8_t* p_message, uint32_t p_length){
    source = p_source;
    destination = p_destination;
    ttl = p_ttl;
    message = p_message;
    length = p_length;
}

Packet::Packet(uint8_t* raw_packet, uint32_t p_length){
    // decode the packet
    source = get_uint32_from_pointer(raw_packet + 0);
    destination = get_uint32_from_pointer(raw_packet + 4);
    ttl = *(raw_packet + 8);
    uint8_t extra = *(raw_packet + 9);
    message = raw_packet + 10;  // we will use the existing string, instead of copying it.
    length = p_length - 10;
}

bool Packet::decay(){
    if (ttl <= 0) return true;
    ttl--;
    return false;
}

uint8_t* Packet::encode() const{
    auto* val = new uint8_t[length + 10];
    put_uint32_to_pointer(val + 0, source);
    put_uint32_to_pointer(val + 4, destination);
    val[8] = ttl;
    *(val + 9) = 0xAB;
    std::memcpy(val+10, message, (length) * sizeof(uint8_t));
    return val;
}

int receive_packet(uint8_t* raw_packet, uint32_t length, NetworkInterface* interface){
    // check the packet is long enough to have enough parameters.
    if (length < 10){
        return 1;
    }

    Packet p = Packet(raw_packet, length);

    if (p.destination == 0x00000013){  // replace 13 with the local destination
        // forward packet to layer 4
        return 0;
    } else{
        // decay the message
        if (p.decay()) return 0;  // message is dead when decay returns true, stop here.

        // resend the decayed packet to all other interfaces
        for (int i = 0; i < network_interface_count; i++){
            if (network_interfaces + i == interface) continue;
            send_packet(&p, network_interfaces + i);
        }
        return 0;
    }
}

void send_packet(Packet* p, NetworkInterface* interface){
    //encode the packet
    uint8_t* e_p = p->encode();
    send_frame(e_p, p->length + 10, interface);
    delete[] e_p;
}