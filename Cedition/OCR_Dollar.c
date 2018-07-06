/***************************************************************************
* Copyright (c) 2018 苏州日宝科技有限责任公司
* All rights reserved
* 
* 文件名称：OCR_Dollar.c
* 
* 摘    要：美元货币的冠字号识别程序
* 
* 创 建 者：上官栋栋
* 
* 创建日期：2018年6月28日
*
* 修改记录
日期  修改者   		版本     修改内容
-------------- 		-------  ------------------------  

****************************************************************************/
#include "OCR_Dollar.h"
#include "Feature.h"
#include "BMP.h"
#include "DataStructure.h"
#include <math.h>
extern const float features_char[DimNumA][CharNumA];
extern const float features_num[DimNumA][NumNum];
extern const char labels_num[NumNum+1];
extern const char labels_char[CharNumA+1];

//arg1是二值化使用的参数，值越大白色区域越大。
const float arg1=1.4;


//使用局部二值化的方法
void binary(IMG *img,U32 **integrogram){
    //计算积分图

	int x1,y1,x2,y2;
    float threshold;
    int width;
    int i,j;
    // U32** integrogram;
    // integrogram=(U32**)malloc(sizeof(U32*)*img->row);
    // for(i=0;i<img->row;i++){
    //     integrogram[i]=(U32*)malloc(sizeof(U32)*img->col);
    // }

    integrogram[0][0]=img->pImg[0][0];
    for(i=1;i<img->col;i++){
        integrogram[0][i]=img->pImg[0][i]+integrogram[0][i-1];
    }
    for(j=1;j<img->row;j++){
        integrogram[j][0]=img->pImg[j][0]+integrogram[j-1][0];
    }
    for(i=1;i<img->row;i++){
        for(j=1;j<img->col;j++){
            integrogram[i][j]=img->pImg[i][j]+
                                integrogram[i-1][j]+
                                integrogram[i][j-1]-
                                integrogram[i-1][j-1];
        }
    }

    //计算局部均值

    width=25;    //方形区域边长的一半

    for(i=0;i<img->row;i++){
        for(j=0;j<img->col;j++){
            x1=j<width?0:j-width;
            y1=i<width?0:i-width;
            x2=j+width>img->col-1?img->col-1:j+width;
            y2=i+width>img->row-1?img->row-1:i+width;
            threshold=(integrogram[y2][x2]-
                            integrogram[y1][x2]-
                            integrogram[y2][x1]+
                            integrogram[y1][x1]);
            threshold/=( (y2-y1)*(x2-x1) );
                            
            if(img->pImg[i][j]>threshold/arg1){
                img->pImg[i][j]=255;
            }else{
                img->pImg[i][j]=0;
            }
        }
    }
    
    // for(i=0;i<img->row;i++){
    //     free(integrogram[i]);
    // }
    // free(integrogram);
}

//将所有与边缘相贴的图案筛选掉
void edgeFilter(IMG *src){
    Chain blackRegions;
	int i,j,m;

    Node *region=NULL;
    short begin,end;
    int pos,num; 
    blackRegions.size=0;
       

    //从下边缘开始扫描
    begin=0;
    end=src->col-1;
    addRegion(src,src->row-1,begin,end,&blackRegions);
    for(i=src->row-2;i>-1;i--){
        pos=0;
        num=blackRegions.size;
        if(num==0){
            break;
        }
        for(j=0;j<num;j++){
            free(region);
            region=pop_front(&blackRegions);
            if(region->data[1]<pos){
                continue;
            }else if(region->data[0]>=pos){
                begin=0;
                end=src->col-1;
                for(m=region->data[0];m>=0;m--){             
                    if(src->pImg[i][m]>100){
                        begin=m;
                        break;
                    }
                }
                for(m=region->data[1];m<src->col;m++){
                    if(src->pImg[i][m]>100){
                        end=m;
                        break;
                    }                    
                }
                pos=end;
                addRegion(src,i,begin,end,&blackRegions);
            }else{
                begin=pos+1;
                for(m=region->data[1];m<src->col;m++){
                    if(src->pImg[i][m]>100){
                        end=m;
                        break;
                    }                    
                }
                pos=end;
                addRegion(src,i,begin,end,&blackRegions);                
            }

        }
    }
    //从上边缘开始扫描
    while(blackRegions.size!=0){
        region=pop_front(&blackRegions);
        free(region);
    }
    region=NULL;
    begin=0;
    end=src->col-1;
    addRegion(src,0,begin,end,&blackRegions);
    for(i=1;i<src->row;i++){
        pos=0;
        num=blackRegions.size;
        if(num==0){
            break;
        }
        for(j=0;j<num;j++){
            free(region);
            region=pop_front(&blackRegions);
            if(region->data[1]<pos){
                continue;
            }else if(region->data[0]>=pos){
                begin=0;
                end=src->col-1;
                for(m=region->data[0];m>=0;m--){
                    if(src->pImg[i][m]>100){
                        begin=m;
                        break;
                    }
                }
                for(m=region->data[1];m<src->col;m++){
                    if(src->pImg[i][m]>100){
                        end=m;
                        break;
                    }                    
                }
                pos=end;
                addRegion(src,i,begin,end,&blackRegions);
            }else{
                begin=pos+1;
                for(m=region->data[1];m<src->col;m++){
                    if(src->pImg[i][m]>100){
                        end=m;
                        break;
                    }                    
                }
                pos=end;
                addRegion(src,i,begin,end,&blackRegions);                
            }

        }
    }  
    while(blackRegions.size!=0){
        region=pop_front(&blackRegions);
        free(region);
    } 
}

//将begin和end中间的黑色区域找出并存放在regions中，同时将该区域置白
void addRegion(IMG *src,int row,int begin,int end,Chain *regions){
    Node *region;
	int i;
    region=(Node*)malloc(sizeof(Node));
    

    if(src->pImg[row][begin]>100){
        region->data[0]=-1;
        region->data[1]=-1;
    }else{
        region->data[0]=begin;
        region->data[1]=-1;        
    }

    for(i=begin+1;i<end+1;i++){
        if(src->pImg[row][i]>100 && src->pImg[row][i-1]<100){
            region->data[1]=i;
            push(regions,region);
        }else if(src->pImg[row][i]<100 && src->pImg[row][i-1]>100){
            region=(Node*)malloc(sizeof(Node));
            region->data[0]=i;
            region->data[1]=-1;
        }
    }
    if(region->data[1]<region->data[0] ){
        region->data[1]=end;
        push(regions,region);
    }

    for(i=begin;i<end+1;i++){
        src->pImg[row][i]=255;
    }
}

//考虑到横向和纵向切割都需要积分图，所以把两个功能放在一个函数中
//kind=0表示字母A-Z kind=1表示数字0-9 kind=2表示星形符号
void split(const IMG *src,Chain* retVal,int *kind, U32** integrogram){

    int i,j;
    int *row_pro,*col_pro;
    int dir=0;
    int l1=0;
    int l2=0;
    int index=0;
    int wordNum=0;        
    int num;
    int x=0;
	int max=0;
    Node* rect;//data的0,1,2,3表示x,y,width,height
    Node* region;    
    Chain up;
    Chain down;
    Chain regions;  
    int x1,x2,y1,y2;    
    regions.size=0;     
    up.size=0;
    down.size=0;
    retVal->size=0;



    // U32** integrogram;
    // integrogram=(U32**)malloc(sizeof(U32*)*src->row);
    // for(i=0;i<src->row;i++){
    //     integrogram[i]=(U32*)malloc(sizeof(U32)*src->col);
    // }


    if(src->pImg[0][0]<100){
        integrogram[0][0]=1;
    }else{
        integrogram[0][0]=0;
    }

    for(i=1;i<src->col;i++){
        if(src->pImg[0][i-1]<100){
            integrogram[0][i]=integrogram[0][i-1]+1;
        }else{
            integrogram[0][i]=integrogram[0][i-1];
        }        
    }

    for(j=1;j<src->row;j++){
        if(src->pImg[j][0]<100){
            integrogram[j][0]=integrogram[j-1][0]+1;
        }else{
            integrogram[j][0]=integrogram[j-1][0];
        }        
    }        

    for(i=1;i<src->row;i++){
        for(j=1;j<src->col;j++){
            if(src->pImg[i][j]<100){
                integrogram[i][j]=integrogram[i-1][j]+
                                    integrogram[i][j-1]-
                                    integrogram[i-1][j-1]+1;                
            }else{
                integrogram[i][j]=integrogram[i-1][j]+
                                    integrogram[i][j-1]-
                                    integrogram[i-1][j-1];                  
            }

        }
    }

    //计算行投影,这里使用IMG只支持U8类型，当数据大于255时会发生错误。不过当前的图片一般在255之下，所以可以接受。

    row_pro=(int*)malloc(sizeof(int)*src->row);
    col_pro=(int*)malloc(sizeof(int)*src->col);
    memset(row_pro,0,sizeof(int)*src->row);
    memset(col_pro,0,sizeof(int)*src->col);

    row_pro[0]=integrogram[0][src->col-1];
    for(i=1;i<src->row;i++){
        row_pro[i]=integrogram[i][src->col-1]-integrogram[i-1][src->col-1];
    }


    
    //统计上升和下降段
    for(i=1;i<src->row;i++){
        if( ( (row_pro[i]-row_pro[i-1])*dir )<=0){
            if(dir>0){
                region->data[1]=i-1;
                region->data[2]=row_pro[region->data[1]]-row_pro[region->data[0]];
                push(&up,region);
                if(row_pro[i]==row_pro[i-1]){
                    dir=0;
                }else{
                    region=(Node*)malloc(sizeof(Node));
                    region->data[0]=i-1;
                    dir=-1;                    
                }
            }else if(dir<0){
                region->data[1]=i-1;
                region->data[2]=row_pro[region->data[0]]-row_pro[region->data[1]];
                push(&down,region);
                if(row_pro[i]==row_pro[i-1]){
                    dir=0;
                }else{
                    region=(Node*)malloc(sizeof(Node));
                    region->data[0]=i-1;
                    dir=1;                    
                }                     
            }else if(dir==0){
                if(row_pro[i]>row_pro[i-1]){
                    region=(Node*)malloc(sizeof(Node));
                    region->data[0]=i-1;
                    dir=1;
                }else if(row_pro[i]<row_pro[i-1]){
                    region=(Node*)malloc(sizeof(Node));
                    region->data[0]=i-1;
                    dir=-1;
                }
            }
        }
    }

    //冒泡排序
    sort(&up,2,false);
    sort(&down,2,false);


    while(abs( l2-l1-20 )>5){
        l1=(at(&up,index)->data[1]+at(&up,index)->data[0])/2;
        l2=(at(&down,index)->data[1]+at(&down,index)->data[0])/2;
        index++;  
    }

    //列投影
    col_pro[0]=integrogram[l2][0]-integrogram[l1-1][0];
    for(j=1;j<src->col;j++){
        col_pro[j]=integrogram[l2][j]-
                    integrogram[l1-1][j]-
                    integrogram[l2][j-1]+
                    integrogram[l1-1][j-1];
    }   

    //筛选文本区域,筛选条件：
    //1.字符宽度在10+-2之间
    //2.字符之间间距<7
    //3.字符域内最大高度>5

    region=(Node*)malloc(sizeof(Node));
    region->data[0]=0;
    
    for(i=1;i<src->col-1;i++){
        if(col_pro[i]==0){
            if(col_pro[i-1]!=0){
                region->data[1]=i;
                region->data[2]=max;
                if(abs(region->data[1]-region->data[0]-10)<3 && max>5){
                    push(&regions,region);
                }else{
                    free(region);
                }
                max=0;                
            }
            if(col_pro[i+1]!=0){
                region=(Node*)malloc(sizeof(Node));
                region->data[0]=i;
            }
        }else{
            if(col_pro[i]>max){
                max=col_pro[i];
            }
        }
    }

    index=0;
    for(i=0;i<regions.size-1;i++){
        if(at(&regions,i+1)->data[0]-at(&regions,i)->data[1]>7){
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

    for(i=0;i<=wordNum;i++){
        rect=(Node*)malloc(sizeof(Node));
        rect->data[0]=at(&regions,index+i)->data[0]-1;
        rect->data[1]=l1-1;
        rect->data[2]=at(&regions,index+i)->data[1]-at(&regions,index+i)->data[0]+2;
        rect->data[3]=l2-l1+2;
        //上
        dir=0;//0方向不定，1拓展，-1收缩
        while(1){
            x1=rect->data[0];
            y1=rect->data[1];
            x2=rect->data[0]+rect->data[2]-1;
            y2=rect->data[1]+rect->data[3]-1;
            num=integrogram[y1][x2]-integrogram[y1][x1-1]-integrogram[y1-1][x2]+integrogram[y1-1][x1-1];
            // num=0;
            // for(j=0;j<rect->data[2];j++){
            //     if(src->pImg[rect->data[1]][rect->data[0]+j]<100){
            //         num+=1;
            //     }
            // }
            if(dir==0){
                if(num==0 ){
                    rect->data[1]+=1;
                    rect->data[3]-=1;
                    dir=-1;
                }else if(dir>3){
                    rect->data[1]-=1;
                    rect->data[3]+=1;
                    dir=1;
                }else{
                    break;
                }       
            }else if(dir==-1){
                if(num==0){
                    rect->data[1]+=1;
                    rect->data[3]-=1;              
                }else{
                    break;
                }
            }else if(dir==1){
                if(num>3){
                    rect->data[1]-=1;
                    rect->data[3]+=1;                  
                }else{
                    rect->data[1]+=1;
                    rect->data[3]-=1;         
                    break;                
                }
            }

        }

        //下
        dir=0;//0方向不定，1拓展，-1收缩
        while(1){
            x1=rect->data[0];
            y1=rect->data[1];
            x2=rect->data[0]+rect->data[2]-1;
            y2=rect->data[1]+rect->data[3]-1;
            num=integrogram[y2+1][x2]-integrogram[y2+1][x1-1]-integrogram[y2][x2]+integrogram[y2][x1-1];
            // num=0;
            // for(j=0;j<rect->data[2];j++){
            //     if(src->pImg[rect->data[1]+rect->data[3]][rect->data[0]+j]<100){
            //         num+=1;
            //     }
            // }
            if(dir==0){
                if(num==0 ){
                    rect->data[3]-=1;
                    dir=-1;
                }else if(num>3){
                    rect->data[3]+=1;
                    dir=1;
                }else{
                    break;
                }               
            }else if(dir==-1){
                if(num==0){
                    rect->data[3]-=1;              
                }else{
                    break;
                }
            }else if(dir==1){
                if(num>3){
                    rect->data[3]+=1;                  
                }else{
                    rect->data[3]-=1;         
                    break;                
                }
            }

        }

        //左
        dir=0;//0方向不定，1拓展，-1收缩
        while(1){
            x1=rect->data[0];
            y1=rect->data[1];
            x2=rect->data[0]+rect->data[2]-1;
            y2=rect->data[1]+rect->data[3]-1;
            num=integrogram[y2][x1]-integrogram[y2][x1-1]-integrogram[y1-1][x1]+integrogram[y1-1][x1-1];
            // num=0;
            // for(j=0;j<rect->data[3];j++){
            //     if(src->pImg[rect->data[1]+j][rect->data[0]]<100){
            //         num+=1;
            //     }
            // }
            if(dir==0){
                if(num==0 ){
                    rect->data[0]+=1;
                    rect->data[2]-=1;
                    dir=-1;
                }else if(num>3){
                    rect->data[0]-=1;
                    rect->data[2]+=1;
                    dir=1;
                }else{
                    break;
                }               
            }else if(dir==-1){
                if(num==0){
                    rect->data[0]+=1;
                    rect->data[2]-=1;              
                }else{
                    break;
                }
            }else if(dir==1){
                if(num>3){
                    rect->data[0]-=1;
                    rect->data[2]+=1;                  
                }else{
                    rect->data[0]+=1;
                    rect->data[2]-=1;         
                    break;                
                }
            }

        }

        //右
        dir=0;//0方向不定，1拓展，-1收缩
        while(1){
            x1=rect->data[0];
            y1=rect->data[1];
            x2=rect->data[0]+rect->data[2]-1;
            y2=rect->data[1]+rect->data[3]-1;
            num=integrogram[y2][x2+1]-integrogram[y1-1][x2+1]-integrogram[y2][x2]+integrogram[y1-1][x2];
            // num=0;
            // for(j=0;j<rect->data[3];j++){
            //     if(src->pImg[rect->data[1]+j][rect->data[0]+rect->data[2]]<100){
            //         num+=1;
            //     }
            // }
            if(dir==0){
                if(num==0 ){
                    rect->data[2]-=1;
                    dir=-1;
                }else if(num>3){
                    rect->data[2]+=1;
                    dir=1;
                }else{
                    break;
                }               
            }else if(dir==-1){
                if(num==0){
                    rect->data[2]-=1;              
                }else{
                    break;
                }
            }else if(dir==1){
                if(num>3){
                    rect->data[2]+=1;                  
                }else{
                    rect->data[2]-=1;         
                    break;                
                }
            }

        }
        rect->data[2]+=1;
        rect->data[3]+=1;

        if(rect->data[3]<15){
            continue;
        }
        push(retVal,rect);

    }
    // writeChain("/home/sgdd/Internship/Data/mat.txt",retVal);

    //数字和字母一起识别可行性不高，建议将数字和字母分开。
    //0表示字母，1表示数字，2表示特殊符号

    if(at(&regions,index+1)->data[0]-at(&regions,index)->data[1]>
        at(&regions,index+2)->data[0]-at(&regions,index+1)->data[1]){
        kind[0]=0;
        x=1;
    }else{
        kind[0]=0;
        kind[1]=0;
        x=2;
    }
    for(i=0;i<8;i++){
        kind[x+i]=1;
    }
    if(kind[retVal->size-1]==-1){
        kind[retVal->size-1]=0;
    }else{
        kind[retVal->size-1]=2;
    }

    //回收内存
    while(regions.size!=0){
        region=pop_front(&regions);
        free(region);        
    }
    while(up.size!=0){
        region=pop_front(&up);
        free(region);
    }
    while(down.size!=0){
        region=pop_front(&down);
        free(region);
    }    
    free(row_pro);
    free(col_pro);
    // for(i=0;i<src->row;i++){
    //     free(integrogram[i]);
    // }
    // free(integrogram);

}

void densityFeature(const IMG* img, const Node* rect,float* feature,U32** integrogram){
    int row=rect->data[3];
    int col=rect->data[2];
    int colIndex[2];
    int rowIndex[3];
    int colStep=col/3;
    int rowStep=row/4;
    float density;
    int cols[4];
    int rows[5];
    int i,j,m,n;
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

    cols[0]=-1;
    cols[1]=colIndex[0];
    cols[2]=colIndex[1];
    cols[3]=col-1;

    rows[0]=-1;
    rows[1]=rowIndex[0];
    rows[2]=rowIndex[1];
    rows[3]=rowIndex[2];
    rows[4]=row-1;    
    int density1;
    for(i=0;i<4;i++){
        for(j=0;j<3;j++){
            // density=0;
            // for(m=rows[i]+1;m<=rows[i+1];m++){
            //     for(n=cols[j]+1;n<=cols[j+1];n++){
            //         if(img->pImg[rect->data[1]+m][rect->data[0]+n]<100){
            //             density+=1;
            //         }
            //     }
            // }
            
            density=integrogram[rect->data[1]+rows[i+1]][rect->data[0]+cols[j+1]]-
                    integrogram[rect->data[1]+rows[i]][rect->data[0]+cols[j+1]]-
                    integrogram[rect->data[1]+rows[i+1]][rect->data[0]+cols[j]]+
                    integrogram[rect->data[1]+rows[i]][rect->data[0]+cols[j]];


            density=density/( (rows[i+1]-rows[i])*(cols[j+1]-cols[j]) );
            feature[i*3+j]=density;
            
        }
    }

    //计算横纵各三条线的密度
    for(i=0;i<3;i++){
        // density=0;
        // for(j=0;j<col;j++){
        //     if(img->pImg[rect->data[1]+rowIndex[i]][rect->data[0]+j]<100){
        //         density+=1;
        //     }
        // }
        density=integrogram[rect->data[1]+rowIndex[i]][rect->data[0]+col-1]-
                integrogram[rect->data[1]+rowIndex[i]-1][rect->data[0]+col-1]-
                integrogram[rect->data[1]+rowIndex[i]][rect->data[0]-1]+
                integrogram[rect->data[1]+rowIndex[i]-1][rect->data[0]-1];    
        density=density/col;
        feature[12+i]=density;
    }

    for(i=0;i<2;i++){
        // density=0;
        // for(j=0;j<row;j++){
        //     if(img->pImg[rect->data[1]+j][rect->data[0]+colIndex[i]]<100){
        //         density+=1;
        //     }
        // }
        density=integrogram[rect->data[1]+row-1][rect->data[0]+colIndex[i]]-
                integrogram[rect->data[1]-1][rect->data[0]+colIndex[i]]-
                integrogram[rect->data[1]+row-1][rect->data[0]+colIndex[i]-1]+
                integrogram[rect->data[1]-1][rect->data[0]+colIndex[i]];         

        density=density/row;
        feature[15+i]=density;
    }    
}

void test(const IMG* img,const Chain* rects,const int* kind,char* outcome,U32** integrogram){
   //读取每个标签的特征

    float distance[30];
    float feature[DimNumA];
    float min;
    int pos;
    int i,n,d;
	float xTimer7 = 0.0;
    for(i=0;i<rects->size;i++){
        if(kind[i]==0){     //字母
            memset(distance,0,sizeof(float)*30);
			
            densityFeature(img,at(rects,i),feature,integrogram);  
               
            for(n=0;n<CharNumA;n++){
			float tmp=0;       
                for(d=0;d<DimNumA;d++){
                    // tmp+=pow(feature[d]-features_char[d][n],2);
					tmp += ((feature[d]-features_char[d][n])*(feature[d]-features_char[d][n]));
                }
				
                // distance[n]=sqrt(tmp);   
                distance[n]=tmp;                 
            }
			
            min=distance[0];
            pos=0;
            for(n=1;n<CharNumA;n++){
                if(distance[n]<min){
                    min=distance[n];
                    pos=n;
                }   
            }   
            outcome[i]=labels_char[pos];
        }else if(kind[i]==1){   //数字
            memset(distance,0,sizeof(float)*30);
            densityFeature(img,at(rects,i),feature,integrogram);   
            for(n=0;n<NumNum;n++){
                float tmp=0;
                for(d=0;d<DimNumA;d++){
                    // tmp+=pow(feature[d]-features_num[d][n],2);
                    tmp += ((feature[d]-features_num[d][n])*(feature[d]-features_num[d][n]));

                }
                // distance[n]=sqrt(tmp);   
                distance[n]=tmp;       
            }
            min=distance[0];
            pos=0;
            for(n=1;n<NumNum;n++){
                if(distance[n]<min){
                    min=distance[n];
                    pos=n;
                }   
            }   
            outcome[i]=labels_num[pos];
        }else if(kind[i]==2){
            outcome[i]='*';
        }

    }
    outcome[rects->size]='\0';
    
}

void OCR(const U8 *buf, int X, int Y, char* outcome){
    IMG img;
    Chain rois;
    int kind[30];
	int i;

    U32** integrogram;
    integrogram=(U32**)malloc(sizeof(U32*)*img.row);
    for(i=0;i<img.row;i++){
        integrogram[i]=(U32*)malloc(sizeof(U32)*img.col);
    }     

    memset(kind,-1,sizeof(int)*30);

    transfer(buf,X, Y, &img);

    binary(&img,integrogram);

    edgeFilter(&img);

    split(&img,&rois,kind,integrogram);

    test(&img,&rois,kind,outcome,integrogram);

    while(rois.size!=0){
        free(pop_back(&rois));
    }

    for(i=0;i<img.row;i++){
        free(integrogram[i]);
    }
    free(integrogram);    

    for(i=0;i<img.row;i++){
        free(img.pImg[i]);
    }
    free(img.pImg);
}