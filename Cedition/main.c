#include"stdafx.h"

#include "OCR_Dollar.h"
#include "BMP.h"
#include <sys/time.h>



int main(int argn,char** argc){
    IMG img;
    Chain rois;
    int kind[30];
    memset(kind,-1,sizeof(int)*30);  
    char outcome[30];
    struct timeval timeStart;
    struct timeval timeEnd;
    int i;

    U64 t1,t2,t3,t4;
    f64 t5;       
    readBMP("/home/sgdd/Internship/Data/raw2/143027_6.BMP",&img);

    U32** integrogram;
    integrogram=(U32**)malloc(sizeof(U32*)*img.row);
    for(i=0;i<img.row;i++){
        integrogram[i]=(U32*)malloc(sizeof(U32)*img.col);
    }       

    gettimeofday(&timeStart,NULL);
    binary(&img,integrogram);
    gettimeofday(&timeEnd,NULL);
    t1 = 1000000*(timeEnd.tv_sec - timeStart.tv_sec) + (timeEnd.tv_usec - timeStart.tv_usec);
	
    gettimeofday(&timeStart,NULL);
    edgeFilter(&img);
    gettimeofday(&timeEnd,NULL);
    t2 = 1000000*(timeEnd.tv_sec - timeStart.tv_sec) + (timeEnd.tv_usec - timeStart.tv_usec);
 
    gettimeofday(&timeStart,NULL);    
    split(&img,&rois,kind,integrogram);
    gettimeofday(&timeEnd,NULL);
    t3 = 1000000*(timeEnd.tv_sec - timeStart.tv_sec) + (timeEnd.tv_usec - timeStart.tv_usec);
    
    gettimeofday(&timeStart,NULL);    
    test(&img,&rois,kind,outcome,integrogram);
    gettimeofday(&timeEnd,NULL);
    t4 = 1000000*(timeEnd.tv_sec - timeStart.tv_sec) + (timeEnd.tv_usec - timeStart.tv_usec);
    
    t5=(t1+t2+t3+t4)/1000.0;
    writeTXT("/home/sgdd/Internship/Data/mat.txt",&img);
// OCR(const U8 *buf, int X, int Y, char* outcome)
}







