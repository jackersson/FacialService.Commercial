#include <memory>
#include <list>

class ITest
{
public:
	virtual void test() const = 0;
};

typedef std::shared_ptr<ITest> ITestPtr;

class Test : public ITest
{
public:
	void test() const override
	{
		
	}
};

typedef std::shared_ptr<Test> TestPtr;

template<typename T>
void testable(T begin) {}

int main()
{
	std::list<TestPtr> tests;
	testable(tests.begin());
	return 0;
}

/*
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
	std::string shwarc = "C:\\Users\\jacke\\Desktop\\2314.jpg";
	Mat image;
	image = cv::imread(shwarc);

	Mat image2 = image.clone();

	std::vector<cv::Rect> faces(detectAndDisplay(image2));

	faces.clear();
	faces.push_back(Rect(0, 0, image2.cols - 20, image2.rows - 20));
	
	cv::Mat result; // segmentation result (4 possible values)
	cv::Mat bgModel, fgModel; // the models (internally used)

	auto start = clock();
	for (auto it = faces.begin(); it != faces.end(); ++it)
	{
		cv::grabCut(image,    // input image
			result,   // segmentation result
			*it,// rectangle containing foreground
			bgModel, fgModel, // models
			1,        // number of iterations
			cv::GC_INIT_WITH_RECT); // use rectangle
	}


	std::cout << clock() - start << std::endl;
	
	cv::compare(result, cv::GC_PR_FGD, result, cv::CMP_EQ);
	
	cv::Mat foreground(image.size(), CV_8UC3, cv::Scalar(255, 255, 255));

	image.copyTo(foreground, result); 
	
	imwrite("Foreground.jpg", foreground);

	return 0;
}
*/