#include "PortItem.h"

#include <QDebug>
#include <QPen>

#include "Node.h"

#include "Scene.h"
#include "NodeItem.h"
#include "EdgeItem.h"
#include "StandardCaptionItem.h"

namespace QGMF {

// ********************************************

PortItem::PortItem(Port *port, Item *parent)
  : Item(parent)
  , m_port(port)
  , m_angle(0)
{
  Q_ASSERT(port);
  Q_ASSERT(nodeItem());

  port->setGraphicsObject(this);
  setName(QString("Port(%1)").arg(port->name().c_str()));

  setFlags(flags() | ItemHasNoContents | ItemSendsGeometryChanges | ItemIsMovable | ItemIsSelectable);
  setCosmetic();

  nodeItem()->addToPortItemList(this);

  setZValue(parent->zValue()+1);
}

PortItem::~PortItem()
{
  //qDebug() << "WILL DELETE port item" << name();
  EdgeItemList el = inEdgeItems();
  for (EdgeItem *e : el) {
    delete e;
  }
  el = outEdgeItems();
  for (EdgeItem *e : el) {
    delete e;
  }
  nodeItem()->removeFromPortItemList(this);

  m_port->setGraphicsObject(nullptr);
  delete m_port;
  //qDebug() << "DELETED port item" << name();
}

NodeItem *PortItem::nodeItem() const
{
  return static_cast<NodeItem *>(m_port->node()->graphicsObject());
}

EdgeItem *PortItem::pullEdgeItem()
{
  EdgeItem *e = scene()->itemFactory()->createTemporaryEdgeItem(this,nodeItem()->parentNodeItem());
  e->grabMouse();
  return e;
}

QPointF PortItem::connectionPoint(const EdgeItem *e) const
{
  Q_UNUSED(e);
  return QPointF(0,0);
}

void PortItem::setAngle(qreal angle)
{
  m_angle = angle;
}

void PortItem::addToInEdgeItemList(EdgeItem *e)
{
  m_inEdges << e;
}

void PortItem::addToOutEdgeItemList(EdgeItem *e)
{
  m_outEdges << e;
}

void PortItem::removeFromInEdgeList(EdgeItem *e)
{
  m_inEdges.removeOne(e);
}

void PortItem::removeFromOutEdgeList(EdgeItem *e)
{
  m_outEdges.removeOne(e);
}

QVariant PortItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
  if ( change == ItemPositionChange ) {
    // ask node for port position
    // FIXME only in interactive moves
    return nodeItem()->portPositionHint(this, value.value<QPointF>()).pos;
  }

  if ( change == ItemPositionHasChanged || change == ItemParentHasChanged ) {
    // TODO start timer to reposition port according to what node suggests

    //qDebug() << "portpos changed";
    // set port angle
    //qreal angle = -nodeItem()->portPositionHint(this, value.value<QPointF>()).angle-90;
    //setAngle(angle);

    // update incident edge shapes
    for (auto *e: inEdgeItems()) {
      //qDebug() << "update edge" << e->name();
      e->updateAll();
    }

    for (auto *e: outEdgeItems()) {
      // will also update path
      //qDebug() << "update edge pos" << e->name() << e->parentItem()->mapFromScene(sceneConnectionPoint(e));
      e->updatePosition();
    }

    // tell node that port position has changed, so it can
    // adjust its shape
    nodeItem()->notifyPortPosChanged(this);
  }

  return Item::itemChange(change, value);
}

void PortItem::applyRotate(const QVariant &v)
{
  //qDebug() << "apply rotate" << v.toDouble();
  Item::applyRotate(v);

  for (auto *e: inEdgeItems())
    e->updatePath();
  for (auto *e: outEdgeItems())
    e->updatePath();
}

}
