#include"RoubleIndex.h"

//使用局部二值化的方法，但仍存在很多噪点
void Rouble::binary(const Mat& src,Mat& retVal){
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

//根据计算的到的上下边界线，修正冠字号的角度
void Rouble::FindWord(const Mat& src,const int &up,const int &down,const float &spinAngle,Mat &retval){
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

//首先对文本可能旋转角度采样，例如文本的角度在【-10,10】度之间，
//我们在这个范围内均匀采10个点，我们就有10个旋转角。然后对每一个旋转角，
//对每一个图像左侧起点做一条直线，计算这个直线上侧黑色点数。那么只有旋转角与
//文本的旋转角度最接近时，上下两条直线之间的距离最短
void Rouble::getHorizontalBoundary(const Mat& src,int &up,int &down,float &spinAngle){
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

//切割文本
void Rouble::split(const Mat& src,vector<Mat> &retVal){
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
void Rouble::ImgToSquare(vector<Mat> &wordImg){
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
void Rouble::cutEdge(vector<Mat> &wordImg){
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
void Rouble::train1(char &aimChar){
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
        Rouble::densityFeature(img,feature);
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

void Rouble::densityFeature(const Mat& img,float* feature){
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
void Rouble::test1(const vector<Mat> &imgs,char* outcome){
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