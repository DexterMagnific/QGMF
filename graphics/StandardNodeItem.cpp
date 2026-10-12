#include "StandardNodeItem.h"

#include <QDebug>
#include <QGraphicsSceneWheelEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "StandardCaptionItem.h"
#include "StandardPortItem.h"
#include "EdgeItem.h"
#include "ScrollAreaItem.h"

namespace QGMF {

// Classic "rectangle" node shape
class StandardNodeBodyItem: public Item {
  public:
    StandardNodeBodyItem(StandardNodeItem *n)
      : Item(n)
      , m_resizing(false)
    {
      setName(nodeItem()->name()+".Body");

      setAcceptHoverEvents(true);
      setFlags(flags() | ItemIsSelectable | ItemIsMovable);
      setExtFlags(extFlags() | ItemMouseMovesParent | ItemPropagatesStateToParent | ItemPropagatesStateToChildren);

      updateAll();
    }

    StandardNodeItem *nodeItem() const { return static_cast<StandardNodeItem *>(parentItem()); }

    virtual QPainterPath getPath() const override;
    virtual Style getStyle() const override;

  protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

  private:
    QRectF resizeCorner() const;
    QSizeF m_initialSize;
    bool m_resizing;
};

QPainterPath StandardNodeBodyItem::getPath() const
{
  QPainterPath p;

  qreal x = 0;
  qreal y = 0;
  qreal xRadius = 5;
  qreal yRadius = 5;

  qreal w = size().width()-pen().widthF();
  qreal h = size().height()-pen().widthF();

  {
    qreal ww = w / 2;
    qreal hh = h / 2;
    if (ww == 0) {
      xRadius = 0;
    } else {
      xRadius = 100 * qMin(xRadius, ww) / ww;
    }
    if (h == 0) {
      yRadius = 0;
    } else {
      yRadius = 100 * qMin(yRadius, hh) / hh;
    }
  }

  if (xRadius <= 0 || yRadius <= 0) {             // add normal rectangle
    p.addRect(x,y,w,h);
    return p;
  }

  qreal rxx2 = w*xRadius/100;
  qreal ryy2 = h*yRadius/100;

  p.moveTo(x,y);
  p.arcTo(x+w-rxx2, y, rxx2, ryy2, 90, -90);
  p.lineTo(x+w,y+h);
  p.arcTo(x, y+h-ryy2, rxx2, ryy2, 270, -90);
  p.closeSubpath();

  return p;
}

Style StandardNodeBodyItem::getStyle() const
{
  const StandardStyle &st = nodeItem()->nodeStyle().body;
  const auto s = states();

  return s & StateFlag::Enabled ?
    s & StateFlag::Accepting ? st.accepting :
    s & StateFlag::Declining ? st.declining :
    s & StateFlag::Toggled ? st.toggled :
    s & StateFlag::Selected ? st.selected :
    s & StateFlag::Hovered ? st.hovered : st.normal
                           : st.disabled;
}

void StandardNodeBodyItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  // FIXME see why this shit returns the previous buttonDownPos() intead of current
  if ( event->buttons() & Qt::LeftButton ) {
    qDebug() << "button down pos" << event->pos();
  }

  if ( event->buttons() & Qt::LeftButton && resizeCorner().contains(event->pos()) ) {
    //qDebug() << "resizeRect" << resizeRect() << "press at" << event->buttonDownPos(Qt::LeftButton);
    //qDebug() << "will resize when moved";
    setCursor(Qt::SizeFDiagCursor);
    m_resizing = true;
    m_initialSize = size();
  }

  Item::mousePressEvent(event);
}

void StandardNodeBodyItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  // TODO move resize logic to Item
  if ( m_resizing ) {
    qDebug() << "stop resize";
  }
  unsetCursor();
  m_resizing = false;
  Item::mouseReleaseEvent(event);
}

void StandardNodeBodyItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if ( m_resizing ) {
    //qDebug() << "this is a resize !";
    const QPointF d = event->scenePos()-event->buttonDownScenePos(Qt::LeftButton);
    nodeItem()->resizeBody(QSizeF(m_initialSize.width()+d.x(),m_initialSize.height()+d.y()));
    event->accept();
    return;
  }

  Item::mouseMoveEvent(event);
}

void StandardNodeBodyItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  if ( resizeCorner().contains(event->buttonDownPos(Qt::LeftButton)) ) {
    // resize to size hint
    nodeItem()->resize(nodeItem()->sizeHint());
  }
}

void StandardNodeBodyItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Item::paint(painter,option,widget);

  painter->fillRect(resizeCorner(), painter->pen().color());
}

QRectF StandardNodeBodyItem::resizeCorner() const
{
  const qreal w = size().width();
  const qreal h = size().height();
  const qreal sz = 8;

  return QRectF(w-sz,h-sz,sz,sz);
}

// ******************************************************************************

void StandardNodeItem::setNodeStyle(const StandardNodeStyle &s)
{
  m_style = s;
  if ( m_body )
    m_body->updateAll();
  if ( m_caption ) {
    m_caption->setCaptionStyle(s.caption);
  }
  for (auto *p: portItems()) {
    auto *pp = static_cast<StandardPortItem *>(p);
    pp->setPortStyle(s.port);
  }
}

void StandardNodeItem::setNodeStyle(const QColor &fg, const QColor &bg)
{
  setNodeStyle(StandardNodeStyle(fg,bg));
}

NodeItem::PortPositionHint StandardNodeItem::portPositionHint(const PortItem *i, const QPointF &p) const
{
  Q_UNUSED(i);
  Q_UNUSED(p);

  // Port side on body
  qreal angle = 0;
  Qt::Edge side = portSideHint(i);
  if ( node()->parent() ) {
    if ( side == Qt::LeftEdge ) {
      angle = 180;
    } else if ( side == Qt::RightEdge ) {
      angle = 0;
    } else {
      angle = 90;
    }
  }

  //qDebug() << i->name() << "side" << side;

  const QRectF bodyRect = mapRectFromItem(m_body, m_body->boundingRect());
  const QRectF portRect = mapRectFromItem(i, i->childrenBoundingRect());

  // distance from pos to center of bounding rect
  const QPointF dist = mapFromItem(i->parentItem(), i->pos())-portRect.center();

  // adjusted temp port position on body
  QPointF P = p;
  if ( side == Qt::LeftEdge ) {
    P.setX(bodyRect.left());
    // dont go below y=topPortMargin of body
    P.setY(qMax(P.y(),bodyRect.y()+m_portsMargins.top()));
    if ( m_resizing ) {
      // currently resizing body ? constrain also bottom
      P.setY(qMin(p.y(), bodyRect.bottom()-m_portsMargins.bottom()));
    }
  } else if ( side == Qt::RightEdge ) {
    P.setX(bodyRect.right());
    P.setY(qMax(P.y(),bodyRect.y()+m_portsMargins.top()));
    if ( m_resizing ) {
      // currently resizing body ? constrain also bottom
      P.setY(qMin(p.y(), bodyRect.bottom()-m_portsMargins.bottom()));
    }
  } else if ( side == Qt::BottomEdge ) {
    P.setY(bodyRect.bottom());
    P.setX(qMax(P.x(),bodyRect.x()+m_portsMargins.left()));
    if ( m_resizing ) {
      // currently resizing body ? constrain also right
      P.setX(qMin(P.x(),bodyRect.right()-m_portsMargins.right()));
    }
  }

  // Now make sure port at P does not overlap any existing ports. If so,
  // move P until no collisions

  // wanted port rect at position P
  QRectF newPortRect = portRect;
  newPortRect.moveCenter(P-dist);

  //qDebug() << i->name() << "wanted rect" << new_r << "center" << new_r.center();

  // avoid collisions with other ports
  auto nb = portItems().count();
  while (nb) {
    for (auto *pi: portItems()) {
      //qDebug() << "port rect" << n->name() << n->childrenBoundingRect();
      const QRectF piRect = mapRectFromItem(pi, pi->childrenBoundingRect());

      if ( (i != pi) && piRect.intersects(newPortRect) ) {
        // collision: move down or right depending on edge
        if ( side == Qt::LeftEdge || side == Qt::RightEdge ) {
          // move below colliding port
          newPortRect.moveTop(piRect.bottom());
        } else {
          // move right of colliding port
          newPortRect.moveLeft(piRect.right());
        }
        // restart collision detection from beginning once moved
        nb = portItems().count();
        break;
      } else {
        // no collision ? remove from list
        nb--;
      }
    }
  }

  return {newPortRect.center()+dist,angle};
}

QSizeF StandardNodeItem::sizeHint()
{
  const QSizeF captionSz = m_caption->childrenBoundingRect().size();
  const QSizeF bodySz = bodySizeHint();

  return QSizeF(qMax(bodySz.width(), captionSz.width()+50), bodySz.height()+captionSz.height());
}

QSizeF StandardNodeItem::bodySizeHint() const
{
  // TODO cache this value.
  // invalidate on port move/add/delete

  // ultra minimum body size
  const QSizeF minSz =
      QSizeF(m_contentsMargins.left()+m_contentsMargins.right()+10.,m_contentsMargins.top()+m_contentsMargins.bottom()+10);

  // with caption
  const QSizeF captionSz = m_caption->childrenBoundingRect().size();
  const QSizeF withCaption =
      QSizeF(captionSz.width()+50, 0); // height not part of body

  // size with ports
  QRectF withPorts(m_body->pos(),QSizeF(0,0));
  if ( m_resizing ) {
    // FIXME does not work
    // size with all ports condensed. Needed if we are potentially shrinking body
    PortItemList leftList, rightList, bottomList;
    for (auto *p : portItems()) {
      Qt::Edge side = portSideHint(p);
      if ( side == Qt::LeftEdge ) {
        leftList << p;
      } else if ( side == Qt::RightEdge ) {
        rightList << p;
      } else {
        bottomList << p;
      }
    }

    qreal left= 0, right = 0, bottom = 0;
    bottom = 0;
    for (int i=0; i<leftList.size(); i++) {
      PortItem *p = leftList.at(i);
      const QRectF pRect = p->childrenBoundingRect();
      if ( i == 0 )
        left += pRect.bottom()+m_portsMargins.top();
      else if ( i == leftList.size()-1 )
        left += pRect.top()+m_portsMargins.bottom();
      else
        left += pRect.height();
    }
    for (int i=0; i<rightList.size(); i++) {
      PortItem *p = rightList.at(i);
      const QRectF pRect = p->childrenBoundingRect();
      if ( i == 0 )
        right += pRect.bottom()+m_portsMargins.top();
      else if ( i == rightList.size()-1 )
        right += pRect.top()+m_portsMargins.bottom();
      else
        right += pRect.height();
    }
    for (int i=0; i<bottomList.size(); i++) {
      PortItem *p = bottomList.at(i);
      const QRectF pRect = p->childrenBoundingRect();
      if ( i == 0 )
        bottom += pRect.right();
      else if ( i == bottomList.size()-1 )
        bottom += pRect.left();
      else
        bottom += pRect.width();
    }
    withPorts = QRectF(m_body->pos(), QSizeF(bottom, qMax(left,right)));
  } else {
    // body size with ports at their current positions
    for (auto *p : portItems()) {
      Qt::Edge side = portSideHint(p);
      QMargins m;
      if ( side == Qt::LeftEdge || side == Qt::RightEdge ) {
        m.setTop(m_portsMargins.top());
        m.setBottom(m_portsMargins.bottom());
      } else {
        m.setLeft(m_portsMargins.left());
        m.setRight(m_portsMargins.right());
      }
      const QPointF pos = mapFromItem(p->parentItem(),p->pos());
      withPorts.setTop(qMin(withPorts.top(),pos.y()-m.top()));
      withPorts.setLeft(qMin(withPorts.left(),pos.y()-m.left()));
      withPorts.setBottom(qMax(withPorts.bottom(),pos.y()+m.bottom()));
      withPorts.setRight(qMax(withPorts.right(),pos.x()+m.right()));
    }
  }

  // body with contents
  const QSizeF withContents =
      m_scrollArea->sizeHint() +
      QSizeF(m_contentsMargins.left()+m_contentsMargins.right(),m_contentsMargins.top()+m_contentsMargins.bottom());

  // final
  return minSz.expandedTo(withCaption).expandedTo(withPorts.size()).expandedTo(withContents);
}

Qt::Edge StandardNodeItem::portSideHint(const PortItem *p) const
{
  Qt::Edge side = Qt::RightEdge;
  if ( node()->parent() ) {
    if ( node()->parent()->model().isOnlySink(p->port()->model()) ) {
      side = Qt::LeftEdge;
    } else if ( node()->parent()->model().isOnlySource(p->port()->model()) ) {
      side = Qt::RightEdge;
    } else {
      side = Qt::BottomEdge;
    }
  }

  return side;
}

void StandardNodeItem::resize(const QSizeF &sz)
{
  const QSizeF captionSz = m_caption->childrenBoundingRect().size();
  resizeBody(QSizeF(sz.width(), sz.height()-captionSz.height()));
}

void StandardNodeItem::resizeBody(const QSizeF &sz)
{
  if ( !sz.isValid() )
    return;

  m_resizing = true;

  // never below size hint
  const QSizeF newSz = sz.expandedTo(bodySizeHint());

  m_body->resize(newSz);
  m_scrollArea->resize(newSz-QSizeF(m_contentsMargins.left()+m_contentsMargins.right(),m_contentsMargins.top()+m_contentsMargins.bottom()));
  updateShadow();

  // Move ports to their position hints. Needed if we shrink
  // the body
  for (auto *p : portItems()) {
    auto pos = portPositionHint(p, p->parentItem()->mapToItem(this, p->pos()));
    p->setPos(p->parentItem()->mapFromItem(this, pos.pos));
  }

  m_resizing = false;
  // TODO inform upper node that node contents has changed
}

Item *StandardNodeItem::contentItem() const
{
  return m_scrollArea->contentItem();
}

void StandardNodeItem::notifyNodeAdded(NodeItem *n)
{
  // reparent to content item
  n->setParentItem(m_scrollArea->contentItem());
}

void StandardNodeItem::notifyEdgeAdded(EdgeItem *e)
{
  // reparent to scroll area contents
  e->setParentItem(m_scrollArea->contentItem());
}

void StandardNodeItem::notifyPortAdded(PortItem *p)
{
  auto pos = portPositionHint(p, p->parentItem()->mapToItem(this, p->pos()));
  p->setPos(p->parentItem()->mapFromItem(this, pos.pos));
  p->setAngle(pos.angle);
}

void StandardNodeItem::notifyPortPosChanged(PortItem *p)
{
  Q_UNUSED(p);

  // avoid ping pong between body resize and port pos change
  // that triggers body resize
  if ( !m_resizing ) {
    // grow body if final port position outside body
    const QSizeF bodySz = bodySizeHint();
    resizeBody(m_body->size().expandedTo(bodySz));
  } else {
    //qDebug() << "already resizing";
  }
}

QVariant StandardNodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
  if ( change == ItemSelectedHasChanged ) {
    // we are an empty item, forward selection to caption and body
    // TODO find a better way to forward selection status of empty items
    m_body->setSelected(value.toBool());
  }

  return NodeItem::itemChange(change, value);
}

void StandardNodeItem::updateShadow()
{
  QPainterPath p_body;
  QPainterPath p_caption;

  QPainterPath p1;

  if ( m_body ) {
    p_body.addPath(m_body->path());
  }
  if ( m_caption )
    p_caption.addRect(m_caption->childrenBoundingRect());

  p1 = p_caption.united(p_body);
  QPainterPath p2 = p1.translated(4,4);

  m_shadow->setPath(p2-p1);
}

// Standard Node item, with body and title
StandardNodeItem::StandardNodeItem(Node *node, Item *parent)
  : NodeItem(node, parent)
  , m_contentsMargins(20,5,20,5)
  , m_portsMargins(20,20,20,20)
  , m_resizing(false)
{
  // caption at 0,0
  m_caption = new StandardCaptionItem(this);
  m_caption->setCaptionStyle(m_style.caption);
  m_caption->setExtFlag(ItemPropagatesStateToParent);
  m_caption->setExtFlag(ItemMouseMovesParent);
  m_caption->setName(name()+".Caption");
  m_caption->setMargins(QMargins(4,5,4,5));
  m_caption->setText(this->node()->name().c_str());

  const QSizeF captionSz = m_caption->childrenBoundingRect().size();
  m_caption->moveBy(0,captionSz.height()-1);

  // body below caption
  m_body = new StandardNodeBodyItem(this);
  if ( parentNodeItem() )
    m_body->resize(qMax(150., captionSz.width()+50),100);
  else
    m_body->resize(700,300);
  m_body->moveBy(0,captionSz.height()-1);

  // scroll area on top of body
  m_scrollArea = new ScrollAreaItem(m_body);
  m_scrollArea->setName(name()+".ScrollContents");
  m_scrollArea->setPos(m_contentsMargins.left(),m_contentsMargins.top());

  // shadow below this node
  m_shadow = new QGraphicsPathItem(this);
  m_shadow->setZValue(zValue()-0.5);
  m_shadow->setPen(Qt::NoPen);
  m_shadow->setBrush(QColor(100,100,100, 100));
  m_shadow->moveBy(0,captionSz.height()-1);

  resizeBody(QSizeF(150,100));
}

}
