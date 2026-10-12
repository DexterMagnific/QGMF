#include <QDebug>
#include <QApplication>

#include "QGMFWindow.h"

int main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  QApplication a(argc, argv);
  QGMFWindow w(nullptr);
  w.show();
  return a.exec();
}
