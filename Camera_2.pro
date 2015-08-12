#-------------------------------------------------
#
# Project created by QtCreator 2015-07-17T15:58:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CannyStill2Qt
CONFIG += resources_big
TEMPLATE = app


SOURCES += main.cpp\
        frmmain.cpp

HEADERS  += frmmain.h \
    camera_driver/Common/exampleHelper.h

FORMS    += frmmain.ui

INCLUDEPATH += C:\\Users\\User\\Lvil\\Work\\AIEA_Camera\\opencv\\build\\include

LIBS += -LC:\\Users\\User\\Lvil\\Work\\AIEA_Camera\\mybuild2\\lib\\Debug \
-lopencv_calib3d2411d \
-lopencv_contrib2411d \
-lopencv_core2411d \
-lopencv_features2d2411d \
-lopencv_flann2411d \
-lopencv_gpu2411d \
-lopencv_haartraining_engined \
-lopencv_highgui2411d \
-lopencv_imgproc2411d \
-lopencv_legacy2411d \
-lopencv_ml2411d \
-lopencv_nonfree2411d \
-lopencv_objdetect2411d \
-lopencv_ocl2411d \
-lopencv_photo2411d \
-lopencv_stitching2411d \
-lopencv_superres2411d \
-lopencv_ts2411d \
-lopencv_video2411d \
-lopencv_videostab2411d

INCLUDEPATH += C:\\Users\\User\\Lvil\\Work\\AIEA_Camera\\Qt5.4_2\\Camera_2\\camera_driver

LIBS += -LC:\\Users\\User\\Lvil\\Work\\AIEA_Camera\\Qt5.4_2\\Camera_2\\camera_driver\\lib \
-lmvDeviceManager \
-lmvDisplay \
