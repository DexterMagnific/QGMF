#ifndef QGMFGRAPHICS_STANDARDEDGESTYLE_H
#define QGMFGRAPHICS_STANDARDEDGESTYLE_H

#include "StandardStyle.h"

namespace QGMF {

struct QGMFGRAPHICS_PUBLIC StandardEdgeStyle {
    StandardStyle link;

    StandardEdgeStyle()
      : link(QColor(0, 85, 127, 200), QColor(), 1.5)
    {
    }

    StandardEdgeStyle(const QColor &fg, const QColor &bg)
      : link(fg,bg, 1.5)
    {}
};

}

#endif // STANDARDEDGESTYLE_H
