#ifndef INCLUDE_ESCPOS_COMMANDS_H
#define INCLUDE_ESCPOS_COMMANDS_H
#include <string>

namespace EscPos{

//Cuts the paper after \x96 vertical spaces
extern std::string CUT;
//Equivalent to ESC @
extern std::string RESET;
//Equivalent to ESC M n for font type A
extern std::string FONT_A;
//Equivalent to ESC M n for font type B
extern std::string FONT_B;
// Equivalent to ESC - n for underline text
extern std::string UNDERLINE_OFF;
extern std::string UNDERLINE_1_DOT; 
extern std::string UNDERLINE_2_DOT;
// Equivalent to ESC
extern std::string BOLD_ON;
extern std::string BOLD_OFF;

}

#endif //INCLUDE_ESCPOS_COMMANDS_H