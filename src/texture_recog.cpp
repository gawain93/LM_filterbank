#include <texture_recog.h>


using namespace FeatureExtract;
using namespace cv;

TextureRecog::TextureRecog(int orients, int elong ,std::vector< double > scales, 
			   std::string& input_path) :  fb_(orients, elong, scales)
{
  input_image_ = cv::imread(input_path);
}


TextureRecog::~TextureRecog()
{}

void TextureRecog::preProcessing()
{
  input_image_.convertTo(input_image_f_, CV_32F, 1.0/255);
}


void TextureRecog::filtering()
{
  for (int i = 0; i < fb_. num_filters_bar_; i++)
  {
    cv::filter2D(input_image_f_, response_bar_ ,-1 , fb_.filter_bank_bar_[i], cv::Point(-1,-1));
    cv::filter2D(input_image_f_,  response_edge_ ,-1 , fb_.filter_bank_edge_[i], cv::Point(-1,-1));
    
    responses_bar_.push_back(response_bar_.clone());
    response_edge_.push_back(response_edge_.clone());
  }
  
  for (int j = 0; j < fb_.num_filters_log_; j++)
  {
    cv::filter2D(input_image_f_, response_log_  ,-1 , fb_.filter_bank_log_[j], cv::Point(-1, -1));
    responses_log_.push_back(response_log_.clone());
  }
}


