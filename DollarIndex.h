#include"stdafx.h"

namespace Dollar{

    void binary(const Mat& src,Mat& retVal);

    void edgeFilter(Mat &src);

    void addRegion(Mat &src,int row,int begin,int end,queue<Vec2s> &regions);    

    void split(Mat &src,vector<Mat> &retVal);

}
