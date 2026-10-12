#include "StandardCaptionItem.h"

#include <QDebug>
#include <QFontMetrics>

namespace QGMF {

class StandardCaptionRectItem: public Item {
  public:
    StandardCaptionRectItem(StandardCaptionItem *c)
      : Item(c)
    {
      setName(c->name()+".Rect");

      setAcceptHoverEvents(true);
      setFlags(flags() | ItemIsMovable | ItemIsSelectable);
      setExtFlags(extFlags() | ItemMouseMovesParent | ItemPropagatesStateToParent);
    }

    StandardCaptionItem *captionItem() const { return static_cast<StandardCaptionItem *>(parentItem()); }

    virtual QPainterPath getPath() const override;
    virtual Style getStyle() const override;
};

QPainterPath StandardCaptionRectItem::getPath() const
{
  QPen pen = this->pen();

  QPainterPath p;

  const QMargins m = captionItem()->margins();
  QSizeF min_sz = captionItem()->textRect().marginsAdded(m).size();
  min_sz.rwidth() += pen.widthF();
  min_sz.rheight() += pen.widthF();

  const QSizeF sz = min_sz.expandedTo(captionItem()->size());

  p.addRect(0,0,sz.width(),-sz.height());

    return p;
}

Style StandardCaptionRectItem::getStyle() const
{
  const StandardStyle &st = captionItem()->captionStyle().rect;
  auto s = states();

  return s & StateFlag::Enabled ?
    s & StateFlag::Accepting ? st.accepting :
    s & StateFlag::Declining ? st.declining :
    s & StateFlag::Toggled ? st.toggled :
    s & StateFlag::Selected ? st.selected :
    s & StateFlag::Hovered ? st.hovered : st.normal
  : st.disabled;
}

// ********************************************

class StandardCaptionTextItem: public Item {
  public:
    StandardCaptionTextItem(StandardCaptionItem *c)
      : Item(c)
    {
      setName(c->name()+".Text");
    }

    StandardCaptionItem *captionItem() const { return static_cast<StandardCaptionItem *>(parentItem()); }

    virtual QPainterPath getPath() const override;
    virtual Style getStyle() const override;
};

QPainterPath StandardCaptionTextItem::getPath() const
{
  Style st = captionItem()->getStyle();

  QPainterPath p;
  Q_ASSERT(captionItem() != nullptr);
  p.addText(0,0, st.font(), captionItem()->text());

  return p;
}

Style StandardCaptionTextItem::getStyle() const
{
  const StandardStyle &st = captionItem()->captionStyle().text;
  auto s = states();

  return s & StateFlag::Enabled ?
    s & StateFlag::Accepting ? st.accepting :
    s & StateFlag::Declining ? st.declining :
    s & StateFlag::Toggled ? st.toggled :
    s & StateFlag::Selected ? st.selected :
    s & StateFlag::Hovered ? st.hovered : st.normal
  : st.disabled;
}

// ********************************************

StandardCaptionItem::StandardCaptionItem(QGraphicsItem *parent)
  : Item(parent)
  , m_margins(4,4,4,4)
{
  setName(QString("Caption('')"));

  setFlags(flags() | ItemHasNoContents | ItemIsSelectable);
  setExtFlags(extFlags() | ItemPropagatesStateToChildren);
  //setExtFlag(ItemAnimatesTransformChanges, false);
  setExtFlag(ItemAnimatesPosChanges, false);

  m_textItem = new StandardCaptionTextItem(this);
  m_rectItem = new StandardCaptionRectItem(this);

  m_rectItem->stackBefore(m_textItem);
}

void StandardCaptionItem::setText(const QString &text)
{
  setName(QString("Caption(%1)").arg(text));

  m_text = text;
  m_textItem->updateAll();
  m_rectItem->updateAll();

  // rect above node body
  //m_rectItem->moveBy(0,-m_rectItem->boundingRect().height());
  // text centered inside rect
  //qDebug() << "set text" << text;
  //qDebug() << "m_rectItem->boundingRect()" << m_rectItem->boundingRect();
  //qDebug() << "m_textItem->boundingRect()" << m_textItem->boundingRect();

  QPointF c0 = mapFromItem(m_rectItem, m_rectItem->boundingRect().center());
  QPointF c1 = mapFromItem(m_textItem, m_textItem->boundingRect().center());
  m_textItem->moveBy((c0-c1).x(),(c0-c1).y());
}

QRectF StandardCaptionItem::textRect() const
{
  return m_textItem->boundingRect();
}

void StandardCaptionItem::setMargins(const QMargins &m)
{
  m_margins = m;
  m_textItem->updateAll();
  m_rectItem->updateAll();

  // rect above node body
  //m_rectItem->moveBy(0,m_rectItem->boundingRect().height());
  // text centered inside rect
  QPointF c0 = mapFromItem(m_rectItem, m_rectItem->boundingRect().center());
  QPointF c1 = mapFromItem(m_textItem, m_textItem->boundingRect().center());
  m_textItem->moveBy((c0-c1).x(),(c0-c1).y());
}

void StandardCaptionItem::setCaptionStyle(const StandardCaptionStyle &s)
{
  m_style = s;
  m_textItem->updateAll();
  m_rectItem->updateAll();
}

void StandardCaptionItem::setCaptionStyle(const QColor &fg, const QColor &bg)
{
  setCaptionStyle(StandardCaptionStyle(fg,bg));
}

}

