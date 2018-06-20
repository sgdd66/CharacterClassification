#include"precompose.h"

//使用局部二值化的方法，但仍存在很多噪点
void local_binary(const Mat& src,Mat& retVal){
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

void Hist_hsv(const Mat& src){
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

void Hist_gray(const Mat& src){
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

void smoothing(Mat& histMat, short win){
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
			tmp[i]=mean(data,length,0,i+win+1);
		else if(i>length-win-1)
			tmp[i]=mean(data,length,i-win,length);
		else
			tmp[i]=mean(data,length,i-win,i+win+1);
	}
	for(int i=0;i<length;i++)
		histMat.at<float>(i,0)=tmp[i];
	delete [] tmp;
    delete [] data;
}


// 对数组data从begin到end-1求均值
float mean(float* data,short length, short begin, short end){
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
void binary(const Mat& src,Mat& retVal){
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


void test(const Mat& src){
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

void hough(const Mat& src,Mat& retval){
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

void getHorizontalBoundary(const Mat& src,Point &upLine,Point &downLine){
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

void FindWord(const Mat& src,const Point &upLine, const Point &downLine,Mat &retval){
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

void FindWord(const Mat& src,const int &up,const int &down,const float &spinAngle,Mat &retval){
    int left=0;
    int right=src.cols;
    Point2f virtualQuad[4], realQuad[4];
	Mat warp_matrix(3,3,CV_32FC1,Scalar(0));


    float k1=-tan(spinAngle);
    float b1=up;


    float k2=-tan(spinAngle);
    float b2=down;



    virtualQuad[0].x=left;
    virtualQuad[0].y=left*k1+b1;
    virtualQuad[1].x=right;
    virtualQuad[1].y=right*k1+b1;
    
    virtualQuad[2].x=left;
    virtualQuad[2].y=left*k2+b2;
    virtualQuad[3].x=right;
    virtualQuad[3].y=right*k2+b2;

    float k=( down-up);
    k=k/(right-left);
    // int height=TextHeight;
    int height=down-up;
    int width = height/k;

	realQuad[0]=Point2f(0,0);
	realQuad[1]=Point2f(width,0);
	realQuad[2]=Point2f(0,height);
	realQuad[3]=Point2f(width,height);

	warp_matrix=getPerspectiveTransform(virtualQuad,realQuad);
	warpPerspective(src,retval,warp_matrix,Size(width,height));      
   
}

void getHorizontalBoundary(const Mat& src,int &up,int &down,float &spinAngle){
    //首先对文本可能旋转角度采样，例如文本的角度在【-10,10】度之间，
    //我们在这个范围内均匀采10个点，我们就有10个旋转角。然后对每一个旋转角，
    //对每一个图像左侧起点做一条直线，计算这个直线上侧黑色点数。那么只有旋转角与
    //文本的旋转角度最接近时，上下两条直线之间的距离最短


    //选择采样点
    float maxSpinAngle=5/180.0*PI;
    int sampleNum=10;
    float *samples=new float[sampleNum];
    float step=maxSpinAngle*2/(sampleNum-1);
    for(int i=0;i<sampleNum;i++){
        samples[i]=-maxSpinAngle+i*step;
    }

    //计算积分图
    int row=src.rows;
    int col=src.cols;
    Mat integrogram=Mat::zeros(row,col,CV_32S);
    int num=0;
    for(int j=0;j<col;j++){
        num=0;
        for(int i=0;i<row;i++){
            if(src.at<uchar>(i,j)==0){
                num+=1;
            }
            integrogram.at<int>(i,j)=num;
        }
    }

    //计算各条直线上方的黑色点数，数组第一维是角度，第二维是图像左侧起点
    Mat blackPtNum=Mat::zeros(sampleNum,row,CV_32S);

    float k,b;
    int y;
    for(int i=0;i<sampleNum;i++){
        for(int j=0;j<row;j++){
            k=-tan(samples[i]);
            b=j;
            num=0;
            for(int x=0;x<col;x++){
                y=k*x+b;
                if(y<0)
                    y=0;
                if(y>=row)
                    y=row-1;
                num+=integrogram.at<int>(y,x);
            }
            blackPtNum.at<int>(i,j)=num;
        }
    }

    //计算梯度的绝对值
    Mat gradient=Mat::zeros(sampleNum,row,CV_32S);

    for(int i=0;i<sampleNum;i++){
        for(int j=1;j<row;j++){
            gradient.at<int>(i,j)=abs(blackPtNum.at<int>(i,j)-blackPtNum.at<int>(i,j-1));
        }
    }  

    //计算梯度数据中每一列非零值的最大长度
    Mat wordWidth=Mat::zeros(sampleNum,1,CV_32S);

    for(int i=0;i<sampleNum;i++){
        num=0;
        for(int j=0;j<row;j++){
            if(gradient.at<int>(i,j)!=0){
                num+=1;
                if(num>wordWidth.at<int>(i,0)){
                    wordWidth.at<int>(i,0)=num;
                }
            }else{
                num=0;
            }
        }
    }

    //选择WordWidth中的最小值
    int min=wordWidth.at<int>(0,0);
    int index=0;
    for(int i=1;i<sampleNum;i++){
        if(wordWidth.at<int>(i,0)<min){
            min=wordWidth.at<int>(i,0);
            index=i;
        }
    }

    //确定旋转角度
    spinAngle=samples[index];

    //确定上边线的左侧起始点，和下边线的左侧起始点
    up=0;
    down=0;
    for(int i=1;i<row;i++){
        if(gradient.at<int>(index,i-1)==0 && gradient.at<int>(index,i)!=0){
            up=i;
        }
        if(gradient.at<int>(index,i-1)!=0 && gradient.at<int>(index,i)==0){
            down=i;
        }
        if(down-up==wordWidth.at<int>(index,0)){
            break;
        }

    }

    // //在图片上绘制上下边线并显示
    Point2i pt1,pt2;
    pt1=Point(0,up);
    pt2=Point(col-1,up-col*tan(spinAngle));
    line(src,pt1,pt2,Scalar(100),1);
    pt1=Point(0,down);
    pt2=Point(col-1,down-col*tan(spinAngle));
    line(src,pt1,pt2,Scalar(100),1);
    imshow("src",src);
    waitKey(0);

    delete [] samples;


}


void split(const Mat& src,vector<Mat> &retVal){
    int wordSum=11;
    retVal.clear();
    retVal.reserve(wordSum);
    int row=src.rows;
    int col=src.cols;
    Mat hist=Mat::zeros(col,1,CV_16S);
    int sum;

    //将文本区域投影在横轴上，统计黑色区域的数目
    for(int j=0;j<col;j++){
        sum=0;
        for(int i=0;i<row;i++){
            if(src.at<uchar>(i,j)<100){
                sum+=1;
            }
        }
        hist.at<short>(j,0)=sum;
    }

    //滤除文字区域中的黑色噪点
    if(hist.at<short>(0,0)!=0 && hist.at<short>(1,0)==0)
        hist.at<short>(0,0)=0;
    if(hist.at<short>(col-1,0)!=0 && hist.at<short>(col-2,0)==0)
        hist.at<short>(col-1,0)=0;        
    for(int i=1;i<col-1;i++){
        if( hist.at<short>(i,0)!=0 && hist.at<short>(i-1,0)==0 && hist.at<short>(i+1,0)==0)
            hist.at<short>(i,0)==0;
    }

    // ofstream file("/home/sgdd/Internship/Data/Mat.csv");
    // file << format(hist, Formatter::FMT_CSV);
    // file.close();   

    //提取文本可能存在的候选区域
    vector<Vec3i> scales;
    scales.reserve(20);
    Vec3i scale;
    int begin,end;
    begin=0;
    end=0;
    for(int i=1;i<col;i++){
        if(hist.at<short>(i,0)==0 && hist.at<short>(i-1,0)!=0){
            end=i;
            scale[0]=begin;
            scale[1]=end;
            scale[2]=end-begin;
            scales.push_back(scale);
        }
        if(hist.at<short>(i,0)!=0 && hist.at<short>(i-1,0)==0){
            begin=i;
        }
        if(i==col-1 && hist.at<short>(i,0)!=0 &&end<begin){
            end=i;
            scale[0]=begin;
            scale[1]=end;
            scale[2]=end-begin;
            scales.push_back(scale);            
        }
    }
    //筛选出区域宽度最大的。这种方法要求前期二值化不能产生过大的黑斑。逻辑上应该参考文本的间距，
    //可以通过计算相邻距离的方差，然后和方差比对来判别那些区域才是字符所在区域。不过先用简单的方法尝试
    if(scales.size()>wordSum){
        std::sort(scales.begin(),scales.end(),[](Vec3i a,Vec3i b){return a[2]>b[2];});
        for(int i=scales.size()-1;i>=wordSum;i--){
            scales.pop_back();
        }
    }

    if(scales.size()<wordSum){
        printf("the photo is incomplete");
        return;
    }


    //分割原图片，按顺序存放到retVal中
    std::sort(scales.begin(),scales.end(),[](Vec3i a,Vec3i b){return a[0]<b[0];});
    Mat wordImg;
    Rect roi;
    for(int i=0;i<wordSum;i++){
        roi=Rect(scales[i][0],0,scales[i][2],row);
        wordImg=src(roi);
        retVal.push_back(wordImg);
    }

}

//将切割出来的字符扩展为一个正方形，方便特征的提取
//方法是增加短的边，用255填充，使长短边长相等
void ImgToSquare(vector<Mat> &wordImg){
    Mat img,src;
    int col,row;
    Rect rect;
    for(int i=0;i<wordImg.size();i++){
        col=wordImg[i].cols;
        row=wordImg[i].rows;
        // imshow("img1",wordImg[i]);
        if(col>row){
            img=Mat(col,col,CV_8UC1,Scalar(255));
            rect=Rect( 0,(col-row)/2, col,row);
            wordImg[i].copyTo(img(rect));
            wordImg[i]=img;
        }else{
            img=Mat(row,row,CV_8UC1,Scalar(255));
            rect=Rect( (row-col)/2, 0, col,row);
            wordImg[i].copyTo(img(rect));
            wordImg[i]=img;            
        }
        // imshow("img2",wordImg[i]);
        // waitKey(0);
    }

}
//将一张图片的边缘裁掉，重点检查上边线和下边线，左右两侧在上一步的处理中可以保证是紧贴的
void cutEdge(vector<Mat> &wordImg){
    int col,row,index1,index2;
    Rect rect;
    Mat img;

    for(int n=0;n<wordImg.size();n++){
        //检测上边线
        col=wordImg[n].cols;
        row=wordImg[n].rows;
        index1=-1;
        for(int i=0;i<row;i++){
            for(int j=0;j<col;j++){
                if(wordImg[n].at<uchar>(i,j)<100){
                    index1=i;
                    break;
                }
            }
            if(index1!=-1)
                break;
        }

        //检测下边线
        index2=-1;
        for(int i=row-1;i>=0;i--){
            for(int j=0;j<col;j++){
                if(wordImg[n].at<uchar>(i,j)<100){
                    index2=i;
                    break;
                }
            }
            if(index2!=-1)
                break;
        }    

        //图片截取    
        // imshow("src",wordImg[n]);
        rect=Rect(0,index1,col,index2-index1+1);
        img=wordImg[n](rect);
        wordImg[n]=img;
        // imshow("img",img);
        // waitKey(0);
    }
    

}

//训练方法1，使用密度图，三条横向分割线，三条纵向分割线，产生22维的特征向量
void train1(char &aimChar){
    string filePath=format("%s/train/%c/file.txt",DataPath,aimChar);
    ifstream file(filePath,ios_base::in);
    if(!file.is_open()){
        printf("error in open file.txt");
        return;
    }

    Mat img;
    string photoPath;
    float *feature;
    vector<float*> features;
    while(getline(file,photoPath)){
        feature=new float[22];
        img=imread(photoPath,IMREAD_GRAYSCALE);
        densityFeature(img,feature);
        features.push_back(feature);
    }
    file.close();

    int sum=features.size();
    Mat outcome=Mat::zeros(22,sum,CV_32F);
    for(int i=0;i<sum;i++){
        for(int j=0;j<22;j++){
            outcome.at<float>(j,i)=features[i][j];
        }
    }

    filePath=format("%s/DensityFeature/%c.csv",DataPath,aimChar);
    ofstream ofile(filePath);
    ofile << format(outcome, Formatter::FMT_CSV);
    ofile.close();  

    for(int i=0;i<sum;i++)
        delete [] features[i];

}

void densityFeature(const Mat& img,float* feature){
    int row=img.rows;
    int col=img.cols;
    int colIndex[3];
    int rowIndex[3];
    int colStep=col/4;
    int rowStep=row/4;

    //确定列分割位点
    int remainder=col%4;
    switch(remainder){
        case 0:
            colIndex[0]=colStep-1;
            colIndex[1]=colIndex[0]+colStep;
            colIndex[2]=colIndex[1]+colStep;      
            break;
        case 1:
            colIndex[0]=colStep-1;
            colIndex[1]=colIndex[0]+colStep;
            colIndex[2]=colIndex[1]+colStep;      
            break;    
        case 2:
            colIndex[0]=colStep;
            colIndex[1]=colIndex[0]+colStep;
            colIndex[2]=colIndex[1]+colStep;      
            break;         
        case 3:  
            colIndex[0]=colStep;
            colIndex[1]=colIndex[0]+colStep;
            colIndex[2]=colIndex[1]+colStep;      
            break;          
    }

    //确定行分割位点
    remainder=row%4;
    switch(remainder){
        case 0:
            rowIndex[0]=rowStep-1;
            rowIndex[1]=rowIndex[0]+rowStep;
            rowIndex[2]=rowIndex[1]+rowStep;      
            break;
        case 1:
            rowIndex[0]=rowStep-1;
            rowIndex[1]=rowIndex[0]+rowStep;
            rowIndex[2]=rowIndex[1]+rowStep;     
            break;    
        case 2:
            rowIndex[0]=rowStep;
            rowIndex[1]=rowIndex[0]+rowStep;
            rowIndex[2]=rowIndex[1]+rowStep;      
            break;         
        case 3:  
            rowIndex[0]=rowStep;
            rowIndex[1]=rowIndex[0]+rowStep;
            rowIndex[2]=rowIndex[1]+rowStep;    
            break;          
    }    

    //计算16个区域的密度
    float density;
    int cols[5];
    cols[0]=-1;
    cols[1]=colIndex[0];
    cols[2]=colIndex[1];
    cols[3]=colIndex[2];
    cols[4]=col-1;

    int rows[5];
    rows[0]=-1;
    rows[1]=rowIndex[0];
    rows[2]=rowIndex[1];
    rows[3]=rowIndex[2];
    rows[4]=row-1;    
    
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            density=0;
            for(int m=rows[i]+1;m<=rows[i+1];m++){
                for(int n=cols[j]+1;n<=cols[j+1];n++){
                    if(img.at<uchar>(m,n)<100){
                        density+=1;
                    }
                }
            }
            density=density/( (rows[i+1]-rows[i])*(cols[j+1]-cols[j]) );
            feature[i*4+j]=density;
            
        }
    }

    //计算横纵各三条线的密度
    for(int i=0;i<3;i++){
        density=0;
        for(int j=0;j<col;j++){
            if(img.at<uchar>(rowIndex[i],j)<100){
                density+=1;
            }
        }
        density=density/col;
        feature[16+i]=density;
    }

    for(int i=0;i<3;i++){
        density=0;
        for(int j=0;j<row;j++){
            if(img.at<uchar>(j,colIndex[i])<100){
                density+=1;
            }
        }
        density=density/row;
        feature[19+i]=density;
    }

}

//使用密度特征辨识测试样本
#define CharacterNum 11
#define DimNum 22
void test1(const vector<Mat> &imgs,char* outcome){
    //读取每个标签的特征
    string filePath=format("%s/DensityFeature/label.txt",DataPath);
    ifstream file(filePath,ios_base::in);
    if(!file.is_open()){
        printf("can't open file:%s");
    }
    int row=DimNum;
    int col=CharacterNum;
    Mat aims=Mat::zeros(row,col,CV_32F);
    char labels[CharacterNum+1]="0123456789A";
    string line;
    char* nums;
    char text[300];
    char splitChar[]=" ";
    int index=0;
    while(getline(file,line)){
        strcpy(text,line.c_str());       
        nums=strtok(text,splitChar);
        for(int i=0;i<col;i++){
            aims.at<float>(index,i)=atof(nums);
            nums = strtok(NULL, splitChar);
        }
        index++;
    }
    file.close();

    vector<float> distance;
    float feature[DimNum];
    for(int i=0;i<imgs.size();i++){
        distance.clear();
        distance.reserve(CharacterNum);
        densityFeature(imgs[i],feature);     
        for(int n=0;n<CharacterNum;n++){
            float tmp=0;
            for(int d=0;d<DimNum;d++){
                tmp+=pow(feature[d]-aims.at<float>(d,n),2);
            }
            distance.push_back(sqrt(tmp));          
        }
        vector<float>::iterator min=std::min_element(distance.begin(),distance.end());  
        int pos=std::distance(distance.begin(),min);      
        outcome[i]=labels[pos];
    }
    outcome[imgs.size()]='\0';






    

}

//xml输出
// FileStorage fs("/home/sgdd/AI_competition/Data/Mat.xml", FileStorage::WRITE);
// fs << "vocabulary" << hist;
// fs.release();

//xml读取
// FileStorage fs(".\\vocabulary.xml", FileStorage::READ);
// Mat mat_vocabulary;
// fs["vocabulary"] >> mat_vocabulary;

//csv输出
// ofstream file("/home/sgdd/Internship/Data/Mat.csv");
// file << format(hist, Formatter::FMT_CSV);
// file.close();

//csv读取
// CvMLData mlData;
// mlData.read_csv("test.csv");//读取csv文件
// cv::Mat data= cv::Mat(mlData.get_values(),true);