#include "segment_dirt.h"

using namespace cv;
using namespace DatasetCreate;
using namespace boost::filesystem;

SegmentDirt::SegmentDirt(std::string& dirt_image_path, 
			 std::string& cropped_image_path, 
			 std::string& cropped_mask_path)
{
  dirt_image_path_ = dirt_image_path;
  cropped_image_path_ = cropped_image_path;
  cropped_mask_path_ = cropped_mask_path;
}


SegmentDirt::~SegmentDirt()
{
}


void SegmentDirt::run()
{
  boost::filesystem::path dirt_images(dirt_image_path_);
  boost::filesystem::directory_iterator end_itr;
  
  for (boost::filesystem::directory_iterator itr(dirt_images); itr != end_itr; ++itr )
  {
    const boost::filesystem::directory_entry entry = *itr;
    std::string path = entry.path().string();
    std::cout << path << std::endl;
    
    dirt_frame_ = cv::imread(path, CV_LOAD_IMAGE_COLOR);
    segment();
  }
}


void SegmentDirt::segment()
{
  cv::Mat gray_dirt;
  cv::cvtColor(dirt_frame_, gray_dirt,  cv::COLOR_BGR2GRAY);
  int cols = gray_dirt.cols;
  int rows = gray_dirt.rows;
  
  cv::Mat binary_map = cv::Mat::zeros(rows, cols, gray_dirt.type());
  cv::threshold(gray_dirt, binary_map, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
  std::cout << binary_map.dims<< ' ' << binary_map.cols << binary_map.rows <<std::endl;
  //cv::imshow("test", binary_map);
  //cv::waitKey(0);
  
  cv::Mat marker = binary_map.clone();
  marker = marker + 1;
  marker.convertTo(marker, CV_32S);

  cv::watershed(dirt_frame_, marker);
 
  mask_frame_ = marker.clone();
  for( int i = 0; i < marker.rows; i++ )
  {
    for( int j = 0; j < marker.cols; j++ )
       {
        int index = marker.at<int>(i,j);
            if( index == -1 )
                mask_frame_.at<int>(i,j) = 0;
            else if( index == 1 )
                mask_frame_.at<int>(i,j) = 1;
            else
                mask_frame_.at<int>(i,j) = 0;
         }
    }
    
  //std::cout << mask_frame_;   
  mask_frame_.convertTo(mask_frame_, gray_dirt.type());
  
  int morph_size = 2;
  Mat element = cv::getStructuringElement( cv::MORPH_RECT, Size( 2*morph_size + 1, 2*morph_size+1 ));
  cv::morphologyEx( mask_frame_, mask_frame_, cv::MORPH_CLOSE, element );
  
  cv::imshow("test", mask_frame_* 255);
  cv::waitKey(0);
     
}
