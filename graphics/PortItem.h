#ifndef QGMFGRAPHICS_PORTITEM_H
#define QGMFGRAPHICS_PORTITEM_H

#include "QGMFGraphicsGlobals.h"

#include <QGraphicsPathItem>

#include <Port.h>

#include "NodeItem.h"
#include "StandardPortStyle.h"

namespace QGMF {

class NodeItem;
class EdgeItem;
class PortItem;

using EdgeItemList = QList<EdgeItem *>;
using PortItemList = QList<PortItem *>;

/**
 * @brief Base class for port node items
 * This class is the graphical counterpart of @ref QGMF::Port,
 * it does not provide a shape by itself. Instead, subclass it
 * to attach shapes or use one of the standard port items
 */
class QGMFGRAPHICS_PUBLIC PortItem: public Item {
  public:
    PortItem(Port *port, Item *parent);
    virtual ~PortItem();

    enum { Type = Item::Type + 2 };
    int type() const override { return Type; }

    Port *port() const { return m_port; }
    NodeItem *nodeItem() const;

    const EdgeItemList &inEdgeItems() const { return m_inEdges; }
    const EdgeItemList &outEdgeItems() const { return m_outEdges; }

    /**
     * @brief connectionPoint
     * @returns the point in local coordinates to which edges should be connected
     * Default implementation returns 0,0
     */
    // TODO add edge as argument so that point depends on edge
    virtual QPointF connectionPoint(const EdgeItem *e) const;
    QPointF sceneConnectionPoint(const EdgeItem *e) const { return mapToScene(connectionPoint(e)); }

    /**
     * @brief connectionRadius
     * @returns the radius at @ref connectionPoint() from which an edge can
     * be pulled
     */
    virtual qreal connectionRadius() const { return 3.; }

    /**
     * @brief Change the orientation angle of the port
     * Default implementation stores angle and does nothing. Can be
     * reimplemented to rotate whole port or parts of it
     * @param angle
     */
    virtual void setAngle(qreal angle);
    qreal angle() const { return m_angle; }

    /**
     * @brief pullEdgeItem pull an "untyped" edge item (whose edge is Edge::null())
     * from this port in order to create a definitive edge later
     * This edge grabs mouse. When done, it must be destroyed and a new
     * edge created between the selected ports with the right model
     * A subclass or a child item must call this method to trigger the
     * creation of a new outgoing edge fro this port
     *
     * @warning the new temporary edge is not added to the underlying model
     * @returns the new temporary edge
     */
    EdgeItem *pullEdgeItem();

  protected:
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    // rotation of ports affects in/out edge connection points
    void applyRotate(const QVariant &v) override;

  private:
    friend class EdgeItem;
    friend class NodeItem;

    void addToInEdgeItemList(EdgeItem *e);
    void addToOutEdgeItemList(EdgeItem *e);
    void removeFromInEdgeList(EdgeItem *e);
    void removeFromOutEdgeList(EdgeItem *e);

    StandardPortStyle m_style;
    Port *m_port;
    qreal m_angle;
    EdgeItemList m_inEdges;
    EdgeItemList m_outEdges;
};

}

#endif // PORTITEM_H
