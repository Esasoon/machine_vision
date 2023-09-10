QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    my_img_process.cpp

HEADERS += \
    mainwindow.h \
    my_img_process.h

FORMS += \
    mainwindow.ui

#win32 {
#    INCLUDEPATH += D:\opencv\opencv\opencv_build\install\include
#                   D:\opencv\opencv\opencv_build\install\include\opencv2

#    LIBS += D:\opencv\opencv\opencv_build\lib\libopencv_*.a
#}

 lenovo
win32 {
    INCLUDEPATH += G:\cv\opencv\opencv_build\install\include
                   G:\cv\opencv\opencv_build\install\include\opencv2

    LIBS += G:\cv\opencv\opencv_build\lib\libopencv_*.a
}



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
