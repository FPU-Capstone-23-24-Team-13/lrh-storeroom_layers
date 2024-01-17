//
// Created by Andrew on 1/17/2024.
//

#ifndef NETWORKTESTER_FRAME_H
#define NETWORKTESTER_FRAME_H

#include <iostream>
#include <stdint.h>



class Frame {
public:
    Frame(uint32_t _msg_len, uint8_t* _msg);
    void calculate_crc();
    uint32_t get_crc();
private:
    uint8_t flag;
    uint32_t msg_len;
    uint8_t* msg;
    uint32_t crc;
};
void receive_frame();
void send_frame();

#endif //NETWORKTESTER_FRAME_H
