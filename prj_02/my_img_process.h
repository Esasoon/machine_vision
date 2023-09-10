#ifndef MY_IMG_PROCESS_H
#define MY_IMG_PROCESS_H
/*
 * 图像处理，复刻opencv相关图像处理函数
 *
void cv::resize(
    InputArray src,          // 输入图像，可以是cv::Mat或其他合适的输入
    OutputArray dst,         // 输出图像，通常是cv::Mat
    Size dsize,              // 目标图像的尺寸，cv::Size(width, height)
    double fx = 0,           // 水平方向的缩放因子
    double fy = 0,           // 垂直方向的缩放因子
    int interpolation = INTER_LINEAR  // 插值方法，默认为线性插值
);
*/
#include <iostream>
#include <opencv2/opencv.hpp>
#include "ui_mainwindow.h"
#include "mainwindow.h"

class MainWindow;
class my_img_process
{
    public:
        ~my_img_process();//析构函数
    protected:


    private:
        // 成员变量
        cv::Mat src_img;        // 读入原始图像
        cv::Mat mid_img_0;      // 中间过程图像
        cv::Mat proced_img;     // 完成处理的图像
        cv::Mat out_proced_img;       // 宏观处理的图像(拖拽、旋转)

        // 成员函数声明

        void my_warpAffine(cv::Mat src, cv::Mat &dst, cv::Mat M,cv::Size size,int way);
        void my_resize(cv::Mat src, cv::Mat &dst,double scale,int way);  // 图片缩放函数
        void my_mat2pixmap(cv::InputArray src, QPixmap *pixmap);   // Mat图片格式转化为pixmap
        void my_rotale(cv::Mat src,cv::Mat *dst,double angle); // 旋转函数，无缺失
        void my_get2dMatrix(cv::Mat src,double seta_x, double seta_y,cv::Mat &M,cv::Size &size); // 生成仿射变换矩阵
        void my_get3dMatrix(const cv::Mat& src, std::vector<cv::Point2f> srcPoints, std::vector<cv::Point2f> dstPoints,cv::Mat &M); // 生成透视变换矩阵
        void my_customEqualizeHist(const cv::Mat& src, cv::Mat& dst); // 直方图均衡
        cv::Mat my_rgb2gray(const cv::Mat& src); // 灰度变换
        void my_calcHist(const cv::Mat& src,cv::Mat& hist); // 灰度计算
        void my_medianBlur(const cv::Mat& src, cv::Mat& dst, int windowSize); // 中值滤波
        void my_sharpen(const cv::Mat& src,cv::Mat& dst,const std::string& operatorType); // 锐化

        // 辅助算法函数
        cv::Vec3b calti_vec_Interpol(const cv::Mat src, double x, double y); // 双线性插值算法函数
        cv::Mat customConvolve(const cv::Mat& inputImage, const cv::Mat& kernel); // 卷积函数


    friend class MainWindow; //定义友元函数

};

#endif // MY_IMG_PROCESS_H
