#ifndef STDAFX
#define STDAFX

#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//自定义类型名
typedef unsigned char   U8;
typedef unsigned short  U16;
typedef unsigned int    U32;
typedef unsigned long   U64;

typedef char    S8;
typedef short   S16;
typedef int     S32;
typedef long    S64;

typedef float   F32;
typedef double  F64;

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long   u64;

typedef char    s8;
typedef short   s16;
typedef int     s32;
typedef long    s64;

typedef float   f32;
typedef double  f64;

//自定义bool变量
typedef enum{false,true} bool;

//图像数据
typedef struct {
    U8** pImg;
    int row;
    int col;
} IMG;

#define DimNumA  17
#define CharNumA 14
#define NumNum  10

#endif