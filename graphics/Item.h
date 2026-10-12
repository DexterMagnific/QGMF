#ifndef QGMFGRAPHICS_ITEM_H
#define QGMFGRAPHICS_ITEM_H

#include "QGMFGraphicsGlobals.h"

#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QVariantAnimation>

#include "Style.h"

Q_DECLARE_METATYPE(QPainterPath);

namespace QGMF {

class Scene;
class ScrollAreaItem;

/** Extended Graphic item flags, in addition to QGraphicsItem::GraphicItemFlags */
enum ItemExtFlag {
  // setItemStyle() will animate style changes
  ItemAnimatesStyleChanges = 0x1,
  // setPath() will animate path changes
  ItemAnimatesPathChanges = 0x2,
  // setPos(), moveBy() will animate position changes
  ItemAnimatesPosChanges = 0x4,
  // setScale(), setRotation(), setTransform() will animate transformation changes
  ItemAnimatesTransformChanges = 0x8,

  // setState() will trigger a style update
  ItemStyleDependsOnState = 0x10,
  // setState() will trigger a path update
  ItemPathDependsOnState = 0x20,
  // updatePath() will trigger children's path update
  ItemPathUpdatesChildren = 0x40,
  // updateStyle() will trigger children's style update
  ItemStyleUpdatesChildren = 0x80,

  // setStates() will propagate to parent
  ItemPropagatesStateToParent = 0x8000,
  // setStates() will propagate to children
  ItemPropagatesStateToChildren = 0x10000,
  // When trying to move the item interactively, the parent moves instead
  ItemMouseMovesParent = 0x20000,
  // Item is rotatable interactively
  ItemIsRotatable = 0x40000,
  // Item is resizeable interactively
  ItemIsResizeable = 0x80000,
};
Q_DECLARE_FLAGS(ItemExtFlags, ItemExtFlag);
Q_DECLARE_OPERATORS_FOR_FLAGS(ItemExtFlags);

#if 0
/** Extended Graphic item changes, in addition to QGraphicsItem::GraphicsItemChange */
enum ItemExtChange {
  ItemNodeAdded = 0x1,
  ItemNodeRemoved = 0x2,

  ItemPortAdded = 0x4,
  ItemPortRemoved = 0x8,

  ItemEdgeAdded = 0x10,
  ItemEdgeRemoved = 0x20,
  ItemEdgeSrcDetached = 0x40,
  ItemEdgeDstDetached = 0x80,
  ItemEdgePulled = 0x100,

};
#endif

/** Graphic item states */
enum StateFlag {
  Enabled   = 0x1,  // Enabled, nothing special
  Hovered   = 0x2,  // Mouse over
  // TODO add pressed
  Selected  = 0x4,  // Selected
  Toggled   = 0x8,  // Item permanently active (e.g. port with connections)
  Accepting = 0x10, // When item shows some interest (e.g. target port interested in
                    // incoming edge connection, ....)
  Declining = 0x20, // When item explicitly declines action
};

// ItemState: combination of state flags
Q_DECLARE_FLAGS(ItemStates, StateFlag);
Q_DECLARE_OPERATORS_FOR_FLAGS(ItemStates);

/**
 * Base class for QGMF graphic items
 * QGMF items are QGraphicsPathItem items with ability to be styled
 * and animated
 */
class QGMFGRAPHICS_PUBLIC Item: public QObject, public QGraphicsPathItem {
  public:
    Item(QGraphicsItem *parent = nullptr);
    virtual ~Item();

    // All graphics items above this value are considered to be inheriting
    // Item. Items will be cast accordingly and below methods will be called
    enum { Type = QGraphicsItem::UserType + 1000 };
    int type() const override { return Type; }

    void setName(const QString &name) { m_name = name; }
    QString name() const { return m_name; }

    /**
     * Sets item state to the given target states,
     * Calls @ref updatePath() if ItemPathDependsOnState is set
     * Calls @ref updateStyle() if ItemStyleDependsOnState is set
     * Propagates the state to children if ItemPropagatesStateToChildren is set
     * Propagates the state to paent if ItemPropagatesStateToParent is set
     */
    // FIXME replace by pushState()/pushStates() and popState()
    void setState(StateFlag s, bool enabled = true);
    void setStates(ItemStates s);
    ItemStates states() const { return m_states; }

    /**
     * Set extra flags
     */
    void setExtFlag(ItemExtFlag f, bool enabled = true);
    void setExtFlags(ItemExtFlags f);
    ItemExtFlags extFlags() const { return m_flags; }

    /**
     * returns the QGMF scene
     */
    Scene *scene() const;

    /**
     * This function returns the path that should be set on this item
     * according to its internal state (e.g. state, size, ....)
     * Reimplement it to compute a path that will be used by @ref setPath()
     * to animate path transitions and eventually set by @ref applyPath()
     * Default implementation returns the current path
     */
    virtual QPainterPath getPath() const { return path(); }

    /**
     * This function returns the style that should be set on this item
     * according to its internal state (e.g. state, size, ....)
     * Reimplement it to compute a style that will be used by @ref setItemStyle()
     * to animate style transitions and eventually set by @ref applyStyle()
     * Default implementation returns the current pen and brush
     */
    virtual Style getStyle() const;

    /**
     * Holds a position on a path and its angle
     */
    typedef struct PathPosition {
        QPointF pos;
        qreal angle;
    } PathPosition;

    /**
     *  Returns the nearest position (coord and angle) on path @a path to the given point @a p
     */
    static PathPosition nearestPointOnPath(const QPainterPath &path, const QPointF &p);

    /**
     * @brief Reimplemented to enable animations.
     */
    void setPos(const QPointF &pos);
    void setPos(qreal x, qreal y) { setPos(QPointF(x,y)); }
    void moveBy(qreal dx, qreal dy);

    /**
     * @brief Reimplemented to enable animations.
     */
    void setScale(qreal s);
    void setRotation(qreal s);
    void setTransform(const QTransform &t);

    /**
     * @brief Reimplemented to enable animations. Will call @ref applyPath()
     * to set the path at each animation step
     */
    void setPath(const QPainterPath &p);
    /**
     * @brief Reimplemented to enable animations. Will call @ref applyStyle()
     * to set the style at each animation step
     */
    void setItemStyle(const Style &s);

    /**
     * @brief Call this function to update the current path and style with regards
     * to current state.
     * Equivalent to calling @ref updateStyle() and @ref updatePath()
     */
    void updateAll();

    /**
     * @brief Updates the path
     * Equivalent to calling setPath(getPath())
     */
    void updatePath();
    /**
     * @brief Updates the style
     * Equivalent to calling setItemStyle(getStyle())
     */
    void updateStyle();

    /**
     * Returns the recommended size of this item.
     * The meaning of the size is item dependent, and might be different
     * from boundingRect().size(). If you pass the size hint to resize(),
     * it should have its recommended size
     * If the size is invalid, no minimum size is recommended.
     */
    virtual QSizeF sizeHint() { return QSizeF(); }
    /**
     * Returns the size of this item.
     * The meaning of the size is item dependent, and might be different
     * from boundingRect().size()
     */
    QSizeF size() const { return m_size; }
    /**
     * Sets the size of this item.
     * Default implementation stores the size and calls @ref updatePath()
     */
    virtual void resize(const QSizeF &sz);
    void resize(qreal w, qreal h);

    /**
     * Fit to contents. Same as resize(sizeHint());
     */
    void resizeToContents();

    /**
     * Returns the rect that is used to draw the selection rectangle
     * Default implementation returns boundingRect()
     */
    virtual QRectF selectionRect() const { return boundingRect(); }

    /**
     * Set this item as cosmetic
     * Cosmetic items are items that are generally used as pure containers
     * for child items.
     * The following flags are set for cosmetic items:
     *   - ItemPropagatesStateToChildren
     *   - ItemPathUpdatesChildren
     *   - ItemStyleUpdatesChildren
     *
     * @note Manually settings these flags won't turn this item into cosmetic
     */
    void setCosmetic();
    bool isCosmetic() const { return m_cosmetic; }

    /**
     * Returns the list of child items that are QGMF Items
     */
    QList<Item *> qgmfChildItems() const;

    // TODO add convenient funs to return rect, ellipse, ....

    static QPainterPath asBezier(const QVector<QPointF> &points);
    static QPainterPath asCatmullRom(const QVector<QPointF> &points, bool closed = false);
    static QPainterPath asBSpline(const QVector<QPointF> &points, bool closed = false);

  protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

   protected slots:
    /**
     * Call this to apply the given state to this item
     * Default implementation does nothing, but you can use it
     * to move item or child items for example on state change
     */
    virtual void applyStates(ItemStates &s);
    /**
     * Call this to apply the given style to this item
     * Default implementation sets the pen and the brush
     */
    virtual void applyStyle(const QVariant &v);
    /**
     * Call this to apply the given path to this item
     * Default implementation sets the path
     */
    virtual void applyPath(const QPainterPath &p);

    virtual void applyPos(const QVariant &v);
    virtual void applyTransform(const QVariant &v);
    virtual void applyScale(const QVariant &v);
    virtual void applyRotate(const QVariant &v);

  private:
    // This sucks: this is a non public class
    friend class ContentItem;

    static inline int level = 0;

    static QString nesting() { return QString("   ").repeated(level); }

    void propagateStates();

    // Gets the item that must be moved when trying to move i
    QGraphicsItem *movableItemFor(QGraphicsItem *i);
    // Tells whether any ancestor of i is in list l
    bool isAncestorIn(QGraphicsItem *i, const QSet<QGraphicsItem *> l);

    bool m_stateFreeze; // avoid state propagation recursion
    bool m_cosmetic;

    QString m_name;
    ItemStates m_states = ItemStates();
    ItemExtFlags m_flags = ItemExtFlags();

    QVariantAnimation m_styleAnim;
    QVariantAnimation m_posAnim;
    QVariantAnimation m_scaleAnim;
    QVariantAnimation m_rotateAnim;
    QVariantAnimation m_transformAnim;

    QSizeF m_size;

    QHash<QGraphicsItem *, QPointF> movingItemsInitialPositions;

    // The scroll area to which this item belogs
    ScrollAreaItem *m_scrollArea;
};

// FIXME WRONG. cannot cast i to QGraphicsItem (it is Item which inherits QObject AND QGraphicsItem)
#if 0
template<typename T>
inline T Item_cast(void *i) {
  return qgraphicsitem_cast<T>(static_cast<Item *>(i));
}
#endif

}

#endif // ITEM_H
