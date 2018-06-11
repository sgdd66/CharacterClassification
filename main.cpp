#include"stdafx.h"
#include"precompose.h"

void test_multiply();
void test_signal();
void outcome();
void showOutcome();

int main(int argc,char** argv){
    // outcome();
    // test_signal();
    showOutcome();
}
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
void test_signal(){
    string filePath=DataPath;
    char tmp[100];
    Mat img,img_binary1,img_binary2,img1,img2,img_binary;
    Point upLine1,upLine2,upLine;
    Point downLine1,downLine2,downLine;

    sprintf(tmp,"%s/train/A%d.png",filePath.c_str(),8873);//2121 3425
    img=imread(tmp,IMREAD_COLOR);
    local_binary(img,img_binary1);
    binary(img,img_binary2);
    imshow("img",img);
    imshow("local_binary",img_binary1);
    imshow("binary",img_binary2);  
    waitKey(0);

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
 

    imshow("img",img);
    imshow("text",img1);
    waitKey(0);    
}

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

void showOutcome(){
    Mat img1,img2;
    string file1,file2;
    char q;
    for(int i=0;i<200;i++){
        file1=format("/home/sgdd/AI_competition/Data/train/A%d.png",i);
        file2=format("/home/sgdd/AI_competition/competition/train/A%d.png",i);
        printf(file2.c_str());
        img1=imread(file1);
        img2=imread(file2);
        imshow("img1",img1);
        imshow("img2",img2);
        q=waitKey(0);
        if(q=='q')
            break;
        if(q=='w')
            q='w';
    }
    
}

