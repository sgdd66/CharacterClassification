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

void readImg(const char* filePath, IMG* img);
int ReadBmp(const char* szFileName);
int GetDIBColor(int X, int Y, U8 *r, U8 *g, U8 *b);
int test(void);

#endif