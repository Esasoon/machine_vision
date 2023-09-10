/**
  * 简易图像浏览器
  * copyright : 岳奕松
  */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "my_img_process.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
using namespace cv;
using namespace std;

my_img_process MyImgProc;

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
    ui->label->setAlignment(Qt::AlignCenter);
//    ui->label->setScaledContents(true); // 图片自适应

    ui->label_2->setScaledContents(true); // 图片自适应
    ui->label_3->setScaledContents(true); // 图片自适应
    ui->label_4->setScaledContents(true); // 图片自适应
    ui->label_5->setScaledContents(true); // 图片自适应
    ui->label_9->setScaledContents(true); // 图片自适应

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
//void MainWindow::InitImg()
//{

//}


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
    MyImgProc.src_img = cv::imread(resultString);
    MyImgProc.proced_img = MyImgProc.src_img.clone();

    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(MyImgProc.src_img,&pixmap);
    ui->label->setPixmap(pixmap);//显示图像


    update_hsv_space(MyImgProc.src_img);
    update_gray_space(MyImgProc.src_img);

//    ui->label->setPixmap(QPixmap(file_list.at(file_location)));
//    ui->label->setPixmap(QPixmap(file_list[file_location]));

}
/**
 * @brief MainWindow::wheelEvent
 * @param event
 * 滚轮事件回调，控制图片缩放
 */
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
    }
    if(img_scale<=0.1)
    {
        img_scale=0.1;
    }
    if(img_scale >= 5) img_scale=5;
    cout<<img_scale<<endl;

    Size dst_sz(static_cast<int>(MyImgProc.src_img.cols * img_scale), static_cast<int>(MyImgProc.src_img.rows * img_scale));
////    Point center(static_cast<float>(x), static_cast<float>(y));   //static_cast<float>()强制类型转换，一定要确保数据类型正确
//    Point center(static_cast<float>(MyImgProc.proced_img.cols / 2.), static_cast<float>(MyImgProc.proced_img.rows / 2.));   //static_cast<float>()强制类型转换，一定要确保数据类型正确
//    center.x += img_Pos.x();
//    center.y += img_Pos.y();


//    // 计算新的宽度和高度
    int newWidth = static_cast<int>(MyImgProc.src_img.cols * img_scale);
    int newHeight = static_cast<int>(MyImgProc.src_img.rows * img_scale);

    // 中心点修正
//    float k_s = img_scale - 1;

//    MyImgProc.my_resize(MyImgProc.src_img,MyImgProc.proced_img,img_scale,1);
    cv::resize(MyImgProc.src_img, MyImgProc.proced_img, cv::Size(newWidth, newHeight),0, 0, cv::INTER_LINEAR);


    img_Pos=img_Pos*0; //清零

//    Mat M_af_r = (cv::Mat_<float>(2, 3) << img_scale, tan(0.01745*45), 0, 0, img_scale,0);
//    cv::warpAffine(MyImgProc.src_img, MyImgProc.proced_img, M_af_r, dst_sz,1,0, cv::INTER_NEAREST); // 平移矩阵

    MyImgProc.out_proced_img = MyImgProc.proced_img.clone();
    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(MyImgProc.out_proced_img,&pixmap);

    ui->label->setPixmap(pixmap);//显示图像
//    else ui->label->setScaledContents(true);
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

/**
 * @brief MainWindow::mouseMoveEvent
 * @param event
 * 鼠标拖拽事件回调，控制图片平移
 */
void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
//    img_Pos=img_Pos*0; //清零
    if (isDragging&&(img_scale>=1)) { // 超过视野范围
        QPoint delta = event->pos() - lastMousePos;
        img_Pos += delta;
//        ui->label->move(pos() + delta);
//        ui->label->move(10, 10);
        Size dst_sz(MyImgProc.proced_img.cols, MyImgProc.proced_img.rows);
        M_af = (cv::Mat_<float>(2, 3) << 1, 0, img_Pos.x(), 0, 1,img_Pos.y());


        cv::warpAffine(MyImgProc.proced_img, MyImgProc.out_proced_img, M_af, dst_sz,1,0); // 平移矩阵
//        if(!M_rot.empty())
//        {
//            cv::warpAffine(MyImgProc.proced_img, MyImgProc.proced_img, M_rot, dst_sz, 1,0);  //仿射变换
//        }

//        // 无宏观操作，直接copy
//        MyImgProc.out_proced_img = MyImgProc.proced_img.clone();

        QPixmap pixmap;
        MyImgProc.my_mat2pixmap(MyImgProc.out_proced_img,&pixmap);

        ui->label->setPixmap(pixmap);//显示图像
        lastMousePos = event->pos();
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

    std::string resultString = file_list[file_location].toStdString();
    src = cv::imread(resultString);
    update_hsv_space(src);
    update_gray_space(MyImgProc.src_img);

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

    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(src_prossed,&pixmap);

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
    MyImgProc.src_img = cv::imread(resultString);
    update_hsv_space(MyImgProc.src_img);
    update_gray_space(MyImgProc.src_img);

    MyImgProc.proced_img = MyImgProc.src_img.clone();
//    rot_angle=0;
//    ui->horizontalSlider->setValue(0);
//    img_scale=1;

//    Size dst_sz(src.cols, src.rows);
//    Point center(static_cast<float>(src.cols / 2.), static_cast<float>(src.rows / 2.));   //static_cast<float>()强制类型转换，一定要确保数据类型正确

//    M_rot = cv::getRotationMatrix2D(center, rot_angle, img_scale);

//    M_af = (cv::Mat_<float>(2, 3) << 1, 0, 0, 0, 1,0);
//    cv::warpAffine(src, src_prossed, M_rot, dst_sz,1,0); // 平移矩阵
//    cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：


    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(src_prossed,&pixmap);

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



    Size dst_sz(MyImgProc.proced_img.cols, MyImgProc.proced_img.rows);
    Point center(static_cast<float>(MyImgProc.proced_img.cols / 2.), static_cast<float>(MyImgProc.proced_img.rows / 2.));   //static_cast<float>()强制类型转换，一定要确保数据类型正确


    M_rot = cv::getRotationMatrix2D(center, rot_angle, 1);

    // 宏观操作
    cv::warpAffine(MyImgProc.proced_img, MyImgProc.out_proced_img, M_rot, dst_sz,1,0); // 平移矩阵
    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(MyImgProc.out_proced_img,&pixmap);

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
    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(dst,&pixmap);

    ui->label->setPixmap(pixmap);//显示图像
}

/**
 * @brief MainWindow::on_pushButton_6_clicked
 * 还原
 */
void MainWindow::on_pushButton_6_clicked()
{
    img_Pos=img_Pos*0; //清零

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
    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(src_prossed,&pixmap);

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
    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(src_prossed,&pixmap);

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
        cv::Mat src_gray;
        MyImgProc.proced_img = MyImgProc.my_rgb2gray(MyImgProc.proced_img);
        imshow("my_gray",MyImgProc.proced_img);

//        cvtColor(MyImgProc.src_img, MyImgProc.proced_img, cv::COLOR_BGR2GRAY);

//        Size dst_sz(src.cols, src.rows);

//        if(!M_af.empty() && M_rot.empty())
//        {
//            cv::warpAffine(src, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
//        }
//        if(M_af.empty() && !M_rot.empty() )
//        {
//            cv::warpAffine(src, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
//        }
//        if(!M_af.empty() && !M_rot.empty() )
//        {
//            cv::warpAffine(src, src_prossed, M_rot, dst_sz, 1,0);  //仿射变换
//            cv::warpAffine(src_prossed, src_prossed, M_af, dst_sz, 1,0);  //仿射变换：
//        }
//        if(M_af.empty() && M_rot.empty() )
//        {
//            src_prossed = src.clone();
//        }
        QPixmap pixmap;
        MyImgProc.my_mat2pixmap(MyImgProc.proced_img,&pixmap);

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
            QPixmap pixmap;
            MyImgProc.my_mat2pixmap(src_prossed,&pixmap);

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
            QPixmap pixmap;
            MyImgProc.my_mat2pixmap(src_prossed,&pixmap);

            ui->label->setPixmap(pixmap);//显示图像
        }
    }


}

/**
 * @brief MainWindow::on_horizontalSlider_2_actionTriggered
 * @param action
 * 控制二值化阈值
 */
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
        QPixmap pixmap;
        MyImgProc.my_mat2pixmap(src_prossed,&pixmap);

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
        QPixmap pixmap;
        MyImgProc.my_mat2pixmap(src_prossed,&pixmap);

        ui->label->setPixmap(pixmap);//显示图像


        isGrey=1;
    }
}

/**
 * @brief update_hsv_space
 * 绘制色域空间直方图
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

    // 归一化
    normalize(b_hist, b_hist, 0, height, NORM_MINMAX);
    normalize(g_hist, g_hist, 0, height, NORM_MINMAX);
    normalize(r_hist, r_hist, 0, height, NORM_MINMAX);

    // 打印直方图结果
//    std::cout << "r_hist : " << r_hist << std::endl;


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
    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(histImage,&pixmap);
    ui->label_2->setPixmap(pixmap);//显示图像
}


/**
 * @brief update_hsv_space
 * 绘制灰度空间直方图
 */
void MainWindow::update_gray_space(Mat src)
{
    //    cv::Mat hist;
        int histSize =256;
        // 生成灰度图
        cv::Mat gray_img = MyImgProc.my_rgb2gray(MyImgProc.src_img);
//        imshow("MyImgProc.proced_img",gray_img);
        // 直方图均衡
//        equalizeHist(gray_img,gray_img);
        MyImgProc.my_customEqualizeHist(gray_img, gray_img);

        QPixmap pixmap1;
        MyImgProc.my_mat2pixmap(gray_img,&pixmap1);
        ui->label->setPixmap(pixmap1);//显示图像

        float range[] = {0, 256};
        const float *histRange = {range};

//        imshow("MyImgProc.proced_img——equalizeHist",gray_img);
        cv::Mat hist; // = cv::Mat::zeros(1, histSize, CV_32SC1);;

        // 我的算法
//        MyImgProc.my_calcHist(gray_img,hist);

        // opencv算法
        calcHist(&gray_img, 1, 0, Mat(), hist, 1, &histSize, &histRange);


        // 归一化直方图数据，以适应画布尺寸
//        cv::normalize(hist, hist, 0, hist.rows, cv::NORM_MINMAX, -1, cv::Mat());


        // 创建直方图画布
        int histWidth = 512;
        int histHeight = 400;

        // 归一化
        normalize(hist, hist, 0, histHeight, NORM_MINMAX);


        // 创建画板
        cv::Mat histImage(histHeight, histWidth, CV_8UC3, cv::Scalar(235, 235, 235));


        int binStep = cvRound((float )histWidth/(float )histSize);
        for(int i=0;i<histSize;i++)
        {
            try {

                line(
                        histImage,
                        Point(binStep*(i),histHeight),
                        Point(binStep*(i),histHeight- cvRound(hist.at<float>(i))),
                        Scalar(22,22,22)
                );

            }catch (Exception exception){
                cout << exception.err <<endl;
            }
        }
//        imshow("histImage",histImage);
        QPixmap pixmap;
        MyImgProc.my_mat2pixmap(histImage,&pixmap);
        ui->label_9->setPixmap(pixmap);//显示图像




}



/**
 * @brief MainWindow::on_pushButton_11_clicked
 * 仿射变换（斜切）
 */
void MainWindow::on_pushButton_11_clicked()
{
    Mat M_2dtrans;
    Size size_2dtranse;

////    // 我的算法
//    MyImgProc.my_get2dMatrix(MyImgProc.src_img, 0, 45 ,M_2dtrans,size_2dtranse);
//    MyImgProc.my_warpAffine(MyImgProc.src_img, MyImgProc.proced_img, M_2dtrans,size_2dtranse,1);


//    Opencv 算法
    M_2dtrans = (cv::Mat_<double>(3, 3) << 1, tan(45*M_PI/180), 0, 0, 1,0,0,0,1);
    cv::warpPerspective(MyImgProc.src_img, MyImgProc.proced_img, M_2dtrans, MyImgProc.src_img.size());


    // 级联，两个方向的变换
//    MyImgProc.my_get2dtran(MyImgProc.proced_img, 30, 0 ,M_2dtrans,size_2dtranse);
//    MyImgProc.my_warpAffine(MyImgProc.proced_img, MyImgProc.proced_img, M_2dtrans,size_2dtranse,1);


    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(MyImgProc.proced_img,&pixmap);
    ui->label->setPixmap(pixmap);//显示图像
}

/**
 * @brief MainWindow::on_pushButton_12_clicked
 * 透视变换
 */
void MainWindow::on_pushButton_12_clicked()
{
    Mat M_3dtrans;
    Size size_3dtranse(1000,800);

    // 创建一个存储2D点的向量
    std::vector<cv::Point2f> srcPoints;
    // 向向量中添加点
    srcPoints.push_back(cv::Point2f(0.0f, 0.0f));
    srcPoints.push_back(cv::Point2f(700.0f, 0.0f));
    srcPoints.push_back(cv::Point2f(700.0f, 700.0f));
    srcPoints.push_back(cv::Point2f(0.0f, 700.0f));

    // 创建一个存储2D点的向量
    std::vector<cv::Point2f> dstPoints;
    // 向向量中添加点
    dstPoints.push_back(cv::Point2f(0.0f+100, 0.0f));
    dstPoints.push_back(cv::Point2f(700.0f-100, 0.0f+100));
    dstPoints.push_back(cv::Point2f(700.0f-100, 700.0f-100));
    dstPoints.push_back(cv::Point2f(0.0f+100, 700.0f));


    // 我的算法
    MyImgProc.my_get3dMatrix(MyImgProc.src_img, srcPoints, dstPoints,M_3dtrans);

    MyImgProc.my_warpAffine(MyImgProc.src_img, MyImgProc.proced_img, M_3dtrans,MyImgProc.src_img.size(),1);


    // opencv算法
//    M_3dtrans = cv::getPerspectiveTransform(srcPoints, dstPoints);
//    cv::warpPerspective(MyImgProc.src_img, MyImgProc.proced_img, M_3dtrans, MyImgProc.src_img.size());

    imshow("src_img",MyImgProc.src_img);
    imshow("proced_img",MyImgProc.proced_img);
    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(MyImgProc.proced_img,&pixmap);
    ui->label->setPixmap(pixmap);//显示图像


}


// 添加椒盐噪声函数
cv::Mat addSaltAndPepperNoise(const cv::Mat& image, float noiseRatio) {
    cv::Mat noisyImage = image.clone();
    int numPixels = image.rows * image.cols;

    // 计算要添加的噪声点数
    int numNoisePixels = static_cast<int>(numPixels * noiseRatio);

    for (int i = 0; i < numNoisePixels; ++i) {
        // 随机选择一个像素位置
        int x = rand() % image.cols;
        int y = rand() % image.rows;

        // 随机选择噪声类型：盐或椒
        if (rand() % 2 == 0) {
            noisyImage.at<uchar>(y, x) = 0; // 添加椒噪声（黑色）
        } else {
            noisyImage.at<uchar>(y, x) = 255; // 添加盐噪声（白色）
        }
    }

    return noisyImage;
}


/**
 * @brief MainWindow::on_pushButton_14_clicked
 * 中值滤波
 */
void MainWindow::on_pushButton_14_clicked()
{
    int windowsize = 3;
    MyImgProc.proced_img = MyImgProc.my_rgb2gray(MyImgProc.src_img); // 灰度变换

    // 添加噪声（例如椒盐噪声）
//    cv::Mat noisyImage = MyImgProc.proced_img.clone();

    // 添加椒盐噪声
    float noiseRatio = 0.05; // 噪声比例，可以根据需要调整
    cv::Mat noisyImage = addSaltAndPepperNoise(MyImgProc.proced_img, noiseRatio);

    imshow("noisyImage+noise",noisyImage);


    // 我的中值滤波
//    MyImgProc.my_medianBlur(noisyImage, MyImgProc.proced_img, windowsize);

    // opencv的中值滤波
    cv::medianBlur(noisyImage, MyImgProc.proced_img, windowsize);

    imshow("med",MyImgProc.proced_img);
    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(MyImgProc.proced_img,&pixmap);
    ui->label->setPixmap(pixmap);//显示图像
}

/**
 * @brief MainWindow::on_pushButton_13_clicked
 * 锐化
 */
void MainWindow::on_pushButton_13_clicked()
{
    MyImgProc.proced_img = MyImgProc.my_rgb2gray(MyImgProc.src_img); // 灰度变换
    // 锐化图像使用Robert算子
//    cv::Mat robertSharpened;
    MyImgProc.my_sharpen(MyImgProc.proced_img,MyImgProc.proced_img, "laplace");//"sobel","laplace","robert"

    imshow("med",MyImgProc.proced_img);

    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(MyImgProc.proced_img,&pixmap);
    ui->label->setPixmap(pixmap);//显示图像

}

/**
 * @brief MainWindow::on_pushButton_16_clicked
 * 高通
 */
void MainWindow::on_pushButton_16_clicked()
{
    MyImgProc.proced_img = MyImgProc.my_rgb2gray(MyImgProc.src_img); // 灰度变换


    // 将图像数据类型转换为32位浮点数
    cv::Mat float_image;
    MyImgProc.proced_img.convertTo(float_image, CV_32F);

    // 进行傅里叶变换
    cv::Mat f_transform;
    cv::dft(float_image, f_transform, cv::DFT_COMPLEX_OUTPUT);

    // 创建一个低通滤波器（高斯滤波器）
    int rows = MyImgProc.proced_img.rows;
    int cols = MyImgProc.proced_img.cols;
    int crow = rows / 2;
    int ccol = cols / 2;
    int D = 30; // 设置截止频率

    cv::Mat low_pass_filter = cv::Mat::zeros(rows, cols, CV_32F);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float distance = sqrt(pow(i - crow, 2) + pow(j - ccol, 2));
            if (distance <= D) {
                low_pass_filter.at<float>(i, j) = 1.0;
            }
        }
    }

    // 将滤波器应用于频域图像
    cv::Mat filtered_spectrum;
    cv::mulSpectrums(f_transform, low_pass_filter, filtered_spectrum, 0);

    // 逆傅里叶变换得到滤波后的图像
    cv::Mat filtered_image;
    cv::idft(filtered_spectrum, filtered_image, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);


    QPixmap pixmap;
    MyImgProc.my_mat2pixmap(filtered_image,&pixmap);
    ui->label->setPixmap(pixmap);//显示图像
}
// 自定义函数来执行频域图像中心化
void fftshift(cv::Mat& mag) {
    int cx = mag.cols / 2;
    int cy = mag.rows / 2;

    cv::Mat q1(mag, cv::Rect(0, 0, cx, cy));
    cv::Mat q2(mag, cv::Rect(cx, 0, cx, cy));
    cv::Mat q3(mag, cv::Rect(0, cy, cx, cy));
    cv::Mat q4(mag, cv::Rect(cx, cy, cx, cy));

    cv::Mat tmp;
    q1.copyTo(tmp);
    q4.copyTo(q1);
    tmp.copyTo(q4);

    q2.copyTo(tmp);
    q3.copyTo(q2);
    tmp.copyTo(q3);
}
/**
 * @brief MainWindow::on_pushButton_15_clicked
 * 低通
 */
void MainWindow::on_pushButton_15_clicked()
{
    cv::Mat image;// = MyImgProc.my_rgb2gray(MyImgProc.src_img); // 灰度变换
    cvtColor(MyImgProc.src_img, image, cv::COLOR_BGR2GRAY);
//    Mat image = imread("input.jpg", CV_LOAD_IMAGE_GRAYSCALE);

    if (image.empty()) {
            cout << "无法加载图像" << endl;
        }

        // 将图像转换为浮点型，以便进行傅里叶变换
        image.convertTo(image, CV_32F);

        // 进行二维傅里叶变换
        Mat fourierTransform;
        dft(image, fourierTransform, DFT_COMPLEX_OUTPUT);

        // 中心化频谱
        int centerX = fourierTransform.cols / 2;
        int centerY = fourierTransform.rows / 2;
        Mat q1(fourierTransform, Rect(0, 0, centerX, centerY));
        Mat q2(fourierTransform, Rect(centerX, 0, centerX, centerY));
        Mat q3(fourierTransform, Rect(0, centerY, centerX, centerY));
        Mat q4(fourierTransform, Rect(centerX, centerY, centerX, centerY));

        Mat tmp;
        q1.copyTo(tmp);
        q4.copyTo(q1);
        tmp.copyTo(q4);

        q2.copyTo(tmp);
        q3.copyTo(q2);
        tmp.copyTo(q3);

        // 创建低通和高通滤波器
        Mat lowPassFilter(fourierTransform.size(), CV_32FC2, Scalar(0));
        Mat highPassFilter(fourierTransform.size(), CV_32FC2, Scalar(0));

        int cutoffFreq = 80; // 设置截止频率

        // 创建圆形低通滤波器
        for (int i = 0; i < lowPassFilter.rows; i++) {
            for (int j = 0; j < lowPassFilter.cols; j++) {
                double distance = sqrt(pow(i - centerY, 2) + pow(j - centerX, 2));
                if (distance <= cutoffFreq) {
                    lowPassFilter.at<Vec2f>(i, j) = Vec2f(1, 1);
                }
            }
        }

        // 创建圆形高通滤波器
        for (int i = 0; i < highPassFilter.rows; i++) {
            for (int j = 0; j < highPassFilter.cols; j++) {
                double distance = sqrt(pow(i - centerY, 2) + pow(j - centerX, 2));
                if (distance > cutoffFreq) {
                    highPassFilter.at<Vec2f>(i, j) = Vec2f(1, 1);
                }
            }
        }

        // 应用滤波器
        Mat lowPassResult, highPassResult;
        mulSpectrums(fourierTransform, lowPassFilter, lowPassResult, 0);
        mulSpectrums(fourierTransform, highPassFilter, highPassResult, 0);

        // 反变换
        Mat lowPassImage, highPassImage;
        idft(lowPassResult, lowPassImage, DFT_SCALE | DFT_REAL_OUTPUT);
        idft(highPassResult, highPassImage, DFT_SCALE | DFT_REAL_OUTPUT);

        // 显示结果
        imshow("Original Image", image);
        imshow("Low Pass", lowPassImage);
        imshow("High Pass", highPassImage);
}
