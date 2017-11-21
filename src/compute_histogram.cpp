#include "compute_histogram.h"

using namespace std;
using namespace cv;
using namespace FeatureExtract;


ComputeHistogram::ComputeHistogram(int radis, int orients, int scales, cv::Mat input_image) : 
radis_(radis), orients_(orients),scales_(scales), input_image_(input_image)
{
  cv::minMaxLoc(input_image_, &min_, &max_);
  min_ = int(min_); max_ = (int) max_;
  histogram_length_ = max_ - min_ + 1;
  histogram_length_ = (int) histogram_length_;
  
  histogram_left = cv::Mat::zeros(1, histogram_length_, CV_8U);
  histogram_right = cv::Mat::zeros(1, histogram_length_, CV_8U);
}


ComputeHistogram::ComputeHistogram(int radis, int orients, int scales) : 
radis_(radis), orients_(orients),scales_(scales)
{
}


ComputeHistogram::~ComputeHistogram()
{
}


void ComputeHistogram::run()
{
  halfDiscHistogram();
}


void ComputeHistogram::computeMask(int radis)
{
  mask_ = cv::Mat::zeros(radis*2+1, radis*2+1,CV_32F);
  angle_mask_ = cv::Mat::ones(radis*2+1, radis*2+1,CV_32F)*(-1);       // the matrix to store the orientation angle of each pixel
  float r_sq = radis * radis;                                           // the oritation is stored as index
  
  for (int i = - radis; i <= radis; i++)
  {
    for (int j = radis; j >= -radis; j--)
    {
      if ((i*i + j*j) <= r_sq)
      {
	mask_.at<float>(radis-j, i+radis) = 1;
	//std::cout << mask_ << std::endl;
	double angle = atan2(j,i) + M_PI;                              // convert the angle from [-pi,pi] to [0,2*pi]
	int angle_index = floor (angle / M_PI * orients_);
	
	if (angle_index >= 2 * orients_)
	  angle_index = 0;
	
	angle_index = (float) angle_index; 
	angle_mask_.at<float>(radis-j, i+radis) = angle_index;
	//std::cout << angle_mask_ << std::endl;
      }
    }
  }
}


void ComputeHistogram::rotateMask()
{
  for (int i = 0; i < scales_; i++)             // for different scales
  {
    int radis = radis_ * pow(2, i);            // TODO other multiscale methods
    computeMask(radis);
    
    for (int ori = 0; ori < orients_; ori++)   // for different orientations
    {
     right_mask_ = cv::Mat::zeros(radis*2+1, radis*2+1,CV_32F);
     left_mask_ = cv::Mat::zeros(radis*2+1, radis*2+1,CV_32F);
     
     for (int m = 0; m < radis*2 + 1; m++)
     {
       for (int n = 0; n < radis*2 + 1; n++)
       {
	 if (mask_.at<float>(m,n) > 0)
	 {
	  float temp = angle_mask_.at<float>(m,n);
	  //std::cout << temp << std::endl;
	  if (temp >= ori && temp < ori + orients_)
	  {right_mask_.at<float>(m,n) = 1;}                // TODO left and right half disc order !!!!!!!!!!!
	  else
	  {left_mask_.at<float>(m,n) = 1;}
	}
      }
    }
     //std::cout << right_mask_ << std::endl;
     //std::cout<< left_mask_ << std::endl;
     
     left_masks_.push_back(left_mask_.clone());
     right_masks_.push_back(right_mask_.clone());
    }
  }
}


void ComputeHistogram::halfDiscHistogram()       // TODO default as the edge pixels are excluded 
{
  rotateMask();
  
  histogram_left = cv::Mat::zeros(1, histogram_length_, CV_8U);    // clear the the vectors
  histogram_right = cv::Mat::zeros(1, histogram_length_, CV_8U);
  
  for (int i = 0; i < scales_; i++)
  {
    int radis = radis_ * pow(2, i);            // TODO other multiscale methods

    for (int ori = 0; ori < orients_; ori++)
    {
      cv::Mat left_mask = left_masks_[i*scales_ + ori];
      cv::Mat right_mask = right_masks_[i*scales_+ ori];
      
      for (int x = radis; x < input_image_.rows - radis; x++)
      {
	for (int j = radis; j < input_image_.cols - radis; j++)
	{
	    Rect r(j-radis, x-radis, 2*radis, 2*radis);
	    cv::Mat roi = input_image_(r);
	    
	   // statisHistogram(roi, left_mask, right_mask, max_, min_,
	   //                 histogram_left, histogram_right);
	}
      }
      
    }
  }
}


void statisHistogram(cv::Mat roi_samples, cv::Mat left_mask, cv::Mat right_mask, int sample_max,
		     int sample_min, cv::Mat histogram_left, cv::Mat histogram_right)
{
  histogram_left = cv::Mat::zeros(1, abs(sample_max-sample_min), CV_8U);
  histogram_right = cv::Mat::zeros(1, abs(sample_max-sample_min), CV_8U);
  
  for (int i = 0; i < roi_samples.rows; i++)
  {
    for (int j = 0; j < roi_samples.cols; j++)
    {
      if (left_mask.at<int>(i,j) == 1)
      {
	int sample = roi_samples.at<int>(i,j);
	int bin = sample - sample_min;
	histogram_left.at<int>(0,bin) += 1;
      }
      if (right_mask.at<int>(i,j) == 1)
      {
	int sample = roi_samples.at<int>(i,j);
	int bin = sample - sample_min;
	histogram_right.at<int>(0,bin) += 1;
      }
      
    }
  }
  
}
