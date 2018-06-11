#include"stdafx.h"

void binary(const Mat& src,Mat& retVal);

void Hist_hsv(const Mat& src);

void Hist_gray(const Mat& src);

void local_binary(const Mat& src,Mat& retVal);

float mean(float* data,short length, short begin, short end);

void smoothing(Mat& data, short win);

void test(const Mat& src);

void getHorizontalBoundary(const Mat& src,Point &upLine,Point &downLine);

void gradientGray(Mat &src, Mat &mag);

void FindWord(const Mat& src,const Point &upLine,const Point &downLine,Mat &retval);