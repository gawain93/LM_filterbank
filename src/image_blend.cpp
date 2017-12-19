#include "image_blend.h"

using namespace std;
using namespace cv;
using namespace DatasetCreate;

ImageBlend::ImageBlend(std::string& clean_ground_path, std::string& artificial_dirt_path,
                      std::string& artificial_dirt_mask_path)
{
  clean_ground_path_ = clean_ground_path;
  artificial_dirt_path_ = artificial_dirt_path;
  artificial_dirt_mask_path_ = artificial_dirt_mask_path;
  
  max_num_dirt_ = 5;
  srand((int)time(0));
}


ImageBlend::~ImageBlend()
{
}

void ImageBlend::run()
{
  clean_ground_pattern_ = cv::imread(clean_ground_path_, CV_LOAD_IMAGE_COLOR);
  artificial_dirt_ = cv::imread(artificial_dirt_path_, CV_LOAD_IMAGE_COLOR);
  artificial_dirt_mask_ = cv::imread(artificial_dirt_mask_path_, CV_LOAD_IMAGE_GRAYSCALE);
  
  // To list the clean ground file names
  boost::filesystem::path clean_ground_images(clean_ground_path_);
  num_clean_ground_images_ = std::distance(boost::filesystem::directory_iterator(clean_ground_images),
  					       boost::filesystem::directory_iterator{}); 
  boost::filesystem::directory_iterator end_itr1;
  boost::filesystem::directory_iterator itr1(clean_ground_images);
  
  for (boost::filesystem::directory_iterator itr1(clean_ground_images); itr1 != end_itr1; ++itr1 )
  {
    const boost::filesystem::directory_entry entry = *itr1;
    std::string path = entry.path().string();
   // std::cout << path << std::endl;
    
    clean_ground_filenames_.push_back(path);
  }
  std::cout << "The number of files is " <<clean_ground_filenames_.size() << std::endl;  
  
  // To list the artificial dirt file names
  boost::filesystem::path artificial_dirt_images(artificial_dirt_path_);
  num_artificial_dirt_images_ = std::distance(boost::filesystem::directory_iterator(artificial_dirt_images),
  					       boost::filesystem::directory_iterator{}); 
  boost::filesystem::directory_iterator end_itr2;
  boost::filesystem::directory_iterator itr2(artificial_dirt_images);
  
  for (boost::filesystem::directory_iterator itr2(artificial_dirt_images); itr2 != end_itr2; ++itr2 )
  {
    const boost::filesystem::directory_entry entry = *itr2;
    std::string path = entry.path().string();
    //std::cout << path << std::endl;
    
    artificial_dirt_filenames_.push_back(path);
  }
  
  // TO list the mask file names
  boost::filesystem::path artificial_dirt_mask_images(artificial_dirt_mask_path_);
  boost::filesystem::directory_iterator end_itr3;
  boost::filesystem::directory_iterator itr3(artificial_dirt_mask_images);
  
  for (boost::filesystem::directory_iterator itr3(artificial_dirt_mask_images); itr3 != end_itr3; ++itr3 )
  {
    const boost::filesystem::directory_entry entry = *itr3;
    std::string path = entry.path().string();
    //std::cout << path << std::endl;
    
    artificial_dirt_mask_filenames_.push_back(path);
  } 
  
  sort(artificial_dirt_filenames_.begin(), artificial_dirt_filenames_.end());
  for (std::vector<std::string>::iterator it = artificial_dirt_filenames_.begin(); it != artificial_dirt_filenames_.end(); it++)
  {
    std::cout << *it << std::endl;
  }
  
  std::cout << artificial_dirt_mask_filenames_.size() << std::endl;
  sort(artificial_dirt_mask_filenames_.begin(), artificial_dirt_mask_filenames_.end());
  for (std::vector<std::string>::iterator it = artificial_dirt_mask_filenames_.begin(); it != artificial_dirt_mask_filenames_.end(); it++)
  {
    std::cout << *it << std::endl;
  }
  
  // start to blend images, TODO reuse of the ground pattern with different dirts
  for (int m = 0; m < clean_ground_filenames_.size(); m++)
  {
    clean_ground_pattern_ = cv::imread(clean_ground_filenames_[m], CV_LOAD_IMAGE_COLOR);
   // srand((int)time(0));
    int dirt_num = rand() % max_num_dirt_ + 1;   // TODO prevent dirt_num = 0
    blendImage(dirt_num = 5);
  }
  
}


void ImageBlend::blendImage(int dirt_num)
{
  int img_cols = clean_ground_pattern_.cols;
  int img_rows = clean_ground_pattern_.rows;
   
  //srand((int)time(0));
  
  blended_img_ = clean_ground_pattern_.clone();
  blended_mask_ = cv::Mat::zeros(img_rows, img_cols, CV_32S);
  
  for (int n = 0; n < dirt_num; n++)
  {
    int anchor_col = rand() % img_cols;
    int anchor_row = rand() % img_rows;     // top left point of the blending position
    
    int dirt_image_index = rand() % num_artificial_dirt_images_ - 1;     // TODO for testing
    artificial_dirt_ = cv::imread(artificial_dirt_filenames_[dirt_image_index], CV_LOAD_IMAGE_COLOR);
    std::cout << artificial_dirt_filenames_[dirt_image_index] << std::endl;
    artificial_dirt_mask_ = cv::imread(artificial_dirt_mask_filenames_[dirt_image_index], CV_LOAD_IMAGE_GRAYSCALE);
    std::cout << artificial_dirt_mask_filenames_[dirt_image_index] << std::endl;
    rotateImage();
    
    rotated_artificial_dirt_mask_.convertTo(rotated_artificial_dirt_mask_, CV_32S);
    rotated_artificial_dirt_.convertTo(rotated_artificial_dirt_, blended_img_.type());
    std::cout << "rotate matrix finished" << std::endl;
    int dirt_cols = rotated_artificial_dirt_.cols;
    int dirt_rows = rotated_artificial_dirt_.rows;
    
    if ((anchor_col + dirt_cols) > img_cols)
      anchor_col = anchor_col - (dirt_cols + anchor_col - img_cols);
    if ((anchor_row + dirt_rows) > img_rows)
      anchor_row = anchor_row - (dirt_rows + anchor_row - img_rows);
    
    for (int i = anchor_row; i < anchor_row + dirt_rows; i++)      // to blend the images
    {
      for (int j = anchor_col; j < anchor_col + dirt_cols; j++)
      {
	int mask_row = i - anchor_row; int mask_col = j - anchor_col;
	if (rotated_artificial_dirt_mask_.at<int>(mask_row, mask_col) == 1)        // data type of the mask !!!
	{
	  blended_img_.at<cv::Vec3b>(i, j) = rotated_artificial_dirt_.at<cv::Vec3b>(mask_row, mask_col);
	  blended_mask_.at<int>(i,j) = 1;
	  //std::cout << n <<std::endl;
	}
      }
    }
  }
  blended_mask_.convertTo(blended_mask_, CV_8UC1);
  std::cout << "datat type is" << artificial_dirt_mask_.type() << std::endl;
  //std::cout << blended_mask_;
  cv::imwrite("./result_mask.bmp", blended_mask_);
  cv::imwrite("./result_mask_vis.jpg", blended_mask_ * 255);
  cv::imwrite("./result_img.jpg", blended_img_);
    cv::imshow("test", blended_img_);
    cv::waitKey(0); 
}


// reference: https://www.pyimagesearch.com/2017/01/02/rotate-images-correctly-with-opencv-and-python/
void ImageBlend::rotateImage()
{
  //srand((int)time(0));               // generate random rotation angle
  double rotate_angle = rand() % 180;
  std::cout << "The rotation angle is " << rotate_angle << std::endl;
  
  int img_cols = artificial_dirt_.cols;
  int img_rows = artificial_dirt_.rows;
  
  cv::Point2f rotate_center = cv::Point(ceil(img_cols / 2), ceil(img_rows / 2));
  cv::Mat rotate_matrix = cv::getRotationMatrix2D(rotate_center, rotate_angle, 1.0);
  std::cout << rotate_matrix << std::endl;
  cv::Mat t = rotate_matrix.clone();
  std::cout << rotate_matrix.at<double>(0,0) << rotate_matrix.at<double>(0,1) << std::endl;
  
  float c = abs(rotate_matrix.at<double>(0,0));
  float s = abs(rotate_matrix.at<double>(0,1));
  
  int nW = img_rows * s + img_cols * c;
  int nH = img_rows * c + img_cols * s;
  
  rotate_matrix.at<double>(0,2) += (nW / 2) - (img_cols / 2);
  rotate_matrix.at<double>(1,2) += (nH / 2) - (img_rows / 2);
  
  std::cout << "start to rotate matrix" << std::endl;
  std::cout << img_cols << ' ' << img_rows <<' '<< nW << ' ' << nH << std::endl;
  rotated_artificial_dirt_.create(nH, nW, artificial_dirt_.type());
  rotated_artificial_dirt_mask_.create(nH, nW, artificial_dirt_mask_.type());
  
  if (artificial_dirt_.type() != CV_8UC3)
     {
      std::cout << "ImageFlip::imageCallback: Error: The image format of the color image is not CV_8UC3.\n" << std::endl;
     }
  
  std::cout << "rotating matrix" << std::endl;
  cv::warpAffine(artificial_dirt_,rotated_artificial_dirt_, rotate_matrix, rotated_artificial_dirt_.size());
  cv::warpAffine(artificial_dirt_mask_, rotated_artificial_dirt_mask_, rotate_matrix, rotated_artificial_dirt_mask_.size());
  
}



