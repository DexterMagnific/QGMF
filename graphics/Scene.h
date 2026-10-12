#ifndef QGMFGRAPHICS_SCENE_H
#define QGMFGRAPHICS_SCENE_H

#include "QGMFGraphicsGlobals.h"

#include <QGraphicsScene>
#include <QString>
#include <QDebug>

#include "Node.h"
#include "Graph.h"
#include "Edge.h"
#include "Port.h"

#include "GraphItem.h"
#include "StandardItemFactory.h"

namespace QGMF {

class QGMFGRAPHICS_PUBLIC Scene: public QGraphicsScene
{
  public:
    Scene(qreal x, qreal y, qreal width, qreal height,
          const GraphModel &mdl,
          const QString &name = QString(),
          ItemFactory *factory = &stdItemFactory,
          QObject *parent = nullptr)
      : QGraphicsScene(x,y,width,height,parent)
      , m_graph(new Graph(mdl, name.toStdString()))
      , m_factory(factory ? factory : &stdItemFactory)
      , m_selectionItem(nullptr)
    {
      m_graphItem = m_factory->createNodeItem(m_graph, nullptr);
      addItem(m_graphItem);
      //m_graphItem->ensureVisible();
    }

    Scene(const QRectF &sceneRect,
          const GraphModel &mdl,
          const QString &name = QString(),
          ItemFactory *factory = &stdItemFactory,
          QObject *parent = nullptr)
      : QGraphicsScene(sceneRect, parent)
      , m_graph(new Graph(mdl, name.toStdString()))
      , m_factory(factory ? factory : &stdItemFactory)
      , m_selectionItem(nullptr)
    {
      m_graphItem = m_factory->createNodeItem(m_graph, nullptr);
      addItem(m_graphItem);
    }

    Scene(const GraphModel &mdl,
          const QString &name = QString(),
          ItemFactory *factory = &stdItemFactory,
          QObject *parent = nullptr)
      : QGraphicsScene(parent)
      , m_graph(new Graph(mdl, name.toStdString()))
      , m_factory(factory ? factory : &stdItemFactory)
      , m_selectionItem(nullptr)
    {
      m_graphItem = m_factory->createNodeItem(m_graph, nullptr);
      addItem(m_graphItem);
    }

    virtual ~Scene() = default;

    const GraphModel &graphModel() const { return m_graph->model(); }
    const Graph *graph() const { return m_graph; }
    GraphItem *graphItem() const { return m_graphItem; }

    ItemFactory *itemFactory() const { return m_factory; }

    /** returns the list of port items near point @c p */
    PortItemList nearbyPorts(const QPointF &pt, qreal radius) const;
    /** returns the list of node items near point @c p */
    NodeItemList nearbyNodes(const QPointF &p, qreal radius) const;

    /** Select nodes that are inside the given rect */
    void setNodeSelectionArea(const QPainterPath path, Qt::ItemSelectionOperation selectionOperation = Qt::AddToSelection);

  protected:
    /**
     * Creates an item that will serve as a selection "rectangle" at the
     * given scene point.
     * resize() calls will be issued by the scene to make the selection
     * item grow or shrink
     */
    virtual Item *createSelectionItem(const QPointF pt);

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

  private:
    Graph *m_graph;
    ItemFactory *m_factory;
    GraphItem *m_graphItem;
    Item *m_selectionItem;
};

}
#endif // SCENE_H
