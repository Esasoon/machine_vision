#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;
// img read test
Mat color_img;
Mat img_prossed;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitImg();
    img_show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::InitImg
 * 从指定路劲加载图片
 */
void MainWindow::InitImg()
{

    color_img = imread("H:/cqu_project/qt/img/011423195.JPG");
    Size dsize = Size(800, 500);

    cv::resize(color_img, img_prossed, dsize, 0, 0, INTER_AREA);
}


/**
 * @brief MainWindow::img_show
 * 显示图像
 */
void MainWindow::img_show()
{
    if (img_prossed.empty())
    {
        return;
    }
    cv::cvtColor(img_prossed, img_prossed, COLOR_BGR2RGB);//图像格式转换
    QImage disImage = QImage((const unsigned char*)(img_prossed.data),img_prossed.cols,img_prossed.rows,img_prossed.cols*img_prossed.channels(),QImage::Format_RGB888);
    ui->label->setPixmap(QPixmap::fromImage(disImage));//显示图像
}


void MainWindow::on_pb_input_clicked()
{
//    ui->play->setEnabled(true);
}

