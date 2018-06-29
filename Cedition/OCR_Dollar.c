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

//arg1是二值化使用的参数，值越大白色区域越大。
const float arg1=1.4;


//使用局部二值化的方法
void binary(IMG *img){
    //计算积分图
    U32** integrogram;
    integrogram=(U32**)malloc(sizeof(U32*)*img->row);
    for(int i=0;i<img->row;i++){
        integrogram[i]=(U32*)malloc(sizeof(U32)*img->col);
    }

    integrogram[0][0]=img->pImg[0][0];
    for(int i=1;i<img->col;i++){
        integrogram[0][i]=img->pImg[0][i]+integrogram[0][i-1];
    }
    for(int j=1;j<img->row;j++){
        integrogram[j][0]=img->pImg[j][0]+integrogram[j-1][0];
    }
    for(int i=1;i<img->row;i++){
        for(int j=1;j<img->col;j++){
            integrogram[i][j]=img->pImg[i][j]+
                                integrogram[i-1][j]+
                                integrogram[i][j-1]-
                                integrogram[i-1][j-1];
        }
    }

    //计算局部均值

    int width=25;    //方形区域边长的一半
    int x1,y1,x2,y2;
    float threshold;
    for(int i=0;i<img->row;i++){
        for(int j=0;j<img->col;j++){
            x1=j<width?0:j-width;
            y1=i<width?0:i-width;
            x2=j+width>img->col-1?img->col-1:j+width;
            y2=i+width>img->row-1?img->row-1:i+width;
            threshold=(float)(integrogram[y2][x2]-
                            integrogram[y1][x2]-
                            integrogram[y2][x1]+
                            integrogram[y1][x1]);
            threshold/=(float)( (y2-y1)*(x2-x1) );
                            
            if(img->pImg[i][j]>threshold/arg1){
                img->pImg[i][j]=255;
            }else{
                img->pImg[i][j]=0;
            }
        }
    }
    
    for(int i=0;i<img->row;i++){
        free(integrogram[i]);
    }
    free(integrogram);
}

//将所有与边缘相贴的图案筛选掉
void edgeFilter(IMG *src){
    Chain blackRegions;
    blackRegions.size=0;

    Node *region=NULL;
    short begin,end;
    
    //从下边缘开始扫描
    begin=0;
    end=src->col-1;
    addRegion(src,src->row-1,begin,end,&blackRegions);

    int pos,num;
    for(int i=src->row-2;i>-1;i--){
        pos=0;
        num=blackRegions.size;
        if(num==0){
            break;
        }
        for(int j=0;j<num;j++){
            free(region);
            region=pop_front(&blackRegions);
            if(region->data[1]<pos){
                continue;
            }else if(region->data[0]>=pos){
                begin=0;
                end=src->col-1;
                for(int m=region->data[0];m>=0;m--){             
                    if(src->pImg[i][m]>100){
                        begin=m;
                        break;
                    }
                }
                for(int m=region->data[1];m<src->col;m++){
                    if(src->pImg[i][m]>100){
                        end=m;
                        break;
                    }                    
                }
                pos=end;
                addRegion(src,i,begin,end,&blackRegions);
            }else{
                begin=pos+1;
                for(int m=region->data[1];m<src->col;m++){
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
    for(int i=1;i<src->row;i++){
        pos=0;
        num=blackRegions.size;
        if(num==0){
            break;
        }
        for(int j=0;j<num;j++){
            free(region);
            region=pop_front(&blackRegions);
            if(region->data[1]<pos){
                continue;
            }else if(region->data[0]>=pos){
                begin=0;
                end=src->col-1;
                for(int m=region->data[0];m>=0;m--){
                    if(src->pImg[i][m]>100){
                        begin=m;
                        break;
                    }
                }
                for(int m=region->data[1];m<src->col;m++){
                    if(src->pImg[i][m]>100){
                        end=m;
                        break;
                    }                    
                }
                pos=end;
                addRegion(src,i,begin,end,&blackRegions);
            }else{
                begin=pos+1;
                for(int m=region->data[1];m<src->col;m++){
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
}

//将begin和end中间的黑色区域找出并存放在regions中，同时将该区域置白
void addRegion(IMG *src,int row,int begin,int end,Chain *regions){
    Node *region;
    region=(Node*)malloc(sizeof(Node));

    if(src->pImg[row][begin]>100){
        region->data[0]=-1;
        region->data[1]=-1;
    }else{
        region->data[0]=begin;
        region->data[1]=-1;        
    }

    for(int i=begin+1;i<end+1;i++){
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

    for(int i=begin;i<end+1;i++){
        src->pImg[row][i]=255;
    }
}

