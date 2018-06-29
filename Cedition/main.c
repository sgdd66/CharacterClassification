#include"stdafx.h"

#include "OCR_Dollar.h"
#include "BMP.h"

int main(int argn,char** argc){
    IMG img;
    readBMP("/home/sgdd/Internship/Data/raw2/143027_6.BMP",&img);
    binary(&img);
    edgeFilter(&img);
    writeTXT("/home/sgdd/Internship/Data/mat.txt",&img);
}







