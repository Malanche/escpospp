#include "escpospp/Printer.h"
#include <stdexcept>
#include <iostream>
#include <stack>
#include <set>

// To sleep a bit :)
#include <chrono>
#include <thread>

#include "escpospp/MDPair.h"
#include "escpospp/EscPosCommands.h"

Printer * Printer::instance = nullptr;

Printer::Printer(std::pair<int, int> ids): context(nullptr), printer(nullptr), ir(Printer::imageResolution::HIGH){
    int r; //for return values
    ssize_t cnt; //holding number of devices in list
    r = libusb_init(&context); //initialize a library session
    if(r < 0) {
        std::cout << "Init Error " << r << std::endl; //there was an error
        throw -1;
    }
    //libusb_set_debug(context, 3); //set verbosity level to 3, as suggested in the documentation
    libusb_set_option(context, LIBUSB_OPTION_LOG_LEVEL, 3);

    libusb_device **list = NULL;
    int count = libusb_get_device_list(context, &list);
    
    for (size_t idx = 0; idx < count; ++idx) {
        libusb_device *device = list[idx];
        libusb_device_descriptor desc = {0};

        int rc = libusb_get_device_descriptor(device, &desc);
        if(desc.idVendor == ids.first && desc.idProduct == ids.second){
            libusb_open(device, &printer);
            break;
        }
        
        // At the moment not necessary to print debug information
        //printf("Vendor:Device = %04x:%04x\n", desc.idVendor, desc.idProduct);
    }
    libusb_free_device_list(list, count);
    
    // A bit riskier method to open the device
    //printer = libusb_open_device_with_vid_pid(context, ids.first, ids.second); //these are vendorID and productID I found for my usb device
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
        instance = new Printer(ids);
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

void Printer::image(std::vector< std::vector<bool> > &bitmap){
    // Quickly check the integrity of the "bitmap"
    int height = bitmap.size();
    int width = bitmap[0].size();
    for(int i = 1; i < height; i++){
        if(width != bitmap[i].size()){
            std::cout << "Error: the bitmap is not squared" << std::endl;
            throw -1;
        }
    }
    // First of all, set line height:
    raw(EscPos::NO_LINE);
    unsigned char nL = (unsigned char) (width%256); // nL
    unsigned char nH = (unsigned char) (width/256); // nH
    unsigned char m = (ir == imageResolution::HIGH) ? (unsigned char) 33 : (unsigned char) 1;
    int rowCount = 0;
    int rowHeight = (ir == imageResolution::HIGH) ? 24 : 8;
    std::string total;
    while(rowCount < height){
        std::string content = EscPos::BITMAP;
        // Now we configure the resolution
        content += m;
        // Notice that I can't do nL + nH because then it adds up nL and nH as integers
        content += nL;
        content += nH;
        for(int j = 0; j < width; j++){
            int bit = 0;
            for(int k = 0; k < rowHeight; k++){
                // If we already got to the last element, then we just skip it all
                if(rowCount + k < height){
                    // Else, we shift this bit with the according position.
                    bit += ((int) bitmap[rowCount+k][j]) << (8-k%8);
                }
                // Finally, we check if it is time to push one byte to the content stream
                if(k%8 == 7){
                    content += (unsigned char) bit;
                    bit = 0;
                }
            }
        }
        rowCount+=rowHeight;
        content+='\n';
        //raw(content);
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        total+=content;
    }
    raw(total);
    raw(EscPos::RESET_LINE);
}

void Printer::reset(){
    raw(EscPos::RESET);
}
