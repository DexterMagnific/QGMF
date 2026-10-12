#include "Item.h"

#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsWidget>

#include "Scene.h"
#include "ScrollAreaItem.h"

#include "thirdparty/cpp-spline/Bezier.h"
#include "thirdparty/cpp-spline/CatmullRom.h"
#include "thirdparty/cpp-spline/BSpline.h"


namespace QGMF {

Item::Item(QGraphicsItem *parent)
  : QObject(nullptr)
  , QGraphicsPathItem(parent)
  , m_stateFreeze(false)
  , m_cosmetic(false)
  , m_size(0,0)
  , m_scrollArea(nullptr)
{
  setExtFlag(ItemAnimatesStyleChanges);
  setExtFlag(ItemAnimatesPathChanges);
  setExtFlag(ItemAnimatesTransformChanges);
  setExtFlag(ItemStyleDependsOnState);
  setExtFlag(ItemPathDependsOnState);

  m_styleAnim.setDuration(200);
  m_styleAnim.setEasingCurve(QEasingCurve::OutQuad);
  connect(&m_styleAnim, &QVariantAnimation::valueChanged,
          this, &Item::applyStyle);

  m_posAnim.setDuration(200);
  m_posAnim.setEasingCurve(QEasingCurve::OutQuad);
  connect(&m_posAnim, &QVariantAnimation::valueChanged,
          this, &Item::applyPos);

  m_scaleAnim.setDuration(200);
  m_scaleAnim.setEasingCurve(QEasingCurve::OutQuad);
  connect(&m_scaleAnim, &QVariantAnimation::valueChanged,
          this, &Item::applyScale);

  m_rotateAnim.setDuration(200);
  m_rotateAnim.setEasingCurve(QEasingCurve::OutQuad);
  connect(&m_rotateAnim, &QVariantAnimation::valueChanged,
          this, &Item::applyRotate);

  m_transformAnim.setDuration(200);
  m_transformAnim.setEasingCurve(QEasingCurve::OutQuad);
  connect(&m_transformAnim, &QVariantAnimation::valueChanged,
          this, &Item::applyTransform);
}

Item::~Item()
{
  //qDebug() << "item" << name() << "removed";
}

void Item::setState(StateFlag s, bool enabled)
{
  ItemStates new_states = m_states;

  if ( enabled )
    new_states |= s;
  else
    new_states &= ~s;

  setStates(new_states);
}

void Item::setStates(ItemStates s)
{
  //qDebug().noquote() << nesting() << "<---------" << name() << "set states" << s;
  level++;

  // avoid recursion when both ItemPropagatesStateToParent and ItemPropagatesStateToChildren
  // are both set on parent/child
  if ( m_stateFreeze ) {
    //qDebug().noquote() << nesting() << "           " << name() << "propagation stopped --------->";
    level--;
    return;
  }

  m_stateFreeze = true;

  if ( s != m_states ) {
    m_states = s;
    // FIXME for Selected state: must call setSelected() on item

    applyStates(s);

    if ( m_flags & ItemStyleDependsOnState ) {
      updateStyle();
    }

    if ( m_flags & ItemPathDependsOnState ) {
      updatePath();
    }
  }

  propagateStates();

  m_stateFreeze = false;

  //qDebug().noquote() << nesting() << "           " << name() << "set states END --------->";
  level--;
}

void Item::setExtFlag(ItemExtFlag f, bool enabled)
{
  if ( enabled )
    m_flags |= f;
  else
    m_flags &= ~f;
}

void Item::setExtFlags(ItemExtFlags f)
{
  m_flags = f;
}

Scene *Item::scene() const
{
  return static_cast<Scene *>(QGraphicsItem::scene());
}

void Item::updateAll()
{
  updateStyle();
  updatePath();
}

Item::PathPosition Item::nearestPointOnPath(const QPainterPath &path, const QPointF &p)
{
  qreal min_d = std::numeric_limits<qreal>().max();
  PathPosition res;
  res.pos = QPointF(0,0);
  res.angle = 0;
  const unsigned steps = 200;

  for (unsigned t=0; t<=steps; t++) {
    const QPointF &pp = path.pointAtPercent((qreal)t/steps);
    const qreal d = (pp-p).manhattanLength();
    if ( d < min_d ) {
      min_d = d;
      res.pos = pp;
      res.angle = path.angleAtPercent(t);
    }
  }

  return res;
}

void Item::setPos(const QPointF &p)
{
  // TODO don't animate for very first position change
  if ( m_flags & ItemAnimatesPosChanges ) {
    // animate position change
    m_posAnim.stop();
    if ( m_posAnim.startValue().isNull() ) {
      m_posAnim.setStartValue(pos());
    } else {
      m_posAnim.setStartValue(m_posAnim.currentValue());
    }
    m_posAnim.setEndValue(p);
    m_posAnim.start();
  } else {
    applyPos(p);
  }
}

void Item::moveBy(qreal dx, qreal dy)
{
  setPos(pos() + QPointF(dx, dy));
}

void Item::setScale(qreal s)
{
  if ( m_flags & ItemAnimatesTransformChanges ) {
    m_scaleAnim.stop();
    if ( m_scaleAnim.startValue().isNull() ) {
      m_scaleAnim.setStartValue(scale());
    } else {
      m_scaleAnim.setStartValue(m_scaleAnim.currentValue());
    }
    m_scaleAnim.setEndValue(s);
    m_scaleAnim.start();
  } else {
    applyScale(s);
  }
}

void Item::setRotation(qreal s)
{
  if ( m_flags & ItemAnimatesTransformChanges ) {
    m_rotateAnim.stop();
    if ( m_rotateAnim.startValue().isNull() ) {
      m_rotateAnim.setStartValue(rotation());
    } else {
      m_rotateAnim.setStartValue(m_rotateAnim.currentValue());
    }
    m_rotateAnim.setEndValue(s);
    m_rotateAnim.start();
  } else {
    applyRotate(s);
  }
}

void Item::setTransform(const QTransform &t)
{
  // FIXME QVariantAnimation does not support QTransform
#if 0
  if ( m_flags & ItemAnimatesTransformChanges ) {
    m_transformAnim.stop();
    if ( m_transformAnim.startValue().isNull() ) {
      m_transformAnim.setStartValue(transform());
    } else {
      m_transformAnim.setStartValue(m_transformAnim.currentValue());
    }
    m_transformAnim.setEndValue(t);
    m_transformAnim.start();
  } else {
    applyTransform(t);
  }
#endif
  applyTransform(t);
}

void Item::setPath(const QPainterPath &p)
{
  if ( m_flags & ItemAnimatesPathChanges ) {
    // TODO animate path
    // m_pathAnim.setEndPath(pathForStates(s));
    applyPath(p);
  } else {
    applyPath(p);
  }
}

void Item::setItemStyle(const Style &s)
{
  if ( m_flags & ItemAnimatesStyleChanges ) {
    // animate style change
    m_styleAnim.stop();
    if ( m_styleAnim.startValue().isNull() ) {
      m_styleAnim.setStartValue(Style(pen(),brush()));
    } else {
      m_styleAnim.setStartValue(m_styleAnim.currentValue());
    }
    m_styleAnim.setEndValue(s);
    m_styleAnim.start();
  } else {
    applyStyle(s);
  }
}

void Item::resize(const QSizeF &sz)
{
  if ( sz.isValid() ) {
    m_size = sz;
    updatePath();
  }
}

void Item::resize(qreal w, qreal h)
{
  resize(QSizeF(w,h));
}

void Item::resizeToContents()
{
  resize(sizeHint());
}

void Item::setCosmetic()
{
  setExtFlags(extFlags() | ItemPropagatesStateToChildren | ItemPathUpdatesChildren | ItemStyleUpdatesChildren);
  m_cosmetic = true;
}

QList<Item *> Item::qgmfChildItems() const
{
  QList<Item *> l;
  for (QGraphicsItem *i: childItems()) {
    if ( i->type() >= Item::Type ) {
      Item *ii = static_cast<Item *>(i);
      l << ii;
    }
  }

  return l;
}

void Item::propagateStates()
{
  if ( m_flags & ItemPropagatesStateToChildren ) {
    for (QGraphicsItem *i: childItems()) {
      if ( i->type() >= Item::Type ) {
        Item *ii = static_cast<Item *>(i);
        ii->setStates(m_states);
      }
    }
  }

  if ( m_flags & ItemPropagatesStateToParent ) {
    QGraphicsItem *i = parentItem();
    if ( i->type() >= Item::Type ) {
      Item *ii = static_cast<Item *>(i);
      ii->setStates(m_states);
    }
  }
}

void Item::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  //qDebug() << "hover enter event" << name();
  setState(Hovered);
  QGraphicsPathItem::hoverEnterEvent(event);
}

void Item::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
  //qDebug() << "hover leave event" << name();
  setState(Hovered, false);
  QGraphicsPathItem::hoverLeaveEvent(event);
}

void Item::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  // same as QGraphicsScene::mousePressEvent, just changed the selection modifier
  if (event->button() == Qt::LeftButton && (flags() & ItemIsSelectable)) {
    bool multiSelect = (event->modifiers() & Qt::ShiftModifier) != 0;
    if (!multiSelect) {
      qDebug() << "mono select";
      if (!isSelected()) {
        if (QGraphicsScene *scene = this->scene()) {
          qDebug() << "clear selection";
          scene->clearSelection();
        }
        qDebug() << "set selected";
        setSelected(true);
      }
    }
  } else if (!(flags() & ItemIsMovable)) {
    event->ignore();
  }
  if (isWidget()) {
    // Qt::Popup closes when you click outside.
    QGraphicsWidget *w = static_cast<QGraphicsWidget *>(static_cast<QGraphicsItem *>(this));
    if ((w->windowFlags() & Qt::Popup) == Qt::Popup) {
      event->accept();
      if (!w->rect().contains(event->pos()))
        w->close();
    }
  }
}

void Item::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  // same as QGraphicsItem::mouseReleaseEvent, just changed the modifier
  if (event->button() == Qt::LeftButton && (flags() & ItemIsSelectable)) {
    bool multiSelect = (event->modifiers() & Qt::ShiftModifier) != 0;
    if (event->scenePos() == event->buttonDownScenePos(Qt::LeftButton)) {
      // The item didn't move
      if (multiSelect) {
        setSelected(!isSelected());
        //qDebug() << "invert selection";
      } else {
        //setSelected(true);
      }
    }
  }
  if ( scene() && !event->buttons() )
    movingItemsInitialPositions.clear();
}

void Item::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  //qDebug() << "contains ?" << contains(event->pos());

  if ((event->buttons() & Qt::LeftButton) && (flags() & ItemIsMovable)) {
    // It is unfortunate that moving selected items is handled by the moved item
    // instead of the scene itself
    if ( scene() ) {
      // code below from QGraphicsItem::mouseMoveEvent()

      // get initial positions (once per move operation)
      // FIXME move movingItemsInitialPositions to scene
      QHash<QGraphicsItem *, QPointF> initialPositions = movingItemsInitialPositions;
      if (initialPositions.isEmpty()) {
        // items to move
        QSet<QGraphicsItem *> toMove, toMove2;
        const auto selection = scene()->selectedItems();
        for (auto *i : qAsConst(selection)) {
          //    qDebug() << "movable item for" << i;
          QGraphicsItem *m = movableItemFor(i);
          if ( m ) {
            //qDebug() << "will move" << m;
            toMove << m;
          }
        }

        // remove items that have one of their ancestors in list
        for (auto *i: qAsConst(toMove)) {
          if ( !isAncestorIn(i, toMove) )
            toMove2 << i;
        }

        for (QGraphicsItem *item : qAsConst(toMove2))
          initialPositions[item] = item->pos();

        movingItemsInitialPositions = initialPositions;
      }
    }

    // move items
    const auto toMove = movingItemsInitialPositions.keys();
    for (QGraphicsItem *item : toMove) {
      QPointF currentParentPos;
      QPointF buttonDownParentPos;

      // Simplified. ItemIgnoresTransformations not handled
      currentParentPos = item->mapToParent(item->mapFromScene(event->scenePos()));
      buttonDownParentPos = item->mapToParent(item->mapFromScene(event->buttonDownScenePos(Qt::LeftButton)));

      item->setPos(movingItemsInitialPositions.value(item) + currentParentPos - buttonDownParentPos);
    }
  } else {
    event->ignore();
  }
}

QVariant Item::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
  if ( change == ItemEnabledChange ||
       change == ItemChildAddedChange ||
       change == ItemChildRemovedChange ||
       change == ItemPositionHasChanged ||
       change == ItemTransformHasChanged ||
       change == ItemSceneChange ||
       change == ItemVisibleHasChanged ||
       change == ItemEnabledHasChanged ||
       change == ItemSelectedHasChanged ||
       change == ItemParentHasChanged ||
       change == ItemSceneHasChanged ||
       change == ItemCursorHasChanged ||
       change == ItemToolTipHasChanged ||
       change == ItemFlagsHaveChanged ||
       change == ItemZValueHasChanged ||
       change == ItemOpacityHasChanged ||
       change == ItemScenePositionHasChanged ||
       change == ItemRotationHasChanged ||
       change == ItemScaleHasChanged ||
       change == ItemTransformOriginPointHasChanged
       ) {
    //qDebug() << change << name();
  }
  if ( change == ItemFlagsHaveChanged ) {
    //qDebug().noquote() << nesting() << name() << "flags" << GraphicsItemFlags(value.toInt());
  }
  if ( change == ItemSelectedHasChanged ) {
    //qDebug() << name() << "selected" << value.toBool();
    setState(Selected, value.toBool());
  }
  if ( change == ItemEnabledHasChanged ) {
    setState(Enabled, value.toBool());
  }
  if ( change == ItemSceneHasChanged ) {
    auto st = ItemStates();
    if ( isEnabled() )
      st |= Enabled;
    if ( isSelected() )
      st |= Selected;

    setStates(states() | st);
  }
  if ( change == ItemVisibleHasChanged ) {
    auto st = ItemStates();
    if ( isEnabled() )
      st |= Enabled;
    if ( isSelected() )
      st |= Selected;

    setStates(states() | st);
  }
  if ( change == ItemPositionHasChanged ||
       change == ItemRotationHasChanged ||
       change == ItemScaleHasChanged ||
       change == ItemTransformHasChanged ||
       change == ItemTransformOriginPointHasChanged ) {
    if ( m_scrollArea ) {
      //qDebug() << name() << "geom change invalidates scroll area";
      m_scrollArea->invalidate();
    }
  }

  return QGraphicsItem::itemChange(change, value);
}

void Item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  // remove selected
  QStyleOptionGraphicsItem opt(*option);
  opt.state &= ~QStyle::State_Selected;

  QGraphicsPathItem::paint(painter, &opt, widget );
}

void Item::updatePath()
{
  setPath(getPath());

  if ( m_flags & ItemPathUpdatesChildren ) {
    for (auto *i : childItems()) {
      if ( i->type() >= Item::Type ) {
        Item *ii = static_cast<Item *>(i);
        ii->updatePath();
      }
    }
  }
}

void Item::updateStyle()
{
  setItemStyle(getStyle());

  if ( m_flags & ItemStyleUpdatesChildren ) {
    for (auto *i : childItems()) {
      if ( i->type() >= Item::Type ) {
        Item *ii = static_cast<Item *>(i);
        ii->updateStyle();
      }
    }
  }
}

Style Item::getStyle() const
{
  return Style(pen(), brush());
}

void Item::applyStyle(const QVariant &v)
{
  const Style &s(v.value<Style>());

  setPen(s.pen());
  setBrush(s.brush());
}

void Item::applyPath(const QPainterPath &p)
{
  //qDebug().noquote() << nesting() << name() << "apply path";
  QGraphicsPathItem::setPath(p);
}

void Item::applyStates(ItemStates &s)
{
  Q_UNUSED(s);
}

void Item::applyPos(const QVariant &v)
{
  QGraphicsPathItem::setPos(v.toPointF());
}

void Item::applyTransform(const QVariant &v)
{
  QGraphicsPathItem::setTransform(v.value<QTransform>());
}

void Item::applyScale(const QVariant &v)
{
  QGraphicsItem::setScale(v.toReal());
}

void Item::applyRotate(const QVariant &v)
{
  QGraphicsItem::setRotation(v.toReal());
}

QGraphicsItem *Item::movableItemFor(QGraphicsItem *i)
{
  if ( !i ) {
    return nullptr;
  }

  if ( i->type() >= Item::Type ) {
    Item *ii = static_cast<Item *>(i);
    if ( ii->extFlags() & ItemMouseMovesParent ) {
      return movableItemFor(i->parentItem());
    }
  }

  if ( i->flags() & ItemIsMovable ) {
    return i;
  }

  return nullptr;
}

bool Item::isAncestorIn(QGraphicsItem *i, const QSet<QGraphicsItem *> l)
{
  if ( !i ) {
    return false;
  }

  QGraphicsItem *ii = i->parentItem();
  while ( ii ) {
    if ( l.contains(ii) )
      return true;

    ii = ii->parentItem();
  }

  return false;
}

QPainterPath Item::asBezier(const QVector<QPointF> &points)
{
  QPainterPath path;

  if ( points.empty() )
    return path;

  if ( points.size() < 4 ) {
    path.moveTo(points.at(0));
    for(int i=1; i<points.size(); i++) {
      path.lineTo(points.at(i));
    }
    return path;
  }

  Bezier *curve = new Bezier();
  curve->set_steps(20);
  for(const QPointF &p: points) {
    curve->add_way_point(Vector(p.x(),p.y(),0));
  }

  path.moveTo(curve->node(0).x,curve->node(0).y);
  if ( curve->node_count() > 1 ) {
    for(int i=1; i<curve->node_count(); i++) {
      path.lineTo(curve->node(i).x,curve->node(i).y);
    }
  }

  delete curve;
  return path;
}

QPainterPath Item::asCatmullRom(const QVector<QPointF> &points, bool closed)
{
  QPainterPath path;

  if ( points.empty() )
    return path;

  if ( points.size() < 4 ) {
    path.moveTo(points.at(0));
    for(int i=1; i<points.size(); i++) {
      path.lineTo(points.at(i));
    }
    return path;
  }

  CatmullRom *curve = new CatmullRom();
  curve->set_steps(20);
  curve->add_way_point(Vector(points.last().x(),points.last().y(),0));
  for(const QPointF &p: points) {
    curve->add_way_point(Vector(p.x(),p.y(),0));
  }
  curve->add_way_point(Vector(points.at(0).x(),points.at(0).y(),0));
  if ( closed )
    curve->add_way_point(Vector(points.at(1).x(),points.at(1).y(),0));

  path.moveTo(curve->node(0).x,curve->node(0).y);
  if ( curve->node_count() > 1 ) {
    for(int i=1; i<curve->node_count(); i++) {
      path.lineTo(curve->node(i).x,curve->node(i).y);
    }
  }

  delete curve;
  return path;
}

QPainterPath Item::asBSpline(const QVector<QPointF> &points, bool closed)
{
  QPainterPath path;

  if ( points.empty() )
    return path;

  if ( points.size() < 4 ) {
    path.moveTo(points.at(0));
    for(int i=1; i<points.size(); i++) {
      path.lineTo(points.at(i));
    }
    return path;
  }

  BSpline *curve = new BSpline();
  curve->set_steps(20);
  curve->add_way_point(Vector(points.last().x(),points.last().y(),0));
  for(const QPointF &p: points) {
    curve->add_way_point(Vector(p.x(),p.y(),0));
  }
  curve->add_way_point(Vector(points.at(0).x(),points.at(0).y(),0));
  if ( closed )
    curve->add_way_point(Vector(points.at(1).x(),points.at(1).y(),0));

  path.moveTo(curve->node(0).x,curve->node(0).y);
  if ( curve->node_count() > 1 ) {
    for(int i=1; i<curve->node_count(); i++) {
      path.lineTo(curve->node(i).x,curve->node(i).y);
    }
  }

  delete curve;
  return path;
}

}
