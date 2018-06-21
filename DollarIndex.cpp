#include"DollarIndex.h"

//使用局部二值化的方法，但仍存在很多噪点
void Dollar::binary(const Mat& src,Mat& retVal){
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

//将所有与边缘相贴的图案筛选掉
void Dollar::edgeFilter(Mat &src){
    int row=src.rows;
    int col=src.cols;
    queue<Vec2s> blackRegions;
    Vec2s region;
    short begin,end;
    
    //从下边缘开始扫描
    begin=0;
    end=col-1;
    addRegion(src,row-1,begin,end,blackRegions);

    int pos,num;
    for(int i=row-2;i>-1;i--){
        pos=0;
        num=blackRegions.size();
        if(num==0){
            break;
        }
        for(int j=0;j<num;j++){
            region=blackRegions.front();
            blackRegions.pop();
            if(region[1]<pos){
                continue;
            }else if(region[0]>=pos){
                begin=0;
                end=col-1;
                for(int m=region[0];m>=0;m--){             
                    if(src.at<uchar>(i,m)>100){
                        begin=m;
                        break;
                    }
                }
                for(int m=region[1];m<col;m++){
                    if(src.at<uchar>(i,m)>100){
                        end=m;
                        break;
                    }                    
                }
                pos=end;
                addRegion(src,i,begin,end,blackRegions);
            }else{
                begin=pos+1;
                for(int m=region[1];m<col;m++){
                    if(src.at<uchar>(i,m)>100){
                        end=m;
                        break;
                    }                    
                }
                pos=end;
                addRegion(src,i,begin,end,blackRegions);                
            }

        }
    }
    //从上边缘开始扫描
    while(!blackRegions.empty()){
        blackRegions.pop();
    }
    begin=0;
    end=col-1;
    addRegion(src,0,begin,end,blackRegions);
    for(int i=1;i<row;i++){
        pos=0;
        num=blackRegions.size();
        if(num==0){
            break;
        }
        for(int j=0;j<num;j++){
            region=blackRegions.front();
            blackRegions.pop();
            if(region[1]<pos){
                continue;
            }else if(region[0]>=pos){
                begin=0;
                end=col-1;
                for(int m=region[0];m>=0;m--){
                    if(src.at<uchar>(i,m)>100){
                        begin=m;
                        break;
                    }
                }
                for(int m=region[1];m<col;m++){
                    if(src.at<uchar>(i,m)>100){
                        end=m;
                        break;
                    }                    
                }
                pos=end;
                addRegion(src,i,begin,end,blackRegions);
            }else{
                begin=pos+1;
                for(int m=region[1];m<col;m++){
                    if(src.at<uchar>(i,m)>100){
                        end=m;
                        break;
                    }                    
                }
                pos=end;
                addRegion(src,i,begin,end,blackRegions);                
            }

        }
    }

}

//将begin和end中间的黑色区域找出并存放在regions中，同时将该区域置白
void Dollar::addRegion(Mat &src,int row,int begin,int end,queue<Vec2s> &regions){
    Vec2s region;
    region[0]=begin;
    region[1]=begin;
    for(int i=begin+1;i<end+1;i++){
        if(src.at<uchar>(row,i)>100 && src.at<uchar>(row,i-1)<100){
            region[1]=i;
            regions.push(region);
        }else if(src.at<uchar>(row,i)<100 && src.at<uchar>(row,i-1)>100){
            region[0]=i;
        }
    }
    if(region[0]>region[1] && (region[0]!=regions.back()[0] || regions.empty()) ){
        region[1]=end;
        regions.push(region);
    }
    for(int i=begin;i<end+1;i++){
        src.at<uchar>(row,i)=255;
    }
}

//考虑到横向和纵向切割都需要积分图，所以把两个功能放在一个函数中
void Dollar::split(Mat &src,vector<Mat> &retVal){
    int row=src.rows;
    int col=src.cols;
    Mat integrogram=Mat::zeros(row,col,CV_16S);
    //计算积分图
    if(src.at<uchar>(0,0)<100){
        integrogram.at<short>(0,0)=1;
    }else{
        integrogram.at<short>(0,0)=0;
    }

    for(int i=1;i<row;i++){
        if(src.at<uchar>(i,0)<100){
            integrogram.at<short>(i,0)=integrogram.at<short>(i-1,0)+1;
        }else{
            integrogram.at<short>(i,0)=integrogram.at<short>(i-1,0);
        }
    }

    for(int i=1;i<col;i++){
        if(src.at<uchar>(0,i)<100){
            integrogram.at<short>(0,i)=integrogram.at<short>(0,i-1)+1;
        }else{
            integrogram.at<short>(0,i)=integrogram.at<short>(0,i-1);
        }
    }    
    for(int i=1;i<row;i++){
        for(int j=1;j<col;j++){
            if(src.at<uchar>(i,j)<100){
                integrogram.at<short>(i,j)=integrogram.at<short>(i-1,j)+
                                            integrogram.at<short>(i,j-1)-
                                            integrogram.at<short>(i-1,j-1)+1;
                                        
            }else{
                integrogram.at<short>(i,j)=integrogram.at<short>(i-1,j)+
                                            integrogram.at<short>(i,j-1)-
                                            integrogram.at<short>(i-1,j-1);
            }
        }
    }

    // csv输出
    // ofstream file("/home/sgdd/Internship/Data/Mat.csv");
    // file << format(integrogram, Formatter::FMT_CSV);
    // file.close();

    //计算行投影和列投影
    Mat row_pro,col_pro;
    row_pro=Mat::zeros(row,1,CV_16S);
    col_pro=Mat::zeros(col,1,CV_16S);
    row_pro.at<short>(0,0)=integrogram.at<short>(0,col-1);
    for(int i=1;i<row;i++){
        row_pro.at<short>(i,0)=integrogram.at<short>(i,col-1)-integrogram.at<short>(i-1,col-1);
    }
    col_pro.at<short>(0,0)=integrogram.at<short>(row-1,0);
    for(int j=1;j<col;j++){
        col_pro.at<short>(j,0)=integrogram.at<short>(row-1,j)-integrogram.at<short>(row-1,j-1);
    }
    // csv输出
    ofstream file("/home/sgdd/Internship/Data/Mat1.csv");
    file << format(row_pro, Formatter::FMT_CSV);
    file.close();

    file.open("/home/sgdd/Internship/Data/Mat2.csv");
    file << format(col_pro, Formatter::FMT_CSV);
    file.close();   

    imshow("img",src);
    waitKey(0);

    FileStorage fs(format("%s/argument.xml",DataPath), FileStorage::READ);
    int arg4,arg3;
    fs["arg3"] >> arg3;   
    fs["arg4"] >> arg4;
    fs.release();

    line(src,Point(0,arg3),Point(col-1,arg3),Scalar(100)); 
    line(src,Point(0,arg4),Point(col-1,arg4),Scalar(100)); 

    

}