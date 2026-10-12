#include "NodeItem.h"

#include <QDebug>
#include <QPen>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QMenu>

#include "StandardNodeStyle.h"
#include "PortItem.h"
#include "ItemFactory.h"
#include "Scene.h"
#include "StandardCaptionItem.h"

namespace QGMF {

NodeItem::NodeItem(Node *node, Item *parent)
  : Item(parent)
  , m_node(node)
{
  Q_ASSERT(node);

  node->setGraphicsObject(this);
  setName(QString("Node(%1)").arg(node->name().c_str()));

  setFlags(flags() | ItemSendsGeometryChanges | ItemHasNoContents | ItemIsSelectable | ItemIsMovable);
  setExtFlag(ItemPropagatesStateToChildren);
  //setExtFlag(ItemAnimatesPosChanges);
  setCosmetic();

  if ( parent )
    setZValue(parent->zValue()+1);

  if ( parentNodeItem() )
    parentNodeItem()->addToNodeItemList(this);
}

NodeItem::~NodeItem()
{
  //qDebug() << "WILL DELETE node item" << name();
  // We can't let QGraphicsItem destroy child items in a non controlled
  // manner (e.g. we can't destroy a port before destroying its in/out edges)

  // will trigger removal of incoming/outgoing edges by parent
  PortItemList pl = m_ports;
  for (PortItem *p : pl) {
    delete p;
  }

  // remove all inner nodes
  NodeItemList nl = m_nodes;
  for (NodeItem *n : nl) {
    delete n;
  }

  if ( parentNodeItem() ) {
    parentNodeItem()->removeFromNodeItemList(this);
  }

  m_node->setGraphicsObject(nullptr);

  delete m_node;
  //qDebug() << "DELETED node item" << name();
}

NodeItem *NodeItem::parentNodeItem() const
{
  Node *p = m_node->parent();
  if (!p)
    return nullptr;
  return static_cast<NodeItem *>(p->graphicsObject());
}

NodeItem *NodeItem::addNodeItem(int typeId, const QString &name)
{
  Node *n = m_node->addNode(typeId, name.toStdString());
  if ( !n )
    return nullptr;

  NodeItem *i = scene()->itemFactory()->createNodeItem(n, this);

  notifyNodeAdded(i);

  return i;
}

NodeItem *NodeItem::addNodeItem(const QString &typeName, const QString &name)
{
  Node *n = m_node->addNode(typeName.toStdString(), name.toStdString());
  if ( !n )
    return nullptr;

  NodeItem *i = scene()->itemFactory()->createNodeItem(n, this);

  notifyNodeAdded(i);

  return i;
}

NodeItem *NodeItem::addNodeItem(const NodeModel &m, const QString &name)
{
  Node *n = m_node->addNode(m, name.toStdString());
  if ( !n )
    return nullptr;

  NodeItem *i = scene()->itemFactory()->createNodeItem(n, this);

  notifyNodeAdded(i);

  return i;
}

PortItem *NodeItem::addPortItem(int typeId, const QString &name)
{
  Port *p = m_node->addPort(typeId, name.toStdString());
  if ( !p )
    return nullptr;

  PortItem *i = scene()->itemFactory()->createPortItem(p, this);

  notifyPortAdded(i);

  return i;
}

PortItem *NodeItem::addPortItem(const QString &typeName, const QString &name)
{
  Port *p = m_node->addPort(typeName.toStdString(), name.toStdString());
  if ( !p )
    return nullptr;

  PortItem *i = scene()->itemFactory()->createPortItem(p, this);

  notifyPortAdded(i);

  return i;
}

PortItem *NodeItem::addPortItem(const PortModel &m, const QString &name)
{
  Port *p = m_node->addPort(m, name.toStdString());
  if ( !p )
    return nullptr;

  PortItem *i = scene()->itemFactory()->createPortItem(p, this);

  notifyPortAdded(i);

  return i;
}

EdgeItem *NodeItem::addEdgeItem(int typeId, PortItem *src, PortItem *dst, const QString &name)
{
  Q_ASSERT(src);
  Q_ASSERT(dst);

  Edge *e = m_node->addEdge(typeId, src->port(), dst->port(), name.toStdString());
  if ( !e )
    return nullptr;

  EdgeItem *i = scene()->itemFactory()->createEdgeItem(e, this);

  notifyEdgeAdded(i);

  return i;
}

EdgeItem *NodeItem::addEdgeItem(const QString &typeName, PortItem *src, PortItem *dst, const QString &name)
{
  Q_ASSERT(src);
  Q_ASSERT(dst);

  Edge *e = m_node->addEdge(typeName.toStdString(), src->port(), dst->port(), name.toStdString());
  if ( !e )
    return nullptr;

  EdgeItem *i = scene()->itemFactory()->createEdgeItem(e, this);

  notifyEdgeAdded(i);

  return i;
}

EdgeItem *NodeItem::addEdgeItem(const EdgeModel &m, PortItem *src, PortItem *dst, const QString &name)
{
  Q_ASSERT(src);
  Q_ASSERT(dst);

  Edge *e = m_node->addEdge(m, src->port(), dst->port(), name.toStdString());
  if ( !e )
    return nullptr;

  EdgeItem *i = scene()->itemFactory()->createEdgeItem(e, this);

  notifyEdgeAdded(i);

  return i;
}

EdgeItem *NodeItem::addEdgeItem(PortItem *src, PortItem *dst, EdgeItem *tempEdge)
{
  EdgeItem *e = nullptr;
  EdgeModelList l; // list of edges that can link src to dst
  QMenu m;
  m.addSection("Choose edge type");
  QAction *a = nullptr;

  Node *n = src->nodeItem()->parentNodeItem() ?
        src->nodeItem()->parentNodeItem()->node() : nullptr;
  if ( !n )
    goto end;

  l = n->edgeModels(*src->port(),*dst->port());

  if ( l.size() == 0 )
    goto end;

#if 0
  if ( l.size() == 1 ) {
    // one possibility -> add edge
    const EdgeModel &emdl = *l.front();
    e = parentNodeItem()->addEdgeItem(emdl,src,dst,0);
    goto end;
  }
#endif

  // many possibilities: show  menu to choose
  for (const EdgeModel &eml: l) {
    a = m.addAction(eml.typeName().c_str());
    a->setData(eml.typeId());
  }
  a = m.exec(QCursor::pos());

  if ( a ) {
    src->nodeItem()->parentNodeItem()->addEdgeItem(
          a->data().toInt(),src,dst);
  }

end:
  delete tempEdge;
  return e;
}

void NodeItem::notifyNodeAdded(NodeItem *n)
{
  Q_UNUSED(n);
}

void NodeItem::notifyEdgeAdded(EdgeItem *e)
{
  Q_UNUSED(e);
}

void NodeItem::notifyPortAdded(PortItem *p)
{
  Q_UNUSED(p);
}

void NodeItem::notifyPortPosChanged(PortItem *p)
{
  Q_UNUSED(p);
}

void NodeItem::applyStates(ItemStates &s)
{
  // put this node above sibling nodes when it is selected
  if ( s & Selected ) {
    auto *p = parentNodeItem();
    if ( p ) {
      for (auto *n: p->nodeItems()) {
        if ( n != this ) {
          n->stackBefore(this);
        }
      }
    }
  }

  Item::applyStates(s);
}

QVariant NodeItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
  if ( change == ItemPositionChange ) {
    //qDebug() << "node pos change";
    if ( parentNodeItem() ) {
      // FIXME only in interactive moves
      return parentNodeItem()->nodePositionHint(this, value.toPointF());
    }
  }

  if ( change == ItemPositionHasChanged ) {
    // update edges
    for (auto *p: portItems()) {
      for (auto *e: p->inEdgeItems()) {
        e->updatePath();
      }
      for (auto *e: p->outEdgeItems()) {
        e->updatePosition();
      }
    }
  }

  return Item::itemChange(change,value);
}

void NodeItem::addToNodeItemList(NodeItem *n)
{
  m_nodes << n;
}

void NodeItem::addToEdgeItemList(EdgeItem *e)
{
  m_edges << e;
}

void NodeItem::addToPortItemList(PortItem *p)
{
  m_ports << p;
}

void NodeItem::removeFromNodeItemList(NodeItem *n)
{
  m_nodes.removeOne(n);
}

void NodeItem::removeFromEdgeItemList(EdgeItem *e)
{
  m_edges.removeOne(e);
}

void NodeItem::removeFromPortItemList(PortItem *p)
{
  m_ports.removeOne(p);
}

}
