#ifndef QGMFGRAPHICS_NODEITEM_H
#define QGMFGRAPHICS_NODEITEM_H

#include "QGMFGraphicsGlobals.h"

#include <QGraphicsPathItem>
#include <QVariantAnimation>
#include <QList>
#include <QPointF>

#include <Node.h>

#include "Item.h"
#include "StandardNodeStyle.h"

namespace QGMF {

class PortItem;
class EdgeItem;
class NodeItem;

using NodeItemList = QList<NodeItem *>;
using PortItemList = QList<PortItem *>;
using EdgeItemList = QList<EdgeItem *>;

/**
 * @brief Base class for graphic node items
 * This class is the graphical counterpart of @ref QGMF::Node,
 * it does not provide a shape by itself. Instead, subclass it
 * to attach shapes or use one of the standard node items
 */
class QGMFGRAPHICS_PUBLIC NodeItem: public Item {
  public:
    NodeItem(Node *node, Item *parent);
    virtual ~NodeItem();

    enum { Type = Item::Type + 1 };
    int type() const override { return Type; }

    Node *node() const { return m_node; }
    NodeItem *parentNodeItem() const;

    const PortItemList &portItems() const { return m_ports; }
    const NodeItemList &nodeItems() const { return m_nodes; }
    const EdgeItemList &edgeItems() const { return m_edges; }

    unsigned nodeItemCount() const {
      Q_ASSERT(node()->nodeCount() == (unsigned) m_nodes.count());
      return m_nodes.count();
    }

    unsigned portItemCount() const {
      Q_ASSERT(node()->portCount() == (unsigned) m_ports.count());
      return m_ports.count();
    }

    unsigned edgeItemCount() const {
      Q_ASSERT(node()->edgeCount() == (unsigned) m_edges.count());
      return m_edges.count();
    }

    NodeItem *addNodeItem(int typeId, const QString &name = QString());
    NodeItem *addNodeItem(const QString &typeName, const QString &name = QString());
    NodeItem *addNodeItem(const NodeModel &m, const QString &name = QString());

    PortItem *addPortItem(int typeId, const QString &name = QString());
    PortItem *addPortItem(const QString &typeName, const QString &name = QString());
    PortItem *addPortItem(const PortModel &m, const QString &name = QString());

    EdgeItem *addEdgeItem(int typeId, PortItem *src, PortItem *dst, const QString &name = QString());
    EdgeItem *addEdgeItem(const QString &typeName, PortItem *src, PortItem *dst, const QString &name = QString());
    EdgeItem *addEdgeItem(const EdgeModel &m, PortItem *src, PortItem *dst, const QString &name = QString());

    /**
     * @brief addEdgeItem interactively offers to add an edge from src to dst by showing a context menu
     * It lets the user choose between one of the edge models that can link src to dst
     * @param src the source port
     * @param dst the destination port
     * @param tempEdge the untyped temporary edge pulled from src (see @ref PortItem::pullEdgeItem())
     * @return the newly added edge item, or null if the user cancelled the action
     * @note the temporary edge is deleted
     */
    virtual EdgeItem *addEdgeItem(PortItem *src, PortItem *dst, EdgeItem *tempEdge = nullptr);

    typedef struct {
        QPointF pos;
        qreal angle;
    } PortPositionHint;

    /** Returns the position and angle assigned by this node to the port @a p
     * given a desired position @a p
     * @a p is expressed in local coordinates
     * the default implementation returns p and zero angle
     */
    virtual PortPositionHint portPositionHint(const PortItem *i, const QPointF &p) const { Q_UNUSED(i); return {p,0}; }

    /** Returns the position assigned by this node to the subnode @a n
     * given a desired position @a p
     * @a p is expressed in local coordinates
     * the default implementation returns p
     */
    virtual QPointF nodePositionHint(const NodeItem *n, const QPointF &p) const { Q_UNUSED(n); return p; }

  protected:
    virtual void notifyNodeAdded(NodeItem *n);
    virtual void notifyEdgeAdded(EdgeItem *e);
    virtual void notifyPortAdded(PortItem *p);
    virtual void notifyPortPosChanged(PortItem *p);

    /*
    virtual void notifyNodeMoveStart(NodeItem *n, const QPointF &initialPos);
    virtual void notifyNodeMoving(NodeItem *n, const QPointF &initialPos, const QPointF &currentPos);
    virtual void notifyNodeMoveEnd(NodeItem *n, const QPointF &initiaPos, const QPointF &finalPos);
    virtual void notifyNodeRemove(NodeItem *n);
    */

    void applyStates(ItemStates &s) override;

  protected:
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

  private:
    void addToNodeItemList(NodeItem *n);
    void addToEdgeItemList(EdgeItem *e);
    void addToPortItemList(PortItem *p);

    void removeFromNodeItemList(NodeItem *n);
    void removeFromEdgeItemList(EdgeItem *e);
    void removeFromPortItemList(PortItem *p);

    friend class EdgeItem;
    friend class PortItem;

    StandardNodeStyle m_style;
    Node *m_node;

    PortItemList m_ports;
    NodeItemList m_nodes;
    EdgeItemList m_edges;
};

}

#endif // NODEITEM_H
