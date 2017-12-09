#ifndef SEGMENT_DIRT_H
#define SEGMENT_DIRT_H

#include <iostream>
#include <string.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <valarray>

namespace DatasetCreate
{
  class SegmentDirt
  {
  public:
    SegmentDirt(std::string& dirt_image_path, 
		std::string& cropped_image_path,
		std::string& cropped_mask_path);
    ~SegmentDirt();
    
    void run();
    void segment();
  private:
    
    std::string dirt_image_path_;
    std::string cropped_image_path_;
    std::string cropped_mask_path_;
    
    cv::Mat dirt_frame_, cropped_dirt_frame_;
    cv::Mat mask_frame_, cropped_mask_frame_;
    
  };
};

#endif