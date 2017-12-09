
#include <filter_bank.h>
#include <time.h>

#include <compute_histogram.h>
#include <image_blend.h>
#include <segment_dirt.h>

using namespace FeatureExtract;
using namespace DatasetCreate;

void rgb2cie(cv::Mat& rgb_frame, cv::Mat& lab_frame, cv::Mat& brightness_frame,
             cv::Mat& a_frame, cv::Mat& b_frame)
{
  cv::cvtColor(rgb_frame, lab_frame, cv::COLOR_BGR2Lab);
  
  std::vector<cv::Mat> channels;
  cv::split(lab_frame, channels);
  brightness_frame = channels[0].clone();
  a_frame = channels[1].clone();
  b_frame = channels[2].clone();
}


void largestResponse(std::vector<cv::Mat> responses, cv::Mat& maxResponse)
{
  int rows = responses[0].rows;
  int cols = responses[0].cols;
  maxResponse = cv::Mat::zeros(rows, cols, CV_32F);
  
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      float temp = 0;
      for (int n = 0; n < responses.size(); n++)
      {
	if (abs(responses[n].at<float>(i,j)) > temp)
	{ temp = abs( responses[n].at<float>(i,j));}
      }
      maxResponse.at<float>(i,j) = temp;
    }
  }
}


int main(int argc, char **argv) 
{  
  /*
   // testing for filter bank
   std::string filename("./triangle.png");
   cv::Mat image = cv::imread(filename.c_str(), CV_LOAD_IMAGE_COLOR);
   cv::Mat image_gray, image_f, response;
   cv::cvtColor(image, image_gray, CV_BGR2GRAY);
   image_gray.convertTo(image_f, CV_32F, 1.0/255);    // TODO should the image be normalised?

   //cv::copyMakeBorder(image_gray, image_gray, 60, 60, 30, 30, cv::BORDER_CONSTANT, 255.);
   //image_gray = 255 - image_gray;                                       
   
   
   double temp[] = {1*sqrt(2)};
   std::vector<double> scales(temp, temp+1);
   int orients = 8;
   int elong = 3;
   FilterBank fb( orients, elong, scales);
   fb.makeFilter();
   std::cout << "filter bank size is " <<fb.filter_bank_bar_.size() << std::endl;
   

   
   std::vector<cv::Mat> responses;
   clock_t t = clock();
   for (int i = 0; i < 8; i++)
   {
   std::cout << i << std::endl;
   cv::Mat filter = fb.filter_bank_bar_[i];
   filter.convertTo(filter, CV_32F);
   //cv::imshow("filter", filter * 255 * 2);    cv::waitKey(0);
   cv::filter2D(image_f,response, -1, filter, cv::Point(-1,-1));
   responses.push_back(response.clone());
   cv::imwrite("../data/response" + patch::to_string(i) + ".bmp", response * 255 * 2);
   }
   std::cout << "time consuming is: " <<  (clock() - t) / CLOCKS_PER_SEC  << std::endl;
   
   cv::imshow("response", response);
   cv::waitKey(0);
   cv::destroyAllWindows();
  int scales = 1;
  int orients  =1;
  int radis = 80;        // initialize values
  
  ComputeHistogram ch(radis, orients, scales);
  //ch.computeMask(radis);
  //std::cout << ch.angle_mask_.rows;
  //cv::imshow("mask", ch.angle_mask_);
  //cv::waitKey(0);
  //cv::destroyAllWindows();
  ch.rotateMask();          
  cv::imshow("mask", ch.right_mask_*255);
  cv::waitKey(0);
  cv::destroyAllWindows();
   
  
  // testing for half disc histogram
  int orients = 8;
  int elong = 3;
  double temp[] = {1*sqrt(2)};
  std::vector<double> scales(temp, temp+1);
  FilterBank fb( orients, elong, scales);
  
  fb.makeFilter();
  std::cout << "filter bank size is " <<fb.filter_bank_bar_.size() << std::endl;
  
  cv::Mat test_image = cv::imread("./lab_ground.jpg",CV_LOAD_IMAGE_COLOR);
  cv::Mat lab_frame, brightness_frame, a_frame, b_frame;
  rgb2cie(test_image, lab_frame, brightness_frame, a_frame, b_frame);

  
  std::vector<cv::Mat> responses;
  for (int i = 0; i < 8; i++)
  {
   std::cout << i << std::endl;
   cv::Mat response;
   cv::Mat filter = fb.filter_bank_edge_[i];
   filter.convertTo(filter, CV_32F);
   brightness_frame.convertTo(brightness_frame, CV_32F);
   
   cv::filter2D(brightness_frame, response, -1, filter, cv::Point(-1,-1));
   responses.push_back(response.clone());
   cv::imwrite("../data/lab_ground_response" + patch::to_string(i) + ".bmp", response * 255 * 2);
  }
  
  cv::Mat maxResponse;
  largestResponse(responses, maxResponse);
  cv::imwrite("../data/lab_ground_maxresponse.bmp", maxResponse);
  
  //******************************** color histogram *************************************
  
  int scale = 1;
  int orient = 8;
  int radis = 10;                                            // initialize values
  ComputeHistogram ch(radis, orient, scale, a_frame);
  ch.halfDiscHistogram();
  //ch.rotateMask();
  
  
  // check rotation
  std::string clean_ground_path = "./ground_texture.jpg";
  std::string artificial_dirt_path = "./screen.png";
  
  ImageBlend ib(clean_ground_path, artificial_dirt_path);
  ib.run();
  */
  
  std::string path = "./pictures";
  SegmentDirt sd(path, path, path);
  
  sd.run();
  
  return 1;
}

