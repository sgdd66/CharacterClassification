/***************************************************************************
* Copyright (c) 2018 苏州日宝科技有限责任公司
* All rights reserved
* 
* 文件名称：OCR_Dollar.h
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

#ifndef __OCR_Dollar_h__
#define __OCR_Dollar_h__
#include "stdafx.h"
#include "DataStructure.h"
//使用局部二值化的方法
void binary(IMG *img,U32 **integrogram);

//将所有与边缘相贴的图案筛选掉
void edgeFilter(IMG *img);

//将begin和end中间的黑色区域找出并存放在regions中，同时将该区域置白
void addRegion(IMG *src,int row,int begin,int end,Chain *regions);    

void split(const IMG *src,Chain* retVal,int *kind,U32** integrogram);

void densityFeature(const IMG* img, const Node* rect,float* feature,U32** integrogram);



void test(const IMG* img,const Chain* rects,const int* kind,char* outcome,U32** integrogram);

void OCR(const U8 *buf, int X, int Y, char* outcome);




#endif