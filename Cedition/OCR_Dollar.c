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
 

BITMAPINFOHEADER bih;
U8 *Buffer = NULL;
long LineByteWidth;
 
int test(void)
{
	int x, y;
	U8 r, g, b;
	int n;
	char szfilename[255] = "/home/sgdd/Internship/Data/raw1/165151_6.BMP";
	if (ReadBmp(szfilename) == 0)
	{
   		printf("failure to read file %s", szfilename);
  	 	return 1;
	}
	printf("Width: %ld\n", bih.biWidth);
	printf("Height: %ld\n", bih.biHeight);
	printf("BitCount: %d\n\n", (int)bih.biBitCount);
	while(1)
	{
   		printf("input the X:");
   		scanf("%d", &x);
   		if (x < 0)
    			break;
   		printf("input the Y:");
   		scanf("%d", &y);
   		if (GetDIBColor(x, y, &r, &g, &b) == 1)
    			printf("(%d, %d): r:%d, g:%d, b:%d\n", x, y, (int)r, (int)g, (int)b);
   		else
   	 		printf("input error.\n");
	}
	free(Buffer);
	return 0;
}
int ReadBmp(const char* szFileName)
{
	FILE *file;
	U16 bfh[7];
	long dpixeladd;
	if (NULL == (file = fopen(szFileName, "rb")))
	{
   		return 0;
	}
	printf("%s\n", szFileName);
	fread(&bfh, sizeof(U16), 7, file);
	if (bfh[0] != (U16)(((U16)'B')|('M'<<8)))
	{
   		fclose(file);
   		return 0;
	}
	fread(&bih, sizeof(BITMAPINFOHEADER), 1, file);
	if (bih.biBitCount < 24)
	{
   		fclose(file);
   		return 0;
	}
	dpixeladd = bih.biBitCount / 8;
	LineByteWidth = bih.biWidth * (dpixeladd);
	if ((LineByteWidth % 4) != 0)
	LineByteWidth += 4 - (LineByteWidth % 4);
	if ((Buffer = (U8*)malloc(sizeof(U8)* LineByteWidth * bih.biHeight)) != NULL)
	{
   		fread(Buffer, LineByteWidth * bih.biHeight, 1, file);
   		fclose(file);
  		 return 1;
	}
	fclose(file);
	return 0;
}
int GetDIBColor(int X, int Y, U8 *r, U8 *g, U8 *b)
{
	int dpixeladd;
	U8 *ptr;
	if (X < 0 || X >= bih.biWidth || Y < 0 || Y >= bih.biHeight)
	{
   		return 0;
	}
	dpixeladd = bih.biBitCount / 8;
	ptr = Buffer + X * dpixeladd + (bih.biHeight - 1 - Y) * LineByteWidth;
	*b = *ptr;
	*g = *(ptr + 1);
	*r = *(ptr + 2);
	return 1;
}