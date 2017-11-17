#ifndef FILTER_BANK_H_
#define FILTER_BANK_H_

#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <boost/concept_check.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace FeatureExtract
{
  class FilterBank
  {
  public:
    FilterBank( int orients, int elong, std::vector<double> scales);
    ~FilterBank();
    double gaussian1D(double sigma, double x, int ord);
    double gaussian2D(double u, double v, double sigma, int phasex, int phasey);
    void orienInvar(double sigma);
    double log2D(double sigma, int x, int y);
    void makeFilter();
    
    std::vector<cv::Mat> filter_bank_edge_;        // edge is the first order of gaussian
    std::vector<cv::Mat> filter_bank_bar_;         // bar is the second order of gaussian
    std::vector<cv::Mat> filter_bank_gaussian_;     // orientation invariant filters
    std::vector<cv::Mat> filter_bank_log_;
    
    std::vector<double> scales_;
    int orients_;
    int elong_;
    int support_;
    int hsz_, sz_;
    int num_filters_edge_;
    int num_filters_bar_;
    int num_filters_log_;
    int num_filters_;
    
  private:
    double pi_;
    cv::Mat x_, y_;
    
    cv::Mat filter_edge_;
    cv::Mat filter_bar_;
    cv::Mat gaussian_;
    cv::Mat log_;
    
    // parameters used for normalise the kernel
    double kernel_bar_sum_;
    double kernel_edge_sum_;
    double abs_kernel_edge_sum_;
    double abs_kernel_bar_sum_;
  };
};


namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

#endif