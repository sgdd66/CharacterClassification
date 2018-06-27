#include"stdafx.h"

namespace Dollar{

    void binary(const Mat& src,Mat& retVal);

    void edgeFilter(Mat &src);

    void addRegion(Mat &src,int row,int begin,int end,queue<Vec2s> &regions);    

    void split(Mat &src,vector<Mat> &retVal,vector<int> &kind);

    void densityFeature(const Mat& img,float* feature);

    void train(char &aimChar);

    void test(const vector<Mat> &imgs,const vector<int> kind,char* outcome);

    const int DimNum=17;
    const int CharNum=14;
    const int NumNum=10;

}
