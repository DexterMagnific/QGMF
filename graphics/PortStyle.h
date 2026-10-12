#ifndef QGMFGRAPHICS_PORTSTYLE_H
#define QGMFGRAPHICS_PORTSTYLE_H

#include "Item.h"
#include "Style.h"

namespace QGMF {

struct QGMFGRAPHICS_PUBLIC PortStyle {
    Style normal;
    Style hovered;
    Style selected;
    Style toggled;
    Style accepting;
    Style declining;
    Style disabled;

    PortStyle()
    {
      disabled.setPen(QColor(200,200,200, 200));
      normal.setPen(QColor(85, 85, 0, 200));
      hovered.setPen(QColor(255, 85, 0, 200));
      selected.setPen(QColor(255, 85, 0, 200));
      toggled.setPen(QColor(85, 0, 255, 200));

      normal.setBrush(QColor(255, 255, 234, 50));
      hovered.setBrush(normal.brush().color().darker(130));
      selected.setBrush(hovered.brush().color().darker(130));
    }

    Style styleForStates(ItemStates s) const {
      return
          s & StateFlag::Enabled ?
            s & StateFlag::Accepting ? accepting :
            s & StateFlag::Declining ? declining :
            s & StateFlag::Toggled ? toggled :
            s & StateFlag::Selected ? selected :
            s & StateFlag::Hovered ? hovered : normal
          : disabled;
    }
};

}
#endif // PortStyle_H
