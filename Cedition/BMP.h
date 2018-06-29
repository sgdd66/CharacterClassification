/***************************************************************************
* Copyright (c) 2018 苏州日宝科技有限责任公司
* All rights reserved
* 
* 文件名称：BMP.h
* 
* 摘    要：BMP文件的读取和写入函数
* 
* 创 建 者：上官栋栋
* 
* 创建日期：2018年6月28日
*
* 修改记录
日期  修改者   		版本     修改内容
-------------- 		-------  ------------------------  

****************************************************************************/

#ifndef __BMP_h__
#define __BMP_h__
#include "stdafx.h"

typedef struct tagBITMAPINFOHEADER{
   	U32	biSize;            //本结构大小 
   	S32	biWidth;           //图像宽度
   	S32	biHeight;          //图像高度
   	U16	biPlanes;          // 
   	U16	biBitCount;        //图像色深，一个像素用多少位表示
   	U32	biCompression;     //压缩方式
  	U32	biSizeImage;       //图像数据大小 
   	S32	biXPelsPerMeter;   //水平分辨率        
   	S32	biYPelsPerMeter;   //垂直分辨率    
   	U32	biClrUsed;         //BMP图像使用的颜色，0表示使用全部颜色
   	U32	biClrImportant;    //重要的颜色，0表示所有颜色都很重要。当显卡不能显示所有颜色，用此值筛选颜色。
} BITMAPINFOHEADER;

//读取BMP文件，返回IMG数据，返回数据是一个灰度图
void readBMP(const char* filePath, IMG* img);

//将IMG数据结构写入TXT文件中
void writeTXT(const char* filePath, IMG *img);

//读取BMP文件，文件信息存储在全局变量中
int ReadBmp(const char* szFileName, BITMAPINFOHEADER *bih, U8* Buffer, long *LineByteWidth);

//获取图片特定位点的RGB值
int GetDIBColor(int X, int Y, U8 *r, U8 *g, U8 *b,BITMAPINFOHEADER *bih,U8* Buffer,long *LineByteWidth);

//测试函数，演示使用
int test(void);

#endif