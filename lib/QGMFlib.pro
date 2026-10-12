TEMPLATE = lib
TARGET = QGMF

CONFIG -= qt
CONFIG += plugin c++1z

DEFINES += QGMF_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_CXXFLAGS += -std=c++17

SOURCES += \
    Edge.cpp \
    Graph.cpp \
    Node.cpp \
    Port.cpp \
    NodeModel.cpp

HEADERS += \
    BaseModel.h \
    Edge.h \
    EdgeModel.h \
    Graph.h \
    GraphModel.h \
    Instance.h \
    Node.h \
    NodeModel.h \
    Port.h \
    PortModel.h \
    QGMF.h \
    QGMFGlobals.h \
    StandardModel.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
