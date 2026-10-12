#ifndef QGMFGRAPHICS_STANDARDSTYLE_H
#define QGMFGRAPHICS_STANDARDSTYLE_H

#include "Style.h"

namespace QGMF {

/**
 * @brief The StandardStyle class implements styling for standard items
 * Standard items provide a different styling for each of the item states
 */
struct StandardStyle
{
    Style normal;
    Style hovered;
    Style selected;
    Style toggled;
    Style accepting;
    Style declining;
    Style disabled;

    StandardStyle() = default;

    /**
     * Builds a standard style from given foreground and background colors
     * The foreground and background colors represent the normal state
     * styling. Other state styles will be inferred.
     * The foreground is used to set up and the pen and the background to set
     * up the brush.
     * The width determines the pen width
     * If the foreground color is invalid, no pen is set
     * If the background color is invalid, no brush is set
     */
    StandardStyle(const QColor &fg, const QColor &bg, qreal width = 1.) {
      if ( fg.isValid() ) {
        normal.setPen(QPen(fg, width, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        hovered.setPen(QPen(fg.lighter(), width, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        selected.setPen(QPen(fg.darker(), width, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        toggled.setPen(QPen(fg.darker(), width, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        disabled.setPen(QColor(200,200,200, 200));
      }

      if ( bg.isValid() ) {
        normal.setBrush(bg);
        hovered.setBrush(bg.lighter());
        selected.setBrush(bg.darker(120));
        toggled.setBrush(bg.darker());
      }

      accepting = toggled;
      declining = disabled;
    }
};

}

#endif // STANDARDSTYLE_H
