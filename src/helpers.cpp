#include "escpospp/helpers.h"
#define STB_IMAGE_IMPLEMENTATION
#include "escpospp/stb_image.h"

std::vector< std::vector<bool> > EscPos::loadImage(std::string filename){
    // To store 
    int width, height, channels;
    stbi_uc *result = stbi_load(filename.c_str(), &width, &height, &channels, 3);

    std::vector< std::vector<bool> > image(height, std::vector<bool>(width, 0));
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            // For each pixel, we will use the Luminosity method:
            // -> (0.3 * R) + (0.59 * G) + (0.11 * B)
            float grayscale = result[3*i + width*j*3] * 0.3;
            grayscale += result[3*i +1 + width*j*3] * 0.59;
            grayscale += result[3*i +2 + width*j*3] * 0.11;
            image[j][i] = grayscale < 150 ;
            //image[j][i] = result[3*i + width*j*3] > (unsigned char) 150;
        }
    }
    stbi_image_free(result);

    return image;
}