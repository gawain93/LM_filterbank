#ifndef COMPUTE_HISTOGRAM_H_
#define COMPUTE_HISTOGRAM_H_

#include <iostream>
#include <math.h>
#include <boost/concept_check.hpp>
#include "filter_bank.h"

#include <opencv2/core/core.hpp>

namespace FeatureExtract
{ 
 
  class ComputeHistogram
  {
  public:
    ComputeHistogram(int radis, int orients, int scales, cv::Mat input_image);
    ComputeHistogram(int radis, int orients, int scales);
    ~ComputeHistogram();
    void rotateMask();
    void halfDiscHistogram();
    void computeMask(int radis);
    void statisHistogram(cv::Mat roi_samples, cv::Mat left_mask, cv::Mat right_mask, int sample_max, int sample_min);
    float orientedGradientSignal(cv::Mat histogram_left, cv::Mat histogram_right);
    
    int radis_;
    int orients_;
    int scales_;
    cv::Mat input_image_;
    
    int histogram_length_;
    
    cv::Mat mask_;
    cv::Mat angle_mask_;
    cv::Mat left_mask_, right_mask_;
    
    std::vector<cv::Mat> oriented_gradient_signal_;     // each element is the X^2 of one orientation
  private:

    std::vector<cv::Mat> left_masks_;
    std::vector<cv::Mat> right_masks_;         // 8 orientations and then 3 scales
    
    cv::Mat histogram_left_;
    cv::Mat histogram_right_;
    
    double min_, max_;
  };
  
};

#endif