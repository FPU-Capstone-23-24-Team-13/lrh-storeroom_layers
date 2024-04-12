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
#include <vector>
#include <filesystem>

using namespace std::chrono_literals;

#define RLC_ID 0
#define MAX_COUNT

#define TIME_PORT 13       // same as IANA datetime port
#define PRESENCE_PORT 11   // same as IANA active users port
#define IDENT_PORT 9       // same as IANA discard port
#define COUNT_PORT 91      // Unassigned port (right above imap)
#define CALIBRATE_PORT 92  // Unassigned port (right above imap)
#define NAME_PORT 53       // same as IANA DNS port

int ticket, session, calibrate_magic_num;

std::mutex m;
std::condition_variable cv;

char* device_names[1024];
uint64_t devices[1024];  // 1024 is way more than enough, since messages have a ttl of 255.
size_t device_count;

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

void callback_time(uint64_t source, uint8_t port, uint8_t *message, uint32_t length){
    //printf("get_time!");
    lrhnet::debug_port_callback(source, port, message, length);
    long long int mn;
    while (length > 0 && (message[0] == '\n' || message[0] == '\r' || message[0] == '\t' || message[0] == ' ')){
        message += sizeof(message[0]);
        length--;
    }
    sscanf(reinterpret_cast<const char *>(message), "get_time(%lld)", &mn);

    time_t t = time(nullptr);
    struct tm tm = *localtime(&t);

    int16_t year = tm.tm_year + 1900;
    int16_t month = tm.tm_mon + 1;
    int16_t day = tm.tm_mday;
    int16_t dotw = tm.tm_wday;
    int16_t hour = tm.tm_hour;
    int16_t min = tm.tm_min;
    int16_t sec = tm.tm_sec;

    int buf_size = 1024;
    uint8_t response_message[buf_size];
    uint32_t response_length = snprintf(reinterpret_cast<char *>(response_message), buf_size, "\nset_time(%lld, %hd, %hd, %hd, %hd, %hd, %hd, %hd)\n", mn, year, month, day, dotw, hour, min, sec);

    //printf("response->%s\n", reinterpret_cast<const char *>(response_message));
    lrhnet::send_message(source, TIME_PORT, response_message, response_length);
}

void callback_calibrate(uint64_t source, uint8_t port, uint8_t *message, uint32_t length) {
    //lrhnet::debug_port_callback(source, port, message, length);
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

bool show_presence_messages;

void callback_name(uint64_t source, uint8_t port, uint8_t *message, uint32_t length) {
    //if (show_presence_messages) std::cout << "Device "  << std::hex << std::setw(16) << source << " named " << message << std::endl;
    for (size_t i = 0; i < device_count; ++i){
        if (devices[i] == source){
            auto name = new char[length+1];
            memcpy(name, message, length);
            name[length] = 0x00;
            if (device_names[i]) if (!strcmp(name, device_names[i])) return; // same name, don't do anything
            delete[] device_names[i];
            device_names[i] = name;
            if (show_presence_messages) std::cout << "Device "  << std::hex << std::setw(16) << source << " is named " << name << std::endl;
            return;
        }
    }
}

void callback_presence(uint64_t source, uint8_t port, uint8_t *message, uint32_t length) {
    for (size_t i = 0; i < device_count; ++i){
        if (devices[i] == source) return;
    }
    if (show_presence_messages) std::cout << "Device "  << std::hex << std::setw(16) << source << " found." << std::endl;
    const char* resp = ".";
    lrhnet::send_message(source, NAME_PORT, (uint8_t *) resp, strlen(resp));
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

    std::vector<std::string> net_devices;
    for (const auto& file : std::filesystem::directory_iterator("/dev/serial/by-id/"))
    {
        std::string pattern = "usb-Florida_Polytechnic_LRHNET_Interface_";
        if(file.is_character_file())
        {
           std::string devicePathName = file.path().filename().string();
           if(devicePathName.compare(0, pattern.size(), pattern) == 0) net_devices.push_back(devicePathName);
        }
    }

    std::cout << net_devices.size() << std::endl;

    lrhnet::device_id = 0x0000000000000000;

    lrhnet::NetworkInterface* ni[net_devices.size()];
    for(int i = 0; i < net_devices.size(); ++i) {
        std::string device_path_string = "/dev/serial/by-id/" + net_devices[i];
        char* device_path = (char*)((device_path_string).c_str());
        ni[i] = new lrhnet::TtyNetworkInterface(i, device_path, 921600);
    }

    //lrhnet::NetworkInterface* ni[] = {
    //        new lrhnet::TtyNetworkInterface(RLC_ID, const_cast<char*>("/dev/serial/by-id/usb-Florida_Polytechnic_LRHNET_Interface_E6613852834C2F28-if00"), 921600)
    //};

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "LocalValueEscapesScope"
    lrhnet::network_interfaces = ni;  // We're in main. We don't care if the variable escapes, there's nowhere to escape to.
    #pragma clang diagnostic pop
    lrhnet::network_interface_count = net_devices.size();

    //for (int i = 0; i < 256; ++i) lrhnet::port_callbacks[i] = lrhnet::debug_port_callback;

    lrhnet::port_callbacks[CALIBRATE_PORT] = callback_calibrate;
    lrhnet::port_callbacks[PRESENCE_PORT] = callback_presence;
    lrhnet::port_callbacks[NAME_PORT] = callback_name;
    lrhnet::port_callbacks[TIME_PORT] = callback_time;

    show_presence_messages = true;

    std::thread t{
        poll_loop
    };

    std::cout << "Listening for sensors. This will take 30 seconds." << std::endl;

    for (int t = 0; t < 30; ++t){
        sleep(1);
        for (size_t i = 0; i < device_count; ++i){
            if (device_names[i]) continue;
            const char* resp = ".";
            lrhnet::send_message(devices[i], NAME_PORT, (uint8_t *) resp, strlen(resp));
        }
    }

    size_t selected_device = 0;

    show_presence_messages = false;

    do {
        std::cout << "Pick the sensor you want to calibrate." << std::endl;

        for (size_t i = 0; i < device_count; ++i){
            const char* dev_name = device_names[i];
            if (dev_name == nullptr){
                dev_name = "<UNKNOWN>";
            }
            std::cout << "\t" << std::dec << std::setw(0) << i + 1 << ". \t" << dev_name << " - " << std::hex << std::setw(16) << std::setfill('0') << devices[i] << std::endl;
        }

        std::cout << ">>: ";

        std::cin >> selected_device;
    } while (selected_device <= 0 || selected_device > device_count);

    uint64_t target_device = devices[selected_device - 1];

    std::cout << "Enter a new name for the node (enter \".\" to leave unchanged): ";

    std::string name;

    getline(std::cin>>std::ws, name);

    if (name.length() >= 2){
        std::cout << "Changing name to " << name << std::endl;
        const char* name_c = name.c_str();
        lrhnet::send_message(target_device, NAME_PORT, (uint8_t *) name_c, strlen(name_c));
        lrhnet::send_message(target_device, NAME_PORT, (uint8_t *) name_c, strlen(name_c));
        lrhnet::send_message(target_device, NAME_PORT, (uint8_t *) name_c, strlen(name_c));
        lrhnet::send_message(target_device, NAME_PORT, (uint8_t *) name_c, strlen(name_c));
    } else {
        std::cout << "Leaving name unchanged." << std::endl;
    }

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
        cv.wait_for(lk, 15s);
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
    sleep(10);

    running = false;
    t.join();
    return 0;
}