# EscPospp: C++ library to control POS printers

It might be late, but I couldn't find an esc/pos utility to use with c++, so I coded a very minimalistic one.

As a very, very important note, **this is a work in progress**.

### How to compile

Simply create a build folder, move into it and execute cmake

    mkdir build
    cd build
    cmake ../

You will need [libusb](http://libusb.info/), but that is the only dependency (at least in linux, no proprietary drivers are required when testing with a TM-T20II).

If you are using g++, then it is enough to type `make` to build the library.

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

### Printing images

This library uses the `ESC *` protocol to send pictures to the printer. At the moment, the functionality is limited. You need to send te picture as a `std::vector` of pixel rows, which are `std::vector<bool>` objects (something like an stl matrix). An example of a mxn picture would be the next one.

    std::vector< std::vector<bool> > myImage(m, std::vector<bool>(n, 0));

By default, the printing resolution is set to high, but you can change this setting with the printer's `setResolution` method.

The utility `EscPos::loadImage` uses the [stb image library](https://github.com/nothings/stb) to load pictures into the said nested-vector format. So you can load a jpg picture, for example, as follows

    std::vector< std::vector<bool> > myImage = EscPos::loadImage("the-file-name.jpg");
    // Now we print it
    printer.image(myImage);

To get more information on which files can be loaded and so, look into the `stb_image.h` file inside `escpospp`.

**Note:** the image might have some horizontal whitelines. I saw that this depends on the printer doing the proper line spacing, but I was not able to remove them in all of the printers.

### Printing barcodes

The functionality will soon be implemented.

### Sending raw data to the printer

You can also send raw data to the printer by calling the `raw` method:

    Printer printer = Printer::getPrinter();
    printer.raw("Some sample text\n" + EscPos::CUT);

The provided `EscPos` namespace contains some useful instructions to be sent through the `raw` method to the printer.

# Troubleshooting

### Getting the printer to work on windows

Windows will install a usbprinter driver as soon as you plug in the thermal printer for the first time, but this driver is not compatible with libusb. To fix this issue, you can use a tool like [Zadig](https://zadig.akeo.ie/) to replace this driver with WinUSB. Just take note that after this, your computer won't recognize the printer as an actual priner ;)