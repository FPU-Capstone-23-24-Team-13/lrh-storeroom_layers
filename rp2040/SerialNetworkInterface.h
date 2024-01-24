//
// Created by rando on 1/19/24.
//

#ifndef NETWORKTESTER_SERIALNETWORKINTERFACE_H
#define NETWORKTESTER_SERIALNETWORKINTERFACE_H

#include "hardware/uart.h"
#include "../core/NetworkInterface.h"

#define READ_BUFFER_SIZE 0x0400
#define WRITE_CHUNK_SIZE 8

namespace lrhnet {

    class SerialNetworkInterface : public NetworkInterface {
    public:
        SerialNetworkInterface(int p_id, uart_inst_t* p_uart);
        bool is_byte_available() override;
        bool is_byte_available_wait() override;
        uint8_t read_byte() override;
        uint8_t read_byte_wait() override;
        void empty_buffer() override;
        void empty_buffer_wait() override;
        void write_buffer(uint8_t* buffer, uint32_t buffer_size) override;
    private:
        uart_inst_t* uart;
        uint8_t* buffer;
        uint32_t buffer_start, buffer_end;
    };

} // lrhnet

#endif //NETWORKTESTER_SERIALNETWORKINTERFACE_H
