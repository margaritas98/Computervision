// 159731 Assisment 2
// Programmed by: Jerry Wang
// Student ID: 02304430
// Massey University, Auckland, NZ

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace cv;

#define PI 3.14159265

#define X 47
#define Y 47
#define CONCENTRIC_X 6
#define CONCENTRIC_Y 6
#define CONCENTRIC_X1 42
#define CONCENTRIC_Y1 42
#define QR_SIZE 2101

//Macros for colour pixels
#define MpixelB(images,x,y) ( (uchar *) ( ((images).data) + (y)*((images).step) ) ) [(x)*((images).channels())]
#define MpixelG(images,x,y) ( (uchar *) ( ((images).data) + (y)*((images).step) ) ) [(x)*((images).channels())+1]
#define MpixelR(images,x,y) ( (uchar *) ( ((images).data) + (y)*((images).step) ) ) [(x)*((images).channels())+2]

char encodingarray[64]={' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','x','y','w','z',
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','X','Y','W','Z',
    '0','1','2','3','4','5','6','7','8','9','.'};

string QRCode="";
int err=0;

/* Function Headers Used */
Mat imgRotate(Mat image);
Mat imgScale(Mat image);
string ReadQRfromImg(Mat image);

/* Function main */
int main( int argc, const char * argv[] )
{  Mat image;
   if (argc <2) // no image filename
   {   cout<<"need at least 1 argument"<<endl;
       exit(0);
   }
   image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
   if (!image.data)  //not an image file
   { cout <<"open image error"<<endl;
       exit(0);
   }
   printf("image size: %d x %d\n", image.cols,image.rows);
   //imshow("Original image",image);
   if (!err) image=imgRotate(image);
   if (!err) image=imgScale(image);
   cout << ReadQRfromImg(image) <<endl;
   return 0;
}

/* Function  imgRotate */
Mat imgRotate(Mat image)
{   Mat image_gray,image2,image_canny;

    cvtColor( image, image_gray, CV_BGR2GRAY );
    medianBlur(image_gray, image_gray, 5);

    vector<Vec3f> circles;
    HoughCircles(image_gray, circles, CV_HOUGH_GRADIENT, 1,
                 image_gray.rows/16,  // change this value to detect circles with different distances to each other
                 100, 30, 1, 30 // change the last two parameters (min_radius & max_radius) to detect larger circles
                 );
    if (circles.size()!=3) {printf("\n---------------------------------------------------------------------------\nWarning : Image processing error,could not locate ALL 3 positioning circles\n---------------------------------------------------------------------------\n"); err=1; return image;}

    for( size_t j = 0; j < circles.size(); j++ )
    {   Vec3i ck = circles[j];
        circle( image_gray, Point(ck[0], ck[1]), ck[2], Scalar(0,0,255), 3, LINE_AA);
        circle( image_gray, Point(ck[0], ck[1]), 1, Scalar(0,255,0), 3, LINE_AA);
    }
    //imshow("grayimg", image_gray);
    //check centers and redius
    Point center[3];
    Vec3i c[3];
    float line1,line2,line3;
    int radius;
    for( size_t i = 0; i < circles.size(); i++ )
    {   c[i] = circles[i];
        center[i] = Point(c[i][0], c[i][1]);
        radius = c[i][2];
        cout <<"Circle:" << center[i] <<"Radius:" <<radius << endl;
    }
    if ((c[0][0]==c[1][0])&&(c[1][1]==c[2][1])) //double check points are parallel to x y axis
    {   printf("It is a upright image\n");
        return image;
    }
    else //rotate image
    {   double angle=0,scale=1,alpha=1,beta,thera,a1,b1,ab,cd;
        Point2f imgcenter;
        int length;
        //find center of the image, at where the center of the longest line
        line1=sqrt((c[0][0]-c[1][0])*(c[0][0]-c[1][0])+(c[0][1]-c[1][1])*(c[0][1]-c[1][1]));
        line2=sqrt((c[2][0]-c[1][0])*(c[2][0]-c[1][0])+(c[2][1]-c[1][1])*(c[2][1]-c[1][1]));
        line3=sqrt((c[2][0]-c[0][0])*(c[2][0]-c[0][0])+(c[2][1]-c[0][1])*(c[2][1]-c[0][1]));
        if ((line1>line2)&&(line1>line3))
        {   length=line1;
            imgcenter.x=(c[0][0]+c[1][0])/2;
            imgcenter.y=(c[0][1]+c[1][1])/2;
            a1 =imgcenter.y - c[0][1];
            b1=imgcenter.x - c[0][0];
        }
        else if ((line2>line3)&&(line2>line1))
        {   length=line2;
            imgcenter.x=(c[2][0]+c[1][0])/2;
            imgcenter.y=(c[2][1]+c[1][1])/2;
            a1 =imgcenter.y - c[1][1];
            b1=imgcenter.x - c[1][0];
        }
        else
        {   length=line3;
            imgcenter.x=(c[2][0]+c[0][0])/2;
            imgcenter.y=(c[2][1]+c[0][1])/2;
            a1 =imgcenter.y - c[0][1];
            b1=imgcenter.x - c[0][0];
        }
        if (b1!=0) beta=a1/b1; else beta=0;

        //calculate angle of the lines
        ab= alpha - beta;
        cd= 1+(alpha*beta);
        if (cd!=0) thera=atan(ab/cd)*180/PI; else thera=90;
        // alpha beta thera and angle
        printf("alpha = %f  beta= %f  thera=%f\n",alpha,beta,thera);
        if (thera<=0)
        { if (beta<0) { angle=180-thera;} else { angle=abs(thera); }}
          else { int aa=beta; if (aa!=0) {  angle=thera; } else {  angle=360-thera; }
        }
        
        int testrotate=(int)thera;
        if (testrotate==0) { printf("It is a upright image\n"); return image; }
        else printf("Rotating angle = %f\n",angle);
        
        image2.create(image.size(),image.channels());

        Mat rotmatrix=getRotationMatrix2D( imgcenter, angle , scale);
        warpAffine(image, image2, rotmatrix, image.size());//,INTERLINEAR);
        //imshow("rotate",image2);
        //crop the image
        int x1,y1,x2,y2;
        int halflength=length/2;
        halflength=(int)sqrt(halflength*halflength/2);

        double m=length/1162.48;
        printf("coefficient=%f\n",m);
        x1=imgcenter.x-halflength-m*90;
        y1=imgcenter.y-halflength-m*90;
        x2=imgcenter.x+halflength+m*90;
        y2=imgcenter.y+halflength+m*90;

        Mat cropimg;
        image2(Rect(x1,y1,x2-x1,y2-y1)).copyTo(cropimg);
        image=cropimg.clone();
    }
    printf("Cropped image size: (%d x %d)\n",image.rows,image.cols);
    return image;
 }

/* Function  imgScale */
Mat imgScale(Mat image)
{   Mat image2;
    if (image.rows==1000) { printf("No scaling done\n"); return image;}
    //scale image, resize
    if (image.rows>1000) cv::resize(image,image2,cv::Size(1000,1000),0,0,cv::INTER_AREA);
    else cv::resize(image,image2,cv::Size(1000,1000),0,0,cv::INTER_LINEAR);
    //imshow("scale",image2);
    return image2;
}

/* Function  ReadQRfromImg */
string ReadQRfromImg(Mat image)
{   QRCode="";
    int QR[QR_SIZE];
    int x,y,z=0;
    int R,G,B;
    for (y=1;y<=Y;y++)
    {   for (x=1;x<=X;x++)
        {   if ((x<=CONCENTRIC_X)&&(y<=CONCENTRIC_Y)) continue;
            if ((x<=CONCENTRIC_X)&&(y>=CONCENTRIC_Y1)) continue;
            if ((x>=CONCENTRIC_X1)&&(y>=CONCENTRIC_Y1)) continue;
            R=0; B=0; G=0;
            for (int i=0;i<20;i++)
            { for(int j=0;j<20;j++)
              {  R += MpixelR(image,(x-1)*20+30+i,(y-1)*20+30+j);
                 G += MpixelG(image,(x-1)*20+30+i,(y-1)*20+30+j);
                 B += MpixelB(image,(x-1)*20+30+i,(y-1)*20+30+j);
              }
            }
            if (R/20/20 >= 127) R=1; else R=0;
            if (G/20/20 >= 127) G=1; else G=0;
            if (B/20/20 >= 127) B=1; else B=0;
            cout <<R <<G <<B <<" ";
            QR[z]=R*100+G*10+B;
            z++;
        }
    }
    printf("\nTotal qrcodes:%d\n\n",z);
    for (int i=0;i<(z/2);i++)
    {   int value=QR[i*2]*1000+QR[i*2+1];
        char result[6]={0};
        sprintf(result,"%06d",value);
        value=0;
        int k=strlen(result);
        for (int j=0;j<k;j++) value+=(result[j]-'0')*pow(2.0,k-j-1);
        //convert value to encodingarray
        QRCode+=encodingarray[value];
    }
    return QRCode;
}
