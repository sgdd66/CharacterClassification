/***************************************************************************
* Copyright (c) 2018 苏州日宝科技有限责任公司
* All rights reserved
* 
* 文件名称：DataStructure.h
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
#ifndef __DATASTRUCTURE_H__
#define __DATASTRUCTURE_H__
#include "stdafx.h"
#define NodeDim 4
typedef struct Node{
    struct Node *next;
    struct Node *last;
    int data[NodeDim];
} Node;

typedef struct Chain{
    struct Node *front;
    struct Node *back;
    int size;
} Chain;




// typedef struct Node_2s{
//     Node_2s *next;
//     short data[2];
// } Node_2s;

// typedef struct Node_3s{
//     Node_3s *next;
//     short data[3];
// } Node_3s;

// typedef struct Chain_2s{
//     Node_2s *front;
//     Node_2s *back;
// } Chain_2s;

// typedef struct Chain_3s{
//     Node_3s *front;
//     Node_3s *back;
// } Chain_3s;

void push(Chain *chain,Node *node);
Node* pop_back(Chain *chain);
Node* pop_front(Chain *chain);
Node* at(const Chain *chain, int pos);
void initIMG(IMG* img,int row,int col);
void freeIMG(IMG* img);
//从大到小排序
void sort(Chain* chain,int index, bool isAsc);
//只交换数据
void swap(Node* node1,Node* node2);
//输出链表
void writeChain(const char* filePath,const Chain* chain);
#endif