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
  
  input_image_.convertTo(input_image_, CV_32F);
  
  histogram_left_ = cv::Mat::zeros(1, histogram_length_, CV_8U);
  histogram_right_ = cv::Mat::zeros(1, histogram_length_, CV_8U);
}


ComputeHistogram::ComputeHistogram(int radis, int orients, int scales) : 
radis_(radis), orients_(orients),scales_(scales)
{
}


ComputeHistogram::~ComputeHistogram()
{
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
    cv::imwrite("../data/rightmask" + patch::to_string(ori) + ".bmp", right_mask_ * 255);
    cv::imwrite("../data/leftmask" + patch::to_string(ori) +  ".bmp", left_mask_ * 255);
    
    left_masks_.push_back(left_mask_.clone());
    right_masks_.push_back(right_mask_.clone());
    }
  }
}


void ComputeHistogram::halfDiscHistogram()       // TODO default as the edge pixels are excluded, start of the class
{
  rotateMask();
  
  histogram_left_ = cv::Mat::zeros(1, histogram_length_, CV_32F);    // clear the the vectors
  histogram_right_ = cv::Mat::zeros(1, histogram_length_, CV_32F);
  
  for (int i = 0; i < scales_; i++)
  {
    int radis = radis_ * pow(2, i);            // TODO other multiscale methods

    for (int ori = 0; ori < orients_; ori++)
    {
      cv::Mat left_mask = left_masks_[i*scales_ + ori];
      cv::Mat right_mask = right_masks_[i*scales_+ ori];
      
      //std::cout << left_mask << std::endl;
      //std::cout << right_mask << std::endl;
      
      cv::Mat X_2 = cv::Mat::zeros(input_image_.rows - 2*radis, input_image_.cols - 2*radis,CV_32F);
      
      for (int x = radis; x < input_image_.rows - radis - 1; x++)
      {
	for (int y = radis; y < input_image_.cols - radis - 1; y++)
	{
	    //std::cout << "x is " << x << "  y is " << y << std::endl;
	    Rect r(y-radis, x-radis, 2*radis + 1, 2*radis + 1);
	    cv::Mat roi = input_image_(r);
	    
	    //std::cout << roi << std::endl;                   
	    
	    statisHistogram(roi, left_mask, right_mask, max_, min_);
	    float g = orientedGradientSignal(histogram_left_, histogram_right_);
	    X_2.at<float>(x-radis, y-radis) = g;
            //std::cout << g << std::endl;	    
	}
	//cv::imshow("../data/X2.bmp", X_2);  cv::waitKey(0);
      }
      oriented_gradient_signal_.push_back(X_2);
      //std::cout << X_2 << std::endl;
      //cv::normalize(X_2, X_2,NORM_L1);
      cv::imwrite("../data/X2" + patch::to_string(ori) + ".bmp", X_2);
    }
  }
}


void ComputeHistogram::statisHistogram(Mat roi_samples, Mat left_mask, Mat right_mask, int sample_max, int sample_min)
{

  histogram_left_ = cv::Mat::zeros(1, abs(sample_max-sample_min), CV_32F);
  histogram_right_ = cv::Mat::zeros(1, abs(sample_max-sample_min), CV_32F);
  
  for (int i = 0; i < roi_samples.rows; i++)
  {
    for (int j = 0; j < roi_samples.cols; j++)
    {
      if (left_mask.at<float>(i,j) == 1)
      {
	int sample = roi_samples.at<float>(i,j);
	int bin = sample - sample_min;                    // absolute distance from the sample intensity to the minimum bin value
	//std::cout << "sample is " << sample << "  bin is " << bin << std::endl;
	histogram_left_.at<float>(0,bin) += 1;
      }
      if (right_mask.at<float>(i,j) == 1)
      {
	int sample = roi_samples.at<float>(i,j);
	int bin = sample - sample_min;
	//std::cout << "sample is " << sample << "  bin is " << bin << std::endl;
	histogram_right_.at<float>(0,bin) += 1;
      }
    }
  }
  
  //std::cout << histogram_left_ << std::endl;
  //std::cout << histogram_right_ << std::endl;
}


float ComputeHistogram::orientedGradientSignal(cv::Mat histogram_left, cv::Mat histogram_right)  // compute X^2
{
  double distance = 0;
  //histogram_left.convertTo(histogram_left, CV_32F);
  //histogram_right.convertTo(histogram_right, CV_32F);
  
  cv::Mat ecuclideanDis = histogram_left - histogram_right;
  ecuclideanDis = ecuclideanDis.mul(ecuclideanDis);                // eculidean distance between the two histograms
  
  cv::Mat histogramSum = histogram_left + histogram_right;
  
  cv::Mat temp;
  cv::divide(ecuclideanDis, histogramSum, temp);
  
  float g = cv::sum(temp)[0];
  g = g / 2;
  
  return g;
}

