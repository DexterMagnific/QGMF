#ifndef QGMFGRAPHICS_STANDARDNODESTYLE_H
#define QGMFGRAPHICS_STANDARDNODESTYLE_H

#include "StandardStyle.h"
#include "StandardCaptionStyle.h"
#include "StandardPortStyle.h"

namespace QGMF {

struct QGMFGRAPHICS_PUBLIC StandardNodeStyle {
    StandardStyle body;
    StandardCaptionStyle caption;
    StandardPortStyle port;

    StandardNodeStyle()
      : body(QColor(0, 85, 127, 255), QColor(200, 200, 234, 50))
      , caption(body.normal.pen().color(), body.normal.brush().color())
      , port(body.normal.pen().color(), body.normal.brush().color())
    {}

    StandardNodeStyle(const QColor &fg, const QColor &bg)
      : body(fg,bg)
      , caption(fg,bg)
      , port(fg,bg)
    {}
};

}
#endif // STANDARDNODESTYLE_H
