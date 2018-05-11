/*This is the assisment 1 for the paper 159731
programmed by Jerry Wang, Student ID: 02304430
return 0 if normal
otherwise return -1
there are two functions
1:int countblob(Mat img) to count and return the blob number in the image
2:Mat medianfilter(Mat Img) to create a new img based on the 3x3 kernels*/
#include <set>
#include <chrono>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
using namespace cv;
using namespace std;
using namespace chrono;
#define Mpixel(images, x, y) ((uchar*)(((images).data)+(y)*((images).step)))[(x)]

int countblobs(Mat img)
{   int counter = 0, s1, s2;
	int c, r, cc, rr;
	Point mypoint;
	vector<Point> myset[2000];
	vector<Point>::iterator itr;
	Mat A=Mat::zeros(img.size(), CV_16U);
	for (r = 0;r < img.rows;r++)
	{	for (c = 0;c < img.cols;c++)
		{	if (Mpixel(img, c, r) != 0)
			{	int img1, img2;
				mypoint.x = c;
				mypoint.y = r;
				if ((c - 1) >= 0) img1 = Mpixel(img, c - 1, r); else img1 = 0;
				if ((r - 1) >= 0) img2 = Mpixel(img, c, r - 1); else img2 = 0;
				if ((img1 != 0) || (img2 != 0))
				{	if ((c - 1) >= 0) s1 = Mpixel(A, c - 1, r); else s1 = 0; //left
					if ((r - 1) >= 0) s2 = Mpixel(A, c, r - 1); else s2 = 0; //upper
					if (s1 != 0)  //left
					{	myset[s1].push_back(mypoint);
						Mpixel(A, c, r) = s1;
					}
					if (s2 != 0)   //upper
					{	myset[s2].push_back(mypoint);
						Mpixel(A, c, r) = s2;
					}
					if ((s1 != s2) && (s1 != 0) && (s2 != 0))//SET[s2]->[SET[s1],keep set SET[s1] and empty the other,Reset all points of A(c, r) belonging to SET[s2] to s1
					{	for (itr=myset[s2].begin();itr!=myset[s2].end();itr++)
						{	cc = itr->x;
							rr = itr->y;
							Mpixel(A, cc, rr) = s1;
							myset[s1].push_back(*itr);
						}
						myset[s2].clear();
					}
				}
				else
				{	counter = counter + 1;
					myset[counter].push_back(mypoint);
					Mpixel(A, c, r) = counter;
				}
			}
		}
	}
	int count = 0;//find numbers of not-empty sets as blob number
	for (int kk = 1;kk <= counter;kk++) if (myset[kk].size() > 40) count += 1;
	return count;
}

Mat medianfilter(Mat img)
{	int r, c;
	int median[9],mediancopy[9];
	for (r = 0;r < img.rows;r++)
	{	mediancopy[9] = { 0 };
		median[9] = { 0 };
		for (c = 0;c < img.cols;c++)//read value from the 3x3 kernel,1st:read all 3x3 when c=0 then copy to mediancopy for next round
		{	if (0 == c)
			{	if ((r - 1 >= 0) && (c - 1 >= 0)) median[0] = Mpixel(img, c-1,r - 1); else median[0] = 0;
				if ((r - 1) >= 0) median[1] = Mpixel(img,c, r-1); else median[1] = 0;
				if (((r - 1) >= 0) && ((c + 1) < img.cols)) median[2] = Mpixel(img, c+1,r-1); else median[2] = 0;
				if ((c - 1) >= 0) median[3] = Mpixel(img, c - 1,r); else median[3] = 0;
				median[4] = Mpixel(img, c,r);
				if ((c + 1) < img.cols) median[5] = Mpixel(img, c + 1,r); else median[5] = 0;
				if (((r + 1) < img.rows) && ((c - 1) >= 0)) median[6] = Mpixel(img, c-1,r+1); else median[6] = 0;
				if ((r + 1) < img.rows) median[7] = Mpixel(img,  c,r + 1); else median[7] = 0;
				if (((r + 1) < img.rows) && ((c + 1) < img.cols)) median[8] = Mpixel(img, c+1,r+1); else median[8] = 0;
			}
			else //read next 3 pixels on the right of the kernel only 
			{	median[0] = mediancopy[1];
				median[1] = mediancopy[2];
				if (((r - 1) >= 0) && ((c + 1) < img.cols)) median[2] = Mpixel(img,c+1,r-1); else median[2] = 0;
				median[3] = median[4];
				median[4] = mediancopy[5];
				if ((c + 1) < img.cols) median[5] = Mpixel(img,c + 1,r); else median[5] = 0;
				median[6] = mediancopy[7];
				median[7] = mediancopy[8];
				if (((r + 1) < img.rows) && ((c + 1) < img.cols)) median[8] = Mpixel(img, c+1,r+1); else median[8] = 0;
			}
			memcpy(mediancopy, median, sizeof(median));//backup median for next movement
			int temp,z,k;// sort array median[]
			for (z = 0;z<9;z++)
			{	for (k = z+1;k<9;k++)
				{	if (median[z]>median[k])
					{	temp = median[z];
						median[z] = median[k];
						median[k] = temp;
					}
				}
			}
			Mpixel(img, c, r) = median[4];//median[4] is the value write back to img
		}
	}
	return img;
}

int main(int argc, char**argv)
{	VideoCapture vcap; //initialize capture
	Mat image,image2; //Create Matrix to store images
	char text[100];
	if (argc < 2)		// Check for invalid input (if>=2,static imgfile), or (video cam  if<2)
	{	vcap.open(0);
		if (!vcap.isOpened())
		{	cout << "Could not open video camera" << endl;
			return -1;
		}
		cout << "Video camera is on" << endl;
		namedWindow("Web Camera", CV_WINDOW_AUTOSIZE);
		vcap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
		vcap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
		float fps = 0.0;
		int count = 0;
		do
		{	system_clock::time_point start = system_clock::now();
			vcap >> image;          //copy webcam stream to image
			if (image.empty()) break;
			cvtColor(image, image2, CV_BGR2GRAY);//BGR -> GRAY
			image2 = medianfilter(image2);// Apply Median
			threshold(image2, image2, 0, 255, cv::THRESH_OTSU);// Apply threshlod to binary image
			count=countblobs(image2);//start count blob
			sprintf_s(text, "%d %2.1f", count,fps);
			putText(image, text, cvPoint(10, 30), FONT_HERSHEY_PLAIN, 2, cvScalar(255, 255, 255), 2, 8);
			imshow("Web Camera", image);          //print image to screen
			system_clock::time_point end = system_clock::now();
			float seconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			fps = 1000000 / seconds;
		} while (waitKey(30) < 0);
	}
	else  //with argument, but read the 1st argument argv[1] only
	{	image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
		if (!image.data)
		{	cout << "Could not open image file: " << argv[1] << endl;
			return -1;
		}
		cout << "image size: " << image.cols << " x " << image.rows << endl;
		namedWindow("Orginal", CV_WINDOW_AUTOSIZE);
		imshow("Orginal", image);          //print image to screen
		Mat grayscaleMat(image.size(), CV_8U);//Grayscale matrix
		cvtColor(image, grayscaleMat, CV_BGR2GRAY);//Convert BGR to Gray
		image = medianfilter(grayscaleMat);
		namedWindow("MedianFilter", CV_WINDOW_AUTOSIZE);
		imshow("MedianFilter", image);          //print image to screen
		threshold(grayscaleMat, image, 0, 255, cv::THRESH_OTSU);//thresholding
		namedWindow("Threshlod", CV_WINDOW_AUTOSIZE);
		imshow("Threshlod", image);         //print image to screen
		int count = 0;						// start count the blob
		count = countblobs(image);
		cout << "Found:" << count << " Blob" <<endl;
		sprintf_s(text, "%d", count);		//show count blob numbers
		putText(image, text, cvPoint(10, 30), FONT_HERSHEY_PLAIN, 2, cvScalar(255, 255, 255), 2, 8);
		namedWindow("CountBlob", CV_WINDOW_AUTOSIZE);
		imshow("CountBlob", image);          //print image to screen
		waitKey(0);
	}
	return 0;
}