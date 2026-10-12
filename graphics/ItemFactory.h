#ifndef QGMFGRAPHICS_ITEMFACTORY_H
#define QGMFGRAPHICS_ITEMFACTORY_H

#include "QGMFGraphicsGlobals.h"

#include <QGMF.h>

#include "NodeItem.h"
#include "PortItem.h"
#include "EdgeItem.h"

namespace QGMF {

// Interface for item factories.
// Implement to get customized item shapes
class QGMFGRAPHICS_PUBLIC ItemFactory {
  public:
    virtual ~ItemFactory() { }

    /**
     * @brief createNodeItem creates graphical item for a given node
     * @param n the node
     * @param parent item
     * @note It is not mandatory that the given parent is the NodeItem which
     * contains the new node. You can pass any Item as a parent. Usually,
     * you pass an Item which itself has the perent NodeItem as a parent
     * (like a container Item for subnodes)
     */
    virtual NodeItem *createNodeItem(Node *n, Item *parent) const = 0;
    virtual PortItem *createPortItem(Port *p, Item *parent) const = 0;
    virtual EdgeItem *createEdgeItem(Edge *e, Item *parent) const = 0;

    virtual EdgeItem *createTemporaryEdgeItem(PortItem *p, Item *parent) const = 0;

    // TODO add
    // virtual RotateHandle *createRotateHandle() const = 0;
    // virtual ResizeHandle *createResizeHandle() const = 0;
};

// Idea: SvgItemFactory

}

#endif // ITEMFACTORY_H
