#include "image_blend.h"

using namespace std;
using namespace cv;
using namespace DatasetCreate;

ImageBlend::ImageBlend(std::string clean_ground_path, std::string artificial_dirt_path)
{
  clean_ground_path_ = clean_ground_path;
  artificial_dirt_path_ = artificial_dirt_path;
}


ImageBlend::~ImageBlend()
{
}

void ImageBlend::run()
{
  clean_ground_pattern_ = cv::imread(clean_ground_path_, CV_LOAD_IMAGE_COLOR);
  artificial_dirt_ = cv::imread(artificial_dirt_path_, CV_LOAD_IMAGE_COLOR);
  
  rotateImage();
  
  cv::imshow("Rotated image", rotated_artificial_dirt_);
  cv::waitKey(0);
}

// reference: https://www.pyimagesearch.com/2017/01/02/rotate-images-correctly-with-opencv-and-python/
void ImageBlend::rotateImage()
{
  srand((int)time(0));               // generate random rotation angle
  double rotate_angle = rand() % 180;
  std::cout << "The rotation angle is " << rotate_angle << std::endl;
  
  int img_cols = artificial_dirt_.cols;
  int img_rows = artificial_dirt_.rows;
  
  cv::Point2f rotate_center = cv::Point(ceil(img_cols / 2), ceil(img_rows / 2));
  cv::Mat rotate_matrix = cv::getRotationMatrix2D(rotate_center, rotate_angle, 1.0);
  std::cout << rotate_matrix << std::endl;
  cv::Mat t = rotate_matrix.clone();
  std::cout << rotate_matrix.at<double>(0,0) << rotate_matrix.at<double>(0,1) << std::endl;
  
  float c = rotate_matrix.at<double>(0,0);
  float s = rotate_matrix.at<double>(0,1);
  
  int nW = img_rows * s + img_cols * c;
  int nH = img_rows * c + img_cols * s;
  
  rotate_matrix.at<double>(0,2) += (nW / 2) - (img_cols / 2);
  rotate_matrix.at<double>(1,2) += (nH / 2) - (img_rows / 2);
  
  rotated_artificial_dirt_.create(nH, nW, artificial_dirt_.type());
  
  if (artificial_dirt_.type() != CV_8UC3)
     {
      std::cout << "ImageFlip::imageCallback: Error: The image format of the color image is not CV_8UC3.\n" << std::endl;
     }
  
  cv::warpAffine(artificial_dirt_,rotated_artificial_dirt_, rotate_matrix, rotated_artificial_dirt_.size());
}



