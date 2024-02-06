//
// Created by rando on 1/19/24.
//

#ifndef LRHNET_TTYNETWORKINTERFACE_H
#define LRHNET_TTYNETWORKINTERFACE_H

#include "../core/NetworkInterface.h"
//#include <windows.h> // needed if we choose windows implementation
#include <string.h> // for strcat
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
//#include <unistd.h>
#include <sys/ioctl.h> // for checking bytes available
// /dev/ttyACM0
#define WRITE_CHUNK_SIZE 8
#define TTY_DELAY 100000
#define INC_DELAY 10

namespace lrhnet {

    class TtyNetworkInterface : public NetworkInterface {
    private:
        int serialHandle;
    public:
        TtyNetworkInterface(int p_id, char* tty_name, int baud);
        ~TtyNetworkInterface();
        bool is_byte_available() override;
        bool is_byte_available_wait() override;
        uint8_t read_byte() override;
        uint8_t read_byte_wait() override;
        void empty_buffer() override;
        void empty_buffer_wait() override;
        void write_buffer(uint8_t* buffer, uint32_t buffer_size) override;
        //bool has_char{};
        //uint8_t last_char{};
    };

} // lrhnet

#endif //LRHNET_TTYNETWORKINTERFACE_H
