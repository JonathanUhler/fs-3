#include "inc/parser.h"
#include "inc/divider.h"
#include "inc/divider_lut.h"
#include <sstream>
#include <string>


Parser::Parser()
    : divider(DIVIDER_LUT, 23, 23)
{
}


std::string Parser::execute(const std::string& command) const {
    std::istringstream command_stream(command);
    Opcode opcode = this->get_opcode(command_stream);

    float result;
    switch (opcode) {
    case Opcode::RCP:
        float x;
        command_stream >> x;
        result = this->divider.reciprocal(x);
        break;
    case Opcode::DIV:
        float numerator, denominator;
        command_stream >> numerator >> denominator;
        result = this->divider(numerator, denominator);
        break;
    case Opcode::UNK:
    default:
        return "error: invalid opcode";
    }
    return std::to_string(result);
}


Parser::Opcode Parser::get_opcode(std::istringstream& command_stream) const {
    std::string opcode_str;
    command_stream >> opcode_str;

    if (opcode_str == "rcp") {
        return Opcode::RCP;
    }
    else if (opcode_str == "div") {
        return Opcode::DIV;
    }
    else {
        return Opcode::UNK;
    }
}
