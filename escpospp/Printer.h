#ifndef INCLUDE_PRINTER_H
#define INCLUDE_PRINTER_H

#include <string>
#include <utility>
#include <vector>
#include <libusb-1.0/libusb.h>


/*
Singleton printer object, functions are self explained

Author: Carlos Malanche, October 2018.
*/
class Printer{
public:
    static void initializePrinter(std::pair<int, int> ids);
    static Printer & getPrinter(void);

    enum imageResolution{
        HIGH,
        LOW
    };

    // Output functions
    //! Outputs semi-markdown text to printer
    bool text(std::string text);
    //! Outputs semi-markdown text to printer cutting in the end
    bool textCut(std::string text);
    //! Sends raw char stream to printer
    void raw(std::string text);
    //! Cuts the paper (by default, leaves some space)
    void cut();
    //! Prints an image, provided as a vector of rows of pixels.
    void image(std::vector< std::vector<bool> > &bitmap);
    //! Set Image resolution
    void setResolution(imageResolution ir);
    //! Underline text
    void underline(bool state);
    //! Set defaults for printing
    void reset();
private:
    Printer(std::pair<int, int> ids);
    ~Printer();
    static Printer * instance;
    imageResolution ir;

    //More variables for libusb
    libusb_context *context;
    libusb_device_handle *printer;
};

#endif //INCLUDE_PRINTER_H