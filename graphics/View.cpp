#include "View.h"

#include <cmath>
#include <QDebug>
#include <QScrollBar>
#include <QFontDatabase>
#include <QWheelEvent>
#include <QGraphicsItem>
#include <QRandomGenerator>

#include "NodeItem.h"

namespace QGMF {

View::View (QGraphicsScene *scene, QWidget *parent)
  : QGraphicsView(scene,parent)
  , m_grid(true)
  , m_gridStep(100)
  , m_gridStepDiv(10)
  , m_hruler(true)
  , m_vruler(true)
  , m_rulerStep(100)
  , m_rulerStepDiv(2)
  , m_rulerSz(20)
  , m_moving(false)
{
  setCacheMode(QGraphicsView::CacheBackground);
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  setDragMode(QGraphicsView::NoDrag);
  setMouseTracking(true);

  setBackgroundBrush(QColor(Qt::white));

  QFont f = m_rulerStyle.normal.font();
  f.setPixelSize(m_rulerSz/2);
  m_rulerStyle.normal.setFont(f);
  m_vpos = verticalScrollBar()->value();
  m_hpos = horizontalScrollBar()->value();

  // redraw portion of background occupied by rulers when scrolling
  connect(verticalScrollBar(),&QScrollBar::valueChanged,
          this, [&](int pos)
  {
    // rulers are drawn as part of the background at fixed scene positions
    // notify that this part of bg is changed when scrolling
    scrollInvalidate(hRulerRect());
    scrollInvalidate(vRulerRect());

//    if ( m_hmouse )
//      scrollInvalidate(m_hmouse->sceneBoundingRect());

    m_vpos = pos;
  });

  connect(horizontalScrollBar(),&QScrollBar::valueChanged,
          this, [&](int pos)
  {
    scrollInvalidate(hRulerRect());
    scrollInvalidate(vRulerRect());

//    if ( m_vmouse )
//      scrollInvalidate(m_vmouse->sceneBoundingRect());

    m_hpos = pos;
  });

}

View::View(QWidget *parent)
  : View(nullptr, parent)
{
}

void View::scrollInvalidate(const QRectF &rect)
{
  // invalidate cached background at old+new positions
  const int dx = horizontalScrollBar()->value()-m_hpos;
  const int dy = verticalScrollBar()->value()-m_vpos;

  // WRONG must map scroll bar dx,dy to scene displacement
  QRectF r = rect;
  if ( dy > 0 ) {
    r.adjust(0,-dy,0,0);
  } else {
    r.adjust(0,0,0,-dy);
  }
  if ( dx > 0 ) {
    r.adjust(-dx,0,0,0);
  } else {
    r.adjust(0,0,-dx,0);
  }

  invalidateScene(r, QGraphicsScene::BackgroundLayer);
}

void View::drawBackground(QPainter *painter, const QRectF &rect)
{
  QGraphicsView::drawBackground(painter,rect);

  drawGrid(painter,rect);
  drawRuler(painter,rect);

#if 0
  painter->fillRect(rect, QBrush(QColor(
                                   QRandomGenerator::global()->bounded(0,255),
                                   QRandomGenerator::global()->bounded(0,255),
                                   QRandomGenerator::global()->bounded(0,255),
                                   200)));
#endif
}

void View::drawGrid(QPainter *painter, const QRectF &rect)
{
  if ( !m_grid )
    return;

  qreal x;
  qreal y;
  qreal s;

  // grid
  s = m_gridStep;
  x = s*lrintf(rect.x()/s);
  y = s*lrintf(rect.y()/s);
  painter->setPen(m_gridStyle.toggled.pen());

  while ( x < rect.right() ) {
    painter->drawLine(x,rect.top(),x,rect.bottom());
    x += s;
  }

  while ( y < rect.bottom() ) {
    painter->drawLine(rect.left(),y,rect.right(),y);
    y += s;
  }

  // grid subdivisions
  s = m_gridStep/m_gridStepDiv;
  x = s*lrintf(rect.x()/s);
  y = s*lrintf(rect.y()/s);
  painter->setPen(m_gridStyle.normal.pen());

  while ( x < rect.right() ) {
    painter->drawLine(x,rect.top(),x,rect.bottom());
    x += s;
  }

  while ( y < rect.bottom() ) {
    painter->drawLine(rect.left(),y,rect.right(),y);
    y += s;
  }

  // drawing limits
  painter->setPen(m_gridStyle.selected.pen());
  if ( rect.left() <= scene()->sceneRect().left() ) {
    painter->drawLine(scene()->sceneRect().left(),rect.top(),scene()->sceneRect().left(),rect.bottom());
  }
  if ( rect.top() <= scene()->sceneRect().top() ) {
    painter->drawLine(rect.left(),scene()->sceneRect().top(),rect.right(),scene()->sceneRect().top());
  }

  if ( rect.right() >= scene()->sceneRect().right() ) {
    painter->drawLine(scene()->sceneRect().right(),rect.top(),scene()->sceneRect().right(),rect.bottom());
  }
  if ( rect.bottom() >= scene()->sceneRect().bottom() ) {
    painter->drawLine(rect.left(),scene()->sceneRect().bottom(),rect.right(),scene()->sceneRect().bottom());
  }
}

void View::drawRuler(QPainter *painter, const QRectF &rect)
{
  painter->setPen(m_rulerStyle.normal.pen());

  if ( m_hruler ) {
    const QRectF &hr = hRulerRect();
    painter->setPen(m_rulerStyle.normal.pen());

    painter->fillRect(hr.adjusted(0,1,0,0), m_rulerStyle.normal.brush());
    painter->drawRect(hr.adjusted(0,1,0,0));

    // shadow
    painter->setPen(m_rulerStyle.normal.pen().color().lighter(200));
    painter->drawLine(hr.topLeft()+QPointF(5,0),
                      hr.topRight());

    // ticks + numbers
    painter->setPen(m_rulerStyle.normal.pen().color().darker());
    painter->setFont(m_rulerStyle.normal.font());
    qreal s = m_rulerStep;
    qreal x = s*lrintf(hr.x()/s);
    while ( x < hr.left() ) {
      x += s;
    }
    while ( x < rect.right() ) {
      painter->drawLine(x,hr.bottom(),x, hr.bottom()-m_rulerSz/2);
      painter->drawText(x+3,hr.bottom()-m_rulerSz/3, QString("%1").arg(mapToHRuler(x)));
      x += s;
    }

    // subdivisons
    s = m_rulerStep/m_rulerStepDiv;
    x = s*lrintf(hr.x()/s);
    while ( x < hr.left() ) {
      x += s;
    }
    while ( x < rect.right() ) {
      painter->drawLine(x,hr.bottom(),x, hr.bottom()-m_rulerSz/4);
      x += s;
    }
  }

  if ( m_vruler ) {
    const QRectF &vr = vRulerRect();
    painter->setPen(m_rulerStyle.normal.pen());

    painter->fillRect(vr.adjusted(1,0,0,0), m_rulerStyle.normal.brush());
    painter->drawRect(vr.adjusted(1,0,0,0));

    painter->setPen(m_rulerStyle.normal.pen().color().lighter(200));
    painter->drawLine(vr.topLeft()+QPointF(0,5),
                      vr.bottomLeft());

    // ticks+numbers
    qreal s = m_rulerStep;
    qreal y = s*lrintf(rect.y()/s);
    painter->setPen(m_rulerStyle.normal.pen());

    while ( y < vr.top() ) {
      y += s;
    }
    const QTransform t = painter->transform();
    const QFontMetricsF fm = QFontMetricsF(m_rulerStyle.normal.font());
    while ( y < rect.bottom() ) {
      painter->drawLine(vr.right(),y,vr.right()-m_rulerSz/2,y);
      qreal w = fm.horizontalAdvance(QString("%1").arg(y));
      painter->translate(vr.right()-m_rulerSz/3,y+w+3);
      painter->rotate(-90);
      painter->drawText(0,0, QString("%1").arg(mapToVRuler(y)));
      painter->setTransform(t);
      y += s;
    }

    // subdivisions
    s = m_rulerStep/m_rulerStepDiv;
    y = s*lrintf(rect.y()/s);
    painter->setPen(m_rulerStyle.normal.pen());

    while ( y < vr.top() ) {
      y += s;
    }
    while ( y < rect.bottom() ) {
      painter->drawLine(vr.right(),y,vr.right()-m_rulerSz/4,y);
      y += s;
    }
  }
}

void View::setRulerVisible(bool h, bool v)
{
  if ( h != m_hruler || v != m_vruler ) {
    m_hruler = h;
    m_vruler = v;
    scrollInvalidate(hRulerRect());
    scrollInvalidate(vRulerRect());
  }
}

void View::setGridVisible(bool val)
{
  if ( val != m_grid ) {
    m_grid = val;
    invalidateBackground();
  }
}

QRectF View::hRulerRect() const
{
  QRectF hr = mapToScene(viewport()->rect()).boundingRect();
  hr.setHeight(m_rulerSz);
  hr.adjust(m_rulerSz,0,0,0);
  return hr;
}

QRectF View::vRulerRect() const
{
  QRectF vr = mapToScene(viewport()->rect()).boundingRect();
  vr.setWidth(m_rulerSz);
  vr.adjust(0,m_rulerSz,0,0);
  return vr;
}

void View::setRulerStep(qreal val)
{
  if ( qFuzzyIsNull(val) )
    return;

  m_rulerStep = val;
  if ( m_hruler || m_vruler ) {
    scrollInvalidate(hRulerRect());
    scrollInvalidate(vRulerRect());
  }
}

void View::setRulerStepDivisor(unsigned val)
{
  if ( !val )
    return;

  m_rulerStepDiv = val;
  if ( m_hruler || m_vruler ) {
    scrollInvalidate(hRulerRect());
    scrollInvalidate(vRulerRect());
  }
}

void View::setRulerSize(qreal val)
{
  if ( qFuzzyIsNull(val) )
    return;

  m_rulerSz = val;
  if ( m_hruler || m_vruler ) {
    scrollInvalidate(hRulerRect());
    scrollInvalidate(vRulerRect());
  }
}

void View::setGridStep(qreal val)
{
  if ( qFuzzyIsNull(val) )
    return;

  m_gridStep = val;
  if ( m_grid ) {
    invalidateBackground();
  }
}

void View::setGridStepDivisor(unsigned val)
{
  if ( !val )
    return;

  m_gridStepDiv = val;
  if ( m_grid ) {
    invalidateBackground();
  }
}

void View::invalidateBackground()
{
  resetCachedContent();
}

qreal View::mapToHRuler(qreal x)
{
  return x;
}

qreal View::mapToVRuler(qreal y)
{
  return y;
}

void View::wheelEvent(QWheelEvent *e)
{
  if ( e->modifiers() != Qt::ControlModifier ||
       e->angleDelta().y() == 0 ) {
    QGraphicsView::wheelEvent(e);
    return;
  }

  const qreal delta = e->angleDelta().y() / std::abs(e->angleDelta().y());

  // scene zoom point
  const QPointF zoomPoint =
        mapToScene(viewport()->mapFromGlobal(QCursor::pos()));

  if ( delta > 0.0 )
    scale(1.2,1.2);
  else
    scale(1/1.2,1/1.2);

  const QPointF diff =
      mapToScene(viewport()->rect().center()) -
      mapToScene(viewport()->mapFromGlobal(QCursor::pos()));

  centerOn(zoomPoint + diff);

  e->accept();
}

void View::mousePressEvent(QMouseEvent *e)
{
  if ( e->button() == Qt::MiddleButton ) {
    m_moving = true;
    m_moveStart = e->pos();
    m_moveOrigCursor = viewport()->cursor();
    viewport()->setCursor(Qt::DragMoveCursor);
    e->accept();
    return;
  }

  QGraphicsView::mousePressEvent(e);
}

void View::mouseReleaseEvent(QMouseEvent *e)
{
  if ( m_moving ) {
    m_moving = false;
    viewport()->setCursor(m_moveOrigCursor);
    return QGraphicsView::mouseReleaseEvent(e);
  }

  QGraphicsView::mouseReleaseEvent(e);
}

void View::mouseMoveEvent(QMouseEvent *e)
{
  if ( m_moving ) {
    QPoint delta = e->pos()-m_moveStart;
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() +
                                    (isRightToLeft() ? delta.x() : -delta.x()));
    verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());

    m_moveStart = e->pos();
    e->accept();
    return;
  }

  QGraphicsView::mouseMoveEvent(e);
}

} // namespace
