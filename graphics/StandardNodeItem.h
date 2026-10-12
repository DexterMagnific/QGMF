#ifndef QGMFGRAPHICS_STANDARDNODEITEM_H
#define QGMFGRAPHICS_STANDARDNODEITEM_H

#include "QGMFGraphicsGlobals.h"

#include "NodeItem.h"

namespace QGMF {

class StandardNodeBodyItem;
class StandardCaptionItem;
class StandardContentsItem;
class ScrollAreaItem;

class StandardNodeItem : public NodeItem
{
  public:
    StandardNodeItem(Node *node, Item *parent);
    virtual ~StandardNodeItem() = default;

    const StandardNodeStyle &nodeStyle() const { return m_style; }
    void setNodeStyle(const StandardNodeStyle &s);
    void setNodeStyle(const QColor &fg, const QColor &bg);

    PortPositionHint portPositionHint(const PortItem *i, const QPointF &p) const override;

    QSizeF sizeHint() override;
    void resize(const QSizeF &sz) override;
    void resizeBody(const QSizeF &sz);

    /**
     * Retuns the Item to which node contents can be added
     */
    // TODO separate content items for subgraph and user content
    Item *contentItem() const;

    // TODO add ability to set custom caption and body

  protected:
    void notifyNodeAdded(NodeItem *n) override;
    void notifyEdgeAdded(EdgeItem *e) override;
    void notifyPortAdded(PortItem *p) override;
    void notifyPortPosChanged(PortItem *p) override;

    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;
  private:
    /* Update shadow path */
    void updateShadow();

    /* Get ideal body size hint */
    QSizeF bodySizeHint() const;

    /* Get port side on body */
    Qt::Edge portSideHint(const PortItem *p) const;

    /* Body contents margins */
    QMargins m_contentsMargins;
    /* Port margins on body */
    QMargins m_portsMargins;

    // This sucks. move to private class
    StandardNodeBodyItem *m_body;
    StandardCaptionItem *m_caption;
    ScrollAreaItem *m_scrollArea;
    StandardNodeStyle m_style;
    QGraphicsPathItem *m_shadow;

    bool m_resizing;
};

}

#endif // STANDARDNODEITEM_H
