#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>//按钮
#include <QLabel>//标签
#include <QHBoxLayout>//水平布局
#include <QVBoxLayout>//垂直布局
#include <QFileDialog>//文件对话框
#include <QDebug>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPainter>
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


protected:
    void mousePressEvent(QMouseEvent* event) override;  // 鼠标点击事件回调
    void wheelEvent(QWheelEvent* event) override;       // 鼠标滚轮事件回调
    void mouseMoveEvent(QMouseEvent* event) override;   // 鼠标移动事件回调
    void mouseReleaseEvent(QMouseEvent* event) override;// 鼠标释放事件回调
    void update_hsv_space(cv::Mat src);                 // 直方图绘制
    void update_gray_space(cv::Mat src);                 // 直方图绘制


private:
    Ui::MainWindow *ui;

    int file_location;                        //定义一个int变量，用来记录图片文件的索引
    QStringList file_list;                    //用来存放图片文件的路径和名字

    double img_scale = 1; //缩放比例
    int rot_angle = 0; // 旋转角度
    QPoint img_Pos;

    bool isDragging=0;
    QPoint lastMousePos;
    bool isGrey=0; // 灰度
    bool isBinary=0;  //二值化
    bool isHSV=0;

private slots:  // 槽函数
    void openfile(); // 打开图片文件夹
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
//    void on_pushButton_clicked(bool checked);
    void on_horizontalSlider_actionTriggered(int action);
    void on_pushButton_4_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_10_clicked();
    void on_horizontalSlider_2_actionTriggered(int action);
    void on_label_5_linkActivated(const QString &link);
    void on_pushButton_5_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_15_clicked();
};



#endif // MAINWINDOW_H
