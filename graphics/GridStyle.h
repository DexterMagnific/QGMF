#ifndef QGMFGRAPHICS_GRIDSTYLE_H
#define QGMFGRAPHICS_GRIDSTYLE_H

#include "QGMFGraphicsGlobals.h"

#include "Style.h"

namespace QGMF {

struct QGMFGRAPHICS_PUBLIC GridStyle {
    Style normal;
    Style toggled;
    Style selected;

    GridStyle()
    {
      normal.setPen(QColor(240,240,240, 100));
      toggled.setPen(normal.pen().color().darker(100));
      selected.setPen(QColor(255,0,0, 200));

      normal.setBrush(Qt::white);
    }
};

}

#endif // GRIDSTYLE_H
