#ifndef IMAGE_BLEND_H
#define IMAGE_BLEND_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <dirent.h>
#include <math.h>
#include <vector>

// opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/lambda/bind.hpp>

namespace DatasetCreate
{
  class ImageBlend
  {
  public:
    ImageBlend(std::string& clean_ground_path, std::string& artificial_dirt_path,
               std::string& artificial_dirt_mask_path);
    ~ImageBlend();
    
    void rotateImage();
    void blendImage(int dirt_num);
    void run();
    
    cv::Mat blended_img_;
    cv::Mat blended_mask_;
  private:
    int max_num_dirt_;
    
    cv::Mat clean_ground_pattern_;
    cv::Mat artificial_dirt_;
    cv::Mat artificial_dirt_mask_;
    
    cv::Mat rotated_artificial_dirt_;
    cv::Mat rotated_artificial_dirt_mask_;
    
    std::string clean_ground_path_;
    std::string artificial_dirt_path_;
    std::string artificial_dirt_mask_path_;
    
    std::vector<std::string> clean_ground_filenames_;
    std::vector<std::string> artificial_dirt_filenames_; 
    std::vector<std::string> artificial_dirt_mask_filenames_; 
    
    int num_clean_ground_images_, num_artificial_dirt_images_;
  };
};

#endif