TEMPLATE = lib
TARGET = QGMFGraphics

CONFIG += plugin

QT += widgets

QMAKE_CXXFLAGS += -std=c++17

INCLUDEPATH += $$PWD/../lib

DEFINES += QGMFGRAPHICS_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    EdgeItem.h \
    GraphItem.h \
    GridStyle.h \
    Item.h \
    ItemFactory.h \
    NodeItem.h \
    PortItem.h \
    QGMFGraphics.h \
    QGMFGraphicsGlobals.h \
    RulerStyle.h \
    Scene.h \
    ScrollAreaItem.h \
    StandardCaptionItem.h \
    StandardCaptionStyle.h \
    StandardEdgeItem.h \
    StandardEdgeStyle.h \
    StandardItemFactory.h \
    StandardItems.h \
    StandardNodeItem.h \
    StandardNodeStyle.h \
    StandardPortItem.h \
    StandardPortStyle.h \
    StandardStyle.h \
    Style.h \
    View.h

SOURCES += \
    EdgeItem.cpp \
    Item.cpp \
    NodeItem.cpp \
    PortItem.cpp \
    Scene.cpp \
    ScrollAreaItem.cpp \
    StandardCaptionItem.cpp \
    StandardEdgeItem.cpp \
    StandardItemFactory.cpp \
    StandardNodeItem.cpp \
    StandardPortItem.cpp \
    Style.cpp \
    View.cpp

include(thirdparty/cpp-spline/cpp-spline.pri)

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
