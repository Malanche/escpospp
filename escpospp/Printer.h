#ifndef INCLUDE_PRINTER_H
#define INCLUDE_PRINTER_H

#include <string>
#include <utility>
#include <libusb-1.0/libusb.h>

class Printer{
public:
    static void initializePrinter(std::pair<int, int> ids);
    static Printer & getPrinter(void);

    // Output functions
    //! Outputs semi-markdown text to printer
    bool text(std::string text);
    //! Outputs semi-markdown text to printer cutting in the end
    bool textCut(std::string text);
    //! Sends raw char stream to printer
    void raw(std::string text);
    //! Cuts the paper (by default, leaves some space)
    void cut();
    //! Underline text
    void underline(bool state);
    //! Set defaults for printing
    void reset();
private:
    Printer();
    ~Printer();
    static Printer * instance;

    //More variables for libusb
    libusb_context *context;
    libusb_device_handle *printer;
};

#endif //INCLUDE_PRINTER_H