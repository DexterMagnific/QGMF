#include "Style.h"

#include <QVariantAnimation>

namespace QGMF {

Style::operator QVariant() const
{
  QVariant v;
  v.setValue(*this);
  return v;
}

static QVariant styleInterpolate(const Style &start, const Style &end, qreal progress)
{
  Style s(start);

  // pen
  QPen p(end.pen());
  QColor c1(start.pen().color());
  QColor c2(end.pen().color());
  p.setWidthF((1-progress)*start.pen().widthF()+progress*end.pen().widthF());
  p.setColor(QColor::fromRgbF(
               (1-progress)*c1.redF()  +progress*c2.redF(),
               (1-progress)*c1.greenF()+progress*c2.greenF(),
               (1-progress)*c1.blueF() +progress*c2.blueF(),
               (1-progress)*c1.alphaF()+progress*c2.alphaF()
               ));
  s.setPen(p);

  // brush
  // NOTE only plain color brushes supported
  // TODO support gradient brushes
  QBrush b(end.brush());
  c1 = start.brush().color();
  c2 = end.brush().color();
  b.setColor(QColor::fromRgbF(
               (1-progress)*c1.redF()  +progress*c2.redF(),
               (1-progress)*c1.greenF()+progress*c2.greenF(),
               (1-progress)*c1.blueF() +progress*c2.blueF(),
               (1-progress)*c1.alphaF()+progress*c2.alphaF()
               ));
  s.setBrush(b);

  return s;
}

static struct _s {
    _s() { qRegisterAnimationInterpolator(styleInterpolate); }
} _s;

}
