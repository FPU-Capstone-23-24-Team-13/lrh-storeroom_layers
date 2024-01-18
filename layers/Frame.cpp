//
// Created by andre on 1/17/2024.
//

#include "Frame.h"
uint32_t crc_polynomial = 0xEDB88320;
Frame::Frame(uint32_t _msg_len, uint8_t* _msg){
    msg = _msg;
    msg_len = _msg_len;
    calculate_crc();
}
void Frame::calculate_crc() {
    // Initialize CRC with the correct initial value
    crc = 0xFFFFFFFF;

    for (uint32_t i = 0; i < msg_len; ++i) {
         unsigned byte = msg[i];            // Get next byte.
          crc = crc ^ byte;
          for (int j = 7; j >= 0; j--) {    // Do eight times.
             unsigned mask = -(crc & 1);
             crc = (crc >> 1) ^ (0xEDB88320 & mask);
          }
    }

    // The final CRC value is the one's complement of the calculated CRC
    crc = ~crc;
}
uint32_t Frame::get_crc(){
    return crc;
}