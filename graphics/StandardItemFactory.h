#ifndef QGMFGRAPHICS_STANDARDITEMFACTORY_H
#define QGMFGRAPHICS_STANDARDITEMFACTORY_H

#include "QGMFGraphicsGlobals.h"

#include <QGMF.h>
#include "ItemFactory.h"

namespace QGMF {

// Standard item factory:
// rectangular nodes, circle ports, line edges
class QGMFGRAPHICS_PUBLIC StandardItemFactory : public ItemFactory {
  public:
    virtual ~StandardItemFactory() { }
    NodeItem *createNodeItem(Node *n, Item *parent) const override;
    PortItem *createPortItem(Port *p, Item *parent) const override;
    EdgeItem *createEdgeItem(Edge *e, Item *parent) const override;

    EdgeItem *createTemporaryEdgeItem(PortItem *p, Item *parent) const override;
};

inline StandardItemFactory stdItemFactory;

// Idea: SvgItemFactory

}
#endif // STANDARDITEMFACTORY_H
