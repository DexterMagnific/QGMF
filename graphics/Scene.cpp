#include "Scene.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

#include "Item.h"

namespace QGMF {

class SelectionRect : public Item {
  public:
    SelectionRect(const QPointF &pt)
      : Item(nullptr)
    {
      setZValue(std::numeric_limits<qreal>().max());
      setPos(pt);

      m_penAnim.setEndValue(0);
      m_penAnim.setEasingCurve(QEasingCurve::Linear);
      m_penAnim.setDuration(200);
      m_penAnim.setLoopCount(-1);

      connect(&m_penAnim, &QVariantAnimation::valueChanged,
              this, &SelectionRect::slot_updatePen);

      m_penAnim.setStartValue(6);
      m_penAnim.start();

      updatePath();
    }
    virtual ~SelectionRect() = default;

    QPainterPath getPath() const override;
    Style getStyle() const override;

  private slots:
    void slot_updatePen(const QVariant &v);

  private:
    QVariantAnimation m_penAnim;
};

QPainterPath SelectionRect::getPath() const
{
  const QSizeF &sz = size();

  QPainterPath p;
  p.addRoundedRect(0,0,sz.width(),sz.height(), 5,5);
  return p;
}

Style SelectionRect::getStyle() const
{
  Style s(QPen(QColor(200,0,0,200), 1., Qt::DashLine),
          QBrush(QColor(00,00,00, 30)));

  QPen p = s.pen();
  p.setDashOffset(m_penAnim.currentValue().toDouble());
  s.setPen(p);
  return s;
}

void SelectionRect::slot_updatePen(const QVariant &v)
{
  Q_UNUSED(v);
  updateStyle();
}

// ****************************************************************************

PortItemList Scene::nearbyPorts(const QPointF &pt, qreal radius) const
{
  PortItemList l;

  QRectF r(-radius,-radius,2*radius,2*radius);
  r.moveCenter(pt);

  QList<QGraphicsItem *> it = items(r, Qt::IntersectsItemBoundingRect);

  for (auto *i : it) {
    if ( i->type() == PortItem::Type ) {
      PortItem *p = static_cast<PortItem *>(i);
      l << p;
      //qDebug() << "near" << p->port().name().c_str();
    }
  }

  return l;
}

NodeItemList Scene::nearbyNodes(const QPointF &pt, qreal radius) const
{
  NodeItemList l;

  QRectF r(-radius,-radius,2*radius,2*radius);
  r.moveCenter(pt);

  QList<QGraphicsItem *> it = items(r);

  for (auto *i : it) {
    if ( i->type() == NodeItem::Type ) {
      NodeItem *p = static_cast<NodeItem *>(i);
      l << p;
      //qDebug() << p->port().name().c_str();
    }
  }

  return l;

}

Item *Scene::createSelectionItem(const QPointF pt)
{
  return new SelectionRect(pt);
}

void Scene::setNodeSelectionArea(const QPainterPath path, Qt::ItemSelectionOperation selectionOperation)
{
  auto unselectItems = selectedItems();

  // Set all items in path to selected.
  const auto items = this->items(path, Qt::IntersectsItemBoundingRect, Qt::DescendingOrder);
  for (QGraphicsItem *item : items) {
    if (item->flags() & QGraphicsItem::ItemIsSelectable && item->type() == NodeItem::Type) {
      unselectItems.removeOne(item);
      item->setSelected(true);
    }
  }

  switch (selectionOperation) {
  case Qt::ReplaceSelection:
    // Deselect all items outside path.
    for (QGraphicsItem *item : qAsConst(unselectItems)) {
      item->setSelected(false);
    }
    break;
  default:
    break;
  }
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  auto selection = selectedItems();

  QGraphicsScene::mousePressEvent(event);

  if ( !event->isAccepted() ) {
    //qDebug() << "event goes to the scene";
    QGraphicsView *view = event->widget() ? qobject_cast<QGraphicsView *>(event->widget()->parentWidget()) : 0;
    bool dontClearSelection = view && view->dragMode() == QGraphicsView::ScrollHandDrag;
    bool extendSelection = (event->modifiers() & Qt::ShiftModifier) != 0;
    dontClearSelection |= extendSelection;
    if (dontClearSelection) {
      for ( auto *i: qAsConst(selection)) {
        i->setSelected(true);
      }
    }
  }
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  if ( m_selectionItem ) {
    // add items to selection
    // TODO only nodes
    // TODO don't add the node that contains click point
    setNodeSelectionArea(m_selectionItem->mapToScene(m_selectionItem->shape()));
    delete m_selectionItem;
    m_selectionItem = nullptr;
  }

  QGraphicsScene::mouseReleaseEvent(event);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if ( (event->buttons() & Qt::LeftButton) && (event->modifiers() & Qt::ShiftModifier) ) {
    //qDebug() << "Should spawn a selection rectangle";
    if ( !m_selectionItem ) {
      m_selectionItem = createSelectionItem(event->buttonDownScenePos(Qt::LeftButton));
      addItem(m_selectionItem);
    }
    const QPointF d = event->scenePos()-event->buttonDownScenePos(Qt::LeftButton);
    m_selectionItem->resize(d.x(),d.y());
    event->accept();
    return;
  }

  QGraphicsScene::mouseMoveEvent(event);
}

}
