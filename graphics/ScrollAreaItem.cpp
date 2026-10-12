#include "ScrollAreaItem.h"

#include <QDebug>
#include <QPen>
#include <QGraphicsSceneWheelEvent>

namespace QGMF {

class ContentItem : public Item {
  public:
    ContentItem(QGraphicsItem *parent)
      : Item(parent)
    {}
    virtual ~ContentItem() = default;

    ScrollAreaItem *scrollArea() { return static_cast<ScrollAreaItem *>(parentItem()); }

  protected:
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;
};

QVariant ContentItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
  if ( change == ItemChildAddedChange ) {
    QGraphicsItem * child = qvariant_cast<QGraphicsItem *>(value);
    if ( child->type() >= Item::Type ) {
      Item *childItem = static_cast<Item *>(child);
      childItem->m_scrollArea = scrollArea();
      //qDebug() << childItem->name() << "added to scroll area";
    }
    // we need to know about geometry changes to invalidate scroll area
    child->setFlag(ItemSendsGeometryChanges);
    scrollArea()->invalidate();
  }

  if ( change == ItemChildRemovedChange ) {
    QGraphicsItem * child = qvariant_cast<QGraphicsItem *>(value);
    if ( child->type() >= Item::Type ) {
      Item *childItem = static_cast<Item *>(child);
      childItem->m_scrollArea = nullptr;
    }
    scrollArea()->invalidate();
  }

  return Item::itemChange(change, value);
}

// *****************************************************************************


ScrollAreaItem::ScrollAreaItem(QGraphicsItem *parent)
  : Item(parent)
  , m_dirtyChildrenRect(true)
  , m_content(new ContentItem(this))
  , m_dx(0)
  , m_dy(0)
{
  //setExtFlags(extFlags() | ItemPropagatesStateToChildren);
  setFlags(flags() | ItemClipsChildrenToShape);

  //m_content->QGraphicsItem::setScale(0.75);
  m_content->setExtFlags(m_content->extFlags() | ItemPropagatesStateToChildren);

  setPen(QPen(QBrush(Qt::gray), 0.5, Qt::DashLine));
  //setBrush(QColor(0,0,0, 10));

  Item::resize(1000,500);
}

void ScrollAreaItem::scrollBy(qreal dx, qreal dy)
{
  //qDebug() << "children bounding rect" << m_childrenRect;
  //qDebug() << "clip size" << size();
  //qDebug() << "request delta" << dx << dy;

  calcChildrenBoundingRect();
  const QRectF clip = boundingRect();
  const QRectF content = m_childrenRect;

  qreal left = 0, right = 0, top = 0, bottom = 0;

  // displacement constraints
  if ( content.left() < clip.left() ) {
    left = content.left()-clip.left();
  }
  if ( content.right() > clip.right() ) {
    right = content.right()-clip.right();
  }

  if ( content.top() < clip.top() ) {
    top = content.top()-clip.top();
  }
  if ( content.bottom() > clip.bottom() ) {
    bottom = content.bottom()-clip.bottom();
  }

  //qDebug() << "contraints" << "left" << left << "right" << right << "top" << top << "bottom" << bottom;

  qreal new_dx = m_dx+dx;
  qreal new_dy = m_dy+dy;

  //qDebug() << "new_dx" << new_dx << "new_dy" << new_dy;

  new_dy = qMax(-bottom, new_dy);
  new_dy = qMin(-top, new_dy);
  new_dx = qMax(-right, new_dx);
  new_dx = qMin(-left, new_dx);

  qreal x = new_dx-m_dx;
  qreal y = new_dy-m_dy;

  //qDebug() << "clip to delta" << x << y;

  m_dx += x;
  m_dy += y;

  //qDebug() << "final total" << m_dx << m_dy;

  if ( !qFuzzyIsNull(x) || !qFuzzyIsNull(y) ) {
    QTransform m;
    m.translate(x,y);
    m_content->QGraphicsItem::setTransform(m, true);
  }
}

void ScrollAreaItem::invalidate()
{
  m_dirtyChildrenRect = true;
  //calcChildrenBoundingRect();
  //updatePath();
}

QPainterPath ScrollAreaItem::getPath() const
{
  const QRectF r(0,0, size().width(), size().height());
  QPainterPath p;
  p.addRect(r);
  return p;
}

QSizeF ScrollAreaItem::sizeHint()
{
  //qDebug() << name() << "size hint" << m_childrenRect;
  calcChildrenBoundingRect();
  QRectF r(m_childrenRect);
  r.setTopLeft(QPointF(0,0));
  return r.size();
}

void ScrollAreaItem::resize(const QSizeF &sz)
{
  Item::resize(sz);
  invalidate();
}

void ScrollAreaItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Item::paint(painter, option, widget);
  // TODO paint scroll bars
}

void ScrollAreaItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
  // don't scroll if no contents
  if ( m_content->childItems().count() ) {
    qreal d = 0;

    if ( !event->buttons() ) {
      if ( event->delta() > 0 ) {
        d = 20;
      } else if ( event->delta() < 0 ) {
        d = -20;
      }
      if ( event->orientation() == Qt::Horizontal || event->modifiers() == Qt::ShiftModifier )
        scrollBy(d,0);
      else
        scrollBy(0,d);

      event->accept();
      return;
    }
  }

  Item::wheelEvent(event);
}

void ScrollAreaItem::calcChildrenBoundingRect()
{
  if ( m_dirtyChildrenRect ) {
    m_childrenRect = mapRectFromItem(m_content, childrenBoundingRectHelper(m_content));
    m_dirtyChildrenRect = false;
  }
}

QRectF ScrollAreaItem::childrenBoundingRectHelper(QGraphicsItem *i)
{
  QRectF rect = m_content->mapRectFromItem(i, i->boundingRect());

  for (auto *child:i->childItems()) {
    rect |= childrenBoundingRectHelper(child);
  }

  //qDebug() << "children bounding rect" << name() << rect;
  return rect;
}

}
