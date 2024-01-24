//
// Created by rando on 1/17/24.
//

#include "util.h"
#include "Packet.h"
#include "Frame.h"
#include <cstring>
//#include <iomanip>
//#include <iostream>

namespace lrhnet {

    uint32_t device_id = 0x00000012;

    void (*port_callbacks[0x255])(uint32_t, uint8_t port, uint8_t*, uint32_t) = {unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback, unbound_port_callback};

    Packet::Packet(uint32_t p_source, uint32_t p_destination, uint8_t p_ttl, uint8_t p_port, uint8_t *p_message, uint32_t p_length) {
        source = p_source;
        destination = p_destination;
        ttl = p_ttl;
        port = p_port;
        message = p_message;
        length = p_length;
    }

    Packet::Packet(uint8_t *raw_packet, uint32_t p_length) {
        // decode the packet
        source = get_uint32_from_pointer(raw_packet + 0);
        destination = get_uint32_from_pointer(raw_packet + 4);
        ttl = *(raw_packet + 8);
        port = *(raw_packet + 9);
        message = raw_packet + 10;  // we will use the existing string, instead of copying it.
        length = p_length - 10;
    }

    bool Packet::decay() {
        if (ttl <= 0) return true;
        ttl--;
        return false;
    }

    uint8_t *Packet::encode() const {
        auto *val = new uint8_t[length + 10];
        put_uint32_to_pointer(val + 0, source);
        put_uint32_to_pointer(val + 4, destination);
        val[8] = ttl;
        *(val + 9) = port;
        std::memcpy(val + 10, message, (length) * sizeof(uint8_t));
        return val;
    }

    int process_packet_bytes(uint8_t *raw_packet, uint32_t length, NetworkInterface *interface) {
        // check the packet is long enough to have enough parameters.
        if (length < 10) {
            return 1;
        }

        Packet p = Packet(raw_packet, length);
        //std::cout << "Handling packet." << std::endl;

        if (p.destination == device_id) {  // replace 13 with the local destination
            // forward packet to layer 4
            port_callbacks[p.port](p.source, p.port, p.message, p.length);
            return 0;
        } else {

            //std::cout << "Rebroadcasting message." << std::endl;
            // decay the message
            if (p.decay()) {
                //std::cout << "Belay that. Message is dead." << std::endl;
                return 0;  // message is dead when decay returns true, stop here.
            }

            // resend the decayed packet to all other interfaces
            for (int i = 0; i < network_interface_count; i++) {
                if (network_interfaces[i] == interface) continue;
                //std::cout << "Rebroadcasting message on interface " << i << std::endl;
                send_packet_from(&p, network_interfaces[i]);
            }
            return 0;
        }
    }

    void send_packet_from(Packet *p, NetworkInterface *interface) {
        //encode the packet
        uint8_t *e_p = p->encode();
        prepare_bytes_send_frame(e_p, p->length + 10, interface);
        delete[] e_p;
    }

    [[maybe_unused]] void send_message(uint32_t destination, uint8_t port, uint8_t *message, uint32_t length){
        Packet p = Packet(device_id, destination, 0xFF, port, message, length);
        for (int i = 0; i < network_interface_count; i++) {
            send_packet_from(&p, network_interfaces[i]);
        }
    }

    void unbound_port_callback(uint32_t source, uint8_t port, uint8_t* message, uint32_t length){
        //std::cout << "(Packet.cpp) Message received from device " << std::hex << std::setw(8) << std::setfill('0') << source << " on unbound port " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(port) << ": ";
        //for (uint32_t i = 0; i != length; i++)
        //{
        //    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(message[i]);
        //}
        //std::cout << std::endl;
    }
}