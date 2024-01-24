//
// Created by rando on 1/19/24.
//

#ifndef LRHNET_USBNETWORKINTERFACE_H
#define LRHNET_USBNETWORKINTERFACE_H

#include "../core/NetworkInterface.h"

namespace lrhnet {

    class UsbNetworkInterface : public NetworkInterface {
    public:
        explicit UsbNetworkInterface(int p_id);
        bool is_byte_available() override;
        bool is_byte_available_wait() override;
        uint8_t read_byte() override;
        uint8_t read_byte_wait() override;
        void write_buffer(uint8_t* buffer, uint32_t buffer_size) override;
    private:
        uint8_t* data;
        std::size_t length;
    };

} // lrhnet

#endif //NETWORKTESTER_USBNETWORKINTERFACE_H
