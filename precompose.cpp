#include"precompose.h"

//使用局部二值化的方法，但仍存在很多噪点
void AI::local_binary(const Mat& src,Mat& retVal){
    // Mat kernal_3x3=(Mat_<double>(3,3)<<0.125,0.125,0.125,
    //                             0.125,0,0.125,
    //                             0.125,0.125,0.125);
    // Mat kernal_3x3=Mat::zeros(Size(3,3),CV_64FC1);
    // kernal_3x3=(kernal_3x3+1)/9.0;
    // Mat kernal_5x5=Mat::zeros(Size(5,5),CV_64FC1);
    // kernal_5x5=(kernal_5x5+1)/25.0;
    int dim=50;
    Mat kernal=Mat::zeros(Size(dim,dim),CV_64FC1);
    kernal=(kernal+1.0)/(dim*dim);
  

    Mat img1,img2,img3,img4;
    int channel=src.channels();
    if(channel!=1){
        cvtColor(src,img1,CV_BGR2GRAY);
    }else{
        src.copyTo(img1);
    }

    filter2D(img1,img2,img1.depth(),kernal);
    

    FileStorage fs(format("%s/argument.xml",DataPath), FileStorage::READ);
    float arg1;
    fs["arg1"] >> arg1;    
    for(int i=0;i<img1.rows;i++){
        for(int j=0;j<img1.cols;j++){
            if(img1.at<uchar>(i,j)>img2.at<uchar>(i,j)/arg1){
                img1.at<uchar>(i,j)=255;
            }else{
                img1.at<uchar>(i,j)=0;
            }
        }
    }
    
    img1.copyTo(retVal);
}

void AI::Hist_hsv(const Mat& src){
    Mat img1,img2,img3;
    cvtColor(src,img1,CV_BGR2HSV);
    Mat hist;
    int histSize[1]={256};
    int channals[]={2};
    float range1[]={0,256};
    float range2[]={0,180};
    const float* ranges1[]={range1};
    const float* ranges2[]={range2};
    
    channals[0]=0;
    calcHist(&img1,1,channals,Mat(),hist,1,histSize,ranges2,true,false);
    ofstream file("/home/sgdd/AI_competition/Data/Mat_h.csv");
    file << format(hist, Formatter::FMT_CSV);
    file.close();    

    channals[0]=1;
    calcHist(&img1,1,channals,Mat(),hist,1,histSize,ranges1,true,false);
    file.open("/home/sgdd/AI_competition/Data/Mat_s.csv");
    file << format(hist, Formatter::FMT_CSV);
    file.close();  

    channals[0]=2;
    calcHist(&img1,1,channals,Mat(),hist,1,histSize,ranges1,true,false);
    file.open("/home/sgdd/AI_competition/Data/Mat_v.csv");
    file << format(hist, Formatter::FMT_CSV);
    file.close();  


}

void AI::Hist_gray(const Mat& src){
    Mat img1,img2,img3;
    if(src.channels()==1){
        src.copyTo(img1);
    }else{
        cvtColor(src,img1,CV_BGR2GRAY);        
    }
    Mat hist;
    int histSize[1]={256};
    int channals[]={0};
    float range[]={0,256};
    const float* ranges[]={range};

    calcHist(&img1,1,channals,Mat(),hist,1,histSize,ranges,true,false);
    string str=format("%s/Mat_gray.csv",DataPath);
    ofstream file(str);
    file << format(hist, Formatter::FMT_CSV);
    file.close();       
}

void AI::smoothing(Mat& histMat, short win){
    short length=histMat.rows;
    float *data=new float[length];
    for(int i=0;i<length;i++){
        data[i]=histMat.at<float>(i,0);
    }

	if(length<win)
		return;
	float* tmp=new float[length];
	for(int i=0;i<length;i++){
		if(i<win)
			tmp[i]=AI::mean(data,length,0,i+win+1);
		else if(i>length-win-1)
			tmp[i]=AI::mean(data,length,i-win,length);
		else
			tmp[i]=AI::mean(data,length,i-win,i+win+1);
	}
	for(int i=0;i<length;i++)
		histMat.at<float>(i,0)=tmp[i];
	delete [] tmp;
    delete [] data;
}


// 对数组data从begin到end-1求均值
float AI::mean(float* data,short length, short begin, short end){
	float sum=0;
	if(begin>=end || end>length)
		return -1;

	for(int i=begin;i<end;i++)
		sum+=data[i];

	return sum/(end-begin);
}

//统计图片的灰度值出现次数。根据测试的结果，文本区域的数目是很少的，基本上不会产生峰值，而纸面
//的白色区域的数目很大，会产生一个明显的峰值。找到小于这个峰值的第一个谷值，将此作为二值化的门限值。
//这种方法对于背景灰度相似的图片适用。如果纸面背景有图案或者由于曝光产生不同的色块，这种方法就会产生黑斑。
void AI::binary(const Mat& src,Mat& retVal){
    Mat img1,img2,img3;
    cvtColor(src,img1,CV_BGR2GRAY);
    Mat hist;
    int histSize[1]={256};
    int channals[]={0};
    float range[]={0,256};
    const float* ranges[]={range};

    calcHist(&img1,1,channals,Mat(),hist,1,histSize,ranges,true,false);
    smoothing(hist,2);
    smoothing(hist,2);    
    double max;
    minMaxLoc(hist,0,&max,0,0);
    double thresh=max*0.5;
    int firstPeak;
    for(int i=0;i<hist.rows;i++){
        if(hist.at<float>(i,0)>thresh){
            float last=hist.at<float>(i-1.0);
            float next=hist.at<float>(i+1,0);
            float loc =hist.at<float>(i,0);
            if(loc>=last && loc>=next){
                firstPeak=i;
                break;
            }
        }
    }
    int firstValley;
    for(int i=firstPeak;i>0;i--){
        float last=hist.at<float>(i-1.0);
        float next=hist.at<float>(i+1,0);
        float loc =hist.at<float>(i,0);
        if(loc<=last && loc<=next){
            firstValley=i;
            break;
        }        
    }
    threshold(img1,img2,firstValley,255,THRESH_BINARY);
    retVal=img2;
}


void AI::test(const Mat& src){
    Mat img1,img2,img3;
    cvtColor(src,img1,CV_BGR2GRAY);
    threshold(img1,img2,120,255,THRESH_BINARY);
    threshold(img1,img3,110,255,THRESH_BINARY);    
    imshow("img2",img2);
    imshow("img3",img3);
    waitKey(0);

    // cvtColor(src,img1,CV_BGR2HSV);
    // Mat hsv[3];
    // split(img1,hsv);
    // img2=Mat::zeros(img1.size(),CV_8UC1);
    // for(int i=0;i<img1.rows;i++){
    //     for(int j=0;j<img1.cols;j++){
    //         if(hsv[0].at<uchar>(i,j)>75 && hsv[0].at<uchar>(i,j)<85){
    //             img2.at<uchar>(i,j)=255;
    //         }
    //     }
    // }
    // imshow("s",hsv[2]);
    // waitKey(0);

}

void AI::hough(const Mat& src,Mat& retval){
    // xml读取
    FileStorage fs("/home/sgdd/AI_competition/Data/argument.xml", FileStorage::READ);
    int arg1,arg2;
    fs["arg1"] >>arg1;
    fs["arg2"] >> arg2;
    Mat img1,img2,img3;
    src.copyTo(retval);
 	Mat element=getStructuringElement(MORPH_RECT,Size(arg2,arg2));
	morphologyEx(retval,retval,MORPH_ERODE,element);   
    Canny(retval,img1,50,200,3);
    vector<Vec2f> lines;
    HoughLines(img1,lines,1,CV_PI/180,arg1);
    for(int i=0;i<lines.size();i++){
        float rho=lines[i][0];
        float theta = lines[i][1];
        float a=cos(theta);
        float b=sin(theta);
        float x0=a*rho;
        float y0=b*rho;
        Point pt1,pt2;
        pt1.x=(x0+1000*(-b));
        pt1.y=(y0+1000*(a));
        pt2.x=(x0-1000*(-b));
        pt2.y=(y0-1000*(a));  
        line(retval,pt1,pt2,Scalar(50),1);
    }
    imshow("img1",img1);
    imshow("img",retval);
    waitKey(0);   
}

void AI::getHorizontalBoundary(const Mat& src,Point &upLine,Point &downLine){
    int row=src.rows;
    float col=static_cast<float>(src.cols);
    Mat hist=Mat::zeros(Size(row,row),CV_32S);

    for(int i=0;i<row;i++){
        for(int j=0;j<row;j++){
            float row1=static_cast<float>(i);
            float row2=static_cast<float>(j);
            float k=(row2-row1)/col;
            float b=row1;
            short blackPointNum=0;
            for(int m=0;m<src.cols;m++){
                float t=k*m+b;
                for(int n=0;n<src.rows;n++){
                    if(n>t)
                        break;
                    if(src.at<uchar>(n,m)<10){
                        blackPointNum+=1;
                    }
                }
            }
            hist.at<short>(i,j)=blackPointNum;
        }
    }
    // Mat outputMat=hist;
    // ofstream file1("/home/sgdd/AI_competition/Data/grad.txt",ios_base::out);
    // char tmp[30];
    // for(int i=0;i<outputMat.rows;i++){
    //     for(int j=0;j<outputMat.cols-1;j++){
    //         sprintf(tmp,"%d\t",outputMat.at<short>(i,j));
    //         file1<<tmp;
    //     }
    //     sprintf(tmp,"%d",outputMat.at<short>(i,outputMat.cols-1));
    //     file1<<tmp<<endl;        
    // }
    // file1.close();


    Mat hist1=Mat::zeros(hist.size(),CV_8UC1);
    double min=hist.at<short>(0,0);
    double max=hist.at<short>(row-1,row-1);
    if(max==min){
        upLine.x=0;
        upLine.y=0;
        downLine.x=src.rows-1;
        downLine.y=src.rows-1;
        return;
    }
    char t;
    for(int i=0;i<hist1.rows;i++){
        for(int j=0;j<hist1.cols;j++){
            t=(hist.at<short>(i,j)-min)/(max-min)*255;
            hist1.at<uchar>(i,j)=t;
        }
    }
    Mat grad_x,grad_y,grad;
    Sobel(hist1,grad_x,hist1.depth(),1,0,3,1,0,BORDER_DEFAULT);
    Sobel(hist1,grad_y,hist1.depth(),0,1,3,1,0,BORDER_DEFAULT);
    grad=grad_x+grad_y;
    // csv输出
    ofstream file("/home/sgdd/AI_competition/Data/grad.csv");
    file << format(grad, Formatter::FMT_CSV);
    file.close();
    file.open("/home/sgdd/AI_competition/Data/hist1.csv");
    file << format(hist1, Formatter::FMT_CSV);
    file.close();   

    // Point maxPt;
    // minMaxLoc(grad,0,0,0,&maxPt);
    // int splitPt=(maxPt.x+maxPt.y)/2;
    vector<Point> array;
    Point pt;
    pt.x=0;
    pt.y=0;
    for(int i=1;i<row;i++){
        if(grad.at<uchar>(i,i)!=0 && grad.at<uchar>(i-1,i-1)==0){
            pt.x=i;
        }
        if(grad.at<uchar>(i,i)==0 && grad.at<uchar>(i-1,i-1)!=0){
            pt.y=i;
            array.push_back(pt);
            pt.x=0;
            pt.y=0;
        }        
    }
    int S=0;  

    std::sort(array.begin(),array.end(),[](Point &pt1,Point &pt2){return (pt1.y-pt1.x)>(pt2.y-pt2.x);});
    int splitPt=(array[0].y+array[0].x)/2;
    if(array.size()>1){
        int x,y;
        x=array[0].x;
        y=array[0].y;
        int diff1=hist1.at<uchar>(y,y)-hist1.at<uchar>(x,x);
        diff1=diff1*(y-x);
        x=array[1].x;
        y=array[1].y;
        int diff2=hist1.at<uchar>(y,y)-hist1.at<uchar>(x,x);
        diff2=diff2*(y-x);          
        if(diff2>diff1) 
            splitPt=(array[1].y+array[1].x)/2;     
    }


    S=0;
    for(int i=0;i<splitPt;i++){
        for(int j=0;j<splitPt;j++){
            if(grad.at<uchar>(i,j)==0 && i*j>S){
                upLine.x=i;
                upLine.y=j;
                S=i*j;
            }
        }
    }
    S=0;
    for(int i=splitPt;i<grad.rows;i++){
        for(int j=splitPt;j<grad.cols;j++){
            if(grad.at<uchar>(i,j)==0 && (grad.rows-i)*(grad.cols-j)>S){
                downLine.x=i;
                downLine.y=j;
                S=(grad.rows-i)*(grad.cols-j);
            }
        }
    }
    // Mat retval;
    // src.copyTo(retval);
    // line(retval,Point(0,upLine.x),Point(retval.cols,upLine.y),Scalar(50),1);
    // line(retval,Point(0,downLine.x),Point(retval.cols,downLine.y),Scalar(50),1);    
    // imshow("img",retval);
    // waitKey(0);

}

void AI::FindWord(const Mat& src,const Point &upLine, const Point &downLine,Mat &retval){
    int left=-1;
    int right=-1;
    Point2f virtualQuad[4], realQuad[4];
	Mat warp_matrix(3,3,CV_32FC1,Scalar(0));

    float line_left = upLine.x;
    float line_right = upLine.y;
    float k1=(line_right-line_left)/src.cols;
    float b1=line_left;

    line_left = downLine.x;
    line_right = downLine.y;
    float k2=(line_right-line_left)/src.cols;
    float b2=line_left;

    for(int i=0;i<src.cols;i++){
        int up = k1*i+b1;
        int down = k2*i+b2;
        for(int j=up;j<down;j++){
            if(src.at<uchar>(j,i)<10){
                left=i;
                break;
            }            
        }
        if(left!=-1)
            break;
    }
    for(int i=src.cols-1;i>-1;i--){
        int up = k1*i+b1;
        int down = k2*i+b2;
        for(int j=up;j<down;j++){
            if(src.at<uchar>(j,i)<10){
                right=i;
                break;
            }            
        }
        if(right!=-1)
            break;
    }

    virtualQuad[0].x=left;
    virtualQuad[0].y=left*k1+b1;
    virtualQuad[1].x=right;
    virtualQuad[1].y=right*k1+b1;
    
    virtualQuad[2].x=left;
    virtualQuad[2].y=left*k2+b2;
    virtualQuad[3].x=right;
    virtualQuad[3].y=right*k2+b2;

    float k=( (downLine.x-upLine.x)+(downLine.y-upLine.y) )/2.0;
    k=k/(right-left);
    int height=TextHeight;
    int width = height/k;

	realQuad[0]=Point2f(0,0);
	realQuad[1]=Point2f(width,0);
	realQuad[2]=Point2f(0,height);
	realQuad[3]=Point2f(width,height);

	warp_matrix=getPerspectiveTransform(virtualQuad,realQuad);
	warpPerspective(src,retval,warp_matrix,Size(width,height));      


}





