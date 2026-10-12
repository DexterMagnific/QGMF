#ifndef QGMFGRAPHICS_BASESTYLE_H
#define QGMFGRAPHICS_BASESTYLE_H

#include "QGMFGraphicsGlobals.h"

#include <QPen>
#include <QBrush>
#include <QFont>
#include <QVariant>

namespace QGMF {

/**
 * @brief Item styling.
 * The pen is for the outline, the brush is for the fill, the font
 * is for the text
 */
class QGMFGRAPHICS_PUBLIC Style
{
  public:
    Style()
      : m_pen(Qt::NoPen)
      , m_brush(Qt::NoBrush)
    { }
    Style(const QPen &p)
      : m_pen(p)
      , m_brush(Qt::NoBrush)
    {
      m_pen.setCapStyle(Qt::SquareCap);
      m_pen.setJoinStyle(Qt::MiterJoin);
    }
    Style(const QBrush &b)
      : m_pen(Qt::NoPen)
      , m_brush(b)
    {}
    Style(const QFont &f)
      : m_pen(Qt::NoPen)
      , m_brush(Qt::NoBrush)
      , m_font(f)
    {}
    Style(const QPen &p, const QBrush &b)
      : m_pen(p), m_brush(b)
    {
      m_pen.setCapStyle(Qt::SquareCap);
      m_pen.setJoinStyle(Qt::MiterJoin);
    }
    Style(const QPen &p, const QBrush &b, const QFont &f)
      : m_pen(p), m_brush(b), m_font(f)
    {
      m_pen.setCapStyle(Qt::SquareCap);
      m_pen.setJoinStyle(Qt::MiterJoin);
    }
    Style(const QColor &fg, const QColor &bg, qreal width = 1.) {
      if ( fg.isValid() ) {
        m_pen = QPen(fg, width, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
      }
      if (bg.isValid()) {
        m_brush = QBrush(bg);
      }
    }

    ~Style() = default;

    void setPen(const QPen &p) { m_pen = p; }
    void setBrush(const QBrush &b) { m_brush = b; }
    void setFont(const QFont &f) { m_font = f; }

    QPen pen() const { return m_pen; }
    QBrush brush() const { return m_brush; }
    QFont font() const { return m_font; }

    operator QVariant() const;

  private:
    // These are all implicitly shared classes, so shouldn't harm
    // having a style for each object
    QPen m_pen;
    QBrush m_brush;
    QFont m_font;
};

}

// Allow QVariant to hold a Style. Needed for QVariantAnimation
Q_DECLARE_METATYPE(QGMF::Style);

#endif // BASESTYLE_H
