#ifndef QGMFGRAPHICS_STANDARDPORTITEM_H
#define QGMFGRAPHICS_STANDARDPORTITEM_H

#include "QGMFGraphicsGlobals.h"

#include "PortItem.h"

namespace QGMF {

class StandardPortShapeItem;
class StandardCaptionItem;

class QGMFGRAPHICS_PUBLIC StandardPortItem: public PortItem {
public:
    StandardPortItem(Port *port, Item *parent);
    virtual ~StandardPortItem() = default;

    QPointF connectionPoint(const EdgeItem *e) const override;
    qreal connectionRadius() const override { return 5.; }

    const StandardPortStyle &portStyle() const { return m_style; }
    void setPortStyle(const StandardPortStyle &s);
    void setPortStyle(const QColor &fg, const QColor &bg);

    void setAngle(qreal angle) override;

  private:
    // FIXME move to d pointer
    StandardPortShapeItem *m_body;
    StandardCaptionItem *m_caption;
    StandardPortStyle m_style;
};

}

#endif // STANDARDPORTITEM_H
