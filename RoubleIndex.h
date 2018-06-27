#include"stdafx.h"

namespace Rouble{
    void getHorizontalBoundary(const Mat& src,int &up,int &down,float &spinAngle);

    void FindWord(const Mat& src,const int &up,const int &down,const float &spinAngle,Mat &retval);

    void split(const Mat& src,vector<Mat> &retVal);

    void ImgToSquare(vector<Mat> &wordImg);

    void cutEdge(vector<Mat> &wordImg);

    void train1(char &aimChar);

    void densityFeature(const Mat& img,float* feature);

    void test1(const vector<Mat> &imgs,char* outcome);

    void binary(const Mat& src,Mat& retVal);

    const int CharacterNum = 11;
    const int DimNum = 22;
    
}
