#ifndef QGMFGRAPHICS_EDGEITEM_H
#define QGMFGRAPHICS_EDGEITEM_H

#include "QGMFGraphicsGlobals.h"

#include <QGraphicsPathItem>

#include <Edge.h>

#include "Item.h"
#include "StandardEdgeStyle.h"

namespace QGMF {

class PortItem;
class NodeItem;
class EdgeItem;

using EdgeItemList = QList<EdgeItem *>;

/**
 * @brief Base class for edge items
 * This class is the graphical counterpart of @ref QGMF::Edge,
 * it does not provide a shape by itself. Instead, subclass it
 * to attach shapes or use one of the standard edge items
 */
class QGMFGRAPHICS_PUBLIC EdgeItem: public Item {
  public:
    EdgeItem(Edge *edge, Item *parent);

    // Temp edge pulled from src port
    // src and dst are in scene coordinates
    /**
     * Use this constructor to create an untyped edge that is pulled from
     * the given source port.
     * An untyped edge is a temporary edge that is newly pulled from its source
     * port (@see PortItem::pullEdgeItem()).
     * It has no type yet (i.e. not associated to any edge model) and no
     * destination port.
     * Once the destination port and final type is known, untyped edges are
     * destroyed and a definite edge is created with the right ends and type
     * Untyped edges do not appear in the underlying graph model
     * Untyped edges become the mouse grabber item and the mouseReleaseEvent()
     * is reimplemented to select the destination port when the mouse is
     * released
     *
     * @param src the source port from which the edge is pulled
     * @param dst the coordinates of the destination point, in scene coordinates
     * @param parent the node item to which this edge belongs
     */
    EdgeItem(PortItem *src, QPointF dst, Item *parent);

    virtual ~EdgeItem();

    enum { Type = Item::Type + 3 };
    int type() const override { return Type; }

    Edge *edge() const { return m_edge; }

    PortItem *srcPortItem() const;
    PortItem *dstPortItem() const;
    NodeItem *nodeItem() const;

    /**
     * Returns the source port's connection point, in scene coordinates.
     * If the edge has been detached from its source port, this returns the
     * current mouse position in scene coordinates
     */
    QPointF srcScenePoint() const;
    /**
     * Returns the destination port's connection point, in scene coordinates.
     * If the edge has been detached from its dest port or has no dest port
     * yet (e.g. pulled edge, see @ref pullEdgeItem()), this returns the
     * current mouse position in scene coordinates
     */
    QPointF dstScenePoint() const;

    /**
     * Marks this edge as temporarily detached from its destination port.
     * A subclass or a child widget must call this method when its detach
     * conditions are met to allow changing the destination port.
     * Note that at this point, the edge can no longer change its type
     * @note The mouse is grabbed after this call
     * @note The dstPortItem() is not changed
     */
    void detachFromDst();
    /**
     * Marks this edge as temporarily detached from its source port.
     * A subclass or a child widget must call this method when its detach
     * conditions are met to allow changing the source port.
     * @note The mouse is grabbed after this call
     * @note The srcPortItem() is not changed
     */
    void detachFromSrc();
    /**
     * Cancels a previous call to @ref detachFromSrc() or @ref detachFromDst()
     * and ungrabs the mouse
     */
    void reattach();

    /**
     * Call this method whenever the source port position has changed
     * This will allow this edge to reposition itself at the source
     * port's connection point
     */
    void updatePosition();

    /** Returns whether this edge has one of its end detached */
    bool isDetached() const { return m_srcDetach || m_dstDetach; }
    bool isSrcDetached() const { return m_srcDetach; }
    bool isDstDetached() const { return m_dstDetach; }
    /** Returns whether this edge is untyped */
    bool isUntyped() const { return m_edge == nullptr; }

  protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

  protected slots:
    void applyStates(ItemStates &s) override;

  private:
    StandardEdgeStyle m_style;
    Edge *m_edge;
    bool m_srcDetach, m_dstDetach;
    PortItem *m_src;
    PortItem *m_dst;
    QPointF m_srcPt, m_dstPt;

    // list used to store edge models when pulling an edge
    // from a source port or when detaching
    // one he edge ends
    EdgeModelList m_pullEdgeMdls;
};

}

#endif // EDGEITEM_H
