#ifndef IMAGE_BLEND_H
#define IMAGE_BLEND_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <math.h>

// opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace DatasetCreate
{
  class ImageBlend
  {
  public:
    ImageBlend(std::string clean_ground_path, std::string artificial_dirt_path);
    ~ImageBlend();
    
    void rotateImage();
    void run();
    
  private:
    cv::Mat clean_ground_pattern_;
    cv::Mat artificial_dirt_;
    cv::Mat artificial_dirt_mask_;
    
    cv::Mat rotated_artificial_dirt_;
    cv::Mat rotated_artificial_dirt_mask_;
    
    std::string clean_ground_path_;
    std::string artificial_dirt_path_;
  };
};

#endif