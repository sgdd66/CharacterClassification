#include"stdafx.h"

#include"precompose.h"
#include"RoubleIndex.h"
#include"DollarIndex.h"


void test();
void outcome();
void RoubleClassify();

int main(int argc,char** argv){
    // outcome();
    // test();
    // RoubleClassify();

    //输出训练样本的特征向量
    char aimChar[25]="0123456789ALJKMGHEFCIBDW";
    for(int i=0;i<25;i++){
        Dollar::train(aimChar[i]);
    }

}

//测试
void test(){


    Mat img,img_binary,img2;
    int up,down;
    float spinAngle;
    string str1,str2,str3;
    char q;
    vector<Mat> imgs;
    vector<char> words;

    str1=format("%s/raw2/file.txt",DataPath);
    ifstream file(str1,ios_base::in);
    int index=0;
    vector<int> kind;

    while(getline(file,str2)){
        imgs.clear();
        imgs.reserve(11);
        kind.clear();
        kind.reserve(11);

        // str2="/home/sgdd/Internship/Data/raw2/143249_6.BMP";
        img=imread(str2);
        imshow("img1",img);

        Dollar::binary(img,img_binary);
        // imshow("img2",img_binary);

        Dollar::edgeFilter(img_binary);
        // imshow("img3",img_binary);
        // waitKey(0);

        Dollar::split(img_binary,imgs,kind);

        //存储样本
        // for(int i=0;i<imgs.size();i++){
        //     str3=format("%s/train3/%d.png",DataPath,index++);
        //     imwrite(str3,imgs[i]);
        // }
        imshow("img4",img_binary);   
        for(int i=0;i<kind.size();i++){
            printf("%d",kind[i]);
        }     
        printf("\n");
        waitKey(0);         
    
    }


}

//卢布测试成功版本
void RoubleClassify(){
    Mat img,img_binary,img2;
    int up,down;
    float spinAngle;
    string str1,str2,str3;
    char q;
    vector<Mat> imgs;
    vector<char> words;


    str1=format("%s/raw1/file.txt",DataPath);
    ifstream file(str1,ios_base::in);
    int index=0;

    while(getline(file,str2)){
        imgs.clear();
        imgs.reserve(11);

        img=imread(str2);
        Rouble::binary(img,img_binary);

        imshow("img",img);
        // imshow("local_binary",img_binary);
        // waitKey(0);       

        Rouble::getHorizontalBoundary(img_binary,up,down,spinAngle);
        Rouble::FindWord(img_binary,up,down,spinAngle,img2);
        // imshow("img2",img2);
    
        Rouble::split(img2,imgs);
        // Rouble::ImgToSquare(imgs); //将图片扩展为标准大小
        Rouble::cutEdge(imgs); //切割边缘

        //存储样本
        // for(int i=0;i<imgs.size();i++){
        //     str3=format("%s/label/%d.png",DataPath,index++);
        //     imwrite(str3,imgs[i]);
        // }

        //输出训练样本的特征向量
        // char aimChar[12]="0123456789A";
        // for(int i=0;i<11;i++){
        //     Rouble::train1(aimChar[i]);
        // }
        char outcome[100];
        Rouble::test1(imgs,outcome);
        printf("%s\n",outcome);  
        waitKey(0);            
    
    }


}

//提取所有图片的文字区域
void outcome(){
    Mat img,img_binary1,img_binary2,img1,img2,img_binary;
    Point upLine1,upLine2,upLine;
    Point downLine1,downLine2,downLine;
    ifstream file("/home/sgdd/AI_competition/competition/train.txt",ios_base::in);
    string tmp,filename;
    ofstream ofile;

    while(getline(file,tmp)){
        printf("%s",tmp.c_str());
        printf("\n");
        img=imread(tmp,IMREAD_COLOR);
        AI::local_binary(img,img_binary1);
        AI::binary(img,img_binary2);
        AI::getHorizontalBoundary(img_binary1,upLine1,downLine1);
        AI::getHorizontalBoundary(img_binary2,upLine2,downLine2);
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
        AI::FindWord(img_binary,upLine,downLine,img1);   
        int pos=tmp.rfind('/')+1;
        filename=format("/home/sgdd/AI_competition/Data/train/%s",tmp.substr(pos,tmp.size()-pos).c_str());
        imwrite(filename,img1);
        ofile.open("/home/sgdd/AI_competition/Data/train.txt",ios_base::app);
        ofile<<filename<<endl;
        ofile.close();


    }
}

