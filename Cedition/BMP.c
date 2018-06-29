/***************************************************************************
* Copyright (c) 2018 苏州日宝科技有限责任公司
* All rights reserved
* 
* 文件名称：BMP.c
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

#include "BMP.h"


void readBMP(const char* filePath, IMG* img){
	FILE *file;
	U16 bfh[7];
	long dpixeladd,LineByteWidth;
    BITMAPINFOHEADER bih;
	if (NULL == (file = fopen(filePath, "rb")))
	{
   		return;
	}
	printf("read %s\n", filePath);
	fread(&bfh, sizeof(U16), 7, file);
	if (bfh[0] != (U16)(((U16)'B')|('M'<<8)))
	{
   		fclose(file);
   		return;
	}
	fread(&bih, sizeof(BITMAPINFOHEADER), 1, file);
	if (bih.biBitCount < 24)
	{
   		fclose(file);
   		return;
	}
	dpixeladd = bih.biBitCount / 8;
	LineByteWidth = bih.biWidth * (dpixeladd);
	if ((LineByteWidth % 4) != 0)
	    LineByteWidth += 4 - (LineByteWidth % 4);
    
    U8 *Buffer;
	if ((Buffer = (U8*)malloc(sizeof(U8)* LineByteWidth * bih.biHeight)) != NULL)
	{
   		fread(Buffer, LineByteWidth * bih.biHeight, 1, file);
	}
	fclose(file);

    img->col=bih.biWidth;
    img->row=bih.biHeight;

	U8 *ptr;
    img->pImg=(U8**)malloc(sizeof(U8*) * img->row);
    for(int i=0;i<img->row;i++){
        img->pImg[i]=(U8*)malloc(sizeof(U8) * img->col);
    }

    u8 b,g,r;
    for(int i=0;i<img->row;i++){
        for(int j=0;j<img->col;j++){
            ptr = Buffer + j * dpixeladd + (img->row-1-i) * LineByteWidth;
            b = *ptr;
            g = *(ptr + 1);
            r = *(ptr + 2);            
            img->pImg[i][j]=(r*299 + g*587 + b*114 + 500) / 1000;
        }
    }

	free(Buffer);
}

void writeTXT(const char* filePath, IMG *img){
    FILE *file;
    file=fopen(filePath,"w");
	char text[10];
    if(file==NULL){
		printf("error in opening file:%s",filePath);
		return;
	}
	for(int i=0;i<img->row;i++){
		for(int j=0;j<img->col-1;j++){
			sprintf(text,"%d ",img->pImg[i][j]);
			fwrite(text,sizeof(char),strlen(text),file);
		}
		sprintf(text,"%d\n",img->pImg[i][img->col-1]);
		fwrite(text,sizeof(char),strlen(text),file);		
	}
	fclose(file);
}
 
int test(void)
{
	int x, y;
	U8 r, g, b;
	int n;
    BITMAPINFOHEADER bih;
    U8 *Buffer = NULL;
    long LineByteWidth;    
	char szfilename[255] = "/home/sgdd/Internship/Data/raw1/165151_6.BMP";
	if (ReadBmp(szfilename,&bih,Buffer,&LineByteWidth) == 0)
	{
   		printf("failure to read file %s", szfilename);
  	 	return 1;
	}
	printf("Width: %d\n", bih.biWidth);
	printf("Height: %d\n", bih.biHeight);
	printf("BitCount: %d\n\n", (int)bih.biBitCount);
	while(1)
	{
   		printf("input the X:");
   		scanf("%d", &x);
   		if (x < 0)
    			break;
   		printf("input the Y:");
   		scanf("%d", &y);
   		if (GetDIBColor(x, y, &r, &g, &b,&bih,Buffer,&LineByteWidth) == 1)
    			printf("(%d, %d): r:%d, g:%d, b:%d\n", x, y, (int)r, (int)g, (int)b);
   		else
   	 		printf("input error.\n");
	}
	free(Buffer);
	return 0;
}

int ReadBmp(const char* szFileName,BITMAPINFOHEADER *bih,U8* Buffer,long *LineByteWidth)
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
	if (bih->biBitCount < 24)
	{
   		fclose(file);
   		return 0;
	}
	dpixeladd = bih->biBitCount / 8;
	*LineByteWidth = bih->biWidth * (dpixeladd);
	if ((*LineByteWidth % 4) != 0)
	*LineByteWidth += 4 - (*LineByteWidth % 4);
	if ((Buffer = (U8*)malloc(sizeof(U8)* *LineByteWidth * bih->biHeight)) != NULL)
	{
   		fread(Buffer, *LineByteWidth * bih->biHeight, 1, file);
   		fclose(file);
  		 return 1;
	}
	fclose(file);
	return 0;
}


int GetDIBColor(int X, int Y, U8 *r, U8 *g, U8 *b,BITMAPINFOHEADER *bih,U8* Buffer,long *LineByteWidth)
{
	int dpixeladd;
	U8 *ptr;
	if (X < 0 || X >= bih->biWidth || Y < 0 || Y >= bih->biHeight)
	{
   		return 0;
	}
	dpixeladd = bih->biBitCount / 8;
	ptr = Buffer + X * dpixeladd + (bih->biHeight - 1 - Y) * (*LineByteWidth);
	*b = *ptr;
	*g = *(ptr + 1);
	*r = *(ptr + 2);
	return 1;
}