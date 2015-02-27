#-------------------------------------------------
#
# Project created by QtCreator 2013-02-28T08:04:35
#
#-------------------------------------------------

QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KickAssGUI
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    preferences.cpp \
    droparea.cpp \
    asssequencemanager.cpp


HEADERS += mainwindow.h \
    gcconstants.h \
    preferences.h \
    droparea.h \
    asssequencemanager.h

FORMS += mainwindow.ui \
	preferences.ui

RESOURCES += gui_images.qrc

ICON = KickAssGUI.icns


QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_OBJECTIVE_CFLAGS_RELEASE = $$QMAKE_OBJECTIVE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO


#INCLUDEPATH += <boost_install_path>
#LIBS += -L<boost_install_path>/stage/lib

#INCLUDEPATH += /opt/local/include
#LIBS += -L/opt/local/lib

#LIBS += -lopencv_calib3d \
#-lopencv_contrib \
#-lopencv_core \
#-lopencv_features2d \
#-lopencv_flann \
#-lopencv_gpu \
#-lopencv_highgui \
#-lopencv_imgproc \
#-lopencv_legacy \
#-lopencv_ml \
#-lopencv_objdetect \
#-lopencv_video
