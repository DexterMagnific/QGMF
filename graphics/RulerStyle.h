#ifndef QGMFGRAPHICS_RULERSTYLE_H
#define QGMFGRAPHICS_RULERSTYLE_H

#include "QGMFGraphicsGlobals.h"

#include "Style.h"

#include <QFontDatabase>

namespace QGMF {

struct QGMFGRAPHICS_PUBLIC RulerStyle {
    Style normal;
    Style toggled;
    Style selected;

    RulerStyle()
    {
      normal.setPen(QColor(100,100,100, 100));
      toggled.setPen(normal.pen().color().darker(110));
      selected.setPen(QColor(Qt::red));

      normal.setBrush(QColor(235, 226, 210, 100));

      normal.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    }
};

}

#endif // RULERSTYLE_H
