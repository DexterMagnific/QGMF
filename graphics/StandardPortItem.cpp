#include "StandardPortItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QtMath>

#include "Scene.h"
#include "StandardCaptionItem.h"
#include "StandardNodeItem.h"

namespace QGMF {

class StandardPortShapeItem: public Item {
  public:
    StandardPortShapeItem(StandardPortItem *p)
      : Item(p)
    {
      setName(portItem()->name()+".Body");

      setAcceptHoverEvents(true);
      setFlags(flags() | ItemIsSelectable | ItemIsMovable);
      setExtFlags(extFlags() | ItemPropagatesStateToParent | ItemMouseMovesParent);
    }

    StandardPortItem *portItem() const { return static_cast<StandardPortItem *>(parentItem()); }

    QPointF connectionPoint(const EdgeItem *e) const;

  protected:
    virtual QPainterPath getPath() const override;
    virtual Style getStyle() const override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    // rotation of body affects edge connection point
    void applyRotate(const QVariant &v) override;

  private:
    // port radius
    const qreal R = 5.;
    // connection point radius
    const qreal r = 3.;
    // port length
    const qreal l = 20.;
};

void StandardPortShapeItem::applyRotate(const QVariant &v)
{
  Item::applyRotate(v);

  auto p = portItem();

  for (auto *e: p->inEdgeItems())
    e->updatePath();
  for (auto *e: p->outEdgeItems())
    e->updatePath();
}

QPointF StandardPortShapeItem::connectionPoint(const EdgeItem *e) const
{
  auto p = portItem();
  int idx = -1;

  // search in out edge items
  for (int i = 0; i< p->outEdgeItems().size(); i++) {
    if ( p->outEdgeItems().at(i) == e ) {
      idx = i;
      break;
    }
  }

  // not found, search in in edge items
  if ( idx == -1 ) {
    for (int i = 0; i< p->inEdgeItems().size(); i++) {
      if ( p->inEdgeItems().at(i) == e ) {
        idx = p->outEdgeItems().size()+i;
        break;
      }
    }
  }

  // still not found ? take last
  if ( idx == -1 ) {
    idx = p->outEdgeItems().size()+p->inEdgeItems().size();
  }

  // keep consistent facing directions between src and dst
  const auto angle = p->angle();
  int dir = qCos(qDegreesToRadians(angle)) <= 0 ? -1 : 1;

  return QPointF(l/2+r,0)+idx*dir*QPointF(0,l/4+2*r);
}

QPainterPath StandardPortShapeItem::getPath() const
{
  QPainterPath p;
  QRectF rect(0,0,R,R);
  rect.moveCenter(QPointF(-l/4-R,0));
  p.addRect(rect);
  //p.addEllipse(QPointF(-l/2-R,0),R,R);

  p.moveTo(QPointF(-l/4-R/2,0));
  // keep consistent facing directions between src and dst
  const auto angle = portItem()->angle();
  int dir = qCos(qDegreesToRadians(angle)) <= 0 ? -1 : 1;

  // first slot always here
  p.lineTo(l/2,0);
  p.addEllipse(QPointF(l/2+r,0), r,r);
  p.moveTo(QPointF(l/2+r,dir*r));

  QPointF start = p.currentPosition();

  // others if connected
  // +1 : empty slot for pulling new edge
  auto nb = portItem()->outEdgeItems().size() + portItem()->inEdgeItems().size() + 1;

  for (int i=1; i<nb; i++) {
    p.lineTo(start.x(), start.y()+dir*l/4);
    p.addEllipse(QPointF(start.x(), start.y()+dir*(l/4+r)), r,r);
    p.moveTo(QPointF(start.x(), start.y()+dir*(l/4+2*r)));

    start = start+QPointF(0,dir*(l/4+2*r));
  }

  return p;
}

// FIXME this should be part of PortItem so that it is not reimplemented
// in every model
void StandardPortShapeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  //return Item::mouseMoveEvent(event);

  PortItem *i = portItem();
  // check that button press is within pull point radius
  const QPointF d =
      event->buttonDownPos(Qt::LeftButton) -
      mapFromParent(i->connectionPoint(nullptr));
  // check that we moved some distance before triggering the creation of an edge
//  const QPointF D =
//      event->lastPos()-event->pos();

  if ( event->buttons()  & Qt::LeftButton &&
       d.manhattanLength() <= i->connectionRadius()
      ) {
    //qDebug() << "********* pull edge";
    i->pullEdgeItem();
  } else
    Item::mouseMoveEvent(event);
}

Style StandardPortShapeItem::getStyle() const
{
  const StandardStyle &st = portItem()->portStyle().body;
  const auto s = states();

  return s & StateFlag::Enabled ?
    s & StateFlag::Accepting ? st.accepting :
    s & StateFlag::Declining ? st.declining :
    s & StateFlag::Toggled ? st.toggled :
    s & StateFlag::Selected ? st.selected :
    s & StateFlag::Hovered ? st.hovered : st.normal
  : st.disabled;
}

// ///////////////////////////////////////////////

StandardPortItem::StandardPortItem(Port *port, Item *parent)
  : PortItem(port, parent)
{
  StandardNodeItem *n = static_cast<StandardNodeItem *>(nodeItem());

  m_body = new StandardPortShapeItem(this);

  m_caption = new StandardCaptionItem(this);
  m_caption->setFlag(ItemIsMovable, true);
  m_caption->setExtFlag(ItemMouseMovesParent, true);
  m_caption->setText(port->name().c_str());
  m_caption->setName(QString(this->port()->name().c_str())+" port caption");

  setPortStyle(n->nodeStyle().port);

  const QRectF r2 = m_body->boundingRect();

#if 1
  // caption at top right side of port shape (when angle=0)
  m_caption->setPos(m_body->mapToItem(this, QPointF(r2.center().y()+3,r2.top()-3)));
#else
  // caption at left side of port shape
  captionItem()->setPos(
        mainShape()->mapToItem(this, QPointF(r2.left()-5,0)).x()-r1.width(),
        r1.height()/2
        );
#endif
}

QPointF StandardPortItem::connectionPoint(const EdgeItem *e) const
{
  return mapFromItem(m_body, m_body->connectionPoint(e));
}

void StandardPortItem::setPortStyle(const StandardPortStyle &s)
{
  m_style = s;
  if ( m_body )
    m_body->updateAll();
  if ( m_caption ) {
    m_caption->setCaptionStyle(s.caption);
    m_caption->updateAll();
  }
}

void StandardPortItem::setPortStyle(const QColor &fg, const QColor &bg)
{
  setPortStyle(StandardPortStyle(fg,bg));
}

void StandardPortItem::setAngle(qreal angle)
{
  if ( !m_body )
    return;
  //qDebug() << "****************** set angle" << angle;

  m_body->setRotation(angle);

  m_caption->resetTransform();
  QTransform t;
  const QPointF o = m_caption->pos();
  const QSizeF sz = m_caption->childrenBoundingRect().size();

  //qDebug() << o << sz;

  // rotate
  t.translate(-o.x(),-o.y());
  if ( qCos(qDegreesToRadians(angle)) < 0 ) {
    // avoid flipped text: act as if the text was located on the other side
    t.translate(-2*o.x()-sz.width(),0);
    // then scale to -1,-1 to mirror
    t.scale(-1,-1);
    // now rotate: will get a text on the right side
    t.rotate(angle);
  } else {
    t.rotate(angle);
  }
  t.translate(o.x(),o.y());
  m_caption->setTransform(t);

  PortItem::setAngle(angle);
}

}
