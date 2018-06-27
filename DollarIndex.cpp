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
//kind=0表示字母A-Z kind=1表示数字0-9 kind=2表示星形符号
void Dollar::split(Mat &src,vector<Mat> &retVal,vector<int> &kind){
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

    //计算行投影
    Mat row_pro,col_pro;
    row_pro=Mat::zeros(row,1,CV_16S);
    col_pro=Mat::zeros(col,1,CV_16S);
    row_pro.at<short>(0,0)=integrogram.at<short>(0,col-1);
    for(int i=1;i<row;i++){
        row_pro.at<short>(i,0)=integrogram.at<short>(i,col-1)-integrogram.at<short>(i-1,col-1);
    }

    // csv输出
    // ofstream file("/home/sgdd/Internship/Data/Mat.csv");
    // file << format(row_pro, Formatter::FMT_CSV);
    // file.close();
    
    //统计上升和下降段
    vector<Vec3s> up;
    vector<Vec3s> down;
    Vec3s region;
    int dir=0;
    for(int i=1;i<row;i++){
        if( ( (row_pro.at<short>(i,0)-row_pro.at<short>(i-1,0))*dir )<=0){
            if(dir>0){
                region[1]=i-1;
                region[2]=row_pro.at<short>(region[1],0)-row_pro.at<short>(region[0],0);
                up.push_back(region);
                if(row_pro.at<short>(i,0)==row_pro.at<short>(i-1,0)){
                    dir=0;
                }else{
                    region[0]=i-1;
                    dir=-1;                    
                }
            }else if(dir<0){
                region[1]=i-1;
                region[2]=row_pro.at<short>(region[0],0)-row_pro.at<short>(region[1],0);
                down.push_back(region);
                if(row_pro.at<short>(i,0)==row_pro.at<short>(i-1,0)){
                    dir=0;
                }else{
                    region[0]=i-1;
                    dir=1;                    
                }                     
            }else if(dir==0){
                if(row_pro.at<short>(i,0)>row_pro.at<short>(i-1,0)){
                    region[0]=i-1;
                    dir=1;
                }else if(row_pro.at<short>(i,0)<row_pro.at<short>(i-1,0)){
                    region[0]=i-1;
                    dir=-1;
                }
            }
        }
    }
    std::sort(up.begin(),up.end(),[](Vec3s &a,Vec3s &b){return a[2]>b[2];});
    std::sort(down.begin(),down.end(),[](Vec3s &a,Vec3s &b){return a[2]>b[2];});

    int l1=0,l2=0;
    int index=0;
    while(abs( l2-l1-20 )>5){
        l1=(up[index][1]+up[index][0])/2;
        l2=(down[index][1]+down[index][0])/2;      
        index++;  
    }

    // line(src,Point(0,l1),Point(col-1,l1),Scalar(100)); 
    // line(src,Point(0,l2),Point(col-1,l2),Scalar(100)); 

    //列投影
    col_pro.at<short>(0,0)=integrogram.at<short>(l2,0)-integrogram.at<short>(l1-1,0);
    for(int j=1;j<col;j++){
        col_pro.at<short>(j,0)=integrogram.at<short>(l2,j)-
                                integrogram.at<short>(l1-1,j)-
                                integrogram.at<short>(l2,j-1)+
                                integrogram.at<short>(l1-1,j-1);
    }   

    //csv输出
    // ofstream file("/home/sgdd/Internship/Data/Mat.csv");
    // file << format(col_pro, Formatter::FMT_CSV);
    // file.close();  

    //筛选文本区域,筛选条件：
    //1.字符宽度在10+-2之间
    //2.字符之间间距<7
    //3.字符域内最大高度>5
    vector<Vec3s> regions;
    region[0]=0;
    int max=0;
    for(int i=1;i<col-1;i++){
        if(col_pro.at<short>(i,0)==0){
            if(col_pro.at<short>(i-1,0)!=0){
                region[1]=i;
                region[2]=max;
                if(abs(region[1]-region[0]-10)<3 && max>5){
                    regions.push_back(region);
                }
                max=0;                
            }
            if(col_pro.at<short>(i+1,0)!=0){
                region[0]=i;
            }
        }else{
            if(col_pro.at<short>(i,0)>max){
                max=col_pro.at<short>(i,0);
            }
        }
    }

    int wordNum=0;
    index=0;
    for(int i=0;i<regions.size()-1;i++){
        if(regions[i+1][0]-regions[i][1]>7){
            if(wordNum>8){
                break;
            }else{
                index=i+1;
            }
        }else{
            wordNum+=1;
        }
    }

    //字符拓展，虽然找到了字符区域，但是并不完整。需要向外拓展，如果有必要要向内收缩。
    //方法：检测每一行黑色点的数目，如果大于5个，添加这一行。如果这一行全部为空白，减少这一行。
    //同时发现行数过少，说明这是一个星形符号，删除这个符号。之后通过识别出来的数字位数来判定是不是要添加星形符号。
    
    Rect rect;
    Mat roi;
    int num;
    for(int i=0;i<=wordNum;i++){
        rect=Rect(regions[index+i][0]-1,
                    l1-1,
                    regions[index+i][1]-regions[index+i][0]+2,
                    l2-l1+2);
        //上
        dir=0;//0方向不定，1拓展，-1收缩
        while(1){
            num=0;
            for(int j=0;j<rect.width;j++){
                if(src.at<uchar>(rect.y,rect.x+j)<100){
                    num+=1;
                }
            }
            if(dir==0){
                if(num==0 ){
                    rect.y+=1;
                    rect.height-=1;
                    dir=-1;
                }else if(dir>3){
                    rect.y-=1;
                    rect.height+=1;
                    dir=1;
                }else{
                    break;
                }       
            }else if(dir==-1){
                if(num==0){
                    rect.y+=1;
                    rect.height-=1;              
                }else{
                    break;
                }
            }else if(dir==1){
                if(num>3){
                    rect.y-=1;
                    rect.height+=1;                  
                }else{
                    rect.y+=1;
                    rect.height-=1;         
                    break;                
                }
            }

        }

        //下
        dir=0;//0方向不定，1拓展，-1收缩
        while(1){
            num=0;
            for(int j=0;j<rect.width;j++){
                if(src.at<uchar>(rect.y+rect.height,rect.x+j)<100){
                    num+=1;
                }
            }
            if(dir==0){
                if(num==0 ){
                    rect.height-=1;
                    dir=-1;
                }else if(num>3){
                    rect.height+=1;
                    dir=1;
                }else{
                    break;
                }               
            }else if(dir==-1){
                if(num==0){
                    rect.height-=1;              
                }else{
                    break;
                }
            }else if(dir==1){
                if(num>3){
                    rect.height+=1;                  
                }else{
                    rect.height-=1;         
                    break;                
                }
            }

        }

        //左
        dir=0;//0方向不定，1拓展，-1收缩
        while(1){
            num=0;
            for(int j=0;j<rect.height;j++){
                if(src.at<uchar>(rect.y+j,rect.x)<100){
                    num+=1;
                }
            }
            if(dir==0){
                if(num==0 ){
                    rect.x+=1;
                    rect.width-=1;
                    dir=-1;
                }else if(num>3){
                    rect.x-=1;
                    rect.width+=1;
                    dir=1;
                }else{
                    break;
                }               
            }else if(dir==-1){
                if(num==0){
                    rect.x+=1;
                    rect.width-=1;              
                }else{
                    break;
                }
            }else if(dir==1){
                if(num>3){
                    rect.x-=1;
                    rect.width+=1;                  
                }else{
                    rect.x+=1;
                    rect.width-=1;         
                    break;                
                }
            }

        }

        //右
        dir=0;//0方向不定，1拓展，-1收缩
        while(1){
            num=0;
            for(int j=0;j<rect.height;j++){
                if(src.at<uchar>(rect.y+j,rect.x+rect.width)<100){
                    num+=1;
                }
            }
            if(dir==0){
                if(num==0 ){
                    rect.width-=1;
                    dir=-1;
                }else if(num>3){
                    rect.width+=1;
                    dir=1;
                }else{
                    break;
                }               
            }else if(dir==-1){
                if(num==0){
                    rect.width-=1;              
                }else{
                    break;
                }
            }else if(dir==1){
                if(num>3){
                    rect.width+=1;                  
                }else{
                    rect.width-=1;         
                    break;                
                }
            }

        }
        rect.width+=1;
        rect.height+=1;
        // rectangle(src,rect,Scalar(100));
        if(rect.height<15){
            continue;
        }
        roi=src(rect);
        retVal.push_back(roi);
        // imshow("src",src);
        // waitKey(0);
    }

    //数字和字母一起识别可行性不高，建议将数字和字母分开。
    //0表示字母，1表示数字，2表示特殊符号
    if(regions[1][0]-regions[0][1]>regions[2][0]-regions[1][1]){
        kind.push_back(0);
    }else{
        kind.push_back(0);
        kind.push_back(0);
    }
    for(int i=0;i<8;i++){
        kind.push_back(1);
    }
    if(kind.size()<retVal.size()){
        kind.push_back(0);
    }else{
        kind.push_back(2);
    }

}

//训练方法，使用密度图，三条横向分割线，两条条纵向分割线，产生17维的特征向量
void Dollar::train(char &aimChar){
    string filePath=format("%s/train2/%c/file.txt",DataPath,aimChar);
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
        feature=new float[17];
        img=imread(photoPath,IMREAD_GRAYSCALE);
        Dollar::densityFeature(img,feature);
        features.push_back(feature);
    }
    file.close();

    int sum=features.size();
    Mat outcome=Mat::zeros(17,sum,CV_32F);
    for(int i=0;i<sum;i++){
        for(int j=0;j<17;j++){
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

//美元的冠字号相比卢布而言，数字和字母更加细长，噪声也使得部分字母的残缺或者笔画太细。如果划分过细，不利于捕捉特征
void Dollar::densityFeature(const Mat& img,float* feature){
    int row=img.rows;
    int col=img.cols;
    int colIndex[2];
    int rowIndex[3];
    int colStep=col/3;
    int rowStep=row/4;

    //确定列分割位点
    int remainder=col%3;
    switch(remainder){
        case 0:
            colIndex[0]=colStep-1;
            colIndex[1]=colIndex[0]+colStep;
            break;
        case 1:
            colIndex[0]=colStep-1;
            colIndex[1]=colIndex[0]+colStep;  
            break;    
        case 2:
            colIndex[0]=colStep;
            colIndex[1]=colIndex[0]+colStep;  
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

    //计算12个区域的密度
    float density;
    int cols[4];
    cols[0]=-1;
    cols[1]=colIndex[0];
    cols[2]=colIndex[1];
    cols[3]=col-1;

    int rows[5];
    rows[0]=-1;
    rows[1]=rowIndex[0];
    rows[2]=rowIndex[1];
    rows[3]=rowIndex[2];
    rows[4]=row-1;    
    
    for(int i=0;i<4;i++){
        for(int j=0;j<3;j++){
            density=0;
            for(int m=rows[i]+1;m<=rows[i+1];m++){
                for(int n=cols[j]+1;n<=cols[j+1];n++){
                    if(img.at<uchar>(m,n)<100){
                        density+=1;
                    }
                }
            }
            density=density/( (rows[i+1]-rows[i])*(cols[j+1]-cols[j]) );
            feature[i*3+j]=density;
            
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
        feature[12+i]=density;
    }

    for(int i=0;i<2;i++){
        density=0;
        for(int j=0;j<row;j++){
            if(img.at<uchar>(j,colIndex[i])<100){
                density+=1;
            }
        }
        density=density/row;
        feature[15+i]=density;
    }

}


void Dollar::test(const vector<Mat> &imgs,const vector<int> kind,char* outcome){
    //读取每个标签的特征
    string filePath=format("%s/DensityFeature/Dollar_number.txt",DataPath);
    ifstream file(filePath,ios_base::in);
    if(!file.is_open()){
        printf("can't open file:%s\n",filePath.c_str());
        return;
    }

    Mat aim_num=Mat::zeros(DimNum,NumNum,CV_32F);
    Mat aim_char=Mat::zeros(DimNum,CharNum,CV_32F);
    char labels_num[NumNum+1]="0123456789";
    char labels_char[CharNum+1]="ALJKMGHEFCIBDW";

    string line;
    char* nums;
    char text[500];
    char splitChar[]=" ";
    int index=0;
    while(getline(file,line)){
        strcpy(text,line.c_str());       
        nums=strtok(text,splitChar);
        for(int i=0;i<NumNum;i++){
            aim_num.at<float>(index,i)=atof(nums);
            nums = strtok(NULL, splitChar);
        }
        index++;
    }
    file.close();

    filePath=format("%s/DensityFeature/Dollar_character.txt",DataPath);
    file.open(filePath,ios_base::in);

    if(!file.is_open()){
        printf("can't open file:%s\n",filePath.c_str());
        return;
    }   

    index=0;
    while(getline(file,line)){
        strcpy(text,line.c_str());       
        nums=strtok(text,splitChar);
        for(int i=0;i<CharNum;i++){
            aim_char.at<float>(index,i)=atof(nums);
            nums = strtok(NULL, splitChar);
        }
        index++;
    }
    file.close();    

    vector<float> distance;
    float feature[DimNum];
    for(int i=0;i<imgs.size();i++){
        if(kind[i]==0){     //字母
            distance.clear();
            distance.reserve(CharNum);
            densityFeature(imgs[i],feature);     
            for(int n=0;n<CharNum;n++){
                float tmp=0;
                for(int d=0;d<DimNum;d++){
                    tmp+=pow(feature[d]-aim_char.at<float>(d,n),2);
                }
                distance.push_back(sqrt(tmp));          
            }
            vector<float>::iterator min=std::min_element(distance.begin(),distance.end());  
            int pos=std::distance(distance.begin(),min);      
            outcome[i]=labels_char[pos];
        }else if(kind[i]==1){   //数字
            distance.clear();
            distance.reserve(NumNum);
            densityFeature(imgs[i],feature);     
            for(int n=0;n<NumNum;n++){
                float tmp=0;
                for(int d=0;d<DimNum;d++){
                    tmp+=pow(feature[d]-aim_num.at<float>(d,n),2);
                }
                distance.push_back(sqrt(tmp));          
            }
            vector<float>::iterator min=std::min_element(distance.begin(),distance.end());  
            int pos=std::distance(distance.begin(),min);      
            outcome[i]=labels_num[pos];
        }

    }
    if(imgs.size()<kind.size()){
        outcome[imgs.size()]='*';
        outcome[imgs.size()+1]='\0';
    }else{
        outcome[imgs.size()]='\0';
    }

}

