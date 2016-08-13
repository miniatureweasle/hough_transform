#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <map>
#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"


int thresh = 0; 

int hough_transform(unsigned char* image, unsigned int* accum, int w, int h, int r); 
void transform_image(std::string file_path, int thresh); 
void hough_get_circles(); 

int main()
{
	std::string file_path = "~/Desktop/hough_transform/testseq100000.pgm";

	std::cout << "creating window";
	cv::namedWindow("Result", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Circle Detection", 	 cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Accumulator",	 cv::WINDOW_AUTOSIZE);

    cvMoveWindow("Result", 10, 10);
    cvMoveWindow("Circle Detection", 680, 10);
    cvMoveWindow("Accumulator", 1350, 10);

    transform_image(file_path, thresh);
    return 0; 
}

int hough_transform(unsigned char* image, unsigned int* accum,int w, int h, int r)
{
	int radius = r;

	//accum height is the same as image height
	int width = w;
	int height = h;

	//loop over each pixel
	for (int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			//check that it is a filled pixel
			if (image[(y * width) + x] > 250)
			{
				//for 360 t
				for (int t = 1; t <= 360; t++)
				{
					int a = ((double)x - ((double)radius * cos((double)t * 0.017453293f)));
					int b = ((double)y - ((double)radius * sin((double)t * 0.017453293f)));

					// check boundary conditions 
					if (( b >= 0 && b < height) && (a >= 0 && a<width))
						accum[(b * width) + a]++;
				}
			}
		}

	}

	return 0; 

}
// puts found circles from accum in passed in vector
void hough_get_circles(int thresh, std::vector< std::pair< std::pair<int, int>, int> > &circles)
{
	std::cout << "getting circles from accumulator";
}

void transform_image(std::string file_path, int thresh)
{
	//load image
	cv::Mat img_edge;
	cv::Mat img_blur;

	cv::Mat img_ori = cv::imread( file_path, 1 );
	cv::blur( img_ori, img_blur, cv::Size(5,5) );
	cv::Canny(img_blur, img_edge, 100, 150, 3);
	
	int w = img_edge.cols;
	int h = img_edge.rows;


	//fill accumulator
	unsigned int* accumulator = (unsigned int*)calloc(w*h, sizeof(unsigned int*));
	//to store circles
	std::vector< std::pair< std::pair<int, int>, int> > circles;

	for (int r = 20; r < 100; r=r+5)
	{
		//transform
		hough_transform(img_edge.data, accumulator, w, h, r); 
		std::cout << r << " search for circle" << std::endl;
		int threshold = 0.95 * (2.0 * (double)r * M_PI);

		//get circles

	}



	//do hough transform


	//display image 


}
