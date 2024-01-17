#include <iostream>
#include "layers/Frame.h"
int main() {
    // Sample message data
    uint8_t sample_message[] = {0x01, 0x02, 0x03, 0x04, 0x05};

    // Calculate the length of the sample message
    uint32_t sample_message_length = sizeof(sample_message) / sizeof(sample_message[0]);

    // Create an instance of the Frame class with the sample message
    Frame frame_instance(sample_message_length, sample_message);

    // Output the calculated CRC
    std::cout << "CRC: 0x" << std::hex << frame_instance.get_crc() << std::endl;

    uint8_t sample_message2[] = {0x06, 0x07, 0x08, 0x09, 0x0A};

    // Calculate the length of the sample message
    uint32_t sample_message2_length = sizeof(sample_message2) / sizeof(sample_message2[0]);

    // Create an instance of the Frame class with the sample message
    Frame frame_instance2(sample_message2_length, sample_message2);

    // Output the calculated CRC
    std::cout << "CRC: 0x" << std::hex << frame_instance2.get_crc() << std::endl;
    return 0;
}
