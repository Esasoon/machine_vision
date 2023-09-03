/**
  * 简易图片编辑器
  * copyright : 岳奕松
  */
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


Mat img_0,img_1,img_2, img_3, img_4, img_5;
Mat src;
Mat src_prossed;
Mat src_grey;
Mat src_binary;
Mat src_prossed_move;
Mat dst;
Mat M_rot; // 旋转&缩放
Mat M_af; // 平移、旋转、缩放、投射

// 主界面绘制
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    file_location = 0;
    ui->label->setScaledContents(true); // 图片自适应
    ui->label_2->setScaledContents(true); // 图片自适应
    ui->label_3->setScaledContents(true); // 图片自适应
    ui->label_4->setScaledContents(true); // 图片自适应
    ui->label_5->setScaledContents(true); // 图片自适应
    // 回调函数连接
    connect(ui->pushButton_3,&QPushButton::clicked,this,&MainWindow::openfile); // 打开文件夹


//    InitImg();
//    img_show();
}

// 析构函数
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

}


/**
 * @brief MainWindow::openfile
 * 打开指定文件夹
 */
void MainWindow::openfile()
{
//    cout <<M_rot<<endl;
    //通过QFileDialog::getOpenFileNames获取选中的图片文件的名字
//    QStringList list=QFileDialog::getOpenFileNames(this,"打开图片",".","Images (*.jpg *.png *.bmp)");
    QStringList list=QFileDialog::getOpenFileNames(this,"打开图片","E:/cqu_project/qt/img","Images (*.jpg *.png *.bmp)");

    if(list.isEmpty())
    {
        return;
    }else
    {
        file_list.clear();
        file_list=list;
        file_location=0;
    }
    if(file_list.length()==1)
    {
        ui->label_4->setPixmap(QPixmap(file_list[file_location]));

    }
    if(file_list.length()==2)
    {
        file_location=1;
        ui->label_4->setPixmap(QPixmap(file_list[file_location]));
        ui->label_3->setPixmap(QPixmap(file_list[file_location-1]));
    }
    if(file_list.length()>2)
    {
        file_location=1;
        ui->label_3->setPixmap(QPixmap(file_list[file_location-1]));
        ui->label_4->setPixmap(QPixmap(file_list[file_location]));
        ui->label_5->setPixmap(QPixmap(file_list[file_location+1]));
    }

    cout << file_location <<endl;
    std::string resultString = file_list[file_location].toStdString();
    src = cv::imread(resultString);
    src_prossed = src.clone();
    // Mat为BGR必须先转化为RGB，再转化为QImage
    cv::cvtColor(src,dst,cv::COLOR_BGR2RGB);


    // 将 cv::Mat 转换为 QImage
    QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

    // 将 QImage 转换为 QPixmap
    QPixmap pixmap = QPixmap::fromImage(image);
    ui->label->setPixmap(pixmap);//显示图像


    update_hsv_space(src);

//    ui->label->setPixmap(QPixmap(file_list.at(file_location)));
//    ui->label->setPixmap(QPixmap(file_list[file_location]));

}
void MainWindow::wheelEvent(QWheelEvent* event) {
    // 获取滚轮滚动的角度
    int delta = event->angleDelta().y();
    /**
     * lable 左边
     * （10，40）
     * 800*600
     */
    int x = event->x() - 10; // 获取鼠标指针的 x 坐标
    int y = event->y() - 40; // 获取鼠标指针的 y 坐标
    if(x<=0) x=0;
    if(y<=0) y=0;
    if(x>=800) x=800;
    if(y>=600) y=600;

    cout<<x<<endl;
    cout<<y<<endl;
    // 判断滚轮是向上滚动还是向下滚动
    if (delta > 0) {
        // 鼠标向上滚动
        img_scale+=0.1;
        cout<<img_scale<<endl;
    } else if (delta < 0) {
        // 鼠标向下滚动
        img_scale-=0.1;
        if(img_scale<=0.1)
        {
            img_scale=0.1;
        }

        cout<<img_scale<<endl;
    }
    Size dst_sz(src.cols, src.rows);
//    Point center(static_cast<float>(x), static_cast<float>(y));   //static_cast<float>()强制类型转换，一定要确保数据类型正确
    Point center(static_cast<float>(src.cols / 2.), static_cast<float>(src.rows / 2.));   //static_cast<float>()强制类型转换，一定要确保数据类型正确

    cout<<center<<endl;
    M_rot = cv::getRotationMatrix2D(center, rot_angle, img_scale);

    cv::warpAffine(src, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换：
    if(!M_af.empty())
    {
        cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz,1,0); // 平移矩阵
    }
    // Mat为BGR必须先转化为RGB，再转化为QImage
    cv::cvtColor(src_prossed,dst,cv::COLOR_BGR2RGB);


    // 将 cv::Mat 转换为 QImage
    QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

    // 将 QImage 转换为 QPixmap
    QPixmap pixmap = QPixmap::fromImage(image);

    ui->label->setPixmap(pixmap);//显示图像
    // 阻止事件传递给父类（如果需要）
    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        cout<<"mousePressed"<<endl;
        isDragging = 1;
        lastMousePos = event->pos();
    }

}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (isDragging) {
        QPoint delta = event->pos() - lastMousePos;
        img_Pos += delta;
//        ui->label->move(pos() + delta);
//        ui->label->move(10, 10);
        Size dst_sz(src_prossed.cols, src_prossed.rows);
        M_af = (cv::Mat_<float>(2, 3) << 1, 0, img_Pos.x(), 0, 1,img_Pos.y());


        cv::warpAffine(src_prossed, src_prossed_move, M_af, dst_sz,1,0); // 平移矩阵
        if(!M_rot.empty())
        {
            cv::warpAffine(src, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
        }


        // Mat为BGR必须先转化为RGB，再转化为QImage
        cv::cvtColor(src_prossed_move,dst,cv::COLOR_BGR2RGB);


        // 将 cv::Mat 转换为 QImage
        QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

        // 将 QImage 转换为 QPixmap
        QPixmap pixmap = QPixmap::fromImage(image);

        ui->label->setPixmap(pixmap);//显示图像

        lastMousePos = event->pos();
//        cout<<"("<<delta.x()<<","<<delta.y()<<")"<<endl;
    }

}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
}



/**
 * @brief MainWindow::on_pushButton_clicked
 * 图片切换--
 */
void MainWindow::on_pushButton_clicked()
{
    isGrey = 0;
    isBinary=0;
    isHSV=0;
    cout<<"--"<<endl;
    if(file_location == 0)
    {
        file_location = file_list.length()-1;
    }
    else
    {
        file_location--;
    }


    if(file_list.length()>2)
    {
        if(file_location == 0) // 首
        {
            ui->label_3->setPixmap(QPixmap(file_list[file_list.length()-1]));
            ui->label_4->setPixmap(QPixmap(file_list[file_location]));
            ui->label_5->setPixmap(QPixmap(file_list[file_location+1]));
        }
        else if(file_location == file_list.length()-1) // 尾
        {
            ui->label_3->setPixmap(QPixmap(file_list[file_location-1]));
            ui->label_4->setPixmap(QPixmap(file_list[file_location]));
            ui->label_5->setPixmap(QPixmap(file_list[0]));
        }
        else //中
        {
            ui->label_3->setPixmap(QPixmap(file_list[file_location-1]));
            ui->label_4->setPixmap(QPixmap(file_list[file_location]));
            ui->label_5->setPixmap(QPixmap(file_list[file_location+1]));
        }
    }

    cout << file_list.length() <<endl;
    std::string resultString = file_list[file_location].toStdString();
    src = cv::imread(resultString);
    update_hsv_space(src);

    src_prossed = src.clone();
    rot_angle=0;
    ui->horizontalSlider->setValue(0);
    img_scale=1;

    Size dst_sz(src.cols, src.rows);
    Point center(static_cast<float>(src.cols / 2.), static_cast<float>(src.rows / 2.));   //static_cast<float>()强制类型转换，一定要确保数据类型正确

    M_rot = cv::getRotationMatrix2D(center, rot_angle, img_scale);

    M_af = (cv::Mat_<float>(2, 3) << 1, 0, 0, 0, 1,0);
    cv::warpAffine(src, src_prossed, M_rot, dst_sz,1,0); // 平移矩阵
    cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：

    // Mat为BGR必须先转化为RGB，再转化为QImage
    cv::cvtColor(src_prossed,dst,cv::COLOR_BGR2RGB);


    // 将 cv::Mat 转换为 QImage
    QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

    // 将 QImage 转换为 QPixmap
    QPixmap pixmap = QPixmap::fromImage(image);

    ui->label->setPixmap(pixmap);//显示图像

}

/**
 * @brief MainWindow::on_pushButton_clicked
 * 图片切换++
 */

void MainWindow::on_pushButton_2_clicked()
{
    isGrey = 0;
    isBinary=0;
    isHSV=0;
    cout<<"++"<<endl;
    if(file_location == file_list.length()-1)
        file_location=0;
    else
        file_location++;

    if(file_list.length()>2)
    {
        if(file_location == 0) // 首
        {
            ui->label_3->setPixmap(QPixmap(file_list[file_list.length()-1]));
            ui->label_4->setPixmap(QPixmap(file_list[file_location]));
            ui->label_5->setPixmap(QPixmap(file_list[file_location+1]));
        }
        else if(file_location == file_list.length()-1) // 尾
        {
            ui->label_3->setPixmap(QPixmap(file_list[file_location-1]));
            ui->label_4->setPixmap(QPixmap(file_list[file_location]));
            ui->label_5->setPixmap(QPixmap(file_list[0]));
        }
        else //中
        {
            ui->label_3->setPixmap(QPixmap(file_list[file_location-1]));
            ui->label_4->setPixmap(QPixmap(file_list[file_location]));
            ui->label_5->setPixmap(QPixmap(file_list[file_location+1]));
        }
    }
    std::string resultString = file_list[file_location].toStdString();
    src = cv::imread(resultString);
    update_hsv_space(src);

    src_prossed = src.clone();
    rot_angle=0;
    ui->horizontalSlider->setValue(0);
    img_scale=1;

    Size dst_sz(src.cols, src.rows);
    Point center(static_cast<float>(src.cols / 2.), static_cast<float>(src.rows / 2.));   //static_cast<float>()强制类型转换，一定要确保数据类型正确

    M_rot = cv::getRotationMatrix2D(center, rot_angle, img_scale);

    M_af = (cv::Mat_<float>(2, 3) << 1, 0, 0, 0, 1,0);
    cv::warpAffine(src, src_prossed, M_rot, dst_sz,1,0); // 平移矩阵
    cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
    // Mat为BGR必须先转化为RGB，再转化为QImage
    cv::cvtColor(src_prossed,dst,cv::COLOR_BGR2RGB);


    // 将 cv::Mat 转换为 QImage
    QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

    // 将 QImage 转换为 QPixmap
    QPixmap pixmap = QPixmap::fromImage(image);

    ui->label->setPixmap(pixmap);//显示图像
}



/**
 * @brief MainWindow::on_horizontalSlider_actionTriggered
 * @param action
 * 控制图片旋转角度
 */
void MainWindow::on_horizontalSlider_actionTriggered(int action)
{

    rot_angle = ui->horizontalSlider->value();
    cout << rot_angle <<endl;
//    std::string resultString = file_list[file_location].toStdString();
//    cv::Mat src = cv::imread(resultString);



    Size dst_sz(src_prossed.cols, src_prossed.rows);
    Point center(static_cast<float>(src.cols / 2.), static_cast<float>(src.rows / 2.));   //static_cast<float>()强制类型转换，一定要确保数据类型正确


    M_rot = cv::getRotationMatrix2D(center, rot_angle, img_scale);

    cv::warpAffine(src, src_prossed, M_rot, dst_sz,1,0); // 平移矩阵
    if(!M_af.empty())
    {
        cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
    }

    // Mat为BGR必须先转化为RGB，再转化为QImage
    cv::cvtColor(src_prossed,dst,cv::COLOR_BGR2RGB);


    // 将 cv::Mat 转换为 QImage
    QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

    // 将 QImage 转换为 QPixmap
    QPixmap pixmap = QPixmap::fromImage(image);

    ui->label->setPixmap(pixmap);//显示图像
}

/**
 * @brief MainWindow::on_pushButton_4_clicked
 * 重置图片旋转角度
 */
void MainWindow::on_pushButton_4_clicked()
{

    cout<<M_rot<<endl;
    ui->horizontalSlider->setValue(0);

    rot_angle = ui->horizontalSlider->value();


    Size dst_sz(src.cols, src.rows);
    Point center(static_cast<float>(src.cols / 2.), static_cast<float>(src.rows / 2.));   //static_cast<float>()强制类型转换，一定要确保数据类型正确


    M_rot = cv::getRotationMatrix2D(center, rot_angle, img_scale);

    cv::warpAffine(src, dst, M_rot, dst_sz, 1,0);  //仿射变换：
    // Mat为BGR必须先转化为RGB，再转化为QImage
    cv::cvtColor(dst,dst,cv::COLOR_BGR2RGB);
    // 将 cv::Mat 转换为 QImage
    QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);
    // 将 QImage 转换为 QPixmap
    QPixmap pixmap = QPixmap::fromImage(image);

    ui->label->setPixmap(pixmap);//显示图像
}

/**
 * @brief MainWindow::on_pushButton_6_clicked
 * 重置
 */
void MainWindow::on_pushButton_6_clicked()
{
    std::string resultString = file_list[file_location].toStdString();
    src = cv::imread(resultString);
    rot_angle=0;
    ui->horizontalSlider->setValue(0);
    img_scale=1;


    isGrey = 0;
    isBinary=0;
    isHSV=0;

    Size dst_sz(src.cols, src.rows);
    Point center(static_cast<float>(src.cols / 2.), static_cast<float>(src.rows / 2.));   //static_cast<float>()强制类型转换，一定要确保数据类型正确

    M_rot = cv::getRotationMatrix2D(center, rot_angle, img_scale);

    M_af = (cv::Mat_<float>(2, 3) << 1, 0, 0, 0, 1,0);
    cv::warpAffine(src, src_prossed, M_rot, dst_sz,1,0); // 平移矩阵
    cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
    // Mat为BGR必须先转化为RGB，再转化为QImage
    cv::cvtColor(src_prossed,dst,cv::COLOR_BGR2RGB);


    // 将 cv::Mat 转换为 QImage
    QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

    // 将 QImage 转换为 QPixmap
    QPixmap pixmap = QPixmap::fromImage(image);

    ui->label->setPixmap(pixmap);//显示图像


}

/**
 * @brief MainWindow::on_pushButton_7_clicked
 * 反色操作
 */
void MainWindow::on_pushButton_7_clicked()
{
    src = ~src;
    update_hsv_space(src);
    Size dst_sz(src.cols, src.rows);

    if(!M_af.empty() && M_rot.empty())
    {
        cv::warpAffine(src, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
    }
    if(M_af.empty() && !M_rot.empty() )
    {
        cv::warpAffine(src, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
    }
    if(!M_af.empty() && !M_rot.empty() )
    {
        cv::warpAffine(src, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
        cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
    }
    if(M_af.empty() && M_rot.empty() )
    {
        src_prossed = src.clone();
    }
    // Mat为BGR必须先转化为RGB，再转化为QImage
    cv::cvtColor(src_prossed,dst,cv::COLOR_BGR2RGB);


    // 将 cv::Mat 转换为 QImage
    QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

    // 将 QImage 转换为 QPixmap
    QPixmap pixmap = QPixmap::fromImage(image);

    ui->label->setPixmap(pixmap);//显示图像
}


/**
 * @brief MainWindow::on_pushButton_8_clicked
 * 灰度化
 */
void MainWindow::on_pushButton_8_clicked()
{
    if(!isGrey)
    {
        cvtColor(src, src, cv::COLOR_BGR2GRAY);
        Size dst_sz(src.cols, src.rows);

        if(!M_af.empty() && M_rot.empty())
        {
            cv::warpAffine(src, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
        }
        if(M_af.empty() && !M_rot.empty() )
        {
            cv::warpAffine(src, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
        }
        if(!M_af.empty() && !M_rot.empty() )
        {
            cv::warpAffine(src, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
            cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
        }
        if(M_af.empty() && M_rot.empty() )
        {
            src_prossed = src.clone();
        }
        // Mat为BGR必须先转化为RGB，再转化为QImage
        cv::cvtColor(src_prossed,dst,cv::COLOR_BGR2RGB);


        // 将 cv::Mat 转换为 QImage
        QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

        // 将 QImage 转换为 QPixmap
        QPixmap pixmap = QPixmap::fromImage(image);

        ui->label->setPixmap(pixmap);//显示图像
        isGrey=1;
    }

}

/**
 * @brief MainWindow::on_pushButton_10_clicked
 * 二值化
 */
void MainWindow::on_pushButton_10_clicked()
{
    if(!isBinary)
    {
        isBinary=1;
        if(isGrey) // 已完成灰度化
        {
            // 设置二值化阈值（可以根据需要调整）
            int threshold_value = 128;
            cout<<"threshold_value:"<<threshold_value<<endl;

            Size dst_sz(src.cols, src.rows);
            // 应用二值化
            threshold(src, src_binary, threshold_value, 255, cv::THRESH_BINARY);
            if(!M_af.empty() && M_rot.empty())
            {
                cv::warpAffine(src_binary, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
            }
            if(M_af.empty() && !M_rot.empty() )
            {
                cv::warpAffine(src_binary, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
            }
            if(!M_af.empty() && !M_rot.empty() )
            {
                cv::warpAffine(src_binary, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
                cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
            }
            if(M_af.empty() && M_rot.empty() )
            {
                src_prossed = src_binary.clone();
            }
            // Mat为BGR必须先转化为RGB，再转化为QImage
            cv::cvtColor(src_prossed,dst,cv::COLOR_BGR2RGB);


            // 将 cv::Mat 转换为 QImage
            QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

            // 将 QImage 转换为 QPixmap
            QPixmap pixmap = QPixmap::fromImage(image);

            ui->label->setPixmap(pixmap);//显示图像
        }
        else
        {
            cvtColor(src, src_grey, cv::COLOR_BGR2GRAY);
            // 设置二值化阈值（可以根据需要调整）
            int threshold_value = 100;

            Size dst_sz(src.cols, src.rows);
            // 应用二值化
            threshold(src_grey, src_binary, threshold_value, 255, cv::THRESH_BINARY);
            if(!M_af.empty() && M_rot.empty())
            {
                cv::warpAffine(src_binary, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
            }
            if(M_af.empty() && !M_rot.empty() )
            {
                cv::warpAffine(src_binary, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
            }
            if(!M_af.empty() && !M_rot.empty() )
            {
                cv::warpAffine(src_binary, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
                cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
            }
            if(M_af.empty() && M_rot.empty() )
            {
                src_prossed = src_binary.clone();
            }
            // Mat为BGR必须先转化为RGB，再转化为QImage
            cv::cvtColor(src_prossed,dst,cv::COLOR_BGR2RGB);


            // 将 cv::Mat 转换为 QImage
            QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

            // 将 QImage 转换为 QPixmap
            QPixmap pixmap = QPixmap::fromImage(image);

            ui->label->setPixmap(pixmap);//显示图像
        }
    }


}

void MainWindow::on_horizontalSlider_2_actionTriggered(int action)
{
    if(isBinary) // 已完成二值化
    {
        int threshold_value = ui->horizontalSlider_2->value();

        Size dst_sz(src.cols, src.rows);
        // 应用二值化
        cout<<"threshold_value:"<<threshold_value<<endl;
        threshold(src, src_binary, threshold_value, 255, cv::THRESH_BINARY);
        if(!M_af.empty() && M_rot.empty())
        {
            cv::warpAffine(src_binary, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
        }
        if(M_af.empty() && !M_rot.empty() )
        {
            cv::warpAffine(src_binary, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
        }
        if(!M_af.empty() && !M_rot.empty() )
        {
            cv::warpAffine(src_binary, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
            cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
        }
        if(M_af.empty() && M_rot.empty() )
        {
            src_prossed = src_binary.clone();
        }
        // Mat为BGR必须先转化为RGB，再转化为QImage
        cv::cvtColor(src_prossed,dst,cv::COLOR_BGR2RGB);


        // 将 cv::Mat 转换为 QImage
        QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

        // 将 QImage 转换为 QPixmap
        QPixmap pixmap = QPixmap::fromImage(image);

        ui->label->setPixmap(pixmap);//显示图像
    }
}

/**
 * @brief MainWindow::on_label_5_linkActivated
 * @param link
 * 右图，点击切换下一张
 */
void MainWindow::on_label_5_linkActivated(const QString &link)
{

}

/**
 * @brief MainWindow::on_pushButton_5_clicked
 * 二值化阈值重置
 */
void MainWindow::on_pushButton_5_clicked()
{

}


/**
 * @brief MainWindow::on_pushButton_9_clicked
 * HSV
 */
void MainWindow::on_pushButton_9_clicked()
{
    if(!isHSV)
    {
        isHSV=1;
        cv::cvtColor(src, src, cv::COLOR_BGR2HSV);
        Size dst_sz(src.cols, src.rows);
        update_hsv_space(src);
        if(!M_af.empty() && M_rot.empty())
        {
            cv::warpAffine(src, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
        }
        if(M_af.empty() && !M_rot.empty() )
        {
            cv::warpAffine(src, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
        }
        if(!M_af.empty() && !M_rot.empty() )
        {
            cv::warpAffine(src, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
            cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
        }
        if(M_af.empty() && M_rot.empty() )
        {
            src_prossed = src.clone();
        }
        // Mat为BGR必须先转化为RGB，再转化为QImage
        cv::cvtColor(src_prossed,dst,cv::COLOR_BGR2RGB);


        // 将 cv::Mat 转换为 QImage
        QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

        // 将 QImage 转换为 QPixmap
        QPixmap pixmap = QPixmap::fromImage(image);

        ui->label->setPixmap(pixmap);//显示图像


        isGrey=1;
    }
}

/**
 * @brief update_hsv_space
 * 绘制hsv色域空间直方图
 */
void MainWindow::update_hsv_space(Mat src)
{
    //将原图进行拆分,并存入向量集合中
    vector<Mat> bgr;
    //将输入图像划分为三个通道R、G、B
    split(src, bgr);
    //设置直方图有256个区间,因为图像的灰度值变化为0~255
    int numbers = 256;
    //定义变量范围，并定义三个矩阵来存储每个直方图
    float range[] = {0, 256};
    const float *histRange = {range};
    Mat b_hist, g_hist, r_hist;
    /*
    * calcHist用来计算图像直方图：
    * 参数详解：
    const Mat* images：输入图像
     int nimages：输入图像的个数
    const int* channels：需要统计直方图的第几通道
    InputArray mask：掩膜，，计算掩膜内的直方图  ...Mat()
    OutputArray hist:输出的直方图数组
    int dims：需要统计直方图    通道的个数
    const int* histSize：指的是直方图分成多少个区间，就是 bin的个数
    const float** ranges： 统计像素值得区间
    bool uniform=true::是否对得到的直方图数组进行归一化处理
    bool accumulate=false：在多个图像时，是否累计计算像素值得个数
    */
    calcHist(&bgr[0], 1, 0, Mat(), b_hist, 1, &numbers, &histRange);
    calcHist(&bgr[1], 1, 0, Mat(), g_hist, 1, &numbers, &histRange);
    calcHist(&bgr[2], 1, 0, Mat(), r_hist, 1, &numbers, &histRange);
    //计算出每个通道的直方图后绘制直方图，并显示给用户
    int width = 512;
    int height = 320;
    //创建一个灰度图像
    Mat histImage(height, width, CV_8UC3, Scalar(235,235,235));
    //
    normalize(b_hist, b_hist, 0, height, NORM_MINMAX);
    normalize(g_hist, g_hist, 0, height, NORM_MINMAX);
    normalize(r_hist, r_hist, 0, height, NORM_MINMAX);

    int binStep = cvRound((float )width/(float )numbers);
    for(int i=0;i<numbers;i++)
    {
        try {
            line(
                    histImage,
                    Point(binStep*(i-1),height- cvRound(b_hist.at<float>(i-1))),
                    Point(binStep*(i),height- cvRound(b_hist.at<float>(i))),
                    Scalar(255,0,0)
            );
            line(
                    histImage,
                    Point(binStep*(i-1),height- cvRound(g_hist.at<float>(i-1))),
                    Point(binStep*(i),height- cvRound(g_hist.at<float>(i))),
                    Scalar(0,255,0)
            );
            line(
                    histImage,
                    Point(binStep*(i-1),height- cvRound(r_hist.at<float>(i-1))),
                    Point(binStep*(i),height- cvRound(r_hist.at<float>(i))),
                    Scalar(0,0,255)
            );
        }catch (Exception exception){
            cout << exception.err <<endl;
        }
    }
    // Mat为BGR必须先转化为RGB，再转化为QImage
    cv::cvtColor(histImage,dst,cv::COLOR_BGR2RGB);


    // 将 cv::Mat 转换为 QImage
    QImage image(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888);

    // 将 QImage 转换为 QPixmap
    QPixmap pixmap = QPixmap::fromImage(image);
    ui->label_2->setPixmap(pixmap);//显示图像
}

void MainWindow::on_pushButton_11_clicked()
{

}
