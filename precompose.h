#include"stdafx.h"

namespace AI{

    void hough(const Mat& src,Mat& retval);

    void binary(const Mat& src,Mat& retVal);

    void Hist_hsv(const Mat& src);

    void Hist_gray(const Mat& src);

    void local_binary(const Mat& src,Mat& retVal);

    float mean(float* data,short length, short begin, short end);

    void smoothing(Mat& data, short win);

    void test(const Mat& src);

    void getHorizontalBoundary(const Mat& src,Point &upline,Point &downline);

    void gradientGray(Mat &src, Mat &mag);

    void FindWord(const Mat& src,const Point &upLine,const Point &downLine,Mat &retval);
}
