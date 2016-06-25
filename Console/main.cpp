#include <fstream>
#include <iostream>
#include <vector>

using namespace std;


struct Vertex  {
	double x;
	double y;
	double z;
};

/*
int main()
{

	
	string filename("F:\\Work\\RGBD dataset\\boy\\007_1_d.dat");
	string filenamevx("F:\\Work\\RGBD dataset\\boy\\007_1_d.vx");

	ifstream firIn(filename, ios::in | ios::binary);

	int width  (640);
	int height (480);

	vector<int> depth;
	while (!firIn.eof())
	{
		int i;
		firIn >> i;
		depth.push_back(i);
	}

	ofstream out(filenamevx, ios::out | ios::binary);
	for (int i = 0; i < 640; ++i)
	{
		for (int j = 0; j < 480; ++j)
		{
			Vertex vx;
			vx.x = i;
			vx.y = j;
			int val = depth[i * j];
			vx.z = ((val > 0 && val < 300 ) || val > 1200) ? 0 : val ;
			out << vx.x << vx.y << vx.z;
		}
	}

	cout << firIn.good() << " " << depth.size() << endl;
	firIn.close();
	cin.get();
	return 0;
}
*/

/*

#include <memory>
#include <list>

#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>

using namespace cv;
using namespace std;

String face_cascade_name = "F:\\Biometric Software\\C++\\git\\Console\\haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;

std::vector<Rect> detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	return faces;
}


int main()
{
	if (!face_cascade.load(face_cascade_name))
	{
		printf("--(!)Error loading\n"); return -1;
	};

	// Open another image
	std::string shwarc = "C:\\Users\\jacke\\Desktop\\kl3.jpg";
	Mat image;
	image = cv::imread(shwarc);

	Mat image2 = image.clone();

	std::vector<cv::Rect> faces(detectAndDisplay(image2));

	cv::Mat bgModel, fgModel;	
	cv::Mat result;

	auto start = clock();
	for (auto it = faces.begin(); it != faces.end(); ++it)
	{
		
		cv::grabCut(image,    // input image
			result,   // segmentation result
			*it,// rectangle containing foreground
			bgModel, fgModel, // models
			5,        // number of iterations
			cv::GC_INIT_WITH_RECT); // use rectangle
			
		//mog(image2, fgModel, 0.01);
	}


	std::cout << clock() - start << std::endl;
	
	cv::compare(result, cv::GC_PR_FGD, result, cv::CMP_EQ);
	
	cv::Mat foreground(image.size(), CV_8UC3, cv::Scalar(255, 255, 255));

	image.copyTo(foreground, result); 
	
	imwrite("Foreground.jpg", foreground);

	std::cin.get();

	return 0;
}
*/

/*
#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

int main(int argc, char *argv[])
{
	cv::Mat frame;
	cv::Mat back;
	cv::Mat fore;
	cv::VideoCapture cap(0); //define video source
	cv::BackgroundSubtractorMOG2 mog(500, 16, false);
	//cv::Ptr<cv::BackgroundSubtractor> bg = cv::createB(500, 16, false);

	std::vector<std::vector<cv::Point> > contours;

	cv::namedWindow("Frame");
	cv::namedWindow("Background");

	for (;;)
	{
		cap >> frame;
		mog(frame, fore);
		mog.getBackgroundImage(back);
		//cv::erode(fore, fore, cv::Mat());
		//cv::dilate(fore, fore, cv::Mat());
		//cv::findContours(fore.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		//cv::drawContours(frame, contours, -1, cv::Scalar(0, 0, 255), 2);
		cv::imshow("Foreground", fore);
		cv::imshow("Frame", frame);
		//cv::imshow("Background", back);
		if (cv::waitKey(30) >= 0) break;
	}
	return 0;
}
*/
