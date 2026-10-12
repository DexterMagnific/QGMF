#ifndef SCROLLAREAITEM_H
#define SCROLLAREAITEM_H

#include "Item.h"

namespace QGMF {

/**
 * @brief The ScrollAreaItem class provides a QGraphicsItem scrollable
 * area
 */
class QGMFGRAPHICS_PUBLIC ScrollAreaItem : public Item
{
  public:
    ScrollAreaItem(QGraphicsItem *parent = nullptr);
    virtual ~ScrollAreaItem() = default;

    enum { Type = Item::Type + 10 };
    int type() const override { return Type; }

    /**
     * @brief Returns the view port to which scrollable items should be parented
     */
    Item *contentItem() const { return m_content; }

    /**
     * Scrolls the content item by the given amount
     */
    void scrollBy(qreal dx, qreal dy);

    /**
     * Call this to invalidate the scroll area whenever the geometry of
     * the scroll area is changed (item added, removed, transformation, ...)
     */
    void invalidate();

    QPainterPath getPath() const override;

    /**
     * Returns the size of the scroll area so that it covers all of its contents
     */
    QSizeF sizeHint() override;

    void resize(const QSizeF &sz) override;

  protected:
    //virtual void paintHorizontalScrollbar(QPainter *p, const QRectF &cursor);
    //virtual void paintVerticalScrollbar(QPainter *p, const QRectF &cursor);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;

  private:
    void calcChildrenBoundingRect();
    QRectF childrenBoundingRectHelper(QGraphicsItem *i);

    QRectF m_childrenRect;
    bool m_dirtyChildrenRect;
    Item *m_content;
    qreal m_dx,m_dy;
};

}

#endif // SCROLLAREAITEM_H
