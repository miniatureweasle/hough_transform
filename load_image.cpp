#include <cstring> 
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc_c.h"

#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <iostream>

using namespace cv; using namespace std;

int main()
{

	char file_path[50] = "~/Desktop/hough_transform/testseq100000.pgm";

	cv::Mat img_edge;
	cv::Mat img_blur;

	cv::Mat img_ori = cv::imread(file_path, 1);
	cv::blur(img_ori, img_blur, cv::Size(5,5));
	cv::Canny(img_blur, img_edge, 100, 150, 3);

	int w = img_edge.cols;
	int h = img_edge.rows; 
	cout << "Image Loaded"; 

}
