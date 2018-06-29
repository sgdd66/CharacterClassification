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