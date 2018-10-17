#ifndef INCLUDE_HELPERS
#define INCLUDE_HELPERS

#include<vector>
#include<string>

namespace EscPos{

// Function to load an image file into the required format for the printer
std::vector< std::vector<bool> > loadImage(std::string filename);

}

#endif //INCLUDE_HELPERS