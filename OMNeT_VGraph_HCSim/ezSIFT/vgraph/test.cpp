/*  Copyright (c) 2013, Robert Wang, email: robertwgh (at) gmail.com
    All rights reserved. https://github.com/robertwgh/ezSIFT

    Description:Detect keypoints and extract descriptors from two input images.
                Then, match features from two images using brute-force method.

    Revision history:
        September 15th, 2013: initial version.
        July 2nd, 2018: code refactor.
*/

#include "ezsift.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <math.h>
#include <iostream>
#include <list>


ezsift::Image<unsigned char> read_jpg(const char *filename){
    int w, h, c;
    unsigned char * rgb_image = stbi_load(filename, &w, &h, &c, 0);
    if (!rgb_image) {
        fprintf(stderr, "Cannot load image \"%s\"\nSTB Reason: %s\n", filename, stbi_failure_reason());
        exit(0);
    }
    ezsift::Image<float> img(w, h);
    int i,j,k;
    for(k = 0; k < c; ++k){
        for(j = 0; j < h; ++j){
            for(i = 0; i < w; ++i){
                int dst_index = i + w*j;
                int src_index = k + c*i + c*w*j;
		if(k==0) img.data[dst_index] = 0.2126 * (float)rgb_image[src_index]/255.;
  		if(k==1) img.data[dst_index] += 0.7152 * (float)rgb_image[src_index]/255.;
  		if(k==2) {
                   img.data[dst_index] += 0.0722 * (float)rgb_image[src_index]/255.;
                   img.data[dst_index] = img.data[dst_index]*255;
		}
            }
        }
    }
    free(rgb_image);
    return img.to_uchar(); 
}

unsigned char* kp_list_serialization(std::list<ezsift::SiftKeypoint> kpt_list){
   unsigned char* data = (unsigned char*)malloc(sizeof(ezsift::SiftKeypoint)*kpt_list.size()+4);
   unsigned char* data_index = data;
   uint32_t size = (uint32_t)kpt_list.size();
   memcpy(data_index, &size, sizeof(uint32_t));
   data_index = data_index + 4;
   for(ezsift::SiftKeypoint it : kpt_list){
      memcpy(data_index, (unsigned char*)&it, sizeof(ezsift::SiftKeypoint));
      data_index = data_index + sizeof(ezsift::SiftKeypoint);
   }
   return data;
}


uint32_t get_kp_size(unsigned char* kp_data){
   uint32_t size;
   unsigned char* data_index = kp_data;
   memcpy(&(size), data_index, sizeof(uint32_t));
   return size;
}

uint32_t get_kp_byte_size(unsigned char* kp_data){
   uint32_t size;
   unsigned char* data_index = kp_data;
   memcpy(&(size), data_index, sizeof(uint32_t));
   return size*sizeof(ezsift::SiftKeypoint);
}

unsigned char* get_kp_data(unsigned char* kp_data){
   unsigned char* data_index = kp_data;
   data_index = data_index + 4;
   return data_index;
}

std::list<ezsift::SiftKeypoint> kp_list_deserialization(unsigned char* data){
   uint32_t size = get_kp_byte_size(data)/sizeof(ezsift::SiftKeypoint);
   unsigned char* data_index = get_kp_data(data);
   std::list<ezsift::SiftKeypoint> kp_list;
   for(uint32_t i = 0; i < size; i++){
      ezsift::SiftKeypoint kp; 
      memcpy((unsigned char*)&kp, data_index, sizeof(ezsift::SiftKeypoint));
      data_index = data_index + sizeof(ezsift::SiftKeypoint);
      kp_list.push_back(kp);
   }
   free(data);
   return kp_list;
}

unsigned char* keypoints_extraction(const char* filename){
    ezsift::Image<unsigned char> img;
    img = read_jpg(filename);
    std::list<ezsift::SiftKeypoint> kpt_list;
    ezsift::double_original_image(true);
    ezsift::sift_cpu(img, kpt_list, true);
    return kp_list_serialization(kpt_list);
}

uint32_t keypoints_matching(unsigned char* kp_data1, unsigned char* kp_data2, const char* file1, const char* file2){
    std::list<ezsift::SiftKeypoint> kpt_list1, kpt_list2;
    kpt_list1 = kp_list_deserialization(kp_data1);
    kpt_list2 = kp_list_deserialization(kp_data2);
    std::list<ezsift::MatchPair> match_list;
    ezsift::match_keypoints(kpt_list1, kpt_list2, match_list);

    // Read two input images
    ezsift::Image<unsigned char> image1, image2;
    image1 = read_jpg(file1);
    image2 = read_jpg(file2);
    
    ezsift::draw_match_lines_to_ppm_file("sift_matching_a_b.ppm", image1,
                                         image2, match_list);
    std::cout << "# of matched keypoints: "
              << static_cast<unsigned int>(match_list.size()) << std::endl;

    return (uint32_t)(match_list.size());
}


#define USE_FIX_FILENAME 0
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
