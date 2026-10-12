#include "StandardEdgeItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QtMath>
#include <QDebug>
#include <QPainter>

#include "StandardPortItem.h"
#include "StandardNodeItem.h"

namespace QGMF {

class StandardEdgeLinkItem: public Item {
  public:
    StandardEdgeLinkItem(StandardEdgeItem *e)
      : Item(e)
    {
      //qDebug() << "create edge link";
      setAcceptHoverEvents(true);
      setFlags(flags() | ItemIsSelectable);
      setFlags(flags() & ~ItemIsMovable);
      setExtFlag(ItemPropagatesStateToParent);
      setExtFlag(ItemAnimatesStyleChanges, false);

      setName(edgeItem()->name()+".Link");
      connect(&m_penAnim, &QVariantAnimation::valueChanged,
              this, &StandardEdgeLinkItem::slot_updatePen);

      m_penAnim.setEndValue(0);
      m_penAnim.setEasingCurve(QEasingCurve::Linear);
      m_penAnim.setDuration(250);
      m_penAnim.setLoopCount(-1);

      updatePath();

      if ( edgeItem()->isDetached() ) {
        m_penAnim.setStartValue(6);
        m_penAnim.start();
      }
    }

    virtual ~StandardEdgeLinkItem() {}

    StandardEdgeItem *edgeItem() const { return static_cast<StandardEdgeItem *>(parentItem()); }

  protected:
    virtual QPainterPath getPath() const override;
    virtual Style getStyle() const override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void applyStyle(const QVariant &s) override;

  private slots:
    void slot_updatePen(const QVariant &v);

  private:
    QVariantAnimation m_penAnim;
};

QPainterPath StandardEdgeLinkItem::getPath() const
{
  //qDebug() << "edge link get path";
  const EdgeItem *e = edgeItem();
  const PortItem *src = e->srcPortItem();
  const PortItem *dst = e->dstPortItem();
  const QPointF srcPt = e->srcScenePoint();
  const QPointF dstPt = e->dstScenePoint();

  // assume that links go from left to right if no ports found/ports detached
  const qreal srcAngle = (src && !e->isSrcDetached()) ? qDegreesToRadians(src->angle()) : 0;
  const qreal dstAngle = (dst && !e->isDstDetached()) ? qDegreesToRadians(dst->angle()) : qDegreesToRadians(180.);

  //qDebug() << "src angle" << qRadiansToDegrees(srcAngle) << "dst angle" << qRadiansToDegrees(dstAngle);
  //qDebug() << "src pt" << srcPt << "dst pt" << dstPt;

  const QPointF srcOrient = QPointF(qCos(srcAngle),qSin(srcAngle));
  const QPointF dstOrient = QPointF(qCos(dstAngle),qSin(dstAngle));

  QPainterPath p;

#if 1
  const qreal minOffset = 100;
  const qreal maxOffset = 500;

  qreal dx = dstPt.x()-srcPt.x();
  qreal dy = dstPt.y()-srcPt.y();

  dx = qMin(minOffset, qMax(maxOffset, dx));
  dy = qMin(minOffset, qMax(maxOffset, dy));

  const QPointF c1 = mapFromScene(srcPt) + QPointF(dx*qCos(srcAngle),dy*qSin(srcAngle));
  const QPointF c2 = mapFromScene(dstPt) + QPointF(dx*qCos(dstAngle),dy*qSin(dstAngle));

  const qreal r = 3.;

  if ( e->isSrcDetached() )
    p.moveTo(mapFromScene(srcPt)+r*srcOrient);
  else
    p.addEllipse(srcOrient, r,r);

  p.cubicTo(
        c1,
        c2,
        mapFromScene(dstPt+r*dstOrient));

  if ( e->isDstDetached() )
    p.moveTo(mapFromScene(dstPt)+r*dstOrient);
  else
    p.addEllipse(mapFromScene(dstPt), r,r);
#endif

  const qreal arrowSz = 8.0;
  const qreal srcArrowPercent = 0.25;
  const qreal srcArrowAngle = p.angleAtPercent(srcArrowPercent) * M_PI / 180.0;
  const QPointF srcArrowPos = p.pointAtPercent(srcArrowPercent);

  QVector<QPointF> srcArrow;
  srcArrow.append(srcArrowPos);
  srcArrow.append(srcArrowPos - QPointF(
                    qSin(srcArrowAngle + M_PI / 2.25) * arrowSz,
                    qCos(srcArrowAngle + M_PI / 2.25) * arrowSz));
  srcArrow.append(srcArrowPos - QPointF(
                    qSin(srcArrowAngle + M_PI - M_PI / 2.25) * arrowSz,
                    qCos(srcArrowAngle + M_PI - M_PI / 2.25) * arrowSz));
  srcArrow.append(srcArrowPos);
  p.addPolygon(QPolygonF(srcArrow));

#if 1
  const qreal dstArrowPercent = 0.75;
  const qreal dstArrowAngle = p.angleAtPercent(dstArrowPercent) * M_PI / 180.0;
  const QPointF dstArrowPos = p.pointAtPercent(dstArrowPercent);

  QVector<QPointF> dstArrow;
  dstArrow.append(dstArrowPos);
  dstArrow.append(dstArrowPos - QPointF(
                 qSin(dstArrowAngle + M_PI / 2.25) * arrowSz,
                 qCos(dstArrowAngle + M_PI / 2.25) * arrowSz));
  dstArrow.append(dstArrowPos - QPointF(
                 qSin(dstArrowAngle + M_PI - M_PI / 2.25) * arrowSz,
                 qCos(dstArrowAngle + M_PI - M_PI / 2.25) * arrowSz));
  dstArrow.append(dstArrowPos);
  p.addPolygon(QPolygonF(dstArrow));
#endif

  return p;
}

Style StandardEdgeLinkItem::getStyle() const
{
  //qDebug() << "edge link get style";
  const StandardStyle &st = edgeItem()->edgeStyle().link;
  const auto s = states();

  Style sss =  s & StateFlag::Enabled ?
    s & StateFlag::Accepting ? st.accepting :
    s & StateFlag::Declining ? st.declining :
    s & StateFlag::Toggled ? st.toggled :
    s & StateFlag::Selected ? st.selected :
    s & StateFlag::Hovered ? st.hovered : st.normal
  : st.disabled;

  // make a gradient between src and dst port bodies
  const StandardPortItem *src = static_cast<StandardPortItem *>(edgeItem()->srcPortItem());
  const StandardPortItem *dst = static_cast<StandardPortItem *>(edgeItem()->dstPortItem());
  const StandardStyle &srcStyle = src ? src->portStyle().body : dst->portStyle().body;
  const StandardStyle &dstStyle = dst ? dst->portStyle().body : src->portStyle().body;

  const QColor srcColor = s & StateFlag::Enabled ?
        s & StateFlag::Accepting ? srcStyle.accepting.pen().color() :
        s & StateFlag::Declining ? srcStyle.declining.pen().color() :
        s & StateFlag::Toggled ? srcStyle.toggled.pen().color() :
        s & StateFlag::Selected ? srcStyle.selected.pen().color() :
        s & StateFlag::Hovered ? srcStyle.hovered.pen().color() : srcStyle.normal.pen().color()
      : srcStyle.disabled.pen().color();

  const QColor dstColor = s & StateFlag::Enabled ?
        s & StateFlag::Accepting ? dstStyle.accepting.pen().color() :
        s & StateFlag::Declining ? dstStyle.declining.pen().color() :
        s & StateFlag::Toggled ? dstStyle.toggled.pen().color() :
        s & StateFlag::Selected ? dstStyle.selected.pen().color() :
        s & StateFlag::Hovered ? dstStyle.hovered.pen().color() : dstStyle.normal.pen().color()
      : dstStyle.disabled.pen().color();

  //qDebug() << srcColor << dstColor;
  const QRectF b = boundingRect();
  QLinearGradient g(b.topLeft(),b.bottomRight());
  if ( src && dst ) {
    // bounding boxes do not have "orientation", so cannot tell whether position src > dst
    // the gradient might be inverted
    g.setStart(mapFromScene(src->sceneConnectionPoint(edgeItem())));
    g.setFinalStop(mapFromScene(dst->sceneConnectionPoint(edgeItem())));
  }
  g.setColorAt(0, srcColor);
  g.setColorAt(1, dstColor);

  QPen p = sss.pen();
  if ( edgeItem()->isDetached() ) {
    p.setStyle(Qt::DashLine);
    p.setDashOffset(m_penAnim.currentValue().toDouble());
  }
  p.setBrush(g);
  sss.setPen(p);
  return sss;
}

void StandardEdgeLinkItem::slot_updatePen(const QVariant &v)
{
  Q_UNUSED(v);
  updateStyle();
}

void StandardEdgeLinkItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  auto *e = edgeItem();
  //qDebug() << "mouse move";

  if ( e->isDetached() ) {
    //qDebug() << "alredy detached";
    return;
  }

  const QPointF srcPt = mapFromParent(e->srcScenePoint());
  const QPointF dstPt = mapFromParent(e->dstScenePoint());

  // check that button press is within one of the connection point radius
  const QPointF dsrc =
      event->buttonDownScenePos(Qt::LeftButton) - srcPt;
  const QPointF ddst =
      event->buttonDownScenePos(Qt::LeftButton) - dstPt;

  if ( event->buttons()  & Qt::LeftButton &&
       dsrc.manhattanLength() <= e->srcPortItem()->connectionRadius() ) {
    // detaching src
    //qDebug() << "detach src";
    e->detachFromSrc();
    return;
  }

  if ( event->buttons()  & Qt::LeftButton &&
       ddst.manhattanLength() <= e->dstPortItem()->connectionRadius() ) {
    // detaching dst
    //qDebug() << "detach dst";
    e->detachFromDst();
    return;
  }
}

void StandardEdgeLinkItem::applyStyle(const QVariant &s)
{
  // we have this here because getStyle() is const
  if ( edgeItem()->isDetached() && m_penAnim.state() != QAbstractAnimation::Running ) {
    m_penAnim.setStartValue(6);
    m_penAnim.start();
  }
  if ( !edgeItem()->isDetached() && m_penAnim.state() == QAbstractAnimation::Running ) {
    m_penAnim.stop();
    //qDebug() << "stop";
  }

  Item::applyStyle(s);
}

// //////////////////////////////////////////////////////////////////////
StandardEdgeItem::StandardEdgeItem(Edge *edge, Item *parent)
  : EdgeItem(edge, parent)
{
  setExtFlag(ItemPathUpdatesChildren);

  //qDebug() << "create edge, pos" << pos();
  m_link = new StandardEdgeLinkItem(this);
}

StandardEdgeItem::StandardEdgeItem(PortItem *src, QPointF dst, Item *parent)
  : EdgeItem(src,dst,parent)
{
  setExtFlag(ItemPathUpdatesChildren);

  //qDebug() << "create temp edge";
  m_link = new StandardEdgeLinkItem(this);
}

void StandardEdgeItem::setEdgeStyle(const StandardEdgeStyle &s)
{
  m_style = s;
  if ( m_link )
    m_link->updateAll();
}

void StandardEdgeItem::setEdgeStyle(const QColor &fg, const QColor &bg)
{
  setEdgeStyle(StandardEdgeStyle(fg,bg));
}

}
