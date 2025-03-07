#include "console.h"
#include "mbed.h"
#include <cstring>
#include <string>
#include <iostream>


Console::Console() {
    static BufferedSerial serial_interface(USBTX, USBRX);
    this->serial_interface = &serial_interface;
    this->serial_interface->set_blocking(false);
}


Console::Console(BufferedSerial *serial_interface) {
    this->serial_interface = serial_interface;
}


bool Console::Read() {
    char c[1] = {0};

    if (this->serial_interface->read(c, sizeof(c)) == -EAGAIN) {
        return false;
    }

    if (*c == '\n' || *c == '\r') {
        this->serial_interface->write(Console::RETURN, sizeof(Console::RETURN));
        return true;
    }

    if (*c == '\b' || *c == '\x7f') {
        if (characters_read > 0) {
            this->serial_interface->write(Console::DELETE, sizeof(Console::DELETE));
            this->characters_read--;
        }
        return false;
    }

    if (this->characters_read < Console::INPUT_BUFFER_SIZE - 1) {
        this->serial_interface->write(c, sizeof(char));
        this->input_buffer[this->characters_read++] = *c;
    }
    return false;
}


void Console::Write(const std::string& message, bool add_newline) {
    this->serial_interface->write(message.c_str(), message.size() + 1);
    if (add_newline) {
        this->serial_interface->write(Console::RETURN, sizeof(Console::RETURN));
    }
    this->serial_interface->sync();
}


std::string Console::GetInput() {
    std::string input(this->input_buffer);

    memset(this->input_buffer, 0, Console::INPUT_BUFFER_SIZE);
    this->characters_read = 0;

    return input;
}
