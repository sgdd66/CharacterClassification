#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv_modules.hpp>  
#include <opencv2/ml/ml.hpp>  
using namespace cv;

#include<stdio.h>
#include<math.h>
#include<vector>
using namespace std;

#include<regex>
#define PI 3.1415926


#ifndef AdjustParam
#define AdjustParam
#define TextHeight 30
#define WorkShopPath "/home/sgdd/Internship"
#define DataPath "/home/sgdd/Internship/Data"

#endif

//xml输出
// FileStorage fs("/home/sgdd/AI_competition/Data/Mat.xml", FileStorage::WRITE);
// fs << "vocabulary" << hist;
// fs.release();

//xml读取
// FileStorage fs(format("%s/argument.xml",DataPath), FileStorage::READ);
// float arg1;
// fs["arg1"] >> arg1;   
// fs.release();

//csv输出
// ofstream file("/home/sgdd/Internship/Data/Mat.csv");
// file << format(hist, Formatter::FMT_CSV);
// file.close();

//csv读取
// CvMLData mlData;
// mlData.read_csv("test.csv");//读取csv文件
// cv::Mat data= cv::Mat(mlData.get_values(),true);