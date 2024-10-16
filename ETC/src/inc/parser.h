#ifndef _PARSER_H_
#define _PARSER_H_

#include "divider.h"
#include <sstream>
#include <string>


class Parser {

public:
    enum class Opcode {
        RCP,
        DIV,
        UNK
    };
    
    Parser();

    std::string execute(const std::string& command) const;

private:
    Divider divider;

    Opcode get_opcode(std::istringstream& command_stream) const;

};

#endif  // _PARSER_H_
