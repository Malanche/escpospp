# EscPospp: C++ library to control POS printers

It might be late, but I couldn't find an esc/pos utility to use with c++, so I coded a very minimalistic one.

### How to compile

Simply create a build folder, move into it and execute cmake

    mkdir build
    cd build
    cmake ../

You will need [libusb](http://libusb.info/), but that is the only dependency (at least in linux, no proprietary drivers are required when testing with a TM-T20II).

### Example usage

The use of this library is pretty straightforward, I tried to follow the notation of [python-escpos](https://github.com/python-escpos/python-escpos). An example code to print a text with a crippled version of markdown supported would be:

    #include <iostream>
    #include "escpospp/escpospp.h"

    int main(){
        try{
            Printer::initializePrinter(std::pair<int,int>(0x04b8, 0x0e15));
            Printer &printer = Printer::getPrinter();
            printer.text("This is a **sample** text!\n");
            printer.cut();
        } catch (int e){
            //There was an error
        }
        return 0;
    }

Notice that the `Printer` class follows a kind of singleton pattern, so `initializePrinter` shall be called **only once**. Remember that you need to link to `libEscPospp.a` and `libusb-1.0.a`.

### Sending raw data to the printer

You can also send raw data to the printer by calling the `raw` method:

    Printer printer = Printer::getPrinter();
    printer.raw("Some sample text\n" + EscPos::CUT);

The provided `EscPos` namespace contains some useful instructions to be sent through the `raw` method to the printer.