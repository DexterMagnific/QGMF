#ifndef QGMFGRAPHICS_EDGESTYLE_H
#define QGMFGRAPHICS_EDGESTYLE_H

#include "QGMFGraphicsGlobals.h"

#include "Style.h"

namespace QGMF {

struct QGMFGRAPHICS_PUBLIC EdgeStyle {
    Style normal;
    Style hovered;
    Style selected;
    Style toggled;
    Style accepting;
    Style declining;
    Style disabled;

    EdgeStyle()
    {
      disabled.setPen(QColor(200,200,200, 200));
      normal.setPen(QColor(170, 0, 127, 200));
      hovered.setPen(QColor(255, 85, 0, 200));
      selected.setPen(QColor(255, 85, 0, 200));
      toggled.setPen(QColor(255,50,0, 200));
    }
};

}

#endif // EDGESTYLE_H
