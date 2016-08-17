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
void transform_image(std::string file_path); 
void hough_get_circles(int threshold, unsigned int* accumulator, int acc_w, int acc_h, int current_radius, std::vector< std::pair< std::pair<int, int>, int> > &circles);  


// this sorts the larger circle over the smaller if there is an overlap 
struct SortCirclesDistance
{
    bool operator()( const std::pair< std::pair<int, int>, int>& a, const std::pair< std::pair<int, int>, int>& b )
    {


    	int distance_between_circles = sqrt( pow(abs(a.first.first - b.first.first), 2) + pow(abs(a.first.second - b.first.second), 2) );
    	if(distance_between_circles <= a.second + b.second)
    	{
    		// circles are overlapping 
    		return a.second > b.second;
    	   		return false;

    	}
    }
 };

int main(int argc, char* argv[])
{
	if (argv[0]){
		std::cout << "daww"; 
	}

	std::string file_path = "/home/jacob/Desktop/hough_transform/images/testseq100000.pgm";

    std::cout << "TO QUIT PROGRAM PLEASE USE A COMMMAND LINE ESCAPE COMMAND E.G CONTROL + C";
    std::cout << "STARTING HOUGH DETECTION";

	cv::namedWindow("Result", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Circle Detection",  cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Accumulator",	 cv::WINDOW_AUTOSIZE);

    cvMoveWindow("Result", 50, 50);
    cvMoveWindow("Circle Detection", 680, 10);
    cvMoveWindow("Accumulator", 1350, 1000);

    transform_image(file_path);


    return 0; 
}

int hough_transform(unsigned char* image, unsigned int* accum,int w, int h, int r)
{
	int radius = r;

	//accum height is the same as image height
	int width = w;
	int height = h;
	float deg_to_rad = 0.017453293f;

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
					int a = ((double)x - ((double)radius * cos((double)t * deg_to_rad)));
					int b = ((double)y - ((double)radius * sin((double)t * deg_to_rad)));

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
void hough_get_circles(int threshold, unsigned int* accumulator, int acc_w, int acc_h, int current_radius, std::vector< std::pair< std::pair<int, int>, int> > &circles)
{
	int found = 0;
	if (accumulator == 0)
	{
		std::cout << "accumulator not filled"; 
		return;
	}

	for (int curr_accum_h = 0; curr_accum_h < acc_h; curr_accum_h++)
	{
		for (int curr_accum_w = 0; curr_accum_w < acc_h; curr_accum_w++)
		{
			// is it greater than our global threshold? 
			int accum_count = (int)accumulator[(curr_accum_h*acc_w) + curr_accum_w];
			if (accum_count >= threshold)
			{
				//local maxima or 'bright spot'? 
				int max = accum_count;
				for(int local_y=-4; local_y<=4;local_y++)
				{
					for(int local_x=-4; local_x<=4;local_x++)
					{
						int height = local_y + curr_accum_w;
						int width = local_x + curr_accum_w; 

						//check boundaries
						if (height >=0 && height <= acc_h && width >=0 && width <= acc_w)
						{
							//does this point have a higher count?
							if ((int)accumulator[(height * acc_w) + width] > max)
							{
								max = accumulator[(height * acc_w) + width];
								//end search
								local_y = local_x = 5; 
							} 
						}
					}
				}
				// if max is larger than current count there is a larger count close by
				if (max > (int)accumulator[(curr_accum_h * acc_w) + curr_accum_w])
				{
					continue;
				}

				circles.push_back(std::pair< std::pair<int, int>, int>(std::pair<int, int>(curr_accum_w, curr_accum_h), current_radius));
				found++;
			}
		}
	}

}


// COPIED CODE DIRECTLY - OPENCV IMAGE SHOWING STUFF, sorts circles so that the larger ones are displayed over the smaller 
void filter_and_show(std::vector< std::pair< std::pair<int, int>, int> > circles, cv::Mat img_edge, cv::Mat img_ori)
{


	//sorts circles 
	std::sort(circles.begin(), circles.end(), SortCirclesDistance());
	int a,b,r;
	a=b=r=0;
	std::vector< std::pair< std::pair<int, int>, int> > result;
	std::vector< std::pair< std::pair<int, int>, int> >::iterator it;
	for(it=circles.begin();it!=circles.end();it++)
	{
		int d = sqrt( pow(abs(it->first.first - a), 2) + pow(abs(it->first.second - b), 2) );
		if( d > it->second + r)
		{
			result.push_back(*it);
			//ok
			a = it->first.first;
			b = it->first.second;
			r = it->second;
		}
	}

	cv::Mat img_res = img_ori.clone();
	for(it=result.begin();it!=result.end();it++)
	{
		std::cout << it->first.first << ", " << it->first.second << std::endl;
		cv::circle(img_res, cv::Point(it->first.first, it->first.second), it->second, cv::Scalar( 0, 0, 255), 2, 8);
	}
	cv::imshow("Result", img_res);
	cv::imshow("Circle Detector", img_edge);
	cv::waitKey(360000);	
}

void transform_image(std::string file_path)
{
	//load image
	cv::Mat img_edge;
	cv::Mat img_blur;

	cv::Mat img_ori = cv::imread( file_path, 1 );
	cv::blur( img_ori, img_blur, cv::Size(5,5) );
	cv::Canny(img_blur, img_edge, 175, 200, 3);
	
	int w = img_edge.cols;
	int h = img_edge.rows;


	//fill accumulator
	unsigned int* accumulator = (unsigned int*)calloc(w*h, sizeof(unsigned int*));
	
	//to store circles
	std::vector< std::pair< std::pair<int, int>, int> > circles;

	// for each radius 
	for (int r = 20; r < 100; r=r+1)
	{
		//transform
		hough_transform(img_edge.data, accumulator, w, h, r);



		int threshold = 0.95 * (2.0 * (double)r * 0.314);
		//get circles

		// draw result

		

		{
			hough_get_circles(threshold, accumulator, w, h, r, circles);
			
			int aw = w;
			int ah = h; 
			int maxa = 0;

			for(int p=0;p<(ah*aw);p++)
			{
				if((int)accumulator[p] > maxa)
					maxa = accumulator[p];
			}
			double contrast = 1.0;
			double coef = 255.0 / (double)maxa * contrast;

			cv::Mat img_accu(ah, aw, CV_8UC3);
			for(int p=0;p<(ah*aw);p++)
			{
				unsigned char c = (double)accumulator[p] * coef < 255.0 ? (double)accumulator[p] * coef : 255.0;
				img_accu.data[(p*3)+0] = 255;
				img_accu.data[(p*3)+1] = 255-c;
				img_accu.data[(p*3)+2] = 255-c;
			}

			cv::imshow("Original", img_ori);
			cv::imshow("Circle Detector", img_edge);
			cv::imshow("Accumulator", img_accu);

			cv::waitKey(1);
	}
	}

	{
		filter_and_show(circles, img_edge, img_ori);
	}


}	
