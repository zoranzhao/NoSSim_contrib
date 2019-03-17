/*  Copyright (c) 2013, Robert Wang, email: robertwgh (at) gmail.com
    All rights reserved. https://github.com/robertwgh/ezSIFT

    Description:Detect keypoints and extract descriptors from two input images.
                Then, match features from two images using brute-force method.

    Revision history:
        September 15th, 2013: initial version.
        July 2nd, 2018: code refactor.
*/
#include "vgraph.h"
#include <iostream>

int main(int argc, char *argv[])
{
    unsigned char* kp1 = keypoints_extraction("0.jpg");
    uint32_t size = get_kp_byte_size(kp1);
    unsigned char* data = get_kp_data(kp1);
    std::cout << "Keypoint size is: "  << size << std::endl;
    unsigned char* kp2 = keypoints_extraction("1.jpg");
    size = get_kp_byte_size(kp2);
    data = get_kp_data(kp2);
    std::cout << "Keypoint size is: "  << size << std::endl;
    keypoints_matching(kp1, kp2, "0.jpg", "1.jpg");
    
    return 0;
}
