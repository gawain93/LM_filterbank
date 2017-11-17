#include <filter_bank.h>

using namespace FeatureExtract;
using namespace cv;
using namespace std;


FilterBank::FilterBank(int orients,int elong, std::vector<double> scales)      // scale is the sigma
: orients_(orients), elong_(elong) ,scales_(scales)
{
  support_ = 3;
  pi_ = 3.1415926;
  kernel_bar_sum_ = 0;
  abs_kernel_bar_sum_ = 0;
  kernel_edge_sum_ = 0;
  abs_kernel_edge_sum_ = 0;
  
  num_filters_bar_ = orients * scales.size();
  num_filters_edge_ = orients * scales.size();
  num_filters_log_ = 8;                         // TODO 8 for the number of LoG filters
  num_filters_ = num_filters_bar_ + num_filters_edge_ + num_filters_log_;        
  filter_bank_bar_.clear();
  filter_bank_edge_.clear();
}


FilterBank::~FilterBank()
{
}


double FilterBank::gaussian1D(double sigma, double x, int ord)
{
  double variance(sigma * sigma);
  double denom = 2 * variance;
  double num = x * x;
  
  double g=exp(-num / denom) / pow((pi_ * denom),0.5);
  if (ord == 1)
  {
    g = -g * ( x / variance );
    return g;
  }
  if (ord == 2)
  {
    g = g * ( (num-variance) / (variance * variance) );
    return g;
  }
  return g;
}


double FilterBank::gaussian2D(double u, double v, double sigma, int phasex, int phasey)
{
  double g_x = gaussian1D(elong_ * sigma, u, phasex);     
  double g_y = gaussian1D(sigma, v, phasey);
  std::cout << "gx, gy, and g are " << g_x << ' ' << g_y << ' ' << g_x*g_y << std::endl;
  return g_x * g_y;
}


double FilterBank::log2D(double sigma, int x, int y)          // TODO only a quarter of it
{
  double var = sigma * sigma;
  double g = -(1 / (pi_* var * var)) * (1 - (x * x + y * y) / (2 * var)) * exp(- (x*x + y*y) / (2*var) );
  return g;
}

void FilterBank::makeFilter()
{
  int count = 0;
  for (std::vector<double>::iterator scale = scales_.begin(); scale != scales_.end(); ++scale)
  {
    std::cout << "The scale is " << *scale << std::endl;
    hsz_ = ceil( support_ * *scale * elong_);
    sz_ = hsz_ * 2 + 1;
    
    filter_edge_ = cv::Mat::zeros(sz_, sz_, CV_64F);
    filter_bar_ = cv::Mat::zeros(sz_, sz_, CV_64F);
    log_ = cv::Mat::zeros(sz_, sz_, CV_64F);
    //gaussian_ = cv::Mat::zeros(sz_, sz_, CV_32F);

    cv::Mat y_coor(1, sz_, CV_64F, cv::Scalar(hsz_));           // the matrix should be non-empty before push_back?
    y_ = y_coor.clone();
    
    for (int row_num = hsz_ - 1; row_num >= -hsz_; row_num--)
    {
      cv::Mat y_coor(1, sz_, CV_64F, cv::Scalar(row_num));     // y coordinate of a row_num, eg. {3,3,3}
      //std::cout << "y_coor " << y_coor << std::endl;          //  for debug  TODO delete
      y_.push_back(y_coor);                                   // y coordinates eg. {3,3,3,2,2,2,....-3,-3,-3}
    }
    cv::transpose(y_, x_);
    cv::flip(x_, x_, 1);
    std::cout << "x coordinates " << x_ << std::endl;
    std::cout << "y coordinates " << y_ << std::endl;
    
    for (int ori = 0; ori < orients_; ori++)
    {
      double angle = pi_ / orients_ * ori;
      double s = sin(angle); double c = cos(angle);
      //std::cout << "s and c are " << s << ' ' << c << std::endl;
      
      /*
      for (int i = 0; i < sz_; i++)
      {
	for (int j = 0; j < sz_; j++)
	{
	  double x = x_.at<double>(i,j);
	  double y = y_.at<double>(i,j);
	  std::cout << "x and y are " << x << ' ' << y << std::endl;
	  double u = c * x - s * y;
	  double v = s * x + c * y;
	  filter_edge.at<double>(i,j) = gaussian2D(u, v, *scale, 0, 1);
	  filter_bar.at<double>(i,j) = gaussian2D(u, v, *scale, 0, 2);
	}
      }
      */
      for (int x = -hsz_; x <= hsz_; x++)
      {
	for (int y = hsz_; y >= -hsz_; y--)
	{
	  int i = hsz_ + x; int j = hsz_ - y;
	  std::cout << "x and y are " << x << ' ' << y << std::endl;
	  double u = c * x - s * y;
	  double v = s * x + c * y;
	  double edge = gaussian2D(u, v, *scale, 0, 1);
	  double bar = gaussian2D(u, v, *scale, 0, 2);
	  
	  kernel_bar_sum_ += bar;
	  kernel_edge_sum_ += edge;
	  abs_kernel_bar_sum_ += abs(bar);
	  abs_kernel_edge_sum_ += abs(edge);
	  
	  filter_edge_.at<double>(j,i) = gaussian2D(u, v, *scale, 0, 1);
	  filter_bar_.at<double>(j,i) = gaussian2D(u, v, *scale, 0, 2);
	  
	  if (ori == 0)
	  {
	    log_.at<double>(j,i) = log2D(*scale, x, y);
	  }
	}
      }
      
      std::cout << "edge filter is: " << filter_edge_ << std::endl;
      std::cout << "bar filter is: " << filter_bar_ << std::endl;
      
      cv::normalize(filter_edge_, filter_edge_,NORM_L1);
      cv::normalize(filter_bar_, filter_bar_,NORM_L1);
      cv::normalize(log_, log_,NORM_L1);
      /*
      filter_edge_ = (filter_edge_ - kernel_edge_sum_ / (sz_ * sz_)) / abs_kernel_edge_sum_;
      filter_edge_ = (filter_edge_ - kernel_edge_sum_ / ( sz_ * sz_)) / abs_kernel_edge_sum_;
      kernel_bar_sum_ = 0; abs_kernel_bar_sum_ = 0;
      kernel_edge_sum_ = 0; abs_kernel_edge_sum_ = 0;
      */
      std::cout << "edge filter is: " << filter_edge_ << std::endl;
      std::cout << "bar filter is: " << filter_bar_ << std::endl;
      //namedWindow( "Display window", WINDOW_AUTOSIZE );
      //cv::imshow("filter edge" , filter_edge * 255);
      //waitKey(10);
      cv::imwrite("../data/filter_edge" + patch::to_string(count) + ".jpg", filter_edge_*255*2);
      cv::imwrite("../data/filter_bar" + patch::to_string(count++) + ".jpg", filter_bar_*255*2);      // for visulization
      //filter_edge_.convertTo(filter_edge_, CV_32F); filter_bar_.convertTo(filter_bar_, CV_32F);
      filter_bank_edge_.push_back(filter_edge_.clone());                         
      filter_bank_bar_.push_back(filter_bar_.clone());
      std::cout << "one orientation finished" << std::endl;
    }
      
    //gaussian_ = cv::getGaussianKernel(sz_, sz_, *scale);
    //filter_bank_gaussian_.push_back(gaussian_);
   // log_.convertTo(log_, CV_32F);
    filter_bank_log_.push_back(log_.clone());
    //cv::imwrite("../data/filter" + patch::to_string(count++) + ".jpg", gaussian_*255*2);
    cv::imwrite("../data/filter_log" + patch::to_string(count++) + ".jpg", log_*255*2);      // for visulization
  } 
}

// TODO convert all filters to cv_32F