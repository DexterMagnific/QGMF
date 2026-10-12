#ifndef QGMFGRAPHICS_STANDARDPORTSTYLE_H
#define QGMFGRAPHICS_STANDARDPORTSTYLE_H

#include "StandardStyle.h"
#include "StandardCaptionStyle.h"

namespace QGMF {

struct QGMFGRAPHICS_PUBLIC StandardPortStyle {
    StandardStyle body;
    StandardCaptionStyle caption;

    StandardPortStyle()
      : body(QColor(0, 85, 127, 200), QColor(200, 200, 234, 50))
      , caption(body.normal.pen().color(), body.normal.brush().color())
    {
      caption.rect.normal.setPen(Qt::NoPen);
      caption.rect.hovered.setPen(Qt::NoPen);
    }

    StandardPortStyle(const QColor &fg, const QColor &bg)
      : body(fg,bg)
      , caption(fg,bg)
    {
      caption.rect.normal.setPen(Qt::NoPen);
      caption.rect.hovered.setPen(Qt::NoPen);
    }

};

}
#endif // STANDARDPORTSTYLE_H
