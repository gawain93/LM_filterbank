#ifndef COMPUTE_HISTOGRAM_H_
#define COMPUTE_HISTOGRAM_H_

#include <iostream>
#include <math.h>
#include <boost/concept_check.hpp>

#include <opencv2/core/core.hpp>

namespace FeatureExtract
{ 
  void statisHistogram(cv::Mat roi_samples, cv::Mat left_mask, cv::Mat right_mask, int sample_max,
		       int sample_min, cv::Mat histogram_left, cv::Mat histogram_right);
  
  class ComputeHistogram
  {
  public:
    ComputeHistogram(int radis, int orients, int scales, cv::Mat input_image);
    ComputeHistogram(int radis, int orients, int scales);
    ~ComputeHistogram();
    void run();
    void rotateMask();
    void halfDiscHistogram();
    void computeMask(int radis);
    
    int radis_;
    int orients_;
    int scales_;
    cv::Mat input_image_;
    
    int histogram_length_;
    
    cv::Mat mask_;
    cv::Mat angle_mask_;
    cv::Mat left_mask_, right_mask_;
    
  private:

    std::vector<cv::Mat> left_masks_;
    std::vector<cv::Mat> right_masks_;         // 8 orientations and then 3 scales
    
    cv::Mat histogram_left;
    cv::Mat histogram_right;
    
    double min_, max_;
  };
  
};

#endif