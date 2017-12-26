#include "escpospp/EscPosCommands.h"

namespace EscPos{

//Cuts the paper after \x96 vertical spaces
std::string CUT = {'\x1d', '\x56', '\x41', '\x96'};
//Equivalent to ESC @
std::string RESET = {'\x1d', '\x40'};
//Equivalent to ESC M n for font type A
std::string FONT_A = {'\x1b', '\x4d', '\x00'};
//Equivalent to ESC M n for font type B
std::string FONT_B = {'\x1b', '\x4d', '\x01'};
// Equivalent to ESC - n for underline text
std::string UNDERLINE_OFF = {'\x1b', '\x2d', '\x00'};
std::string UNDERLINE_1_DOT = {'\x1b', '\x2d', '\x01'};
std::string UNDERLINE_2_DOT = {'\x1b', '\x2d', '\x02'};
// Equivalent to ESC
std::string BOLD_ON = {'\x1b', '\x45', '\x01'};
std::string BOLD_OFF = {'\x1b', '\x45', '\x00'};

}