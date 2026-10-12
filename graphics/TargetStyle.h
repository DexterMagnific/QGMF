#ifndef QGMFGRAPHICS_TRACKERSTYLE_H
#define QGMFGRAPHICS_TRACKERSTYLE_H

#include "QGMFGraphicsGlobals.h"

#include <QDebug>
#include <QFontDatabase>

#include "Style.h"

namespace QGMF {

struct QGMFGRAPHICS_PUBLIC TargetStyle {
    Style normal;
    Style toggled;

    TargetStyle()
    {
      QFont f(QFontDatabase::systemFont(QFontDatabase::FixedFont));
      f.setPointSize(9);
      normal.setFont(f);
    }
};

}

#endif // TRACKERSTYLE_H
