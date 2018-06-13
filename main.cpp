#include"stdafx.h"
#include"precompose.h"

void test_multiply();
void test_signal();
void outcome();
void showOutcome();

int main(int argc,char** argv){
    // outcome();
    test_signal();

    // showOutcome();


    // Mat img;
    // img=imread("/home/sgdd/Internship/Data/165200_6.BMP",IMREAD_COLOR);
    // Hist_gray(img);
    


}

//使用两种二值化方法测试
void test_multiply(){
    string filePath=DataPath;
    char tmp[100];
    Mat img,img_binary1,img_binary2,img1,img2,img_binary;
    Point upLine1,upLine2,upLine;
    Point downLine1,downLine2,downLine;
    for(int i=0;i<20;i++){
        sprintf(tmp,"%s/train/A%d.png",filePath.c_str(),i);
        img=imread(tmp,IMREAD_COLOR);
        local_binary(img,img_binary1);
        binary(img,img_binary2);
        getHorizontalBoundary(img_binary1,upLine1,downLine1);
        getHorizontalBoundary(img_binary2,upLine2,downLine2);
        int width1=(downLine1.x-upLine1.x)+(downLine1.y-upLine1.y);
        int width2=(downLine2.x-upLine2.x)+(downLine2.y-upLine2.y);
        if(width1>width2){
            upLine=upLine2;
            downLine=downLine2;
            img_binary2.copyTo(img_binary);
        }else{
            upLine=upLine1;
            downLine=downLine1;
            img_binary1.copyTo(img_binary);
        }


        FindWord(img_binary,upLine,downLine,img1);
        imshow("text",img1);
        imshow("local_binary",img_binary1);
        imshow("binary",img_binary2);
        waitKey(0);
    }        
}

//使用一种方法测试
void test_signal(){

    Mat img,img_binary,img2;
    int up,down;
    float spinAngle;
    string str1,str2,str3;
    char q;
    vector<Mat> imgs;
    vector<char> words;


    str1=format("%s/file.txt",DataPath);
    ifstream file(str1,ios_base::in);


    while(getline(file,str2)){
        imgs.clear();
        imgs.reserve(11);
        printf(str2.c_str());
        img=imread(str2);
        local_binary(img,img_binary);

        // imshow("img",img);
        // imshow("local_binary",img_binary);


        getHorizontalBoundary(img_binary,up,down,spinAngle);
        FindWord(img_binary,up,down,spinAngle,img2);
        // imshow("img2",img2);
        // waitKey(0);        
        split(img2,imgs);



        

    }
}

//提取所有图片的文字区域所用图片
void outcome(){
    Mat img,img_binary1,img_binary2,img1,img2,img_binary;
    Point upLine1,upLine2,upLine;
    Point downLine1,downLine2,downLine;
    ifstream file("/home/sgdd/AI_competition/competition/train.txt",ios_base::in);
    string tmp,filename;
    ofstream ofile;

    while(getline(file,tmp)){
        printf(tmp.c_str());
        printf("\n");
        img=imread(tmp,IMREAD_COLOR);
        local_binary(img,img_binary1);
        binary(img,img_binary2);
        getHorizontalBoundary(img_binary1,upLine1,downLine1);
        getHorizontalBoundary(img_binary2,upLine2,downLine2);
        int width1=(downLine1.x-upLine1.x)+(downLine1.y-upLine1.y);
        int width2=(downLine2.x-upLine2.x)+(downLine2.y-upLine2.y);
        if(width1>width2){
            upLine=upLine2;
            downLine=downLine2;
            img_binary2.copyTo(img_binary);
        }else{
            upLine=upLine1;
            downLine=downLine1;
            img_binary1.copyTo(img_binary);
        }
        FindWord(img_binary,upLine,downLine,img1);   
        int pos=tmp.rfind('/')+1;
        filename=format("/home/sgdd/AI_competition/Data/train/%s",tmp.substr(pos,tmp.size()-pos).c_str());
        imwrite(filename,img1);
        ofile.open("/home/sgdd/AI_competition/Data/train.txt",ios_base::app);
        ofile<<filename<<endl;
        ofile.close();


    }
}

//显示图片
void showOutcome(){
    Mat img1,img2;
    string str1,str2,str3;
    str1=format("%s/file.txt",DataPath);
    ifstream file(str1,ios_base::in);
    char q;

    while(getline(file,str2)){

        printf(str2.c_str());
        img1=imread(str2);
        imshow("img1",img1);
        q=waitKey(0);
        if(q=='q')
            break;

    }
    
}

