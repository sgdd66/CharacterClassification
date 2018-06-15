#include"stdafx.h"

void binary(const Mat& src,Mat& retVal);

void Hist_hsv(const Mat& src);

void Hist_gray(const Mat& src);

void local_binary(const Mat& src,Mat& retVal);

float mean(float* data,short length, short begin, short end);

void smoothing(Mat& data, short win);

void test(const Mat& src);

void getHorizontalBoundary(const Mat& src,int &up,int &down,float &spinAngle);

void getHorizontalBoundary(const Mat& src,Point &upline,Point &downline);

void gradientGray(Mat &src, Mat &mag);

void FindWord(const Mat& src,const Point &upLine,const Point &downLine,Mat &retval);

void FindWord(const Mat& src,const int &up,const int &down,const float &spinAngle,Mat &retval);

void split(const Mat& src,vector<Mat> &retVal);

void ImgToSquare(vector<Mat> &wordImg);

void cutEdge(vector<Mat> &wordImg);

void train1(char &aimChar);

void densityFeature(const Mat& img,float* feature);