#ifndef EDGE_H
#define EDGE_H

#include "QGMFGlobals.h"

#include "EdgeModel.h"
#include "Instance.h"

namespace QGMF {

class Port;

// List of Edges
using EdgeList = List<Edge>;

// Class for Edges
class QGMF_PUBLIC Edge final: public Instance<EdgeModel> {
  public:
    virtual ~Edge(); // detach from port/node will be done from Node

  private:
    // edges can only be created using Node::addEdge()
    Edge(const EdgeModel &model,
         Node *node,
         Port *src,
         Port *dst,
         std::string name = std::string());
    
    // Constructor for untyped/temporary edges
    Edge(Node *node,
         Port *src,
         Port *dst,
         std::string name = std::string());

    friend class Node;

  public:
    Node *node() const { return m_node; }
    Port *src() const { return m_src; }
    Port *dst() const { return m_dst; }

    bool isUntyped() const { return m_untyped; }

    /**
     * Changes source port.
     * Returns true if successful.
     */
    bool changeSrc(Port *p);
    bool changeDst(Port *p);

  private:
    friend class Port;

    Node *m_node;
    Port *m_src;
    Port *m_dst;
    bool m_untyped;
};

}

#endif // EDGE_H
