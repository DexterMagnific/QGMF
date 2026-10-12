#ifndef QGMFGRAPHICS_STANDARDEDGEITEM_H
#define QGMFGRAPHICS_STANDARDEDGEITEM_H

#include "EdgeItem.h"
#include "StandardEdgeStyle.h"

namespace QGMF {

class StandardEdgeLinkItem;

class StandardEdgeItem : public EdgeItem
{
  public:
    StandardEdgeItem(Edge *edge, Item *parent);
    StandardEdgeItem(PortItem *src, QPointF dst, Item *parent);
    virtual ~StandardEdgeItem() = default;

    const StandardEdgeStyle &edgeStyle() const { return m_style; }
    void setEdgeStyle(const StandardEdgeStyle &s);
    void setEdgeStyle(const QColor &fg, const QColor &bg);

   protected:
#if 0
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
#endif


  private:
    // FIXME move to d pointer
    StandardEdgeLinkItem *m_link;
    StandardEdgeStyle m_style;
};

}

#endif // STANDARDEDGEITEM_H
