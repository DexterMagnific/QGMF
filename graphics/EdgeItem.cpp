#include "EdgeItem.h"

#include <QDebug>
#include <QPen>
#include <QGraphicsSceneMoveEvent>
#include <QtMath>

#include "Scene.h"
#include "Node.h"
#include "Port.h"

#include "PortItem.h"
#include "NodeItem.h"

namespace QGMF {

EdgeItem::EdgeItem(Edge *edge, Item *parent)
  : Item(parent)
  , m_edge(edge)
  , m_srcDetach(false)
  , m_dstDetach(false)
  , m_src(srcPortItem())
  , m_dst(dstPortItem())
  , m_srcPt(srcScenePoint())
  , m_dstPt(dstScenePoint())
{
  Q_ASSERT(edge);
  Q_ASSERT(nodeItem());
  Q_ASSERT(srcPortItem());
  Q_ASSERT(dstPortItem());

  edge->setGraphicsObject(this);
  setName(QString("Edge(%1.%2 -> %3.%4)")
          .arg(srcPortItem()->nodeItem()->name())
          .arg(srcPortItem()->name())
          .arg(dstPortItem()->nodeItem()->name())
          .arg(dstPortItem()->name()));

  setFlags(flags() | ItemHasNoContents | ItemSendsScenePositionChanges | ItemIsSelectable);
  //setExtFlag(ItemAnimatesPosChanges);
  setCosmetic();

  srcPortItem()->addToOutEdgeItemList(this);
  dstPortItem()->addToInEdgeItemList(this);

  nodeItem()->addToEdgeItemList(this);
  setZValue(parent->zValue()+3);

  // once the type of the edge is known, it is no longer possible
  // to change it, even if we detach it to reattach it elsewhere,
  // it keeps its type
  m_pullEdgeMdls.push_back(std::cref(edge->model()));

  updatePosition();

  srcPortItem()->updatePath();
  dstPortItem()->updatePath();

  nodeItem()->addToEdgeItemList(this);
}

EdgeItem::EdgeItem(PortItem *src, QPointF dst, Item *parent)
  : Item(parent)
  , m_edge(nullptr)
  , m_srcDetach(false)
  , m_dstDetach(true)
  , m_src(src)
  , m_dst(nullptr)
  , m_srcPt(srcScenePoint())
  , m_dstPt(dst)
{
  //Q_ASSERT(nodeItem());
  Q_ASSERT(srcPortItem());

  setName(QString("Edge(%1.%2 -> [None])")
          .arg(srcPortItem()->nodeItem()->name())
          .arg(srcPortItem()->name()));

  setFlags(flags() | ItemHasNoContents | ItemSendsScenePositionChanges | ItemIsSelectable);
  //setExtFlag(ItemAnimatesPosChanges, false);
  setCosmetic();

  // subnode is at +1
  // ports of subnode are at +2
  setZValue(parent->zValue()+3);

  // compute the list of edge models that can be pulled from src port
  m_pullEdgeMdls = m_src->port()->node()->parent()->model().outEdgeModels(m_src->port()->model());

  setPos(mapFromScene(srcPortItem()->sceneConnectionPoint(nullptr)));

  srcPortItem()->updatePath();
}

EdgeItem::~EdgeItem()
{
  //qDebug() << "WILL DELETE edge item" << name();
  srcPortItem()->removeFromOutEdgeList(this);
  if ( m_edge ) { // non temp edge
    dstPortItem()->removeFromInEdgeList(this);
    nodeItem()->removeFromEdgeItemList(this);
    m_edge->setGraphicsObject(nullptr);
    delete m_edge;
  }
  //qDebug() << "DELETED edge item" << name();
}

PortItem *EdgeItem::srcPortItem() const
{
  if ( m_edge && m_edge->src()->graphicsObject() ) {
    return static_cast<PortItem *>(m_edge->src()->graphicsObject());
  } else
    return m_src;
}

PortItem *EdgeItem::dstPortItem() const
{
  if ( m_edge && m_edge->dst()->graphicsObject() ) {
    return static_cast<PortItem *>(m_edge->dst()->graphicsObject());
  } else
    return m_dst;
}

NodeItem *EdgeItem::nodeItem() const
{
  if ( m_edge && m_edge->node() )
    return static_cast<NodeItem *>(m_edge->node()->graphicsObject());
  else
    return nullptr; // temp edges are not registered to the model instance
}

QPointF EdgeItem::srcScenePoint() const
{
  if (!m_srcDetach && srcPortItem())
    return srcPortItem()->mapToScene(srcPortItem()->connectionPoint((this)));
  else
    return m_srcPt;
}

QPointF EdgeItem::dstScenePoint() const
{
  if (!m_dstDetach && dstPortItem())
    return dstPortItem()->mapToScene(dstPortItem()->connectionPoint(this));
  else
    return m_dstPt;
}

void EdgeItem::detachFromDst()
{
  if ( isDetached() )
    return;

  m_dstPt = dstScenePoint();
  m_dstDetach = true;

  updateAll();
  grabMouse();
}

void EdgeItem::detachFromSrc()
{
  if ( isDetached() )
    return;

  m_srcPt = srcScenePoint();
  m_srcDetach = true;

  updateAll();
  grabMouse();
}

void EdgeItem::reattach()
{
  m_srcDetach = false;
  m_dstDetach = false;
  ungrabMouse();

  updateAll();
}

void EdgeItem::updatePosition()
{
  if ( srcPortItem() )
    setPos(parentItem()->mapFromScene(srcPortItem()->sceneConnectionPoint(this)));
}

void EdgeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  //qDebug() << "mouse move evt" << type();

  if ( !isDetached() ) {
    // TODO add detach code for detaching currently attached edge
    return Item::mouseMoveEvent(event);
  }

  // moving detached edge
  const qreal r = 30;
  PortItemList pl;

  // look for nearby ports
  if ( m_dstDetach ) {
    m_dstPt = event->scenePos();
    //qDebug() << "destination pt" << m_dstPt;
    pl = scene()->nearbyPorts(m_dstPt, r);
  } else if ( m_srcDetach ) {
    m_srcPt = event->scenePos();
    pl = scene()->nearbyPorts(m_srcPt, r);
  }

  updateAll();

  // set ports to interested if edge can be added
  Node *n = m_dstDetach ?
        srcPortItem()->port()->node()->parent() :
        dstPortItem()->port()->node()->parent();

  for (auto *p: pl) {
    PortItem *from = m_dstDetach ? srcPortItem() : p;
    PortItem *to   = m_dstDetach ? p : dstPortItem();

    if ( n ) {
      for (const EdgeModel &eml : m_pullEdgeMdls) {
        if ( n->canAddEdge(eml, *from->port(),*to->port()) ) {
          //p->setState(Accepting); // FIXME push/pop state
        } else {
          //p->setState(Declining);
        }
      }
    }
  }
}

void EdgeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  if ( !isDetached() )
    return Item::mouseReleaseEvent(event);

  // root node ?
  if ( !srcPortItem()->nodeItem()->parentNodeItem() ) {
    delete this;
    return;
  }

  // releasing detached edge
  const qreal r = 30;
  PortItemList l;

  // look for nearby ports
  if ( m_dstDetach ) {
    m_dstPt = event->scenePos();
    l = scene()->nearbyPorts(m_dstPt, r);
  } else if ( m_srcDetach ) {
    m_srcPt = event->scenePos();
    l = scene()->nearbyPorts(m_srcPt, r);
  }

  // look for nearest port
  PortItem *near = nullptr;
  qreal min_d = 9999999;
  for (auto *p: l) {
    qreal d = (p->scenePos()-event->scenePos()).manhattanLength();
    if ( d < min_d ) {
      min_d = d;
      near = p;
    }
  }

  if ( near ) {
    //qDebug() << "nearest port" << near->port().name().c_str();
    // see what to do
    if ( isUntyped() ) {
      // untyped -> new edge
      srcPortItem()->nodeItem()->parentNodeItem()->addEdgeItem(srcPortItem(),near, this);
    } else if ( m_srcDetach ) {
      //qDebug() << "change source port";
      if ( m_edge->changeSrc(near->port()) ) {
        m_src->removeFromOutEdgeList(this);
        near->addToOutEdgeItemList(this);
        m_src = near;
        setPos(parentItem()->mapFromScene(near->sceneConnectionPoint(this)));
        reattach();
      } else {
        reattach();
      }
    } else if ( m_dstDetach ) {
      //qDebug() << "change dest port";
      if ( m_edge->changeDst(near->port()) ) {
        //qDebug() << "change dest port OK";
        m_dst->removeFromInEdgeList(this);
        near->addToInEdgeItemList(this);
        m_dst = near;
        reattach();
      } else {
        reattach();
      }
    }
  } else {
    // no near ports ? reattach or discard
    if ( isUntyped() )
      delete this;
    else {
      reattach();
    }
  }
}

QVariant EdgeItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
  auto res = Item::itemChange(change, value);

  if ( change == ItemPositionHasChanged ) {
    // link shape/style will change when edge is moved
    //qDebug() << "edge pos changed";
    updateAll();
  }

  if ( change == ItemParentHasChanged ) {
    // we need to position ourselves at src port's connection point
    updatePosition();
  }

  return res;
}

void EdgeItem::applyStates(ItemStates &s)
{
  Item::applyStates(s);

  // FIXME lot of conflicting states.
  if ( srcPortItem() )
    srcPortItem()->setState(Toggled, s & Selected);
  if ( dstPortItem() )
    dstPortItem()->setState(Toggled, s & Selected);
}

}
