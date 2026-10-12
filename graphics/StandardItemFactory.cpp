#include "StandardItemFactory.h"

#include "StandardNodeItem.h"
#include "StandardPortItem.h"
#include "StandardEdgeItem.h"

namespace QGMF {

NodeItem *StandardItemFactory::createNodeItem(Node *n, Item *parent) const
{
  return new StandardNodeItem(n, parent);
}

PortItem *StandardItemFactory::createPortItem(Port *p, Item *parent) const
{
  return new StandardPortItem(p, parent);
}

EdgeItem *StandardItemFactory::createEdgeItem(Edge *e, Item *parent) const
{
  return new StandardEdgeItem(e, parent);
}

EdgeItem *StandardItemFactory::createTemporaryEdgeItem(PortItem *p, Item *parent) const
{
  // pull edge from source port
  return new StandardEdgeItem(p, p->sceneConnectionPoint(nullptr), parent);
}

}
