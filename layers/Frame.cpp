//
// Created by andre on 1/17/2024.
//

#include "Frame.h"
uint32_t crc_polynomial = 0x4C11DB7;
Frame::Frame(uint32_t _msg_len, uint8_t* _msg){

}
void Frame::calculate_crc(){
    crc = 0;

    for (uint32_t i = 0; i < msg_len; i++) {
        crc ^= msg[i] << 8;  // XOR with the next byte

        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ msg;
            } else {
                crc <<= 1;
            }
        }
    }
}