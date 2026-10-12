#ifndef QGMFGRAPHICS_DEMO_H
#define QGMFGRAPHICS_DEMO_H

#include <QMainWindow>

#include "ui_MainWindow.h"

#include "GStreamerModel.h"

class QGMFWindow : public QMainWindow, public Ui::MainWindow
{
  public:
    explicit QGMFWindow(QWidget *parent = nullptr);
    ~QGMFWindow();
};

#endif // QGMFDEMO_H
