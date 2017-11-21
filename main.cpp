#include <filter_bank.h>
#include <time.h>

#include <compute_histogram.h>

using namespace FeatureExtract;

int main(int argc, char **argv) 
{  
  /*
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
   */
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
  
  return 1;
}
