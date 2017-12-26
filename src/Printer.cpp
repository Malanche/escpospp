#include "escpospp/Printer.h"
#include <stdexcept>
#include <iostream>
#include <stack>
#include <set>

#include "escpospp/MDPair.h"
#include "escpospp/EscPosCommands.h"

Printer * Printer::instance = nullptr;

Printer::Printer(): context(nullptr), printer(nullptr){
    int r; //for return values
    ssize_t cnt; //holding number of devices in list
    r = libusb_init(&context); //initialize a library session
    if(r < 0) {
        std::cout << "Init Error " << r << std::endl; //there was an error
        throw -1;
    }
    libusb_set_debug(context, 3); //set verbosity level to 3, as suggested in the documentation

    printer = libusb_open_device_with_vid_pid(context, 0x04b8, 0x0e15); //these are vendorID and productID I found for my usb device
    if(printer == NULL){
        std::cout << "Cannot open device" << std::endl;
        throw -1;
    }
    
    if(libusb_kernel_driver_active(printer, 0) == 1) { //find out if kernel driver is attached
        std::cout << "Kernel Driver Active" << std::endl;
        if(libusb_detach_kernel_driver(printer, 0) == 0) //detach it
            std::cout << "Kernel Driver Detached!" << std::endl;
    }

    r = libusb_claim_interface(printer, 0); //claim interface 0 (the first) of device (mine had jsut 1)
    if(r < 0) {
        std::cout << "Cannot Claim Interface" << std::endl;
        throw -1;
    }
}

Printer::~Printer(){
    int r;
    r = libusb_release_interface(printer, 0); //release the claimed interface
    if(r!=0) {
        std::cout << "Cannot Release Interface" << std::endl;
    } else {
        std::cout << "Released Interface" << std::endl;
    }

    libusb_close(printer); //close the device we opened
    libusb_exit(context); //close the session
}

void Printer::initializePrinter(std::pair<int, int> ids){
    if(instance == nullptr){
        instance = new Printer();
    }
}

Printer & Printer::getPrinter(void){
    return *instance;
}

bool Printer::text(std::string text){
    bool finished = false;
    std::stack<MDPair> formats;
    std::set<MDPair> elements;
    std::string toPrint;
    for(unsigned int counter = 0;; counter++){
        try{
            if(counter == text.length()-1) {
                counter ++;
                throw -2;
            }
            MDPair mode = MDPair::getMatching(text.substr(counter,2));
            if(elements.find(mode) != elements.end() && formats.top() != mode ){
                std::cout << "Incorrect text string with markdown" << std::endl;
                return false;
            } else if(elements.find(mode) == elements.end()){
                //First we add the characters of the string before this change
                if(counter != 0){
                    toPrint += text.substr(0,counter);
                }
                //We apply the effect
                toPrint += mode.first;
                elements.insert(mode);
                formats.push(mode);

                //We resize the string and are ready to continue;
                text = text.substr(counter + 2, text.length() - counter - 2);
                counter =-1;
            } else if(elements.find(mode) != elements.end() && formats.top() == mode){
                //Mode already inside, apply second sequence to string
                if(counter != 0){
                    toPrint += text.substr(0,counter);
                }

                toPrint += mode.second;
                elements.erase(elements.find(mode));
                formats.pop();

                //We resize the string and are ready to continue;
                text = text.substr(counter + 2, text.length() - counter - 2);
                counter =-1;
            }
        } catch ( int e ) {
            //This means the text is still ok
            if(counter == text.length()){
                if( !elements.empty() ){
                    std::cout << "Unmatched characters in markdown string" << std::endl;
                    return false;
                }
                toPrint += text;
                break;
            }
        }
    }
    raw(toPrint);
    return true;
}

bool Printer::textCut(std::string text){
    if(Printer::text(text)){
        cut();
        return true;
    }
    return false;
}

void Printer::raw(std::string text){
    int totalChars = text.length();
    int actual = 0;
    int r = libusb_bulk_transfer(printer, (1 | LIBUSB_ENDPOINT_OUT), (unsigned char *) &text[0], totalChars, &actual, 0);
    if(r != 0 || actual != totalChars)
        std::cout << "Write Error" << std::endl;
}

void Printer::cut(){
    raw(EscPos::CUT);
}

void Printer::reset(){
    raw(EscPos::RESET);
}
