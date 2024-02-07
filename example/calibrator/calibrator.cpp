//
// Created by rando on 2/4/24.
//

#include <iostream>
#include <core/Frame.h>
#include <core/Packet.h>
#include <core/util.h>
#include <rlc/TtyNetworkInterface.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <iomanip>

using namespace std::chrono_literals;

#define RLC_ID 0
#define MAX_COUNT

#define TIME_PORT 13       // same as IANA datetime port
#define PRESENCE_PORT 11   // same as IANA active users port
#define IDENT_PORT 9       // same as IANA discard port
#define COUNT_PORT 91      // Unassigned port (right above imap)
#define CALIBRATE_PORT 92  // Unassigned port (right above imap)

int ticket, session, calibrate_magic_num;

std::mutex m;
std::condition_variable cv;

char *remove_whitespace(char *input) {
    size_t input_length = strlen(input);
    size_t disallowed_count = 0;
    for (size_t i = 0; i < input_length; ++i) {
        switch (input[i]) {
            case '\n':
            case ' ':
            case '\r':
            case '\t':
                ++disallowed_count;
                break;
            default:
                break;
        }
    }

    auto *output = new char[input_length - disallowed_count + 1];
    output[input_length - disallowed_count] = 0x00;
    size_t output_index = 0;
    for (size_t i = 0; i < input_length; ++i) {
        switch (input[i]) {
            case '\n':
            case ' ':
            case '\r':
            case '\t':
                break;
            default:
                output[output_index++] = input[i];
                break;
        }
    }

    return output;
}

char *get_command(char *input) {
    size_t end_index = strchr(input, '(') - input;
    auto *output = new char[end_index + 1];
    output[end_index] = 0x00;
    memcpy(output, input, end_index);
    return output;
}

void callback_calibrate_ready([[maybe_unused]] uint64_t source, [[maybe_unused]] uint8_t port, [[maybe_unused]] uint8_t *message, [[maybe_unused]] uint32_t length, char *cleaned_message) {
    int mn, n_session, n_ticket;
    sscanf(reinterpret_cast<const char *>(cleaned_message), "calibrate_ready(%d,%d,%d)", &mn, &n_session, &n_ticket);
    if (mn == calibrate_magic_num) return;
    if (n_ticket != ticket) return;
    calibrate_magic_num = mn;

    session = n_session;

    std::cout << "Calibration session opened." << std::endl;
    cv.notify_all();
}

void callback_calibrate(uint64_t source, uint8_t port, uint8_t *message, uint32_t length) {
    char *cleaned_message = remove_whitespace(reinterpret_cast<char *>(message));
    char *command = get_command(cleaned_message);

    if (strcmp(command, "calibrate_ready") == 0)
        callback_calibrate_ready(source, port, message, length, cleaned_message);
    //if (strcmp(command, "calibrate_sample") == 0)
    //    callback_calibrate_sample(source, port, message, length, cleaned_message);
    //if (strcmp(command, "calibrate_finalize") == 0)
    //    callback_calibrate_finalize(source, port, message, length, cleaned_message);

    delete[] cleaned_message;
    delete[] command;
    //delete[] message;
}

uint64_t devices[1024];  // 1024 is way more than enough, since messages have a ttl of 255.
size_t device_count;

bool show_presence_messages;

void callback_presence(uint64_t source, uint8_t port, uint8_t *message, uint32_t length) {
    for (size_t i = 0; i < device_count; ++i){
        if (devices[i] == source) return;
    }
    if (show_presence_messages) std::cout << "Device "  << std::hex << std::setw(16) << source << " found." << std::endl;
    devices[device_count++] = source;
}

bool running = true;

void poll_loop(){
    while (running){
        lrhnet::poll();
        //usleep(100 * 1000); // n ms x 1000 us / 1 ms
    }
    std::cout << "done polling." << std::endl;
}

int main() {
    srand(time(nullptr));

    lrhnet::device_id = 0x0000000000000000;

    lrhnet::NetworkInterface* ni[] = {
            new lrhnet::TtyNetworkInterface(0, const_cast<char*>("/dev/serial/by-id/usb-Florida_Polytechnic_LRHNET_Interface_E661640843945622-if00"), 921600)
    };
    lrhnet::network_interfaces = ni;
    lrhnet::network_interface_count = 1;

    lrhnet::port_callbacks[CALIBRATE_PORT] = callback_calibrate;
    lrhnet::port_callbacks[PRESENCE_PORT] = callback_presence;

    show_presence_messages = true;

    std::thread t{
        poll_loop
    };

    std::cout << "Listening for sensors. This will take 60 seconds." << std::endl;

    sleep(6);

    size_t selected_device = 0;

    show_presence_messages = false;

    do {
        std::cout << "Pick the sensor you want to calibrate." << std::endl;

        for (size_t i = 0; i < device_count; ++i){
        std::cout << "\t" << std::dec << std::setw(0) << i + 1 << ". \t" << std::hex << std::setw(16) << std::setfill('0') << devices[i] << std::endl;
        }

        std::cout << ">>: ";

        std::cin >> selected_device;
    } while (selected_device <= 0 || selected_device > device_count);

    uint64_t target_device = devices[selected_device - 1];

    std::cout << "Opening calibration session." << std::endl;

    const size_t buf_size = 1024;
    int mn = rand();
    ticket = rand();
    uint8_t message[buf_size];
    uint32_t length = snprintf(reinterpret_cast<char *>(message), buf_size, "\ncalibrate_reset(%d, %d, %d)\n", mn, ticket, 5);
    lrhnet::send_message(target_device, CALIBRATE_PORT, message, length);

    // let the reset response come in
    {
        std::unique_lock lk(m);
        cv.wait_for(lk, 5s);
    }

    if (session <= 0){
        std::cout << "Calibration session failed to open in time." << std::endl;
        running = false;
        t.join();
        return 0;
    }
    std::cout << "Proceeding to manual input section." << std::endl;

    int input = -1;
    std::cout << "At the prompts, enter the number of items on the scale. Enter -1 to exit." << std::endl;
    std::cout << "# items on the scale: ";
    std::cin >> input;
    while (input >= 0) {
        mn = rand();
        ticket = rand();
        length = snprintf(reinterpret_cast<char *>(message), buf_size, "\ncalibrate_sample(%d, %d, %d, %d)\n", mn, session, ticket, input);
        lrhnet::send_message(target_device, CALIBRATE_PORT, message, length);
        sleep(8);
        std::cout << "# items on the scale: ";
        std::cin >> input;
    }
    std::cout << "Finalizing calibration." << std::endl;

    mn = rand();
    ticket = rand();
    length = snprintf(reinterpret_cast<char *>(message), buf_size, "\ncalibrate_finalize(%d, %d, %d)\n", mn, session, ticket);
    lrhnet::send_message(target_device, CALIBRATE_PORT, message, length);

    running = false;
    t.join();
    return 0;
}