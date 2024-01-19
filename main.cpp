#include <iostream>
#include <iomanip>
#include "core/Frame.h"
#include "core/Packet.h"
#include "core/util.h"
#include "dummy/DummyNetworkInterface.h"

int main() {
    std::cout << "Testing CRC." << std::endl;

    // Sample message data
    uint8_t sample_message[] = {0x01, 0x02, 0x03, 0x04, 0x05};

    // Calculate the length of the sample message
    uint32_t sample_message_length = sizeof(sample_message) / sizeof(sample_message[0]);

    // Create an instance of the Frame class with the sample message
    lrhnet::Frame frame_instance(sample_message_length, sample_message);

    // Output the calculated CRC
    std::cout << "CRC: 0x" << std::hex << frame_instance.get_crc() << std::endl;

    uint8_t sample_message2[] = {0x06, 0x07, 0x08, 0x09, 0x0A, 0x7E, 0x00, 0x7D};

    // Calculate the length of the sample message
    uint32_t sample_message2_length = sizeof(sample_message2) / sizeof(sample_message2[0]);

    // Create an instance of the Frame class with the sample message
    lrhnet::Frame frame_instance2(sample_message2_length, sample_message2);

    // Output the calculated CRC
    std::cout << "CRC: 0x" << std::hex << frame_instance2.get_crc() << std::endl;

    std::cout << "Generating and sending a test packet." << std::endl;
    lrhnet::NetworkInterface* ni[] = {
            new lrhnet::DummyNetworkInterface(0, sample_message2, sample_message2_length),
            new lrhnet::DummyNetworkInterface(1, sample_message2, sample_message2_length),
            new lrhnet::DummyNetworkInterface(2, sample_message2, sample_message2_length)};
    lrhnet::network_interfaces = ni;
    lrhnet::network_interface_count = 3;
    lrhnet::Packet p1 = lrhnet::Packet(0x00000013, 0x00000012, 0x66, sample_message2, sample_message2_length);
    send_packet(&p1, ni[0]);
    std::cout << "Simulating receiving that same packet from interface 1." << std::endl;
    uint8_t* sample_packet = p1.encode();
    process_packet_bytes(sample_packet, p1.length + 10, ni[1]);

    return 0;
}
