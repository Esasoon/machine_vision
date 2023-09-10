#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>




QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void InitImg();

private:
    Ui::MainWindow *ui;

private slots:

    void img_show();
//    void update_img();

    void on_pb_input_clicked();

//    void on_stop_clicked();


};
#endif // MAINWINDOW_H
