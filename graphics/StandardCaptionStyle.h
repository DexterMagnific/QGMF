#ifndef QGMFGRAPHICS_CAPTIONSTYLE_H
#define QGMFGRAPHICS_CAPTIONSTYLE_H

#include "StandardStyle.h"

namespace QGMF {

struct QGMFGRAPHICS_PUBLIC StandardCaptionStyle {
  public:
    StandardStyle text, rect;

    StandardCaptionStyle()
      : text(QColor(), QColor(Qt::white))
      , rect(QColor(0, 85, 127, 200).darker(), QColor(0, 85, 127, 200))
    {
      rect.selected.setBrush(rect.selected.pen().color());
      // we use darker() for background, so white should be ok
      text.selected.setBrush(Qt::white);

      rect.toggled = rect.selected;
      text.toggled = text.selected;
    }

    StandardCaptionStyle(const QColor &fg, const QColor &bg)
      : text(QColor(), fg)
      , rect(fg, bg)
    {
      rect.selected.setBrush(rect.selected.pen().color());
      text.selected.setBrush(Qt::white);

      rect.toggled = rect.selected;
      text.toggled = text.selected;
    }
};

}
#endif // NODESTYLE_H
