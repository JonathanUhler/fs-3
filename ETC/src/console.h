#ifndef _CONSOLE_H_
#define _CONSOLE_H_


#include "mbed.h"
#include <cstdint>
#include <string>


class Console {

public:
    static constexpr char DELETE[] = "\b \b";
    static constexpr char RETURN[] = "\n\r";

    static const size_t INPUT_BUFFER_SIZE = 256;


    Console();

    Console(BufferedSerial *serial_interface);

    bool Read();

    void Write(const std::string& message, bool add_newline = true);

    std::string GetInput();

private:
    BufferedSerial *serial_interface;
    char input_buffer[INPUT_BUFFER_SIZE] = {0};
    size_t characters_read = 0;

};

#endif  // _CONSOLE_H_
