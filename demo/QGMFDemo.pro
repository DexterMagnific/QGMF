TEMPLATE = app
TARGET = QGMFDemo

QT += widgets

QMAKE_CXXFLAGS += -std=c++17

INCLUDEPATH += $$PWD/../lib
INCLUDEPATH += $$PWD/../graphics

LIBS += -L$$OUT_PWD/../lib -lQGMF
LIBS += -L$$OUT_PWD/../graphics -lQGMFGraphics

SOURCES += \
  QGMFWindow.cpp \
  ctkCollapsibleButton.cpp \
  main.cpp

FORMS += \
  MainWindow.ui \
  FileSrcNodeUI.ui

HEADERS += \
  GStreamerModel.h \
  QGMFWindow.h \
  ctkCollapsibleButton.h
