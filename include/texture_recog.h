#ifndef FEATURE_RECOG_H_
#define FEATURE_RECOG_H_

#include <filter_bank.h>
#include <opencv2/gpu/gpu.hpp>

namespace FeatureExtract
{
  class TextureRecog
  {
  public:
    cv::Mat input_image_;            // The input frame is the brightness frame or the grayscaled one
    cv::Mat input_image_f_;
    FilterBank fb_;
    
    std::vector<cv::Mat> responses_edge_;
    std::vector<cv::Mat> responses_bar_;
    std::vector<cv::Mat> responses_log_;
    cv::Mat response_edge_;
    cv::Mat response_bar_;
    cv::Mat response_log_;
    
    TextureRecog(int orients, int elong, 
		 std::vector<double> scales, std::string& input_path);
    void preProcessing();
    void filtering();
    void postProcessing();
    ~TextureRecog();
  };
};


#endif