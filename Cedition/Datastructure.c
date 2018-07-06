/***************************************************************************
* Copyright (c) 2018 苏州日宝科技有限责任公司
* All rights reserved
* 
* 文件名称：DataStructure.c
* 
* 摘    要：存储数据结构以及相关方法
* 
* 创 建 者：上官栋栋
* 
* 创建日期：2018年6月28日
*
* 修改记录
日期  修改者   		版本     修改内容
-------------- 		-------  ------------------------  

****************************************************************************/

#include "DataStructure.h"

void push(Chain *chain,Node *node){
    if(chain->size==0){
        chain->back=node;
        chain->front=node;
        chain->size+=1;
    }else{
        chain->back->next=node;
        node->last = chain->back;
        chain->back=node;    
        chain->size+=1;            
    }

}

Node* pop_back(Chain *chain){
    Node *back;
    if(chain->size==0){
        return NULL;
    }else{
        back=chain->back;
        chain->back=back->last;
        if(chain->back!=NULL)
            chain->back->next=NULL;
        chain->size-=1;
        return back;
    }
    
}

Node* pop_front(Chain *chain){

    Node *front;
    if(chain->size==0){
        return NULL;
    }else{
        front=chain->front;
        chain->front=front->next;
        if(chain->front!=NULL){
            chain->front->last=NULL;
        }   
        chain->size-=1;
        return front;
    }
    
}

Node* at(const Chain *chain, int pos){
    int i;
    Node* aim=chain->front;

    for(i=0;i<pos;i++){
        aim=aim->next;
    }
    return aim;
}

void initIMG(IMG* img,int row,int col){
    int i;
    img->row=row;
    img->col=col;
    img->pImg=(U8**)malloc(sizeof(U8*)*row);
    for(i=0;i<row;i++){
        img->pImg[i]=(U8*)malloc(sizeof(U8)*col);
        memset(img->pImg[i],0,sizeof(U8)*col);
    }

}

void freeIMG(IMG* img){
    int i;
    for(i=0;i<img->row;i++){
        free(img->pImg[i]);
    }
    free(img->pImg);
}

//从大到小排序
void sort(Chain* chain,int index, bool isAsc){
    int i,j;
    Node *pos;
    for(i=0;i<chain->size-1;i++){
        pos=chain->front;
        for(j=0;j<chain->size-1-i;j++){
            if(pos->data[index]>pos->next->data[index]){
                if(isAsc){
                    swap(pos,pos->next);
                }
            }else{
                if(!isAsc){
                    swap(pos,pos->next);
                }
            }
            pos=pos->next;
        }
    }
    // for(i=0;i<chain->size;i++){
    //     printf("%d ",at(chain,i)->data[index]);
    // }
    // printf("\n");

}

void swap(Node* node1,Node* node2){
    int tmp,i;
    for(i=0;i<NodeDim;i++){
        tmp=node1->data[i];
        node1->data[i]=node2->data[i];
        node2->data[i]=tmp;
    }
}

void writeChain(const char* filePath,const Chain* chain){
    FILE *file;
	char text[30];
    Node* node;
    int i;

    file=fopen(filePath,"w");
    if(file==NULL){
		printf("error in opening file:%s",filePath);
		return;
	}
    node=chain->front;
    while(node!=NULL){
        for(i=0;i<NodeDim-1;i++){
            sprintf(text,"%d\t",node->data[i]);
            fwrite(text,sizeof(char),strlen(text),file);
        }
		sprintf(text,"%d\n",node->data[NodeDim-1]);
		fwrite(text,sizeof(char),strlen(text),file);   
        node=node->next;     
    }
	fclose(file);
}

//数据转换
void transfer(const U8 *buf, int X, int Y, IMG* img){
    int i,j;
    img->row=Y;
    img->col=X;
    img->pImg=(U8**)malloc(sizeof(U8*)*Y);
    for(i=0;i<Y;i++){
        img->pImg[i]=(U8*)malloc(sizeof(U8)*X);
    }
    for(i=0;i<Y;i++){
        for(j=0;j<X;j++){
            img->pImg[i][j]=buf[j+i*X];
        }
        
    }
}