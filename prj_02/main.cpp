#include "mainwindow.h"
#include <QApplication>

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


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    QPalette palette(w.palette());
//    palette.setColor(QPalette::Background, QColor("grey"));
//    w.setAutoFillBackground(true);
//    w.setPalette(palette);
    w.show();
    return a.exec();
}
